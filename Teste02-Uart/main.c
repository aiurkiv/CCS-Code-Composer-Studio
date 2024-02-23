/* Teste02-Uart : Código de teste criado para testar a configuração do CCS e da biblioteca TivaWare.
 *
 * Placa: EK-TMC129EXL Rev A
 *
 * *** Resumo, arrumar depois.
 * Para configurar uma interrupção, tenho de primeiramente criar uma função do tipo void.
 * Após devo declarar a função como extern no arquivo "tm4c129encpdt_startup_ccs.c" (linha 57 do arquivo)
 * Também devo colocar o nome da função no vetor de de interrupção. (ver arquivo tm4... linha 92)
 *
 * Configuração:
 * - UART0, PA0 e PA1 - RX e TX respectivamente
 * - GPIO A, pinos 0 e 1 - RX e TX respectivamente
 * - Configuração UART: 9600, 1 stop bit, sem paridade.
 * - Habilito interrupção na UART0 e a configuro por tempo de recebimento.
 *
 * Como quero que os leds acendam quando o botão for precionado, a lógica do código tem de ser invertida
 * devido a lógica dos botões ser invertida (é como negar o sinal 2x). Ou seja, se a leitura do botão
 * for 1, ele não está apertado e o led deve ficar apagado, Já se a leitura do botão for 0, ele está
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
unsigned char posicao;      // Utilizo char pois aqui o vetor terá menos de 256 posições
bool flagPronto;


/* inicia_variaveis() - Função
 * Inicia o valor das variáveis
 */
void inicia_variaveis()
{
    sprintf(dados, "Dados recebidos: ");    // Preenche a frase nas primeiras 17 posições do vetor dados.
    posicao = INICIO;
    flagPronto = false;
}

/* UARTIntHandler() - Interrupção UART RX
 *
 * Adiciona os dados recebidos ao vetor até o momento que encontra o char '\n'
 * Após seta a flagPronto que indica que recebeu os dados e adicionou ao vetor dados.
 */
void UARTIntHandler(void)
{
    uint32_t ui32Status;

    // Pega o estado da interrupção
    ui32Status = UARTIntStatus(UART0_BASE, true);

    // Limpa a flag de interrupção
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

/* UARTSend() - Função
 *
 * Envia um vetor pela UART até encontrar o caracter '\n' (envia inclusive o '\n').
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
 * Inicia a configuração do sistema e as variáveis.
 * Após fica em loop infinito aguardando o recebimento de dados pela UART sinalizado pela
 * flagPronto que é tratado por interrupção. Na interrupção também monto o vetor "dados" que
 * armazena os dados para enviar pela UART.
 * Quando a flagPronto é setada, envio o vetor dados pela UART.
 */
void main(void)
{
    // Configurando o clock em 120MHz.
    g_ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                         SYSCTL_OSC_MAIN |
                                         SYSCTL_USE_PLL |
                                         SYSCTL_CFG_VCO_480), 120000000);

    // Iniciando as variáveis globais.
    inicia_variaveis();

    // Habilito os periféricos da UART.
    // Conforme datasheet pg 751, UART0 é no IO PA. PA0 = U0RX, PA1 = U0TX
    // Tem de habilitar ambos os periféricos UART0 e GPIOA
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

    // Habilito a interrupção global.
    IntMasterEnable();

    // Habilito a interrupção da UART0 por tempo de recebimento do dado
    IntEnable(INT_UART0);
    UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);

    // Delay inicial de 1s
    SysCtlDelay((g_ui32SysClock / (1000 * 3)) * DELAY_1S);

    // Sinaliza que programa iniciou.
    UARTSend((uint8_t *)"Iniciado\n");

    // Loop infinito
    while(1)
    {
        // Quando recebe dados pela UART, a interrupção monta a mensagem de retorno no vetor dados
        // e habilita flagPronto.
        if(flagPronto)
        {
            UARTSend((uint8_t *)dados);
            posicao = INICIO;
            flagPronto = false;
        }
    }
}
