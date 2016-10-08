/*
 * Serial.h
 */
 
#ifndef __SERIAL_H__
#define __SERIAL_H__

#include <Hs.h>
#include <HsPhone.h>
#include <HsCreators.h>
#include <palmOneResources.h>
#include <PalmTypes.h>
#include <SerialMgr.h>
#include <NetMasterLibrary.h>
#include <NetPrefLibrary.h>
#include <StringMgr.h>

#include "Utils.h"

#define AT_CMD_QUERY_STRING						"AT+CREG=2\r"
#define AT_CMD_RESPONSE_SEARCH_STRING			"+CREG: 1,\""
#define AT_CMD_RESPONSE_TIMEOUT_SECONDS			5

// Prototypes
extern Err 					getSerialLACCID( UInt32* pLAC, UInt32* pCellID );
extern Err 					netClose();
 
#endif /* __SERIAL_H__ */

/*
 * Serial.h
 */