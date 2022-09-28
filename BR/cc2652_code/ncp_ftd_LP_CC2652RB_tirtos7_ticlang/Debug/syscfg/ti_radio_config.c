/*
 *  ======== ti_radio_config.c ========
 *  Configured RadioConfig module definitions
 *
 *  DO NOT EDIT - This file is generated for the CC2652RB1FRGZ
 *  by the SysConfig tool.
 *
 *  Radio Config module version : 1.14
 *  SmartRF Studio data version : 2.26.0
 */

#include "ti_radio_config.h"
#include DeviceFamily_constructPath(rf_patches/rf_patch_cpe_ieee_802_15_4.h)


// *********************************************************************************
//   RF Frontend configuration
// *********************************************************************************
// RF design based on: LP_CC2652RB (CC2652REM-7ID)

// TX Power tables
// The RF_TxPowerTable_DEFAULT_PA_ENTRY and RF_TxPowerTable_HIGH_PA_ENTRY macros are defined in RF.h.
// The following arguments are required:
// RF_TxPowerTable_DEFAULT_PA_ENTRY(bias, gain, boost, coefficient)
// RF_TxPowerTable_HIGH_PA_ENTRY(bias, ibboost, boost, coefficient, ldoTrim)
// See the Technical Reference Manual for further details about the "txPower" Command field.
// The PA settings require the CCFG_FORCE_VDDR_HH = 0 unless stated otherwise.

// 2400 MHz, 5 dBm
RF_TxPowerTable_Entry txPowerTable_2400_pa5[TXPOWERTABLE_2400_PA5_SIZE] =
{
    {-20, RF_TxPowerTable_DEFAULT_PA_ENTRY(6, 3, 0, 2) }, // 0x04C6
    {-18, RF_TxPowerTable_DEFAULT_PA_ENTRY(8, 3, 0, 3) }, // 0x06C8
    {-15, RF_TxPowerTable_DEFAULT_PA_ENTRY(10, 3, 0, 3) }, // 0x06CA
    {-12, RF_TxPowerTable_DEFAULT_PA_ENTRY(12, 3, 0, 5) }, // 0x0ACC
    {-10, RF_TxPowerTable_DEFAULT_PA_ENTRY(15, 3, 0, 5) }, // 0x0ACF
    {-9, RF_TxPowerTable_DEFAULT_PA_ENTRY(16, 3, 0, 5) }, // 0x0AD0
    {-6, RF_TxPowerTable_DEFAULT_PA_ENTRY(20, 3, 0, 8) }, // 0x10D4
    {-5, RF_TxPowerTable_DEFAULT_PA_ENTRY(22, 3, 0, 9) }, // 0x12D6
    {-3, RF_TxPowerTable_DEFAULT_PA_ENTRY(19, 2, 0, 12) }, // 0x1893
    {0, RF_TxPowerTable_DEFAULT_PA_ENTRY(19, 1, 0, 20) }, // 0x2853
    {1, RF_TxPowerTable_DEFAULT_PA_ENTRY(22, 1, 0, 20) }, // 0x2856
    {2, RF_TxPowerTable_DEFAULT_PA_ENTRY(25, 1, 0, 25) }, // 0x3259
    {3, RF_TxPowerTable_DEFAULT_PA_ENTRY(29, 1, 0, 28) }, // 0x385D
    {4, RF_TxPowerTable_DEFAULT_PA_ENTRY(35, 1, 0, 39) }, // 0x4E63
    {5, RF_TxPowerTable_DEFAULT_PA_ENTRY(23, 0, 0, 57) }, // 0x7217
    RF_TxPowerTable_TERMINATION_ENTRY
};



//*********************************************************************************
//  RF Setting:   IEEE 802.15.4-2006, 250 kbps, OQPSK, DSSS = 1:8
//
//  PHY:          ieee154
//  Setting file: setting_ieee_802_15_4.json
//*********************************************************************************

// PARAMETER SUMMARY
// TX Power (dBm): 5

// TI-RTOS RF Mode Object
RF_Mode RF_prop =
{
    .rfMode = RF_MODE_AUTO,
    .cpePatchFxn = &rf_patch_cpe_ieee_802_15_4,
    .mcePatchFxn = 0,
    .rfePatchFxn = 0
};

// Overrides for CMD_RADIO_SETUP
uint32_t pOverrides[] =
{
    // override_ieee_802_15_4.json
    // DC/DC regulator: In Tx, use DCDCCTL5[3:0]=0x3 (DITHER_EN=0 and IPEAK=3).
    (uint32_t)0x00F388D3,
    // Rx: Set LNA bias current offset to +15 to saturate trim to max (default: 0)
    (uint32_t)0x000F8883,
    // override_hposc.json
    // HPOSC frequency offset override, freqOffset=2^22*(F_nom-F_hposc)/F_hposc
    HPOSC_OVERRIDE(0),
    (uint32_t)0xFFFFFFFF
};



// CMD_RADIO_SETUP
// Radio Setup Command for Pre-Defined Schemes
const rfc_CMD_RADIO_SETUP_t RF_cmdRadioSetup =
{
    .commandNo = 0x0802,
    .status = 0x0000,
    .pNextOp = 0,
    .startTime = 0x00000000,
    .startTrigger.triggerType = 0x0,
    .startTrigger.bEnaCmd = 0x0,
    .startTrigger.triggerNo = 0x0,
    .startTrigger.pastTrig = 0x0,
    .condition.rule = 0x1,
    .condition.nSkip = 0x0,
    .mode = 0x01,
    .loDivider = 0x00,
    .config.frontEndMode = 0x0,
    .config.biasMode = 0x0,
    .config.analogCfgMode = 0x0,
    .config.bNoFsPowerUp = 0x0,
    .config.bSynthNarrowBand = 0x0,
    .txPower = 0x7217,
    .pRegOverride = pOverrides
};

// CMD_IEEE_TX
// IEEE 802.15.4 Transmit Command
const rfc_CMD_IEEE_TX_t RF_cmdIeeeTx =
{
    .commandNo = 0x2C01,
    .status = 0x0000,
    .pNextOp = 0,
    .startTime = 0x00000000,
    .startTrigger.triggerType = 0x0,
    .startTrigger.bEnaCmd = 0x0,
    .startTrigger.triggerNo = 0x0,
    .startTrigger.pastTrig = 0x0,
    .condition.rule = 0x1,
    .condition.nSkip = 0x0,
    .txOpt.bIncludePhyHdr = 0x0,
    .txOpt.bIncludeCrc = 0x0,
    .txOpt.payloadLenMsb = 0x0,
    .payloadLen = 0x1E,
    .pPayload = 0,
    .timeStamp = 0x00000000
};

// CMD_IEEE_RX
// IEEE 802.15.4 Receive Command
const rfc_CMD_IEEE_RX_t RF_cmdIeeeRx =
{
    .commandNo = 0x2801,
    .status = 0x0000,
    .pNextOp = 0,
    .startTime = 0x00000000,
    .startTrigger.triggerType = 0x0,
    .startTrigger.bEnaCmd = 0x0,
    .startTrigger.triggerNo = 0x0,
    .startTrigger.pastTrig = 0x0,
    .condition.rule = 0x1,
    .condition.nSkip = 0x0,
    .channel = 0x00,
    .rxConfig.bAutoFlushCrc = 0x0,
    .rxConfig.bAutoFlushIgn = 0x0,
    .rxConfig.bIncludePhyHdr = 0x0,
    .rxConfig.bIncludeCrc = 0x0,
    .rxConfig.bAppendRssi = 0x1,
    .rxConfig.bAppendCorrCrc = 0x1,
    .rxConfig.bAppendSrcInd = 0x0,
    .rxConfig.bAppendTimestamp = 0x0,
    .pRxQ = 0,
    .pOutput = 0,
    .frameFiltOpt.frameFiltEn = 0x0,
    .frameFiltOpt.frameFiltStop = 0x0,
    .frameFiltOpt.autoAckEn = 0x0,
    .frameFiltOpt.slottedAckEn = 0x0,
    .frameFiltOpt.autoPendEn = 0x0,
    .frameFiltOpt.defaultPend = 0x0,
    .frameFiltOpt.bPendDataReqOnly = 0x0,
    .frameFiltOpt.bPanCoord = 0x0,
    .frameFiltOpt.maxFrameVersion = 0x3,
    .frameFiltOpt.fcfReservedMask = 0x0,
    .frameFiltOpt.modifyFtFilter = 0x0,
    .frameFiltOpt.bStrictLenFilter = 0x0,
    .frameTypes.bAcceptFt0Beacon = 0x1,
    .frameTypes.bAcceptFt1Data = 0x1,
    .frameTypes.bAcceptFt2Ack = 0x1,
    .frameTypes.bAcceptFt3MacCmd = 0x1,
    .frameTypes.bAcceptFt4Reserved = 0x1,
    .frameTypes.bAcceptFt5Reserved = 0x1,
    .frameTypes.bAcceptFt6Reserved = 0x1,
    .frameTypes.bAcceptFt7Reserved = 0x1,
    .ccaOpt.ccaEnEnergy = 0x0,
    .ccaOpt.ccaEnCorr = 0x0,
    .ccaOpt.ccaEnSync = 0x0,
    .ccaOpt.ccaCorrOp = 0x1,
    .ccaOpt.ccaSyncOp = 0x1,
    .ccaOpt.ccaCorrThr = 0x0,
    .ccaRssiThr = 0x64,
    .__dummy0 = 0x00,
    .numExtEntries = 0x00,
    .numShortEntries = 0x00,
    .pExtEntryList = 0,
    .pShortEntryList = 0,
    .localExtAddr = 0x12345678,
    .localShortAddr = 0xABBA,
    .localPanID = 0x0000,
    .__dummy1 = 0x000000,
    .endTrigger.triggerType = 0x1,
    .endTrigger.bEnaCmd = 0x0,
    .endTrigger.triggerNo = 0x0,
    .endTrigger.pastTrig = 0x0,
    .endTime = 0x00000000
};

// CMD_TX_TEST
// Transmitter Test Command
const rfc_CMD_TX_TEST_t RF_cmdTxTest =
{
    .commandNo = 0x0808,
    .status = 0x0000,
    .pNextOp = 0,
    .startTime = 0x00000000,
    .startTrigger.triggerType = 0x0,
    .startTrigger.bEnaCmd = 0x0,
    .startTrigger.triggerNo = 0x0,
    .startTrigger.pastTrig = 0x0,
    .condition.rule = 0x0,
    .condition.nSkip = 0x0,
    .config.bUseCw = 0x0,
    .config.bFsOff = 0x0,
    .config.whitenMode = 0x0,
    .__dummy0 = 0x00,
    .txWord = 0x0000,
    .__dummy1 = 0x00,
    .endTrigger.triggerType = 0x0,
    .endTrigger.bEnaCmd = 0x0,
    .endTrigger.triggerNo = 0x0,
    .endTrigger.pastTrig = 0x0,
    .syncWord = 0x00000000,
    .endTime = 0x00000000
};

// CMD_IEEE_ED_SCAN
// IEEE 802.15.4 Energy Detect Scan Command
const rfc_CMD_IEEE_ED_SCAN_t RF_cmdIeeeEdScan =
{
    .commandNo = 0x2802,
    .status = 0x0000,
    .pNextOp = 0,
    .startTime = 0x00000000,
    .startTrigger.triggerType = 0x0,
    .startTrigger.bEnaCmd = 0x0,
    .startTrigger.triggerNo = 0x0,
    .startTrigger.pastTrig = 0x0,
    .condition.rule = 0x0,
    .condition.nSkip = 0x0,
    .channel = 0x00,
    .ccaOpt.ccaEnEnergy = 0x0,
    .ccaOpt.ccaEnCorr = 0x0,
    .ccaOpt.ccaEnSync = 0x0,
    .ccaOpt.ccaCorrOp = 0x0,
    .ccaOpt.ccaSyncOp = 0x0,
    .ccaOpt.ccaCorrThr = 0x0,
    .ccaRssiThr = 0x00,
    .__dummy0 = 0x00,
    .maxRssi = 0x00,
    .endTrigger.triggerType = 0x0,
    .endTrigger.bEnaCmd = 0x0,
    .endTrigger.triggerNo = 0x0,
    .endTrigger.pastTrig = 0x0,
    .endTime = 0x00000000
};

// CMD_IEEE_CSMA
// IEEE 802.15.4 CSMA-CA Command
const rfc_CMD_IEEE_CSMA_t RF_cmdIeeeCsma =
{
    .commandNo = 0x2C02,
    .status = 0x0000,
    .pNextOp = 0,
    .startTime = 0x00000000,
    .startTrigger.triggerType = 0x0,
    .startTrigger.bEnaCmd = 0x0,
    .startTrigger.triggerNo = 0x0,
    .startTrigger.pastTrig = 0x0,
    .condition.rule = 0x0,
    .condition.nSkip = 0x0,
    .randomState = 0x0000,
    .macMaxBE = 0x00,
    .macMaxCSMABackoffs = 0x00,
    .csmaConfig.initCW = 0x0,
    .csmaConfig.bSlotted = 0x0,
    .csmaConfig.rxOffMode = 0x0,
    .NB = 0x00,
    .BE = 0x00,
    .remainingPeriods = 0x00,
    .lastRssi = 0x00,
    .endTrigger.triggerType = 0x0,
    .endTrigger.bEnaCmd = 0x0,
    .endTrigger.triggerNo = 0x0,
    .endTrigger.pastTrig = 0x0,
    .lastTimeStamp = 0x00000000,
    .endTime = 0x00000000
};

// CMD_IEEE_RX_ACK
// IEEE 802.15.4 Receive Acknowledgement Command
const rfc_CMD_IEEE_RX_ACK_t RF_cmdIeeeRxAck =
{
    .commandNo = 0x2C03,
    .status = 0x0000,
    .pNextOp = 0,
    .startTime = 0x00000000,
    .startTrigger.triggerType = 0x0,
    .startTrigger.bEnaCmd = 0x0,
    .startTrigger.triggerNo = 0x0,
    .startTrigger.pastTrig = 0x0,
    .condition.rule = 0x0,
    .condition.nSkip = 0x0,
    .seqNo = 0x00,
    .endTrigger.triggerType = 0x0,
    .endTrigger.bEnaCmd = 0x0,
    .endTrigger.triggerNo = 0x0,
    .endTrigger.pastTrig = 0x0,
    .endTime = 0x00000000
};


