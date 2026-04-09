/*
 ******************************************************************************
 * @file    BLE_User_main.c
 * @author  AMS - RF Application Team
 * @version V2.0.0
 * @date    12 - 02 - 2020
 * @brief   User application initialization functions.
 This C file has been created automatically by BlueNRG GUI.
 This code has to be reviewed.
 ******************************************************************************
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
 * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
 * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
 * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
 * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * <h2><center>&copy; COPYRIGHT 2020 STMicroelectronics</center></h2>
 ******************************************************************************
 */

/** \cond DOXYGEN_SHOULD_SKIP_THIS
 */

/* Includes-----------------------------------------------------------------*/
#include <stdio.h>
#include <stdint.h>
#include "BlueNRG1_it.h"
#include "BlueNRG1_conf.h"
#include "ble_const.h"
#include "bluenrg1_stack.h"
#include "SDK_EVAL_Config.h"
#include "sleep.h"
#include "User_config.h"
#include "user.h"
#include "osal.h"
#include "ssd1306.h"
#include "string.h"
#include "clock.h"

#ifndef DEBUG
#define DEBUG 1
#endif

#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

volatile uint32_t lSystickCounter = 0;
/* Private function prototypes -----------------------------------------------*/
void I2C_ConfigurationMaster(void);
void SdkDelayMs(volatile uint32_t lTimeMs);

/* I2C clock frequency */
#define SDK_EVAL_I2C_CLK_SPEED  (100000)

int main(void) {
//	int ret;

	/* System Init */
	SystemInit();

	/* Identify BlueNRG1 platform */
	SdkEvalIdentification();

	SysTick_Config(SYST_CLOCK / 1000 - 1);
	/* Configure I/O communication channel */
	SdkEvalComUartInit(UART_BAUDRATE);

	/* BlueNRG-1 stack init */
//	ret = BlueNRG_Stack_Initialization(&BlueNRG_Stack_Init_params);
//	if (ret != BLE_STATUS_SUCCESS) {
//		printf("Error in BlueNRG_Stack_Initialization() 0x%02x\r\n", ret);
//		while (1)
//			;
//	}
	/* Application demo Led Init */
	SdkEvalLedInit(LED1); //Activity led
	SdkEvalLedInit(LED3); //Error led
	SdkEvalLedOn(LED1);
	SdkEvalLedOn(LED3);

	//device_initialization
	//device_initialization();

	//set_device_discoverable
	//set_device_discoverable();

	I2C_ConfigurationMaster();

	SSD1306_Init();
	SSD1306_ClearDisplay();
	SSD1306_DrawString(10, 10, "Compteur", &FreeMono9pt7b, 1);
	SSD1306_UpdateScreen();
	SdkDelayMs(500);

	uint16_t compteur = 0;

	while (1) {

		/* BlueNRG-1 stack tick */
		//BTLE_StackTick();
		char buffer[10];
		sprintf(buffer, "%d", compteur);

		SSD1306_FillRect(30, 19, 128-30, 30, 0);
		SSD1306_DrawString_GFX(30, 30, buffer, &FreeMono9pt7b,1);
		SSD1306_UpdateScreen();

		compteur++;
		SdkDelayMs(100);   // 1 seconde

		/* Application Tick */
		APP_Tick();

		/* Power Save management */
		//BlueNRG_Sleep(SLEEPMODE_NOTIMER, 0, 0);
	}
}

void I2C_ConfigurationMaster(void) {
	GPIO_InitType GPIO_InitStructure;
	I2C_InitType I2C_InitStruct;

	/* Enable I2C and GPIO clocks */
	if ((I2C_Type*) SDK_EVAL_I2C== I2C2) {
		SysCtrl_PeripheralClockCmd(CLOCK_PERIPH_I2C2 | CLOCK_PERIPH_GPIO, ENABLE);
	}
	else {
		SysCtrl_PeripheralClockCmd(CLOCK_PERIPH_I2C1 | CLOCK_PERIPH_GPIO, ENABLE);
	}

	/* Configure I2C pins */
	GPIO_InitStructure.GPIO_Pin = SDK_EVAL_I2C_CLK_PIN;
	GPIO_InitStructure.GPIO_Mode = SDK_EVAL_I2C_DATA_MODE;
	GPIO_InitStructure.GPIO_Pull = DISABLE;
	GPIO_InitStructure.GPIO_HighPwr = DISABLE;
	GPIO_Init(&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = SDK_EVAL_I2C_DATA_PIN;
	GPIO_InitStructure.GPIO_Mode = SDK_EVAL_I2C_DATA_MODE;
	GPIO_Init(&GPIO_InitStructure);

	/* Configure I2C in master mode */
	I2C_StructInit(&I2C_InitStruct);
	I2C_InitStruct.I2C_OperatingMode = I2C_OperatingMode_Master;
	I2C_InitStruct.I2C_ClockSpeed = SDK_EVAL_I2C_CLK_SPEED;
	I2C_Init((I2C_Type*) SDK_EVAL_I2C, &I2C_InitStruct);

	/* Clear all I2C pending interrupts */
	I2C_ClearITPendingBit((I2C_Type*) SDK_EVAL_I2C, I2C_IT_MSK);

}

void SdkDelayMs(uint32_t delay) {
	uint32_t start = lSystickCounter;
	while ((lSystickCounter - start) < delay)
		;
}
/* Hardware Error event. 
 This event is used to notify the Host that a hardware failure has occurred in the Controller.
 Hardware_Code Values:
 - 0x01: Radio state error
 - 0x02: Timer overrun error
 - 0x03: Internal queue overflow error
 After this event is recommended to force device reset. */

void hci_hardware_error_event(uint8_t Hardware_Code) {
	NVIC_SystemReset();
}

/**
 * This event is generated to report firmware error informations.
 * FW_Error_Type possible values:
 * Values:
 - 0x01: L2CAP recombination failure
 - 0x02: GATT unexpected response
 - 0x03: GATT unexpected request
 After this event with error type (0x01, 0x02, 0x3) it is recommended to disconnect.
 */
void aci_hal_fw_error_event(uint8_t FW_Error_Type, uint8_t Data_Length,
		uint8_t Data[]) {
	if (FW_Error_Type <= 0x03) {
		uint16_t connHandle;

		/* Data field is the connection handle where error has occurred */
		connHandle = LE_TO_HOST_16(Data);

		aci_gap_terminate(connHandle, BLE_ERROR_TERMINATED_REMOTE_USER);
	}
}

#ifdef USE_FULL_ASSERT


/******************************************************************************
 * Function Name  : assert_failed.
 * Description    : Reports the name of the source file and the source line number
                    where the assert_param error has occurred.
 * Input          : - file: pointer to the source file name,
                    - line: assert_param error line source number.
 * Output         : None.
 * Return         : None.
******************************************************************************/
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
  ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}
#endif

/** \endcond 
 */
