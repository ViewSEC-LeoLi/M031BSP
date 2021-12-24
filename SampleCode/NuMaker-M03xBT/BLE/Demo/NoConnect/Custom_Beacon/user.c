/**************************************************************************//**
 * @file     user.c
 * @version  V0.9
 * $Revision: 01 $
 * @brief
 *           Demonstrate how to use LIB pre-build function to start and stop a BLE
 *           connection.
 * @note
 *
 ******************************************************************************/
#include <stdio.h>
#include "BleAppSetting.h"
#include "ble_cmd.h"
#include "ble_event.h"
#include "ble_host.h"
#include "porting_misc.h"

/**************************************************************************
* Application Definitions
**************************************************************************/
// Advertising parameters
#define APP_ADV_INTERVAL_MIN            160U    // 160*0.625ms=100ms
#define APP_ADV_INTERVAL_MAX            160U    // 160*0.625ms=100ms


/**************************************************************************
 * Variable
 **************************************************************************/
uint8_t ble_state = STATE_BLE_STANDBY;

//Adv data format. Set according to user select profile. See Bluetooth Spec. Ver5.0 [Vol 3], Part C, Section 11
uint8_t advData[] =
{
    0x0F,                                               // Total data length
    GAP_AD_TYPE_LENGTH_2, GAP_AD_TYPE_FLAGS, 0x06,      // LE General Discoverable Mode
    0x0B, GAP_AD_TYPE_MANUFACTURER_SPECIFIC_DATA,       // Manufacturer data length, Type: Manufacturer data
    0x00,                                               // Variable 1
    0x00,                                               // Variable 2
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,     // Constants
};


/**************************************************************************
 * Function Prototype Declaration
 **************************************************************************/
BleStackStatus Ble_AdvStart(void);
static void BleEvent_Callback(BleCmdEvent event, void *param);

/**************************************************************************
 * Function
 **************************************************************************/
#pragma push
//#pragma Otime
#pragma Ospace

void handle_App_NoConnect(void)
{
    BleStackStatus status;

    if (ble_state == STATE_BLE_STANDBY)
    {
        // enable advertisement
        status = Ble_AdvStart();
        if (status == BLESTACK_STATUS_SUCCESS)
        {
            ble_state = STATE_BLE_ADVERTISING;
        }
    }
}

BleStackStatus Ble_AdvInit(void)
{
    BleStackStatus status;
    BLE_Adv_Param advParam;


    // Adv. parameter init
    advParam.advType = ADV_TYPE_ADV_NONCONN_IND;
    advParam.advIntervalMin = APP_ADV_INTERVAL_MIN;
    advParam.advIntervalMax = APP_ADV_INTERVAL_MAX;
    advParam.advChannelMap = ADV_CHANNEL_ALL;
    advParam.advFilterPolicy = ADV_FILTER_POLICY_ACCEPT_ALL;
    status = setBLE_AdvParam(&advParam);
    BLESTACK_STATUS_CHECK(status);

    // Adv. data init
    status = setBLE_AdvData((uint8_t *)advData, sizeof(advData));
    BLESTACK_STATUS_CHECK(status);

    return BLESTACK_STATUS_SUCCESS;
}


BleStackStatus Ble_AdvStart(void)
{
    BleStackStatus status;

    status = Ble_AdvInit();
    BLESTACK_STATUS_CHECK(status);

    status = setBLE_AdvEnable(BLE_HOSTID_RESERVED);
    BLESTACK_STATUS_CHECK(status);

    return BLESTACK_STATUS_SUCCESS;
}


BleStackStatus BleApp_Init(void)
{
    BleStackStatus status = BLESTACK_STATUS_SUCCESS;

    /* set company Id*/
    setBLE_CompanyId(((uint16_t)BLE_COMPANY_ID_H << 8) | BLE_COMPANY_ID_L);

    /* register command event callback function */
    setBLE_RegisterBleEvent(BleEvent_Callback);

    return status;
}

void TMR0_IRQHandler(void)
{
    static uint8_t var1 = 0x00;
    static uint8_t var2 = 0xFF;

    if (TIMER_GetIntFlag(TIMER0) == 1)
    {
        /* Clear Timer0 time-out interrupt flag */
        TIMER_ClearIntFlag(TIMER0);
    }

    if (ble_state == STATE_BLE_ADVERTISING)
    {
        /* Update variables */
        advData[6] = var1++;
        advData[7] = var2--;
        setBLE_AdvData((uint8_t *)advData, sizeof(advData));
    }
}

void BleApp_Main(void)
{
    // Handle App for No Connect
    handle_App_NoConnect();
} //end of BleApp_Main()


/* BLE Callback Function */
static void BleEvent_Callback(BleCmdEvent event, void *param)
{
    switch (event)
    {
    case BLECMD_EVENT_ADV_COMPLETE:
        printf("Advertising...\n");
        break;

    default:
        break;
    }
}
#pragma pop

