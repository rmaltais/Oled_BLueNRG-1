/*
  ******************************************************************************
  * @file    user.h 
  * @author  AMG - RF Application Team
  * @version V1.0.0
  * @date    12 - 10 - 2017
  * @brief   Application Header functions
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
  * <h2><center>&copy; COPYRIGHT 2017 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 
  
/** \cond DOXYGEN_SHOULD_SKIP_THIS
 */ 
 
#ifndef _user_H_
#define _user_H_

/**
  * @brief  This function initializes the BLE GATT & GAP layers and it sets the TX power level 
  * @param  None
  * @retval None
  */
void device_initialization(void);
tBleStatus Add_RC_Service(void);
uint8_t RC_DeviceInit(void);
void Attribute_Modified_CB(uint16_t handle, uint8_t data_length, uint8_t *att_data);
void RC_Send(uint8_t *data, uint8_t len);

#define LED_NUMBER 3

uint16_t RCServHandle, controlPointHandle;

/* UUIDs */
Service_UUID_t service_uuid;
Char_UUID_t controlPointUuid;


/**
  * @brief  This function handles the BLE advertising mode 
  * @param  None
  * @retval None
  */
void set_device_discoverable(void);


/**
  * @brief  User Application tick 
  * @param  None
  * @retval None
  */
void APP_Tick(void);



#endif /* _user_H_ */
/** \endcond 
*/
