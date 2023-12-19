/***************************************************************************//**
 * @file  zap-event.h
 * @brief Generated file for zcl events using ZAP. Do not update file manually.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/


#ifdef SL_COMPONENT_CATALOG_PRESENT
#include "sl_component_catalog.h"
#endif

#ifdef SL_CATALOG_ZIGBEE_ZCL_FRAMEWORK_CORE_PRESENT
#include "af.h"

#define EMBER_AF_GENERATED_UC_EVENTS_DEF \
sl_zigbee_event_t emberAfIdentifyClusterServerTickCallbackEvent1; \
sl_zigbee_event_t emberAfOnOffClusterServerTickCallbackEvent1; \
sl_zigbee_event_t emberAfOtaBootloadClusterClientTickCallbackEvent1; \
sl_zigbee_event_t emberAfIdentifyClusterServerTickCallbackEvent2; \
sl_zigbee_event_t emberAfOnOffClusterServerTickCallbackEvent2; \
sl_zigbee_event_t emberAfOtaBootloadClusterClientTickCallbackEvent2; \
sl_zigbee_event_t emberAfIdentifyClusterServerTickCallbackEvent3; \
sl_zigbee_event_t emberAfOnOffClusterServerTickCallbackEvent3; \
sl_zigbee_event_t emberAfOtaBootloadClusterClientTickCallbackEvent3; \
sl_zigbee_event_t emberAfIdentifyClusterServerTickCallbackEvent4; \
sl_zigbee_event_t emberAfOnOffClusterServerTickCallbackEvent4; \
sl_zigbee_event_t emberAfOtaBootloadClusterClientTickCallbackEvent4; \


#define EMBER_AF_GENERATED_UC_EVENTS_INIT \
extern void emberAfIdentifyClusterServerTickCallback(uint8_t endpoint); \
extern void emberAfOnOffClusterServerTickCallback(uint8_t endpoint); \
extern void emberAfOtaBootloadClusterClientTickCallback(uint8_t endpoint); \
sl_zigbee_endpoint_event_init(&emberAfIdentifyClusterServerTickCallbackEvent1, (void *)emberAfIdentifyClusterServerTickCallback, 1); \
sl_zigbee_endpoint_event_init(&emberAfOnOffClusterServerTickCallbackEvent1, (void *)emberAfOnOffClusterServerTickCallback, 1); \
sl_zigbee_endpoint_event_init(&emberAfOtaBootloadClusterClientTickCallbackEvent1, (void *)emberAfOtaBootloadClusterClientTickCallback, 1); \
sl_zigbee_endpoint_event_init(&emberAfIdentifyClusterServerTickCallbackEvent2, (void *)emberAfIdentifyClusterServerTickCallback, 2); \
sl_zigbee_endpoint_event_init(&emberAfOnOffClusterServerTickCallbackEvent2, (void *)emberAfOnOffClusterServerTickCallback, 2); \
sl_zigbee_endpoint_event_init(&emberAfOtaBootloadClusterClientTickCallbackEvent2, (void *)emberAfOtaBootloadClusterClientTickCallback, 2); \
sl_zigbee_endpoint_event_init(&emberAfIdentifyClusterServerTickCallbackEvent3, (void *)emberAfIdentifyClusterServerTickCallback, 3); \
sl_zigbee_endpoint_event_init(&emberAfOnOffClusterServerTickCallbackEvent3, (void *)emberAfOnOffClusterServerTickCallback, 3); \
sl_zigbee_endpoint_event_init(&emberAfOtaBootloadClusterClientTickCallbackEvent3, (void *)emberAfOtaBootloadClusterClientTickCallback, 3); \
sl_zigbee_endpoint_event_init(&emberAfIdentifyClusterServerTickCallbackEvent4, (void *)emberAfIdentifyClusterServerTickCallback, 4); \
sl_zigbee_endpoint_event_init(&emberAfOnOffClusterServerTickCallbackEvent4, (void *)emberAfOnOffClusterServerTickCallback, 4); \
sl_zigbee_endpoint_event_init(&emberAfOtaBootloadClusterClientTickCallbackEvent4, (void *)emberAfOtaBootloadClusterClientTickCallback, 4); \

// sl_zigbee_event_context_t structs used to populate the sli_zigbee_app_event_context table
#define EMBER_AF_GENERATED_UC_EVENT_CONTEXT \
{ 1, 0x3, false, EMBER_AF_LONG_POLL, EMBER_AF_OK_TO_SLEEP, &emberAfIdentifyClusterServerTickCallbackEvent1 },     \
{ 1, 0x6, false, EMBER_AF_LONG_POLL, EMBER_AF_OK_TO_SLEEP, &emberAfOnOffClusterServerTickCallbackEvent1 },     \
{ 1, 0x19, true, EMBER_AF_LONG_POLL, EMBER_AF_OK_TO_SLEEP, &emberAfOtaBootloadClusterClientTickCallbackEvent1 },     \
{ 2, 0x3, false, EMBER_AF_LONG_POLL, EMBER_AF_OK_TO_SLEEP, &emberAfIdentifyClusterServerTickCallbackEvent2 },     \
{ 2, 0x6, false, EMBER_AF_LONG_POLL, EMBER_AF_OK_TO_SLEEP, &emberAfOnOffClusterServerTickCallbackEvent2 },     \
{ 2, 0x19, true, EMBER_AF_LONG_POLL, EMBER_AF_OK_TO_SLEEP, &emberAfOtaBootloadClusterClientTickCallbackEvent2 },     \
{ 3, 0x3, false, EMBER_AF_LONG_POLL, EMBER_AF_OK_TO_SLEEP, &emberAfIdentifyClusterServerTickCallbackEvent3 },     \
{ 3, 0x6, false, EMBER_AF_LONG_POLL, EMBER_AF_OK_TO_SLEEP, &emberAfOnOffClusterServerTickCallbackEvent3 },     \
{ 3, 0x19, true, EMBER_AF_LONG_POLL, EMBER_AF_OK_TO_SLEEP, &emberAfOtaBootloadClusterClientTickCallbackEvent3 },     \
{ 4, 0x3, false, EMBER_AF_LONG_POLL, EMBER_AF_OK_TO_SLEEP, &emberAfIdentifyClusterServerTickCallbackEvent4 },     \
{ 4, 0x6, false, EMBER_AF_LONG_POLL, EMBER_AF_OK_TO_SLEEP, &emberAfOnOffClusterServerTickCallbackEvent4 },     \
{ 4, 0x19, true, EMBER_AF_LONG_POLL, EMBER_AF_OK_TO_SLEEP, &emberAfOtaBootloadClusterClientTickCallbackEvent4 },     \

#define EMBER_AF_GENERATED_UC_EVENT_CONTEXT_COUNT 12

#endif // SL_CATALOG_ZIGBEE_ZCL_FRAMEWORK_CORE_PRESENT
