/******************************************************************************
 * File Name        : main.c
 *
 * Description      : This is the source code for Main CM33 non-secure application
 *
 * Related Document : See README.md
 *
 *******************************************************************************
* (c) 2026, Infineon Technologies AG, or an affiliate of Infineon
* Technologies AG. All rights reserved.
* This software, associated documentation and materials ("Software") is
* owned by Infineon Technologies AG or one of its affiliates ("Infineon")
* and is protected by and subject to worldwide patent protection, worldwide
* copyright laws, and international treaty provisions. Therefore, you may use
* this Software only as provided in the license agreement accompanying the
* software package from which you obtained this Software. If no license
* agreement applies, then any use, reproduction, modification, translation, or
* compilation of this Software is prohibited without the express written
* permission of Infineon.
*
* Disclaimer: UNLESS OTHERWISE EXPRESSLY AGREED WITH INFINEON, THIS SOFTWARE
* IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* INCLUDING, BUT NOT LIMITED TO, ALL WARRANTIES OF NON-INFRINGEMENT OF
* THIRD-PARTY RIGHTS AND IMPLIED WARRANTIES SUCH AS WARRANTIES OF FITNESS FOR A
* SPECIFIC USE/PURPOSE OR MERCHANTABILITY.
* Infineon reserves the right to make changes to the Software without notice.
* You are responsible for properly designing, programming, and testing the
* functionality and safety of your intended application of the Software, as
* well as complying with any legal requirements related to its use. Infineon
* does not guarantee that the Software will be free from intrusion, data theft
* or loss, or other breaches ("Security Breaches"), and Infineon shall have
* no liability arising out of any Security Breaches. Unless otherwise
* explicitly approved by Infineon, the Software may not be used in any
* application where a failure of the Product or any consequences of the use
* thereof can reasonably be expected to result in personal injury.
 *******************************************************************************/

/*******************************************************************************
 * Header Files
 ********************************************************************************/

#include "cy_pdl.h"
#include "cybsp.h"
#include <stdio.h>
#include <string.h>
#include "nsc.h"

/*******************************************************************************
 * Macros
 ********************************************************************************/

/* These are the flash addresses where the PPCA core0 and core1 images are located. */
#define CORE0_IMAGE_ADDRESS (CYMEM_CM33_0_m33ns_ppca0_nvm_C_START)
#define CORE1_IMAGE_ADDRESS (CYMEM_CM33_0_m33ns_ppca1_nvm_C_START)

#define CORE0_IMAGE_SIZE (CYMEM_CM33_0_ppca0_code_SIZE)
#define CORE1_IMAGE_SIZE (CYMEM_CM33_0_ppca1_code_SIZE)

/* These are the addresses that the PPCA cores should be using to update. */
#define PPCA_M1_VAR_ADDRESS 0x43020400
#define PPCA_M3_VAR_ADDRESS 0x43040800

/* LED Toggle interval */
#define LED_TOGGLE_INTERVAL_MS 1000U

/* Buffer size available to send message to SPE */
#define LOG_BUFFER_SIZE (256)

/* Macro for Logging */
#define LOG(fmt, ...) log_msg(log_buffer, snprintf(log_buffer, LOG_BUFFER_SIZE, (fmt), ##__VA_ARGS__))

/*******************************************************************************
 * Global Variables
 ********************************************************************************/

/* Log buffer */
CY_SECTION_SHAREDMEM char log_buffer[LOG_BUFFER_SIZE];

/*******************************************************************************
 * Function Prototypes
 ********************************************************************************/


 /*******************************************************************************
 * Function Name: main
 ********************************************************************************
 * Summary:
 *  This is the main function for CM33 CPU. It does...
 *    1. LED 3 Blink
 *    2. Read shared memory updated from PPCA cores and print values using NSC
 *
 * Parameters:
 *  void
 *
 * Return:
 *  int
 *
 *******************************************************************************/
int main(void)
{
    cy_rslt_t result;
    uint32_t count = 0;
    uint32_t var;
    uint32_t prevValueCore0 = 0;
    uint32_t prevValueCore1 = 0;
    uint32_t *pVarCore0 = (uint32_t *)PPCA_M1_VAR_ADDRESS;
    uint32_t *pVarCore1 = (uint32_t *)PPCA_M3_VAR_ADDRESS;

    /* Initialize the device and board peripherals */
    result = cybsp_init();
    if (result != CY_RSLT_SUCCESS)
    {
        /* Board init failed. Stop program execution */
        LOG("Board init failed\r\n");
        CY_ASSERT(0);
    }

    /* Enable interrupts */
    __enable_irq();

    LOG("Main CM33 Non-secure app is running\r\n");

    /* Start PPCA Cores */
    LOG("Starting PPCA CM33 cores\r\n");
    Cy_System_Init_CPU0((void *)CORE0_IMAGE_ADDRESS, CORE0_IMAGE_SIZE);
    Cy_System_Init_CPU1((void *)CORE1_IMAGE_ADDRESS, CORE1_IMAGE_SIZE);

    for (;;)
    {
        /* Check and Toggle LED */
        if ((count % LED_TOGGLE_INTERVAL_MS) == 0)
        {
            Cy_GPIO_Inv(CYBSP_USER_LED1_PORT, CYBSP_USER_LED1_PIN);
        }

        /* Check and Print PPCA Core0 value */
        var = *pVarCore0;
        if (var != prevValueCore0)
        {
            LOG("PPCA Core0 Variable = %u", (unsigned int)var);
            prevValueCore0 = var;
        }

        /* Check and Print PPCA Core1 value */
        var = *pVarCore1;
        if (var != prevValueCore1)
        {
            LOG("PPCA Core1 Variable = %u",  (unsigned int)var);
            prevValueCore1 = var;
        }

        Cy_SysLib_Delay(1U);
        count++;
    }
}
