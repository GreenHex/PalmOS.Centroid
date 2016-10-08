/*
 * SignalLvl.h
 */

#ifndef __SIGNALLVL_H__
#define __SIGNALLVL_H__
 
#include <Hs.h>
#include <HsPhone.h>

/*
From pdn documentation:

On GSM devices, the Received Signal Strength Indicator (RSSI) 
returned by the API correspond to the following values in dBm:

RSSI: Value in dBm
0: -113 or less 
1: -111 
2-30: -109 to -53
31: -51 or greater
99: not known or not detectable

*/

// Prototypes
extern Err 				GetSignalInfo( UInt16* qualityP, Char* strSignalLvl );

#endif /* __SIGNALLVL_H__ */

/*
 * SignalLvl.h
 */