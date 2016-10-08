/*
 * GetGID.h
 */

#ifndef __GETGID_H__
#define __GETGID_H__

#include <Hs.h>
#include <HsPhone.h>
#include <HsPhoneGSM.h>

#include "GID_DB.h"
#include "Prefs.h"
#include "AddForm.h"
#include "AttnAlerts.h"
#include "ProfileSwitch.h"

#define CARRIER_ID_BUFFER_SIZE					8

// Layout
//  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20
// <---MCC--->  : <---MNC--->  : <---LAC--->  : <---CID--->  NULL 		

// Prototypes
extern Err 			getLACCID( UInt32* pLAC, UInt32* pCellID );
extern Err 			getMNCMCC( UInt32* pMCC, UInt32* pMNC );
extern void 		MakeGIDString( GID_t* pGID, Char* sGID );

#endif /* __GETGID_H__ */ 
 
/*
 * GetGID.h
 */