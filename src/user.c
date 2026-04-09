/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "BlueNRG1_it.h"
#include "BlueNRG1_conf.h"
#include "ble_const.h"
#include "bluenrg1_stack.h"
#include "bluenrg1_events.h"
#include "gp_timer.h"
#include "SDK_EVAL_Config.h"
#include "OTA_btl.h"
#include "user.h"
#include "osal.h"

#define DEBUG 1
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

#define ENABLE_SECURITY 0

/* === GLOBALS POUR LE SERVICE RC ========================================== */

uint16_t RCServHandle;

uint16_t rxCharHandle;
uint16_t txCharHandle;


Char_UUID_t rxUuid;
Char_UUID_t txUuid;






/* ========================================================================== */

void device_initialization(void)
{
    uint16_t service_handle, dev_name_char_handle, appearance_char_handle;
    uint8_t status;
    uint8_t device_name[] = {'O','l','e','d','E','M','B'};

    uint8_t public_addr[] = {0xAA,0x9A,0x78,0x56,0x34,0x12};   //170 154 120 86 52 18

    status = aci_hal_write_config_data(0x00, 6, public_addr);
    PRINTF("aci_hal_write_config_data: 0x%02X\n", status);

    status = aci_gatt_init();
    PRINTF("aci_gatt_init: 0x%02X\n", status);

    status = aci_gap_init(GAP_PERIPHERAL_ROLE, 0, 7,
                          &service_handle, &dev_name_char_handle, &appearance_char_handle);
    PRINTF("aci_gap_init: 0x%02X\n", status);

    status = aci_hal_set_tx_power_level(1, 4);
    PRINTF("aci_hal_set_tx_power_level: 0x%02X\n", status);

    status = aci_gatt_update_char_value_ext(0, service_handle, dev_name_char_handle,
                                            0, sizeof(device_name),
                                            0, sizeof(device_name), device_name);
    PRINTF("aci_gatt_update_char_value_ext: 0x%02X\n", status);

    status = Add_RC_Service();
    PRINTF("Add_RC_Service: 0x%02X\n", status);
}

void set_device_discoverable(void)
{
    uint8_t status;

    status = aci_gap_set_discoverable(ADV_IND,
                                      0x00A0, 0x00A0,
                                      0x00,
                                      NO_WHITE_LIST_USE,
                                      0, NULL,
                                      0, NULL,
                                      0, 0);
    PRINTF("aci_gap_set_discoverable: 0x%02X\n", status);
}

tBleStatus Add_RC_Service(void)
{
    tBleStatus ret;




    // UUID RX (WRITE)
    uint8_t RX_UUID_128[16] = {
        0xba,0x5c,0xf7,0x93,0x3b,0x12,0xd3,0x89,
        0xe4,0x11,0x0d,0x9b,0x1a,0xfb,0x0e,0xed
    };

    // UUID TX (NOTIFY)
    uint8_t TX_UUID_128[16] = {
        0xba,0x5c,0xf7,0x93,0x3b,0x12,0xd3,0x89,
        0xe4,0x11,0x0d,0x9b,0x2a,0xfb,0x0e,0xed
    };

    // Copier UUID service
    //Osal_MemCpy(service_uuid.Service_UUID_128, Service_UUID_128, 16);

    // Copier UUID RX/TX
    Osal_MemCpy(rxUuid.Char_UUID_128, RX_UUID_128, 16);
    Osal_MemCpy(txUuid.Char_UUID_128, TX_UUID_128, 16);

    // Service avec assez d'attributs
    ret = aci_gatt_add_service(UUID_TYPE_128, &service_uuid,
                               PRIMARY_SERVICE, 20, &RCServHandle);
    PRINTF("Add service: 0x%02X\n", ret);
    if (ret != BLE_STATUS_SUCCESS) return ret;

    // === RX characteristic (WRITE ONLY) ===
    ret = aci_gatt_add_char(RCServHandle,
                            UUID_TYPE_128,
                            &rxUuid,
                            64,
                            CHAR_PROP_WRITE | CHAR_PROP_WRITE_WITHOUT_RESP,
                            ATTR_PERMISSION_NONE,
                            GATT_NOTIFY_ATTRIBUTE_WRITE,
                            7,
                            0,
                            &rxCharHandle);
    PRINTF("Add RX char: 0x%02X\n", ret);
    if (ret != BLE_STATUS_SUCCESS) return ret;

    // === TX characteristic (NOTIFY ONLY) ===
    ret = aci_gatt_add_char(RCServHandle,
                            UUID_TYPE_128,
                            &txUuid,
                            64,
                            CHAR_PROP_NOTIFY | CHAR_PROP_READ,
                            ATTR_PERMISSION_NONE,
                            GATT_NOTIFY_ATTRIBUTE_WRITE,
                            7,
                            0,
                            &txCharHandle);
    PRINTF("Add TX char: 0x%02X\n", ret);
    if (ret != BLE_STATUS_SUCCESS) return ret;

    return BLE_STATUS_SUCCESS;
}

void RC_Send(uint8_t *data, uint8_t len)
{
    tBleStatus ret;

    ret = aci_gatt_update_char_value(RCServHandle,
                                     txCharHandle,   // ← le global
                                     0,
                                     len,
                                     data);

    PRINTF("RC_Send status: 0x%02X\n", ret);
}


/* === CALLBACK D'ÉCRITURE SUR LA CARACTÉRISTIQUE ========================== */

char rx_buffer[64];
int  rx_index = 0;
int  message_ready = 0;

void Attribute_Modified_CB(uint16_t handle, uint8_t data_length, uint8_t *att_data)
{
    if (handle != rxCharHandle + 1)
        return;

    if (data_length >= sizeof(rx_buffer))
        data_length = sizeof(rx_buffer) - 1;

    memcpy(rx_buffer, att_data, data_length);
    rx_buffer[data_length] = '\0';
    rx_index = data_length;

    PRINTF("Texte reçu (%d bytes): %s\n", data_length, rx_buffer);

    message_ready = 1;   // ← juste un flag, rien d’autre
}




/* === DISPATCH DES ÉVÉNEMENTS BLE ======================================== */



void APP_Tick(void)
{
    /* Rien ici pour l’instant */
}
