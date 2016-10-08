/*
 * ProfileSwitch.h
 */

#ifndef __PROFILESWITCH_H__
#define __PROFILESWITCH_H__

#include <Hs.h>
#include <HsPhone.h>
#include <TonesLibTypes.h>
#include <TonesLib.h>

#include "Global.h"
#include "AppResources.h"
#include "Prefs.h"
#include "GID_DB.h"
#include "ProfilesDB.h"
#include "RegionsDB.h"
#include "Utils.h"
#include "DispProgress.h"

// Prototypes
extern void 			DoProfileSwitch( lastLocation_t* pLastLoc );
extern void 			ActivateProfile( profiles_t* pProfile );
extern void 			DoAlarmProfileCancel( UInt32 alarmProfileID );
extern void 			SetPhnSoundPrefs( mute_t* pMute, appVolumePrefs_t* pAppVolPrefs, MuteAction_e state, ProgressPtr pProgress );
extern void 			SetMuteState( mute_t* pMute, appVolumePrefs_t* pAppVolPrefs, MuteAction_e state, ProgressPtr pProgress );
extern void 			RestoreSoundPrefs( void );

#endif /* __PROFILESWITCH_H__ */
 
/*
 * ProfileSwitch.h
 */