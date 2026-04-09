
/* Includes----------------------------------------------------------------------*/
#include "BlueNRG1_it.h"
#include "BlueNRG1_conf.h"
#include "ble_const.h"
#include "bluenrg1_stack.h"
#include "SDK_EVAL_Com.h"
#include "clock.h"
#include "bluenrg1_events.h"
#include "user.h"   // pour Attribute_Modified_CB()

/******************************************************************************/
/********************Cortex-M0 Processor Exceptions Handlers********************/
/******************************************************************************/

extern uint32_t lSystickCounter;

/******************************************************************************
 * Function Name  : nmi_handler.
 * Description    : This function handles NMI exception.
 * Input          : None.
 * Output         : None.
 * Return         : None.
******************************************************************************/
void NMI_Handler(void)
{
}


/******************************************************************************
 * Function Name  : hardfault_handler.
 * Description    : This function handles Hard Fault exception.
 * Input          : None.
 * Output         : None.
 * Return         : None.
******************************************************************************/
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {}
}


/******************************************************************************
 * Function Name  : svc_handler.
 * Description    : This function handles SVCall exception.
 * Input          : None.
 * Output         : None.
 * Return         : None.
******************************************************************************/
void SVC_Handler(void)
{
}


/******************************************************************************
 * Function Name  : pendsv_handler.
 * Description    : This function handles PendSV_Handler exception.
 * Input          : None.
 * Output         : None.
 * Return         : None.
******************************************************************************/
void PendSV_Handler(void)
{
}


/******************************************************************************
 * Function Name  : systick_handler.
 * Description    : This function handles SysTick Handler.
 * Input          : None.
 * Output         : None.
 * Return         : None.
******************************************************************************/
void SysTick_Handler(void)
{
	lSystickCounter++;
}


/******************************************************************************
 * Function Name  : gpio_handler.
 * Description    : This function handles GPIO Handler.
 * Input          : None.
 * Output         : None.
 * Return         : None.
******************************************************************************/
void GPIO_Handler(void)
{
}
/******************************************************************************
*                 BlueNRG1LP Peripherals Interrupt Handlers                   *
*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the   *
*  available peripheral interrupt handler/'s name please refer to the startup *
* file (startup_BlueNRG1lp.s).
******************************************************************************/


/******************************************************************************
 * Function Name  : uart_handler.
 * Description    : This function handles UART interrupt request.
 * Input          : None.
 * Output         : None.
 * Return         : None.
******************************************************************************/
void UART_Handler(void)
{
}

/* This function is called when a DB attribute is modified */
void aci_gatt_attribute_modified_event(uint16_t Connection_Handle,
                                       uint16_t Attr_Handle,
                                       uint16_t Offset,
                                       uint16_t Attr_Data_Length,
                                       uint8_t Attr_Data[])
{
  Attribute_Modified_CB(Attr_Handle, Attr_Data_Length, Attr_Data);
}


/******************************************************************************
 * Function Name  : blue_handler.
 * Description    : This function handles Blue Handlers.
 * Input          : None.
 * Output         : None.
 * Return         : None.
******************************************************************************/
void Blue_Handler(void)
{
//  Call RAL_Isr
   RAL_Isr();
}
