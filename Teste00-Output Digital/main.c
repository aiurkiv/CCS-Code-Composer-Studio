/* Teste00 - Código de teste criado para testar a configuração do CCS e da biblioteca TivaWare.
 *
 * Placa: EK-TMC129EXL Rev A
 * CCS versão: 12.4.0
 *
 * Configuração:
 * - GPIO N, pinos 0 e 1 - Output (leds D2 e D1 respectivamente)
 * - GPIO F, pinos 0 e 4 - Output (leds D4 e D3 respectivamente)
 * O código configura as GPIOs N e F como saída e acende os leds em uma sequencia programada.
 * Na sequencia controlo os leds de 2 formas: por grupo de pinos da GPIO e por pino individual.
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
    //
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



    while(1)
    {
        // Desligo as GPIOs
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1, 0);
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4, 0);
        SysCtlDelay((g_ui32SysClock / (1000 * 3)) * 1000 );

        // Liga GPIO F porta 0 e desliga as demais.
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4, GPIO_PIN_0);
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1, 0);
        SysCtlDelay((g_ui32SysClock / (1000 * 3)) * 1000 );

        // Liga GPIO F porta 4 e desliga as demais.
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4, GPIO_PIN_4);
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1, 0);
        SysCtlDelay((g_ui32SysClock / (1000 * 3)) * 1000 );

        // Liga GPIO N porta 0 e desliga as demais.
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4, 0);
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_PIN_0);
        SysCtlDelay((g_ui32SysClock / (1000 * 3)) * 1000 );

        // Liga GPIO N porta 1 e desliga as demais.
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4, 0);
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_PIN_1);
        SysCtlDelay((g_ui32SysClock / (1000 * 3)) * 1000 );

        // Liga GPIO F porta 0 e 4 e desliga as demais.
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4, GPIO_PIN_0 | GPIO_PIN_4);
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1, 0);
        SysCtlDelay((g_ui32SysClock / (1000 * 3)) * 1000 );

        // Liga GPIO N porta 0 e 1 e desliga as demais.
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4, 0);
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_PIN_0 | GPIO_PIN_1);
        SysCtlDelay((g_ui32SysClock / (1000 * 3)) * 1000 );

        // Abaixo faço a mesma sequencia, porém pino a pino.

        // Desligo as GPIOs
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1, 0);
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4, 0);
        SysCtlDelay((g_ui32SysClock / (1000 * 3)) * 1000 );

        //
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, GPIO_PIN_1);
        SysCtlDelay((g_ui32SysClock / (1000 * 3)) * 1000 );

        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, 0);
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, GPIO_PIN_0);
        SysCtlDelay((g_ui32SysClock / (1000 * 3)) * 1000 );

        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, 0);
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_PIN_4);
        SysCtlDelay((g_ui32SysClock / (1000 * 3)) * 1000 );

        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 0);
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_PIN_0);
        SysCtlDelay((g_ui32SysClock / (1000 * 3)) * 1000 );

        // Liga GPIO N porta 0 e 1 e desliga as demais.
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4, 0);
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_PIN_0 | GPIO_PIN_1);
        SysCtlDelay((g_ui32SysClock / (1000 * 3)) * 1000 );

        // Liga GPIO F porta 0 e 4 e desliga as demais.
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4, GPIO_PIN_0 | GPIO_PIN_4);
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1, 0);
        SysCtlDelay((g_ui32SysClock / (1000 * 3)) * 1000 );
    }
}
