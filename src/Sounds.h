/*
 * Sounds.h
 */

#ifndef __SOUNDS_H__
#define __SOUNDS_H__

#include <Hs.h>
#include <TonesLibTypes.h>
#include <TonesLib.h>
#include <HsExtCommon.h>

#include "Sections.h"
#include "Global.h"
#include "Prefs.h"

// Prototypes
extern void 		PlayAlertSound( void );
extern Err 			LoadTonesList( ListType* pList ) EXTRA_SECTION_ONE;
extern Err 			GetToneID( UInt16 idx, ToneIdentifier* pToneID );
extern void 		PlayTone( ToneIdentifier ToneID, UInt16 volume );
extern void 		StopTone( UInt16 TonesLibRefNum );
extern UInt16		GetSoundVolume( void );

#endif /* __SOUNDS_H__ */
 
/*
 * Sounds.h
 */