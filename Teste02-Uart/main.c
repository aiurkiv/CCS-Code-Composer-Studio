/* Teste02-Uart : C�digo de teste criado para testar a configura��o do CCS e da biblioteca TivaWare.
 *
 * Placa: EK-TMC129EXL Rev A
 *
 * *** Resumo, arrumar depois.
 * Para configurar uma interrup��o, tenho de primeiramente criar uma fun��o do tipo void.
 * Ap�s devo declarar a fun��o como extern no arquivo "tm4c129encpdt_startup_ccs.c" (linha 57 do arquivo)
 * Tamb�m devo colocar o nome da fun��o no vetor de de interrup��o. (ver arquivo tm4... linha 92)
 *
 * Configura��o:
 * - UART0, PA0 e PA1 - RX e TX respectivamente
 * - GPIO A, pinos 0 e 1 - RX e TX respectivamente
 * - Configura��o UART: 9600, 1 stop bit, sem paridade.
 * - Habilito interrup��o na UART0 e a configuro por tempo de recebimento.
 *
 * Como quero que os leds acendam quando o bot�o for precionado, a l�gica do c�digo tem de ser invertida
 * devido a l�gica dos bot�es ser invertida (� como negar o sinal 2x). Ou seja, se a leitura do bot�o
 * for 1, ele n�o est� apertado e o led deve ficar apagado, J� se a leitura do bot�o for 0, ele est�
 * apertado e o led deve acender.
*/

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"

#define TAMANHO     40
#define INICIO      17
#define DELAY_1S    1000


uint32_t g_ui32SysClock;

char dados[TAMANHO];
unsigned char posicao;      // Utilizo char pois aqui o vetor ter� menos de 256 posi��es
bool flagPronto;


/* inicia_variaveis() - Fun��o
 * Inicia o valor das vari�veis
 */
void inicia_variaveis()
{
    sprintf(dados, "Dados recebidos: ");    // Preenche a frase nas primeiras 17 posi��es do vetor dados.
    posicao = INICIO;
    flagPronto = false;
}

/* UARTIntHandler() - Interrup��o UART RX
 *
 * Adiciona os dados recebidos ao vetor at� o momento que encontra o char '\n'
 * Ap�s seta a flagPronto que indica que recebeu os dados e adicionou ao vetor dados.
 */
void UARTIntHandler(void)
{
    uint32_t ui32Status;

    // Pega o estado da interrup��o
    ui32Status = UARTIntStatus(UART0_BASE, true);

    // Limpa a flag de interrup��o
    UARTIntClear(UART0_BASE, ui32Status);

    // Permanece em loop enquanto houver dados para ler
    while(UARTCharsAvail(UART0_BASE))
    {
        // Adiciona um char recebido na "posicao" do vetor
        dados[posicao] = (char)UARTCharGetNonBlocking(UART0_BASE);
        if(dados[posicao] == '\n')
            flagPronto = true;
        posicao++;
        if(posicao >= TAMANHO)
            posicao = INICIO;
    }
}

/* UARTSend() - Fun��o
 *
 * Envia um vetor pela UART at� encontrar o caracter '\n' (envia inclusive o '\n').
 */
void UARTSend(const uint8_t *pui8Buffer)
{
    // Loop while there are more characters to send.
    while(1)
    {
        // Write the next character to the UART.
        UARTCharPutNonBlocking(UART0_BASE, *pui8Buffer);
        if(*pui8Buffer == '\n')
            return;
        else
            *pui8Buffer++;
    }
}

/* main()
 *
 * Inicia a configura��o do sistema e as vari�veis.
 * Ap�s fica em loop infinito aguardando o recebimento de dados pela UART sinalizado pela
 * flagPronto que � tratado por interrup��o. Na interrup��o tamb�m monto o vetor "dados" que
 * armazena os dados para enviar pela UART.
 * Quando a flagPronto � setada, envio o vetor dados pela UART.
 */
void main(void)
{
    // Configurando o clock em 120MHz.
    g_ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                         SYSCTL_OSC_MAIN |
                                         SYSCTL_USE_PLL |
                                         SYSCTL_CFG_VCO_480), 120000000);

    // Iniciando as vari�veis globais.
    inicia_variaveis();

    // Habilito os perif�ricos da UART.
    // Conforme datasheet pg 751, UART0 � no IO PA. PA0 = U0RX, PA1 = U0TX
    // Tem de habilitar ambos os perif�ricos UART0 e GPIOA
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    // Configuro PA0 and PA1 como pinos da UART0.
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // Configuro a UART: 9600, 8 bits / 1 stop bit / sem paridade.
    UARTConfigSetExpClk(UART0_BASE, g_ui32SysClock, 9600,
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                         UART_CONFIG_PAR_NONE));

    // Habilito a interrup��o global.
    IntMasterEnable();

    // Habilito a interrup��o da UART0 por tempo de recebimento do dado
    IntEnable(INT_UART0);
    UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);

    // Delay inicial de 1s
    SysCtlDelay((g_ui32SysClock / (1000 * 3)) * DELAY_1S);

    // Sinaliza que programa iniciou.
    UARTSend((uint8_t *)"Iniciado\n");

    // Loop infinito
    while(1)
    {
        // Quando recebe dados pela UART, a interrup��o monta a mensagem de retorno no vetor dados
        // e habilita flagPronto.
        if(flagPronto)
        {
            UARTSend((uint8_t *)dados);
            posicao = INICIO;
            flagPronto = false;
        }
    }
}
