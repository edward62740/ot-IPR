/*
 *  ======== ti_radio_config.h ========
 *  Configured RadioConfig module definitions
 *
 *  DO NOT EDIT - This file is generated for the CC2652RB1FRGZ
 *  by the SysConfig tool.
 *
 *  Radio Config module version : 1.14
 *  SmartRF Studio data version : 2.26.0
 */
#ifndef _TI_RADIO_CONFIG_H_
#define _TI_RADIO_CONFIG_H_

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(driverlib/rf_mailbox.h)
#include DeviceFamily_constructPath(driverlib/rf_common_cmd.h)
#include DeviceFamily_constructPath(driverlib/rf_ieee_cmd.h)
#include <ti/drivers/rf/RF.h>

/* SmartRF Studio version that the RF data is fetched from */
#define SMARTRF_STUDIO_VERSION "2.26.0"

// *********************************************************************************
//   RF Frontend configuration
// *********************************************************************************
// RF design based on: LP_CC2652RB (CC2652REM-7ID)
#define LP_CC2652RB

// RF frontend configuration
#define FRONTEND_24G_DIFF_RF
#define FRONTEND_24G_INT_BIAS

// Supported frequency bands
#define SUPPORT_FREQBAND_2400

// TX power table size definitions
#define TXPOWERTABLE_2400_PA5_SIZE 16 // 2400 MHz, 5 dBm

// TX power tables
extern RF_TxPowerTable_Entry txPowerTable_2400_pa5[]; // 2400 MHz, 5 dBm



//*********************************************************************************
//  RF Setting:   IEEE 802.15.4-2006, 250 kbps, OQPSK, DSSS = 1:8
//
//  PHY:          ieee154
//  Setting file: setting_ieee_802_15_4.json
//*********************************************************************************

// PA table usage
#define TX_POWER_TABLE_SIZE TXPOWERTABLE_2400_PA5_SIZE

#define txPowerTable txPowerTable_2400_pa5

// TI-RTOS RF Mode object
extern RF_Mode RF_prop;

// RF Core API commands
extern const rfc_CMD_RADIO_SETUP_t RF_cmdRadioSetup;
extern const rfc_CMD_IEEE_TX_t RF_cmdIeeeTx;
extern const rfc_CMD_IEEE_RX_t RF_cmdIeeeRx;
extern const rfc_CMD_TX_TEST_t RF_cmdTxTest;
extern const rfc_CMD_IEEE_ED_SCAN_t RF_cmdIeeeEdScan;
extern const rfc_CMD_IEEE_CSMA_t RF_cmdIeeeCsma;
extern const rfc_CMD_IEEE_RX_ACK_t RF_cmdIeeeRxAck;

// RF Core API overrides
extern uint32_t pOverrides[];

#endif // _TI_RADIO_CONFIG_H_
