/*
 * SignalLvl.c
 */
 
#include "SignalLvl.h"

/*
 * GetSignalInfo()
 */
Err GetSignalInfo( UInt16* qualityP, Char* strSignalLvl )
{
	Err				error = errNone;
	UInt16 			phnLibRefNum = sysInvalidRefNum;
	UInt16			quality = 0;
	
	if ( qualityP ) *qualityP = 103;
	if ( strSignalLvl ) StrCopy( strSignalLvl, "!lib" );
	
	if ( ( error = HsGetPhoneLibrary( &phnLibRefNum ) ) )
		return ( error );
					
	if ( ( error = PhnLibOpen( phnLibRefNum ) ) )
		return ( error );
		
	if ( PhnLibModulePowered( phnLibRefNum ) == phnPowerOn ) 	
	{		
		if ( PhnLibRegistered( phnLibRefNum ) )
		{
			if ( ( error = PhnLibSignalQuality( phnLibRefNum, &quality ) ) == errNone )
			{			
				if ( strSignalLvl )
				{
					if ( quality < 32 )
					{
						StrIToA( strSignalLvl, (UInt32) quality ); 
					}
					else if ( quality == 99 )
					{
						StrCopy( strSignalLvl, "err" );
					}
					else
					{
						StrCopy( strSignalLvl, "??" );
					}
				}
			}
			else
			{
				quality = 100;
				if ( strSignalLvl ) StrCopy( strSignalLvl, "!sig" );	
			}
		}
		else
		{
			quality = 101;
			if ( strSignalLvl ) StrCopy( strSignalLvl, "!reg" );
		}
	}
	else
	{
		quality = 102;
		if ( strSignalLvl ) StrCopy( strSignalLvl, "off" );
	}

	if ( qualityP ) *qualityP = quality;
							
	PhnLibClose( phnLibRefNum );
	
	return ( error );
	
} // GetSignalInfo()

/*
 * SignalLvl.c
 */