/*
 * Sounds.c
 */

#include "Sounds.h"
 
// Prototypes
static Err 			GetTonesLib( UInt16* ToneLibRefNumP, Boolean* libWasLoadedP );

/*
 * PlayAlertSound(()
 */
void PlayAlertSound( void )
{
	appPrefs_t					prefs;
	ToneIdentifier				toneID = { 0, 0 };

	ReadAppPrefs( &prefs );
	
	if ( !GetToneID( prefs.toneIdx, &toneID ) )
	{
		prefs.toneVolume = GetSoundVolume(); // used as scratch
		PlayTone( toneID, prefs.toneVolume );
	}
	
	return;

} // PlayAlertSound(()

/*
 * GetTonesLib()
 */
static Err GetTonesLib( UInt16* ToneLibRefNumP, Boolean* libWasLoadedP )
{
	Err					error = errNone;
	
	if ( !ToneLibRefNumP ) return ( tonesLibErrBadParams );
	
	if ( libWasLoadedP ) *libWasLoadedP = false;
	
   	error = SysLibFind( tonesLibName, ToneLibRefNumP );

	if ( error )
	{
		error = SysLibLoad( tonesLibType, tonesLibCreator, ToneLibRefNumP );
		if ( libWasLoadedP ) *libWasLoadedP = true;
	}	
	
	error = TonesLibOpen( *ToneLibRefNumP );
	
	return ( error );
		
} // GetTonesLib()

/*
 * GetToneID()
 */
Err GetToneID( UInt16 idx, ToneIdentifier* pToneID )
{
	Err					error = errNone;
	UInt16  			TonesLibRefNum = sysInvalidRefNum;
	Boolean				libWasLoaded = false;
  	ToneItemPtr	   		ToneListP = NULL;
	UInt16		   		TotalTones = 0;
	
	if ( ( error = GetTonesLib( &TonesLibRefNum, &libWasLoaded ) ) ) return ( error );

	if ( ( error = TonesLibGetToneList( TonesLibRefNum, &ToneListP, &TotalTones ) ) ) return ( error );
	
	if ( TotalTones > 0 )
	{
		CopyToneIdentifier( pToneID, ToneListP[ (idx < TotalTones) ? idx : 0 ].identifier );
	}
	else
	{
		*pToneID = (ToneIdentifier) { 0, 0 };
		error = tonesLibErrNoTone;
	}
	
	TonesLibClose( TonesLibRefNum );
	
	if ( libWasLoaded ) SysLibRemove( TonesLibRefNum );
	
	if ( ToneListP ) MemPtrFree ( ToneListP );  	

	return ( error );
	
} // GetToneID()

/*
 * LoadTonesList()
 */
Err LoadTonesList( ListType* pList )
{
	Err             	error = errNone;
	UInt16				itemCount = 0;
	Char				nameStr[kMaxTonesNameSize];
	UInt16 				nameListSize = 0;
	UInt16				nameStrSize = 0;
	Char**          	listOptions = NULL;
	UInt16  			TonesLibRefNum = sysInvalidRefNum;
	Boolean				libWasLoaded = false;
  	ToneItemPtr	   		ToneListP = NULL;
	UInt16		   		TotalTones = 0;
	
	if ( gTonesH )
    {
    	MemHandleUnlock( gTonesH );
    	MemHandleFree( gTonesH );
    	gTonesH = NULL;
    }
    
   	if ( ( error = GetTonesLib( &TonesLibRefNum, &libWasLoaded ) ) ) return ( error );

	if ( ( error = TonesLibGetToneList( TonesLibRefNum, &ToneListP, &TotalTones ) ) ) return ( error );
 	
 	for ( itemCount = 0 ; itemCount < TotalTones ; itemCount++ )
 	{
 		StrCopy( nameStr, ToneListP[itemCount].name );
		nameStrSize = StrLen( nameStr ) + 1;
		MemMove( gTonesList + nameListSize, nameStr, nameStrSize );
		nameListSize = nameListSize + nameStrSize;				
 	}
 	  
	if ( itemCount )
	{
    	gTonesH = SysFormPointerArrayToStrings( gTonesList, itemCount);
	    listOptions = MemHandleLock( gTonesH ); // to be unlocked somewhere...
	}
	
    LstSetListChoices( pList, listOptions, itemCount );
    
    TonesLibClose( TonesLibRefNum );
    
  	if ( libWasLoaded ) SysLibRemove( TonesLibRefNum );
  	
  	if ( ToneListP ) MemPtrFree( ToneListP );
  	 
	return ( error );

} // LoadTonesList()

/*
 * PlayTone()
 */
void PlayTone( ToneIdentifier ToneID, UInt16 volume )
{
	UInt16				TonesLibRefNum = sysInvalidRefNum;
	Boolean				libWasLoaded = false;
	UInt16				muteSwitchPosition = hsAttrRingSwitchLoud; 
	
	if ( GetTonesLib( &TonesLibRefNum, &libWasLoaded ) ) return;
	
	HsAttrGet( hsAttr68KRingSwitch, 0, (UInt32 *) &muteSwitchPosition );
	
	TonesLibPlayTone( TonesLibRefNum, ToneID, 1 /* playCount */, volume, 
							( muteSwitchPosition == hsAttrRingSwitchLoud ) ? toneVibrateNone : toneVibrateRing,
							true /*blocking*/ );

    TonesLibClose( TonesLibRefNum );
    
	if ( libWasLoaded ) SysLibRemove( TonesLibRefNum );
	
	return;
	
} // PlayTone()

/*
 * StopTone()
 */
void StopTone( UInt16 TonesLibRefNum )
{
	TonesLibStopTone( TonesLibRefNum );

	return;

} // StopTone()

/*
 * GetSoundVolume()
 */
UInt16 GetSoundVolume( void )
{
	Err					error = errNone;
	UInt16				volume = toneVolume3;
	UInt16  			TonesLibRefNum = sysInvalidRefNum;
	Boolean				libWasLoaded = false;
	SoundPreference		sndPref;
	
	if ( ( error = GetTonesLib( &TonesLibRefNum, &libWasLoaded ) ) ) return ( volume );
	
	TonesLibGetSoundPrefs( TonesLibRefNum, soundPrefTypePhone, &sndPref );

	volume = sndPref.soundOnVolume;
	
	TonesLibClose( TonesLibRefNum );
	
	if ( libWasLoaded ) SysLibRemove( TonesLibRefNum );
	
	return ( volume );
	
} // GetSoundVolume()


/*
 * Sounds.c
 */