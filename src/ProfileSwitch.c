/*
 * ProfileSwitch.c
 */

#include "ProfileSwitch.h"
 
// Prototypes
static void 			SetProfileCancelAlarm( profiles_t* pProfile );
static void 			SetTheme( profiles_t* pProfile, ProgressPtr pProgress );
static void 			SetTimeZone( profiles_t* pProfile, ProgressPtr pProgress );
static Err				SetCallForwarding( profiles_t* pProfile, ProgressPtr pProgress );

/*
 * DoProfileSwitch()
 */
void DoProfileSwitch( lastLocation_t* pLastLoc )
{
	locations_t					loc;
	regions_t					region;
	attachedProfile_t 			lastProfile;
	profiles_t					profile;
	UInt16						idx = 0;
	Boolean						bFound = false;
	
	if ( pLastLoc->regionID )
	{
		region.uniqueID = pLastLoc->regionID;
		
		if ( !RegionsFindRecord( &region, &idx, true, &bFound, (DmComparF *) &Regions_CompareUniqueIDF ) 
				&& ( bFound ) )
		{
			profile.uniqueID = region.AttachedProfile.profileID;
		}
	}
	else
	{
		loc.uniqueID = pLastLoc->uniqueID;
		
		if ( ( !GIDFindRecord( &loc, &idx, true, &bFound, (DmComparF *) &GID_CompareUniqueIDF ) )
				&& ( bFound ) )
		{
			profile.uniqueID = loc.AttachedProfile.profileID;			
		}
	}
	
	if ( ( bFound ) 
			&& ( !ProfilesFindRecord( &profile, &idx, true, &bFound, (DmComparF *) &Profiles_CompareUniqueIDF ) )
			&& ( bFound ) ) /* interesting "bFound" used three times */
	{
		ReadLastProfilePrefs( &lastProfile );
		
		if ( lastProfile.profileID != profile.uniqueID )
		{		
			lastProfile.profileID = profile.uniqueID;
			lastProfile.profileIdx = idx;
			lastProfile.profileStartTime = TimGetSeconds();
			lastProfile.profileValidity = profile.validitySecs;
			StrCopy( lastProfile.profileName, profile.name );

			WriteLastProfilePrefs( &lastProfile );
			
			ActivateProfile( &profile );		
		}	
		
		SetProfileCancelAlarm( &profile );
	}
		 
	return;	
	
} // DoProfileSwitch()

/*
 * SetProfileCancelAlarm()
 */
static void SetProfileCancelAlarm( profiles_t* pProfile )
{
	UInt16 				cardNo = 0;
   	LocalID 			dbID = 0;
	UInt32				ref = 0;
	UInt32				defaultUniqueID = ProfilesDBGetUniqueID();
			
	if ( !SysCurAppDatabase( &cardNo, &dbID ) )
	{
		AlmGetAlarm( cardNo, dbID, &ref );
		
		if ( ( defaultUniqueID  == -1 ) || ( defaultUniqueID == pProfile->uniqueID ) )
		{
			AlmSetAlarm( cardNo, dbID, 0, 0, false );
		}
		else if ( ref != pProfile->uniqueID )
		{
			if ( pProfile->validitySecs )
			{
				AlmSetAlarm( cardNo, dbID, pProfile->uniqueID, TimGetSeconds() + pProfile->validitySecs, false );
			}
			else
			{
				AlmSetAlarm( cardNo, dbID, 0, 0, false );			
			}
		}
	}
		
	return;
	
} // SetProfileCancelAlarm()

/*
 * DoAlarmProfileCancel()
 */
void DoAlarmProfileCancel( UInt32 alarmProfileID )
{
	attachedProfile_t 			lastProfile;
	profiles_t					profile;
	Boolean						bFound = false;
	UInt16						defaultIdx = 0;
	UInt32						defaultUniqueID = ProfilesDBGetUniqueID();
	
	ReadLastProfilePrefs( &lastProfile );
			
	if ( ( defaultUniqueID != -1 ) )
	{
		profile.uniqueID = defaultUniqueID;
		
		if ( ( ProfilesFindRecord( &profile, &defaultIdx, true, &bFound, (DmComparF *) &Profiles_CompareUniqueIDF ) == errNone )
				&& ( bFound ) 
				&& ( lastProfile.profileID != profile.uniqueID ) )
		{
			ActivateProfile( &profile );
						
			lastProfile.profileID = profile.uniqueID;
			lastProfile.profileIdx = 0;
			lastProfile.profileStartTime = TimGetSeconds();
			lastProfile.profileValidity = profile.validitySecs;
			StrCopy( lastProfile.profileName, profile.name );
			
			WriteLastProfilePrefs( &lastProfile );			
		}
	}
	
	return;
	
} // DoAlarmProfileCancel()

/*
 * ActivateProfile()
 */
void ActivateProfile( profiles_t* pProfile )
{
	ProgressPtr				pProgress = NULL;
	appVolumePrefs_t		appVolPrefs;
			
	pProgress = PrgStartDialog( APP_NAME, ProfilesProgressCallback, pProfile->name );
	
	SetTheme( pProfile, pProgress );

	SetCallForwarding( pProfile, pProgress );	
	
	SetTimeZone( pProfile, pProgress );
	
	RestoreSoundPrefs();
	
	ReadVolumePrefs( &appVolPrefs );
	SetPhnSoundPrefs( (mute_t *) &(pProfile->Settings.Mute), &appVolPrefs, phnMute, pProgress );
	SetMuteState( (mute_t *) &(pProfile->Settings.Mute), &appVolPrefs, phnMute, pProgress );
	WriteVolumePrefs( &appVolPrefs );
		
	if ( pProgress )
	{
		PrgUpdateDialog( pProgress, 0, 0, NULL, true );
		PrgStopDialog( pProgress, true );
	}
	
	return;
	
} // ActivateProfile()
 
/* 
 * SetTimeZone()
 */
static void SetTimeZone( profiles_t* pProfile, ProgressPtr pProgress )
{
	if ( ( pProfile ) && ( pProfile->Settings.bTimeZone ) )
	{
		
		UInt32 		timeUTC = TimTimeZoneToUTC( TimGetSeconds(), PrefGetPreference( prefTimeZone ),
													PrefGetPreference( prefDaylightSavingAdjustment ) );
		
		UInt32		timeNewTZ = TimUTCToTimeZone( timeUTC, pProfile->Settings.TimeZone.uTC,
													pProfile->Settings.TimeZone.dSTAdjustmentInMinutes );
		
		if ( pProgress ) PrgUpdateDialog( pProgress, 0, 1, "TimeZone...", true );
		
		PrefSetPreference( prefDaylightSavingAdjustment, pProfile->Settings.TimeZone.dSTAdjustmentInMinutes );
		PrefSetPreference( prefTimeZone, pProfile->Settings.TimeZone.uTC );
		PrefSetPreference( prefTimeZoneCountry, pProfile->Settings.TimeZone.country );
		
		TimSetSeconds( timeNewTZ ); 
	}
	
	return;
	
} // SetTimeZone()

/*
 * SetTheme()
 */
static void SetTheme( profiles_t* pProfile, ProgressPtr pProgress )
{
	if ( ( pProfile ) && ( pProfile->Settings.bTheme ) )
	{
		if ( pProgress ) PrgUpdateDialog( pProgress, 0, 1, "Theme...", true );
	
		PrefSetPreference( prefColorThemeID, pProfile->Settings.Theme.themeID );
	}
	
	return;		
	
} // SetTheme()

/*
 * SetCallForwarding()
 */
static Err SetCallForwarding( profiles_t* pProfile, ProgressPtr pProgress  )
{
	Err					error = errNone;
	UInt16 				phnLibRefNum = sysInvalidRefNum;
	MemHandle			phnNumH = NULL;
	
	if ( ( pProfile ) && ( pProfile->Settings.bFwdCalls ) && ( IsPhoneReady() ) )
	{
		if ( pProgress ) PrgUpdateDialog( pProgress, 0, 1, "Call-forwarding...", true );
		
		if ( ( error = HsGetPhoneLibrary( &phnLibRefNum ) ) )
			return ( error );
					
		if ( ( error = PhnLibOpen( phnLibRefNum ) ) )
			return ( error );
		
		if ( StrLen ( pProfile->Settings.strFwdCallsNum ) )
		{		
			phnNumH = PhnLibNewAddress( phnLibRefNum, pProfile->Settings.strFwdCallsNum, phnLibUnknownID );
			
			if ( phnNumH )
			{
				error = PhnLibSetForwarding( phnLibRefNum, phnForwardUnconditional, phnForwardModeEnable, phnNumH );							
			}
		}
		else
		{
			phnNumH = PhnLibNewAddress( phnLibRefNum, "", phnLibUnknownID );
			
			if ( phnNumH )
			{
				error = PhnLibSetForwarding( phnLibRefNum, phnForwardUnconditional, phnForwardModeDisable, phnNumH );
			}
		}
		
		if ( phnNumH ) MemHandleFree( phnNumH );
			
		PhnLibClose( phnLibRefNum );
	}
	
	if ( ( pProgress ) && ( error ) )
	{
		PrgUpdateDialog( pProgress, 0, 1, "Call-fwd... Failed!", true );
	}
	
	return ( error );		
	
} // SetCallForwarding()

/*
 * SetPhnSoundPrefs()
 */
void SetPhnSoundPrefs( mute_t* pMute, appVolumePrefs_t* pVolumePrefs, MuteAction_e state, ProgressPtr pProgress )
{
	Err					error = errNone;
	UInt16				tonesLibRefNum = sysInvalidRefNum;
	SoundPreference		sndPref;
	
	if ( ( !pMute ) || ( !pVolumePrefs ) ) return;
	
	if ( ( pMute->bMutePhone ) || ( pMute->bMuteMessging ) || ( pMute->bMuteCalendar ) )
	{
		if ( pProgress ) PrgUpdateDialog( pProgress, 0, 1, "Ringer volume...", true );
	}
	else
	{
		return;	
	}
		
	error = SysLibFind( tonesLibName, &tonesLibRefNum );

	if ( error ) SysLibLoad( tonesLibType, tonesLibCreator, &tonesLibRefNum );

	error = TonesLibOpen( tonesLibRefNum );
					
	if ( !error )
	{	
		if ( pMute->bMutePhone )
		{
			TonesLibGetSoundPrefs( tonesLibRefNum, soundPrefTypePhone, &sndPref );
	
			if ( state == phnMute )
			{
				if ( sndPref.soundOnVolume != toneVolumeOff )
				{
					pVolumePrefs->phnAlertVolume = sndPref.soundOnVolume; // save previous value...
				}
				
				sndPref.soundOnVolume = toneVolumeOff;
				sndPref.soundOnVibrate = pMute->bVibrateON;
				sndPref.soundOffVibrate = pMute->bVibrateON;
			}
			else if ( state == phnNormal )
			{	
				if ( ( sndPref.soundOnVolume == toneVolumeOff ) && ( pVolumePrefs->phnAlertVolume != toneVolumeOff ) )
				{
					sndPref.soundOnVolume = pVolumePrefs->phnAlertVolume;
				}
					
				sndPref.soundOnVibrate = false;
				sndPref.soundOffVibrate = true;	
			}
			
			TonesLibSetSoundPrefs( tonesLibRefNum, soundPrefTypePhone, &sndPref );
		}
		
		if ( pMute->bMuteMessging )
		{
			TonesLibGetSoundPrefs( tonesLibRefNum, soundPrefTypeSMS, &sndPref );
	
			if ( state == phnMute )
			{
				if ( sndPref.soundOnVolume != toneVolumeOff )
				{
					pVolumePrefs->smsAlertVolume = sndPref.soundOnVolume; // save previous value...
				}
				
				sndPref.soundOnVolume = toneVolumeOff;
				sndPref.soundOnVibrate = pMute->bVibrateON;
				sndPref.soundOffVibrate = pMute->bVibrateON;
				
			}
			else if ( state == phnNormal )
			{	
				if ( ( sndPref.soundOnVolume == toneVolumeOff ) && ( pVolumePrefs->smsAlertVolume != toneVolumeOff ) )
				{
					sndPref.soundOnVolume = pVolumePrefs->smsAlertVolume;
				}
					
				sndPref.soundOnVibrate = false;
				sndPref.soundOffVibrate = true;	
			}
			
			TonesLibSetSoundPrefs( tonesLibRefNum, soundPrefTypeSMS, &sndPref );
		}
		
		if ( pMute->bMuteCalendar )
		{
			TonesLibGetSoundPrefs( tonesLibRefNum, soundPrefTypeCalendar, &sndPref );
	
			if ( state == phnMute )
			{
				if ( sndPref.soundOnVolume != toneVolumeOff )
				{
					pVolumePrefs->calAlertVolume = sndPref.soundOnVolume; // save previous value...
				}
				
				sndPref.soundOnVolume = toneVolumeOff;
				sndPref.soundOnVibrate = pMute->bVibrateON;
				sndPref.soundOffVibrate = pMute->bVibrateON;
				
			}
			else if ( state == phnNormal )
			{	
				if ( ( sndPref.soundOnVolume == toneVolumeOff ) && ( pVolumePrefs->calAlertVolume != toneVolumeOff ) )
				{
					sndPref.soundOnVolume = pVolumePrefs->calAlertVolume;
				}
					
				sndPref.soundOnVibrate = false;
				sndPref.soundOffVibrate = true;	
			}
			
			TonesLibSetSoundPrefs( tonesLibRefNum, soundPrefTypeCalendar, &sndPref );
		}
		
		TonesLibClose( tonesLibRefNum );	
	}
	
	return;
	
} // SetPhnSoundPrefs()

/*
 * SetMuteState()
 */
void SetMuteState( mute_t* pMute, appVolumePrefs_t* pVolumePrefs, MuteAction_e state, ProgressPtr pProgress )
{
	if ( ( !pMute ) || ( !pVolumePrefs ) ) return;
	
	if ( ( pMute->bMuteSystem ) || ( pMute->bMuteAlarm ) || ( pMute->bMuteGame ) )
	{
		if ( pProgress ) PrgUpdateDialog( pProgress, 0, 1, "System volume...", true );
	}
	else
	{
		return;	
	}
		
	if ( ( state == phnMute ) && ( !pVolumePrefs->bMuted ) )
	{	
		if ( pMute->bMuteSystem )
		{
			if (PrefGetPreference( prefSysSoundVolume ) != 0)
			{
				pVolumePrefs->sndLevelSystem = PrefGetPreference( prefSysSoundVolume );
			}
			
			PrefSetPreference( prefSysSoundVolume, 0 );
		}
		
		if ( pMute->bMuteAlarm )
		{	
			if ( PrefGetPreference( prefAlarmSoundVolume ) != 0 )
			{
				pVolumePrefs->sndLevelAlarm = PrefGetPreference( prefAlarmSoundVolume ); 
			}
			
			PrefSetPreference( prefAlarmSoundVolume, 0 );
		}
		
		if ( pMute->bMuteGame )
		{	
			if ( PrefGetPreference( prefGameSoundVolume ) != 0 )
			{
				pVolumePrefs->sndLevelGame = PrefGetPreference( prefGameSoundVolume );
			}
			
			PrefSetPreference( prefGameSoundVolume, 0 );
		}
		
		pVolumePrefs->bMuted = true;
	}
	else if ( ( state == phnNormal ) && ( pVolumePrefs->bMuted ) )
	{
		if ( ( pMute->bMuteSystem ) && ( pVolumePrefs->sndLevelSystem != 0 ) )
		{
			PrefSetPreference(prefSysSoundVolume, pVolumePrefs->sndLevelSystem );
		}
		
		if ( ( pMute->bMuteAlarm ) && ( pVolumePrefs->sndLevelAlarm != 0 ) )
		{
			PrefSetPreference( prefAlarmSoundVolume, pVolumePrefs->sndLevelAlarm ); 
		}
		
		if ( ( pMute->bMuteGame ) && ( pVolumePrefs->sndLevelGame != 0 ) )
		{
			PrefSetPreference( prefGameSoundVolume, pVolumePrefs->sndLevelGame );
		}
		
		pVolumePrefs->bMuted = false;		
	}
	
	return;
	
} // SetMuteState

/*
 * RestoreSoundPrefs( void )
 */
void RestoreSoundPrefs( void )
{
	mute_t					mute;
	appVolumePrefs_t		volumePrefs;
	
	MemSet( &mute, sizeof( mute_t ), 1 );
					
	ReadVolumePrefs( &volumePrefs );
	SetPhnSoundPrefs( &mute, &volumePrefs, phnNormal, NULL );
	SetMuteState( &mute, &volumePrefs, phnNormal, NULL );
	WriteVolumePrefs( &volumePrefs );

	return;
		
} // RestoreSoundPrefs()

/*
 * ProfileSwitch.c
 */