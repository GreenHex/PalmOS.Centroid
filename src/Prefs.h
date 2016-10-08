/*
 * Prefs.h
 */
 
#ifndef __PREFS_H__
#define __PREFS_H__

#include <Hs.h>
#include <Preferences.h>
#include <TonesLibTypes.h>
#include <TonesLib.h>

#include "AppResources.h"
#include "GID_DB.h"
#include "FormUtils.h"
#include "ProfilesDB.h"
#include "Sounds.h"

#define appPrefID						0x00
#define appPrefVersionNum				0x06
//
#define lastLocPrefID					appPrefID + 0
#define lastLocPrefVersionNum			0x02
//
#define lastProfilePrefID				appPrefID + 1
#define lastProfilePrefVersionNum		0x01
//
#define appVolumePrefID					appPrefID + 2
#define appVolumePrefVersionNum			0x01
//
#define appWakePrefID					appPrefID + 3
#define appWakePrefVersionNum			0x01

typedef enum {
	onEntry								= 0,
	onExit 								= 1
} attentionMode_e;

typedef enum {
	phnMute = 0,
	phnNormal = 1
} MuteAction_e;

typedef struct {
	Boolean						bEnabled;
	Boolean						bAddLocs;
	Boolean						bAutoAddLocs;
	Boolean						bIgnoreCellID;
	UInt16						GIDValidityIdx;
	UInt16						toneIdx;
	UInt16						toneVolume; // not used
	Boolean						bGIDDBRebuildOnExit;
	UInt16						numMsgAttemptsIdx;
	Char						DefaultMsg[STRLEN_MSG];
	Boolean						bEnableLogging;
} appPrefs_t;

#define STR_DEFAULT_MSG			"^2ing ^3 @ ^0 ^1"

#define APP_VOL_PREFS_MUTED						false
#define APP_VOL_PREFS_PHN_ALERT_VOLUME			toneVolume4
#define APP_VOL_PREFS_SMS_ALERT_VOLUME			toneVolume4
#define APP_VOL_PREFS_CAL_ALERT_VOLUME			toneVolume4
#define APP_VOL_PREFS_SYSTEM_VOLUME				sndMaxAmp / 4
#define APP_VOL_PREFS_ALARM_VOLUME				sndMaxAmp / 4
#define APP_VOL_PREFS_GAME_VOLUME				sndMaxAmp / 4

typedef struct {
	GID_t						GID;
	UInt32						uniqueID;
	UInt32						regionID;
	UInt32						profileID;
	UInt32						TimeAtLoc;
} lastLocation_t;

typedef struct {
	UInt16						phnAlertVolume;
	UInt16						smsAlertVolume;
	UInt16						calAlertVolume;
	UInt16						sndLevelSystem;
	UInt16						sndLevelAlarm;
	UInt16						sndLevelGame;
	Boolean						bMuted;
} appVolumePrefs_t;

typedef struct {
	Boolean						bSleep;
} appWakePrefs_t;

// Prototypes
extern Boolean 			PrefsFormHandleEvent( EventType* pEvent );
extern void 			WriteAppPrefs( appPrefs_t* pPrefs );
extern void 			ReadAppPrefs( appPrefs_t* pPrefs );
extern void 			WriteLastLocationPrefs( lastLocation_t* pLastLoc );
extern void 			ReadLastLocationPrefs( lastLocation_t* pLastLoc );
extern void 			WriteLastProfilePrefs( attachedProfile_t* pLastProfile );
extern void 			ReadLastProfilePrefs( attachedProfile_t* pLastProfile );
extern void 			WriteVolumePrefs( appVolumePrefs_t* pAppVolPrefs );
extern void 			ReadVolumePrefs( appVolumePrefs_t* pAppVolPrefs );
extern void 			WriteWakePrefs( Boolean bSleep );
extern Boolean 			ReadWakePrefs( void );

#endif /* __PREFS_H__ */
 
/*
 * Prefs.h
 */