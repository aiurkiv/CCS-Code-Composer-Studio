/* Teste01 - Código de teste criado para testar a configuração do CCS e da biblioteca TivaWare.
 *
 * Placa: EK-TMC129EXL Rev A
 *
 * Configuração:
 * - GPIO J, pinos 0 e 1 - Input (botões SW1 e SW2 respectivamente)
 * - GPIO N, pinos 0 e 1 - Output (leds D2 e D1 respectivamente)
 * - GPIO F, pinos 0 e 4 - Output (leds D4 e D3 respectivamente)
 * Conforme datasheet da placa, botões SW1 e SW2 tem lógica invertida (quando não apertados o sinal é 1,
 * quando apertados o sinal é 0), logo os seto como pull-up open drain para a lógica invertida
 * e limito a corrente em 2ma.
 * Como quero que os leds acendam quando o botão for precionado, a lógica do código tem de ser invertida
 * devido a lógica dos botões ser invertida (é como negar o sinal 2x). Ou seja, se a leitura do botão
 * for 1, ele não está apertado e o led deve ficar apagado, Já se a leitura do botão for 0, ele está
 * apertado e o led deve acender.
*/

#include <stdint.h>
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

uint32_t g_ui32SysClock;


/**
 * main.c
 */
void main(void)
{
    // Configurando o clock de 120MHz.
    //
    g_ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                             SYSCTL_OSC_MAIN |
                                             SYSCTL_USE_PLL |
                                             SYSCTL_CFG_VCO_480), 120000000);

    // Habilitando GPIO N
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    // Configurando pinos 0 e 1 do GPIO N como saída
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // Habilitando GPIO F
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    // Configurando pinos 0 e 4 do GPIO F como saída
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4);

    // Habilitando GPIO J
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);

    // Configurando pinos 0 e 1 como entrada
    GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    // GPIO J, pinos 0 e 1, corrente 2ma, STD=push pull WPU=open drain.
    GPIOPadConfigSet(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);


    // loop infinito
    while(1)
    {
        // Se SW2 não estiver apertado, apaga os leds da GPIO F
        if (GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_0) == GPIO_PIN_0)
        {
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4, 0);
        }
        // Caso contrario acende
        else
        {
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4, GPIO_PIN_0 | GPIO_PIN_4);
        }
        // Se SW1 não estiver apertado, apaga os leds da GPIO N
        if (GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_1) == GPIO_PIN_1)
        {
            GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1, 0);
        }
        // Caso contrario acende
        else
        {
            GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_PIN_0 | GPIO_PIN_1);
        }
    }
}
