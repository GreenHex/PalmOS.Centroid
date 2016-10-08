/*
 * Prefs.c
 */

#include "Prefs.h"

// Prototypes
static void 			InitializePrefsForm( FormType* pForm, appPrefs_t* pPrefs );
static void 			CopyPrefsFromForm( FormType* pForm, appPrefs_t* pPrefs );
static void 			InitializeAppPrefs( appPrefs_t* pPrefs );
static void 			InitializeVolumePrefs( appVolumePrefs_t* pAppVolPrefs );

/*
 * PrefsFormHandleEvent()
 */
Boolean PrefsFormHandleEvent( EventType* pEvent )
{
	Boolean 				handled = false;
	FormType* 				pForm = FrmGetActiveForm();
	appPrefs_t				prefs;
	ToneIdentifier			toneID = { 0, 0 };
		
	switch ( pEvent->eType )
	{		
		case frmOpenEvent:
		
			ReadAppPrefs( &prefs );
		
			InitializePrefsForm( pForm, &prefs );
			
			FrmDrawForm( pForm );
			
			handled = true;
						
			break;
		
		case popSelectEvent:
		
			if ( pEvent->data.popSelect.controlID == PREFS_TONE_IDX_POP )
			{
				if ( !GetToneID( pEvent->data.popSelect.selection, &toneID ) )
				{
					CtlSetLabel( pEvent->data.popSelect.controlP, 
									LstGetSelectionText( pEvent->data.popSelect.listP, pEvent->data.popSelect.selection ) );
					
					PlayTone( toneID, toneVolume3 );
				
					handled = true;
				}
			}
			
			break;
			
		case ctlSelectEvent:
		
			switch (pEvent->data.ctlSelect.controlID)
			{
				case PREFS_OK_BTN:
		
					ReadAppPrefs( &prefs );
					
					CopyPrefsFromForm( pForm, &prefs );
								
					WriteAppPrefs( &prefs );
					
					FrmReturnToForm( 0 );
					
					handled = true;

					break;

				case PREFS_CANCEL_BTN:
				
					FrmReturnToForm( 0 );
			
					handled = true;
					
					break;
						
				default:
					break;
			}
		
			break;
			
		default:
								
			break;
	}
	
	return ( handled );
	
} // PrefsFormHandleEvent()

/*
 * InitializePrefsForm()
 */
static void InitializePrefsForm( FormType* pForm, appPrefs_t* pPrefs )
{
	ListType*		pList = FrmGetObjectPtr( pForm, FrmGetObjectIndex( pForm, PREFS_TONE_IDX_LST ) );
		
	if ( ( !pForm ) || ( !pPrefs ) ) return;

	SetCtlBoolValue( pForm, PREFS_ADD_NEW_LOCS_CBX, pPrefs->bAddLocs );
	SetCtlBoolValue( pForm, PREFS_AUTO_ADD_NEW_CBX, pPrefs->bAutoAddLocs );
	SetCtlBoolValue( pForm, PREFS_IGNORE_CELLID_CBX, pPrefs->bIgnoreCellID );
	SetCtlBoolValue( pForm, PREFS_REBUILD_DB_CBX, pPrefs->bGIDDBRebuildOnExit );
	
	SetPopLabelFromList3( pForm, PREFS_GID_VALIDITY_POP, PREFS_GID_VALIDITY_LST, pPrefs->GIDValidityIdx );
	
	if ( pList )
	{
		LoadTonesList( pList );
		SetPopLabelFromList3( pForm, PREFS_TONE_IDX_POP, PREFS_TONE_IDX_LST, pPrefs->toneIdx );
	}
	
	SetPopLabelFromList3( pForm, PREFS_NUM_MSG_ATTEMPTS_POP, PREFS_NUM_MSG_ATTEMPTS_LST, pPrefs->numMsgAttemptsIdx );
	
	SetFieldStr( pForm, PREFS_DEFAULT_MSG_FLD, pPrefs->DefaultMsg );
	
	return;
	
} // InitializePrefsForm()

/*
 * CopyPrefsFromForm()
 */
static void CopyPrefsFromForm( FormType* pForm, appPrefs_t* pPrefs )
{
	ListType*		pList = FrmGetObjectPtr( pForm, FrmGetObjectIndex( pForm, PREFS_GID_VALIDITY_LST ) );
		
	if ( ( !pForm ) || ( !pPrefs ) ) return;

	pPrefs->bAddLocs = GetCtlBoolValue( pForm, PREFS_ADD_NEW_LOCS_CBX );
	pPrefs->bAutoAddLocs = GetCtlBoolValue( pForm, PREFS_AUTO_ADD_NEW_CBX );
	pPrefs->bIgnoreCellID = GetCtlBoolValue( pForm, PREFS_IGNORE_CELLID_CBX );
	pPrefs->bGIDDBRebuildOnExit = GetCtlBoolValue( pForm, PREFS_REBUILD_DB_CBX );
	
	pList = FrmGetObjectPtr( pForm, FrmGetObjectIndex( pForm, PREFS_GID_VALIDITY_LST ) );
	if ( pList )
	{
		pPrefs->GIDValidityIdx = LstGetSelection( pList );
	}
	
	pList = FrmGetObjectPtr( pForm, FrmGetObjectIndex( pForm, PREFS_TONE_IDX_LST ) );
	if ( pList )
	{
		pPrefs->toneIdx = LstGetSelection( pList );
	}
	
	pList = FrmGetObjectPtr( pForm, FrmGetObjectIndex( pForm, PREFS_NUM_MSG_ATTEMPTS_LST ) );
	if ( pList )
	{
		pPrefs->numMsgAttemptsIdx = LstGetSelection( pList );
	}
	
	GetFieldStr( pForm, PREFS_DEFAULT_MSG_FLD, pPrefs->DefaultMsg, STRLEN_MSG );

	return;	

} // CopyPrefsFromForm()

/*
 * WriteAppPrefs()
 */
void WriteAppPrefs( appPrefs_t* pPrefs )
{
	if ( !pPrefs ) return;
	
	PrefSetAppPreferences( appFileCreator, appPrefID, appPrefVersionNum, pPrefs, sizeof( appPrefs_t ), true );
	
	return;
	
} // WriteAppPrefs()

/*
 * ReadAppPrefs()
 */
void ReadAppPrefs( appPrefs_t* pPrefs )
{
	UInt16 			prefSize = sizeof( appPrefs_t );
	UInt16			prefsVersion = 0;

	if ( !pPrefs ) return;
		
	prefsVersion = PrefGetAppPreferences( appFileCreator, appPrefID, pPrefs, &prefSize, true );
	
	if ( prefsVersion != appPrefVersionNum ) // remove old prefs...
	{
		PrefSetAppPreferences( appFileCreator, appPrefID, prefsVersion, NULL, 0, true );				
	
		InitializeAppPrefs( pPrefs );
		
		WriteAppPrefs( pPrefs );
	}
	
	return;
	
} // ReadAppPrefs()

/*
 * InitializeAppPrefs()
 */
static void InitializeAppPrefs( appPrefs_t* pPrefs )
{
	if ( !pPrefs ) return;
	
	pPrefs->bEnabled = true;
	pPrefs->bAddLocs = true;
	pPrefs->bAutoAddLocs = true;
	pPrefs->bIgnoreCellID = false;
	pPrefs->GIDValidityIdx = 0;
	pPrefs->toneIdx = 0;
	pPrefs->toneVolume = toneVolume3;
	pPrefs->bGIDDBRebuildOnExit = false;
	pPrefs->numMsgAttemptsIdx = 2;
	StrCopy( pPrefs->DefaultMsg, STR_DEFAULT_MSG );
	pPrefs->bEnableLogging = true;
	
	return;
	
} // InitializeAppPrefs

/*
 * WriteLastLocationPrefs()
 */
void WriteLastLocationPrefs( lastLocation_t* pLastLoc )
{
	if ( !pLastLoc ) return;
	
	PrefSetAppPreferences( appFileCreator, lastLocPrefID, lastLocPrefVersionNum, pLastLoc, sizeof( lastLocation_t ), false );
	
	return;
	
} // WriteLastLocationPrefs()

/*
 * ReadLastLocationPrefs()
 */
void ReadLastLocationPrefs( lastLocation_t* pLastLoc )
{
	UInt16 			prefSize = sizeof( lastLocation_t );
	UInt16			prefsVersion = 0;
	
	if ( !pLastLoc ) return;

	prefsVersion = PrefGetAppPreferences( appFileCreator, lastLocPrefID, pLastLoc, &prefSize, false );
	
	if ( prefsVersion != lastLocPrefVersionNum ) // remove old prefs...
	{
		PrefSetAppPreferences( appFileCreator, lastLocPrefID, prefsVersion, NULL, 0, false );		

		MemSet( pLastLoc, sizeof( lastLocation_t ), 0 );
			
		WriteLastLocationPrefs( pLastLoc );
	
	}
	
	return;
	
} // ReadLastLocationPrefs()

/*
 * WriteLastProfilePrefs()
 */
void WriteLastProfilePrefs( attachedProfile_t* pLastProfile )
{
	if ( !pLastProfile ) return;
	
	PrefSetAppPreferences( appFileCreator, lastProfilePrefID, lastProfilePrefVersionNum, pLastProfile, sizeof( locations_t ), false );
	
	return;
	
} // WriteLastProfilePrefs()

/*
 * ReadLastProfilePrefs()
 */
void ReadLastProfilePrefs( attachedProfile_t* pLastProfile )
{
	UInt16 			prefSize = sizeof( attachedProfile_t );
	UInt16			prefsVersion = 0;
	
	if ( !pLastProfile ) return;

	prefsVersion = PrefGetAppPreferences( appFileCreator, lastProfilePrefID, pLastProfile, &prefSize, false );
	
	if ( prefsVersion != lastProfilePrefVersionNum ) // remove old prefs...
	{
		PrefSetAppPreferences( appFileCreator, lastProfilePrefID, prefsVersion, NULL, 0, false );		

		MemSet( pLastProfile, sizeof( attachedProfile_t ), 0 );
		
		pLastProfile->profileIdx = dmMaxRecordIndex;
			
		WriteLastProfilePrefs( pLastProfile );
	}
	
	return;
	
} // ReadLastProfilePrefs()

/*
 * WriteVolumePrefs()
 */
void WriteVolumePrefs( appVolumePrefs_t* pAppVolPrefs )
{
	PrefSetAppPreferences( appFileCreator, appVolumePrefID, appVolumePrefVersionNum, pAppVolPrefs, sizeof( appVolumePrefs_t ), false );

	return;
	
} // WriteAppVolumePrefs()

/*
 * ReadVolumePrefs()
 */
void ReadVolumePrefs( appVolumePrefs_t* pAppVolPrefs )
{	
	UInt16 		prefSize = sizeof( appVolumePrefs_t );
	UInt16		prefsVersion = 0;

	if ( !pAppVolPrefs ) return;
		
	prefsVersion = PrefGetAppPreferences( appFileCreator, appVolumePrefID, pAppVolPrefs, &prefSize, false );
	
	if ( prefsVersion != appVolumePrefVersionNum ) // remove old prefs...
	{	
		PrefSetAppPreferences( appFileCreator, appVolumePrefID, prefsVersion, NULL, 0, false );
		
		InitializeVolumePrefs( pAppVolPrefs );
		
		WriteVolumePrefs( pAppVolPrefs );
	}
	
	return;
	
} // ReadVolumePrefs()

/*
 * InitializeVolumePrefs()
 */
static void InitializeVolumePrefs( appVolumePrefs_t* pAppVolPrefs )
{
	if ( !pAppVolPrefs ) return;
	
	pAppVolPrefs->bMuted = APP_VOL_PREFS_MUTED;
	pAppVolPrefs->phnAlertVolume = APP_VOL_PREFS_PHN_ALERT_VOLUME;
	pAppVolPrefs->smsAlertVolume = APP_VOL_PREFS_SMS_ALERT_VOLUME;
	pAppVolPrefs->calAlertVolume = APP_VOL_PREFS_CAL_ALERT_VOLUME;
	pAppVolPrefs->sndLevelSystem = APP_VOL_PREFS_SYSTEM_VOLUME;
	pAppVolPrefs->sndLevelAlarm = APP_VOL_PREFS_ALARM_VOLUME;
	pAppVolPrefs->sndLevelGame = APP_VOL_PREFS_GAME_VOLUME;
	
	return;
	
} // InitializeVolumePrefs()

/*
 * WriteWakePrefs()
 */
void WriteWakePrefs( Boolean bSleep )
{
	appWakePrefs_t	 		wakePref;
	
	wakePref.bSleep = bSleep;
	
	PrefSetAppPreferences( appFileCreator, appWakePrefID, appWakePrefVersionNum, &wakePref, sizeof( appWakePrefs_t ), false );

	return;
	
} // WriteWakePrefs()

/*
 * ReadWakePrefs()
 */
Boolean ReadWakePrefs( void )
{	
	Boolean					retVal = false;
	appWakePrefs_t	 		wakePref;
	UInt16 					prefSize = sizeof( appWakePrefs_t );
	UInt16					prefsVersion = 0;

	prefsVersion = PrefGetAppPreferences( appFileCreator, appWakePrefID, &wakePref, &prefSize, false );
	
	if ( prefsVersion != appWakePrefVersionNum ) // remove old prefs...
	{	
		PrefSetAppPreferences( appFileCreator, appWakePrefID, prefsVersion, NULL, 0, false );
		
		WriteWakePrefs( false );
	}
	else
	{
		retVal = wakePref.bSleep;
	}
	
	return ( retVal );
	
} // ReadWakePrefs()

/*
 * Prefs.c
 */