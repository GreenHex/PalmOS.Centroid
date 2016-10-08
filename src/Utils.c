/*
 * Utils.c
 */
 
#include "Utils.h"

/*
 * StrHexToInt()
 */
long StrHexToInt(const char* pszHex)
{
    long nResult = 0;
    int	i = 0;
    char c = 0;
    
    for (i = 0; *pszHex && (i < (sizeof(long) << 1)); i++)
    {
        nResult <<= 4;
        c = *pszHex++;
        if ((c & 0xF0) == 0x30)
            nResult |= (c & 0x0F);
        else
            nResult |= ((c & 0x0F) + 9);
    }
    
    return (nResult);
    
} //StrHexToInt()

/*
 * DoubleToString()
 */
Char* DoubleToString( double dbl, Char* strDbl )
{
	FlpCompDouble		flpDbl;
	Err					error = errNone;
	
	if ( !strDbl ) return ( NULL );
	
	flpDbl.d = dbl;
	
	error = FlpFToA( flpDbl.fd, strDbl );
	
	if ( error ) return ( NULL );
	
	return ( strDbl );
	
} // DoubleToString()

/*
 * StringToDouble()
 */
double StringToDouble( Char* strDbl )
{
	FlpCompDouble		flpDbl;
		
	if ( !strDbl ) return ( 0 );
	
	FlpBufferAToF ( (FlpDouble *) &( flpDbl.fd ), strDbl );
	
	return ( flpDbl.d );
	
} // StringToDouble()

/*
 * MakeTimeInterval()
 */
void MakeTimeInterval( TimeType startTime, TimeType endTime, Char* strTimeInterval )
{
	Char					strTime[timeStringLength] = "\0";
	
	if ( ( *( (UInt16 *) &startTime ) == noTime ) || ( *( (UInt16 *) &endTime ) == noTime ) )
	{
		StrCopy( strTimeInterval, "No Time" );
	}
	else
	{
		TimeToAscii( startTime.hours, startTime.minutes, PrefGetPreference( prefTimeFormat ), strTime );
		StrCopy( strTimeInterval, strTime );
		
		StrCat( strTimeInterval, " - " );
	
		TimeToAscii( endTime.hours, endTime.minutes, PrefGetPreference( prefTimeFormat ), strTime );
		StrCat( strTimeInterval, strTime );
	}
	
	return;
	
} // MakeTimeInterval()

/*
 * MakeTimeDateStr()
 */
void MakeTimeDateStr( UInt32 timeSecs, Char** strTimeDatePP, Char* strTimeP, Char* strDateP, Boolean bRemoveYear )
{
	DateTimeType 			dt;
	DateType				d;
	TimeFormatType			tfmt = PrefGetPreference( prefTimeFormat );
	DateFormatType 			dfmt = PrefGetPreference( prefDateFormat );
	Char					strTime[timeStringLength] = "\0";
	Char					strDate[longDateStrLength] = "\0";
	
	// Time
	TimSecondsToDateTime( timeSecs, &dt );
	TimeToAscii( dt.hour, dt.minute, tfmt, strTime );
    // ReplaceChar( strTime, 'm', chrNull );
	// StripSpaces( strTime );
	
	if ( strTimeP )
	{
		StrCopy( strTimeP, strTime );	
	}

	// Date
	DateSecondsToDate( timeSecs, &d );
	DateToAscii( d.month, d.day, d.year + 1904, dfmt, strDate );

	if ( bRemoveYear )
	{
		if ( ( dfmt == dfYMDWithSlashes ) 
			|| ( dfmt == dfYMDWithDots ) 
			|| ( dfmt == dfYMDWithDashes ) )
		{
			MemMove( strDate, strDate + 3, StrLen( strDate ) );
		}
		else
		{
			strDate[StrLen(strDate) - 3] = chrNull;
		}		
	}
	
	if ( strDateP )
	{
		StrCopy( strDateP, strDate );	
	}
	
	if ( strTimeDatePP )
	{
		if ( !( *strTimeDatePP ) ) *strTimeDatePP = MemPtrNew( timeStringLength + longDateStrLength + 3 );
		
		if ( *strTimeDatePP )
		{
			StrCopy( *strTimeDatePP, strTime );
			StrCat( *strTimeDatePP, "  " );
			StrCat( *strTimeDatePP, strDate );
		}
	}
		
	return;
		
} // MakeTimeDateStr()

/*
 * ReplaceDateTimeTokens()
 */
void ReplaceDateTimeTokens( Char* str, UInt16 strLen )
{
	Char				dateStr[dateStringLength];
	Char				timeStr[timeStringLength];
	
	if ( !str ) return;
	
	MakeTimeDateStr( TimGetSeconds(), NULL, timeStr, dateStr, false );
	
	if ( timeStr )
	{
		TxtReplaceStr( str, strLen, timeStr, 0 ); // ^0
	}
	else
	{
		TxtReplaceStr( str, strLen, " ", 0 );
	}
	
	if ( dateStr )
	{			
		TxtReplaceStr( str, strLen, dateStr, 1 ); // ^1
	}
	else
	{
		TxtReplaceStr( str, strLen, " ", 1 );
	}
	
	return;
	
} // ReplaceDateTimeTokens()

/*
 * DrawStatusStr()
 */
void DrawStatusStr( Char* str, RectangleType* pRect, FontID fontID, hAlign_e hAlign, vAlign_e vAlign, color_e color, Boolean draw )
{
	Coord 					posX = 0;
	Coord 					posY = 0;
	Int16					strHeight = 0;
	Int16					strWidth = 0;	
    RGBColorType 			rgb;
    IndexedColorType		StrColor;
		
	WinPushDrawState();
	
	WinEraseRectangle( pRect, 2 ); 
	
	if ( draw )
	{
		switch ( color )
		{
			case colorGreen:
	
				rgb.r=0; rgb.g=153; rgb.b=0; // Green
			
				break;
				
			case colorRed:
			
				rgb.r=250; rgb.g=0; rgb.b=51; // Red
	
				break;
				
			default:
			
				rgb.r=0; rgb.g=51; rgb.b=200; // Blue
				
				break;		
		}
		
		StrColor = WinRGBToIndex( &rgb );	
	    WinSetTextColor( StrColor );
	    
		FntSetFont( fontID );
		strHeight = FntLineHeight();
		strWidth = FntLineWidth( str, StrLen( str ) );
		
		switch ( hAlign )
		{
			case hAlignLeft:
				posX = pRect->topLeft.x;		
				break;
			case hAlignRight:
				posX = pRect->topLeft.x + ( pRect->extent.x - strWidth );
				break;
			default:
				posX = pRect->topLeft.x + ( ( pRect->extent.x - strWidth ) / 2 );
				break;
		}
		
		switch ( vAlign )
		{
			case vAlignTop:
				posY = pRect->topLeft.y;
				break;
			case vAlignBottom:
				posY = pRect->topLeft.y + ( pRect->extent.y - strHeight );
				break;
			default:
				posY = pRect->topLeft.y + ( ( pRect->extent.y - strHeight ) / 2 );
				break;
		}
		
		WinDrawChars( str, StrLen( str ), posX, posY );
	}
		    
	WinPopDrawState();
	
	return;
	
} // DrawStatusStr()

/*
 * GetStrDelayTime
 */
void GetStrDelayTime( UInt32 delayTime, Char* str )
{
	DateTimeType		dt;
	
	TimSecondsToDateTime( delayTime, &dt );
	StrPrintF( str, "  [%02uh %02um]", dt.hour, dt.minute );	

	return;
	
} // GetStrDelayTime


/*
 * ReplaceChar
 */
Char* ReplaceChar( unsigned char *s, char orgChr, char newChr )
{
	unsigned char c, *d = s;
	
	while ( ( c = *s++ ) )
	{
		if ( c == orgChr )
			*d++ = newChr;
		else
			*d++ = c;
	}
					
	*d = 0;
	
	return ( s );
	
} // ReplaceChar

/*
 * StripSpace
 */
Char* StripSpaces( unsigned char *s )
{
	unsigned char c, *d = s;
	
	while ( ( c = *s++ ) )
	{
		if ( c != chrSpace )
		{
			*d++ = c;
		}
	}
					
	*d = 0;
	
	return ( s );
	
} // StripSpaces

/*
 * MakeNum
 */
Char* MakeNum( unsigned char* s )
{
	unsigned char c, *d = s;
	
	while ( ( c = *s++ ) )
	{
		if ( TxtCharIsDigit( c ) )
		{
			*d++ = c;
		}
	}
					
	*d = 0;
	
	return ( s );
	
} // MakeNum

/*
 * is_display_off()
 */
Boolean is_display_off( void )
{
    UInt32 tui16;

    return ( !( !HsAttrGet( hsAttrDisplayOn, 0, (UInt32 *) &tui16 ) && tui16 ) ); // weird!!! not thinking here!

} // is_display_off()

/* 
 * IsPhoneReady()
 */
Boolean IsPhoneReady( void )
{
	Boolean			retVal = false;
	UInt16 			phnLibRefNum = sysInvalidRefNum;
	
	if ( HsGetPhoneLibrary( &phnLibRefNum ) )
		return ( retVal );
					
	if ( PhnLibOpen( phnLibRefNum ) )
		return ( retVal );
		
	if ( PhnLibModulePowered( phnLibRefNum ) == phnPowerOn )
	{
		retVal = PhnLibRegistered( phnLibRefNum );
	}
		
	PhnLibClose( phnLibRefNum );
	
	return ( retVal );
	
} // IsPhoneReady()

/* 
 * IsRoaming()
 */
Boolean IsRoaming( void )
{
	Boolean					retVal = false;
	UInt16 					phnLibRefNum = sysInvalidRefNum;
	PhnRoamStatusEnum		roamingStatus = PhnRoamStatusBlink;
	
	if ( HsGetPhoneLibrary( &phnLibRefNum ) )
		return ( retVal );
					
	if ( PhnLibOpen( phnLibRefNum ) )
		return ( retVal );
		
	if ( ( PhnLibModulePowered( phnLibRefNum ) == phnPowerOn )
			&& ( PhnLibRegistered( phnLibRefNum ) ) )
	{
		if ( !PhnLibRoaming( phnLibRefNum, &roamingStatus ) )
		{
			retVal = ( roamingStatus == PhnRoamStatusOn );
		}
	}
		
	PhnLibClose( phnLibRefNum );
	
	return ( retVal );
	
} // IsRoaming()

/* 
 * GetPhnOperator()
 */
Err GetPhnOperator( Char* operatorName, UInt16 operatorNameStrLen )
{
	Err						error = errNone;
	UInt16 					phnLibRefNum = sysInvalidRefNum;
	PhnOperatorID			operatorID = phnLibUnknownID;
	Char*					operatorNameP = NULL;
	GSMRegistrationMode		regMode = 0;
	
	if ( !operatorName ) return ( error );
	
	if ( ( error = HsGetPhoneLibrary( &phnLibRefNum ) ) )
		return ( error );
					
	if ( ( error = PhnLibOpen( phnLibRefNum ) ) )
		return ( error );
		
	if ( ( PhnLibModulePowered( phnLibRefNum ) == phnPowerOn )
			&& ( PhnLibRegistered( phnLibRefNum ) ) )
	{
		if ( ( error = PhnLibCurrentOperator( phnLibRefNum, &operatorID, &operatorNameP, &regMode ) ) == errNone )
		{
			StrNCopy( operatorName, operatorNameP, operatorNameStrLen - 1 );
			operatorName[operatorNameStrLen - 1] = chrNull;		
		}
	}
		
	PhnLibClose( phnLibRefNum );

	if ( operatorNameP ) MemPtrFree( operatorNameP );
	
	return ( error );
	
} // GetPhnOperator()

/*
 * IsPhoneGSM()
 */
Boolean IsPhoneGSM( void )
{
  	UInt32 phnType = hsAttrPhoneTypeGSM; 
  
  	HsAttrGet( hsAttrPhoneType, 0, &phnType );
  	
  	return ( phnType == hsAttrPhoneTypeGSM );

} // IsPhoneGSM()

/*
 * IsCentro()
 */
Boolean IsCentro( void )
{
	UInt32	hsFtrVersion;
	UInt32 	hal = 0;
	UInt32	device = 0;
	
	if( FtrGet( hsFtrCreator, hsFtrIDVersion, &hsFtrVersion ) != 0 )
	{
		return ( false );
	}
	
    FtrGet( sysFtrCreator, sysFtrNumOEMDeviceID, &device );	
	FtrGet( sysFtrCreator, sysFtrNumOEMHALID, &hal );
    
	return ( ( device == 'D061' /* Centro */ ) && ( hal == 'P053' /* Centro */ ) );
    
} // IsCentro()

/*
 * RomVersionCompatible()
 */
Err RomVersionCompatible( UInt32 requiredVersion, UInt16 launchFlags )
{	
	UInt32 	rom = 0;
	UInt32 	hal = 0;
	UInt32	company = 0;
	UInt32	device = 0;
	UInt32	hsFtrVersion;

	if( FtrGet( hsFtrCreator, hsFtrIDVersion, &hsFtrVersion ) != 0 )
	{
		return ( 1 );
	}
	
    FtrGet( sysFtrCreator, sysFtrNumOEMHALID, &hal );
    FtrGet( sysFtrCreator, sysFtrNumOEMCompanyID, &company );
    FtrGet( sysFtrCreator, sysFtrNumOEMDeviceID, &device );
	FtrGet( sysFtrCreator, sysFtrNumROMVersion, &rom );
		
	if ( rom >= requiredVersion
		
			&&	( company == kPalmCompanyIDPalm
					|| company == kPalmCompanyIDOldPalm
					|| kPalmCompanyIDHandspring )
				
			&&	( device == kPalmOneDeviceIDTreo650
					|| device == kPalmOneDeviceIDTreo650Sim
		
					|| device == kPalmOneDeviceIDTreo600
					|| device == kPalmOneDeviceIDTreo600Sim
		
					|| device == 'D053' // Treo 680
					|| device == 'H104'
				
					|| device == 'D061' /* Centro */ )
			
			&& ( hal == hsHALIDHandspringOs5Rev2 // Treo 650
					|| hal == hsHALIDHandspringOs5Rev2Sim
		
					|| hal == hsHALIDHandspringOs5Rev1 // Treo 600
					|| hal == hsHALIDHandspringOs5Rev1Sim
		
					|| hal == hsHALIDHandspringOs5Rev4 // Treo 680, 755p
					|| hal == hsHALIDHandspringOs5Rev4Sim		
					
					||hal == 'P053' /* Centro */ )
		)
	{
		return 0;
	}

	if ((launchFlags & LAUNCH_FLAGS) == LAUNCH_FLAGS)
	{
		// Pilot 1.0 will continuously relaunch this app unless we switch to 
		// another safe one.
		if ( rom < sysMakeROMVersion( 2, 0, 0, sysROMStageRelease, 0 ) )
			AppLaunchWithCommand( sysFileCDefaultApp, sysAppLaunchCmdNormalLaunch, NULL );
	}

	return ( sysErrRomIncompatible );

} // RomVersionCompatible()

/*
 * CommonInitGauge
 */
void CommonInitGauge( FormPtr pForm )
{
	UInt32 			hsStatusVersion;
	UInt16 			libRefNum = sysInvalidRefNum;
	Err				error = errNone;
	
	if ( FtrGet( hsFtrCreator, hsFtrIDStatusGadgetRev, &hsStatusVersion ) == 0 )
	{
		HsStatusSetGadgetType( pForm, MAIN_BATT_GDT, hsStatusGadgetBattery );
		
		HsStatusSetGadgetType( pForm, MAIN_SIG_GDT, hsStatusGadgetSignal );
		
		error = SysLibFind( kPmSysGadgetLibName, &libRefNum );
			if ( error ) SysLibLoad( kPmSysGadgetLibType, kPmSysGadgetLibCreator, &libRefNum );
		
		PmSysGadgetLibOpen( libRefNum );
		PmSysGadgetStatusGadgetTypeSet( libRefNum, pForm, MAIN_BT_GDT, pmSysGadgetStatusGadgetBt );
		PmSysGadgetLibClose( libRefNum );
	}
	
	return;
	
} // CommonInitGauge

/*
 * AboutFormHandleEvent()
 */
Boolean AboutFormHandleEvent( EventType* pEvent )
{
	Boolean 			handled = false;
	FormType*			pForm = FrmGetActiveForm();
	MemHandle			strVerH = NULL;
	Char*				strVerP = NULL;
	Char				strVer[20];
		
	switch ( pEvent->eType )
	{		
		case frmOpenEvent:
		
			strVerH = DmGet1Resource('tver', 1000);
	
			if ( strVerH )
			{
				strVerP = MemHandleLock( strVerH );
		
				if ( strVerP )
				{
						StrCopy( strVer, "v" );
						StrCat( strVer, strVerP );
						StrCat( strVer, " [Freeware]" );
						strVer[StrLen( (Char *)FrmGetLabel( pForm, ABOUT_VER_LBL ) )] = chrNull;
						
						FrmCopyLabel( pForm, ABOUT_VER_LBL, strVer );
			
						MemHandleUnlock( strVerH );
				}
	   	
			   	DmReleaseResource( strVerH );
			   	strVerH = NULL;
			}
		
			FrmDrawForm( pForm );
			
			handled = true;
						
			break;
		
		case ctlSelectEvent:
		
			switch ( pEvent->data.ctlSelect.controlID )
			{
				case ABOUT_OK_BTN:
				
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
	
} // AboutFormHandleEvent()

/*
 * BackupDBs()
 */
void BackupDBs( void )
{
	Err						error = errNone;
	Err						error2 = vfsErrFileGeneric;
	UInt16 					volRefNum = sysInvalidRefNum; 
	UInt32 					volIterator = vfsIteratorStart;
	VolumeInfoType 			volInfo;
	DmSearchStateType		stateInfo; 
	UInt16					cardNo = 0;
	LocalID					dbID = 0;
	
	while ( volIterator != vfsIteratorStop )
	{ 
		error = VFSVolumeEnumerate( &volRefNum, &volIterator );  
		
		if ( error == expErrEnumerationEmpty ) break;

		if ( error == errNone ) 
		{ 
			if ( ( error = VFSVolumeInfo( volRefNum, &volInfo ) ) != errNone ) break;

			if ( volInfo.attributes & vfsVolumeAttrReadOnly ) break;
			   					
			error = VFSDirCreate( volRefNum, "/PALM/" );
			error = VFSDirCreate( volRefNum, "/PALM/Programs/" );
			error = VFSDirCreate( volRefNum, "/PALM/Programs/Centroid/" );

			if ( ( error == errNone ) || ( error == vfsErrFileAlreadyExists ) )
			{
				// GIDs
				error = DmGetNextDatabaseByTypeCreator( true, &stateInfo, GIDDbType, GIDDbCreator, true, &cardNo, &dbID );
				
				if ( error == errNone )
				{
					error = VFSFileDelete( volRefNum, STR_NAME_CARD_GID_DB );
  					
  					if ( ( error == errNone ) 
  							|| ( error == vfsErrFileGeneric ) 
  							|| ( error == vfsErrFileNotFound ) 
  							|| ( error == vfsErrBadName ) )
  					{
  						error2 = VFSExportDatabaseToFile( volRefNum, STR_NAME_CARD_GID_DB, cardNo, dbID );
  					}
				}
				
				// Profiles
				error = DmGetNextDatabaseByTypeCreator( true, &stateInfo, ProfilesDbType, ProfilesDbCreator, true, &cardNo, &dbID );
				
				if ( error == errNone )
				{
					error = VFSFileDelete( volRefNum, STR_NAME_CARD_PROFILES_DB );
  					
  					if ( ( error == errNone ) 
  							|| ( error == vfsErrFileGeneric ) 
  							|| ( error == vfsErrFileNotFound ) 
  							|| ( error == vfsErrBadName ) )
  					{
      					error2 |= VFSExportDatabaseToFile( volRefNum, STR_NAME_CARD_PROFILES_DB, cardNo, dbID );
  					}
				}
				
				// Regions
				error = DmGetNextDatabaseByTypeCreator( true, &stateInfo, RegionsDbType, RegionsDbCreator, true, &cardNo, &dbID );
				
				if ( error == errNone )
				{
					error = VFSFileDelete( volRefNum, STR_NAME_CARD_REGIONS_DB );
  					
  					if ( ( error == errNone ) 
  							|| ( error == vfsErrFileGeneric ) 
  							|| ( error == vfsErrFileNotFound ) 
  							|| ( error == vfsErrBadName ) )
  					{
      					error2 |= VFSExportDatabaseToFile( volRefNum, STR_NAME_CARD_REGIONS_DB, cardNo, dbID );
  					}
				}
			}   								
		}
		
		break;
		
	}
	
	if ( error2 )
	{		
		FrmCustomAlert( ErrOKAlert, "Error while backing-up.", "", "" );
	}
	else
	{
		FrmCustomAlert( InfoOKAlert, "Files saved to: [/PALM/Programs/\nCentroid/]", "", "" );
	}		 

	return;
	
} // BackupDBs()

/*
 * GetLastBackupDate()
 */
Err GetLastBackupDate( Char* strModDate )
{
	Err						error = errNone;
	UInt16 					volRefNum = sysInvalidRefNum;
	UInt32 					volIterator = vfsIteratorStart;
	VolumeInfoType 			volInfo;
	FileRef 				fileRef;
	UInt32					modDate;
	Char*					strModDateTime = NULL;
						
	while ( volIterator != vfsIteratorStop )
	{ 
		error = VFSVolumeEnumerate( &volRefNum, &volIterator );  
		
		if ( error == expErrEnumerationEmpty )
		{
			StrCopy( strModDate, "           Card not present" );						
			
			break;
		}
		else if ( error == errNone ) 
		{ 
			if ( ( error = VFSVolumeInfo( volRefNum, &volInfo ) ) != errNone ) break;

			if ( ( error = VFSFileOpen( volRefNum, STR_NAME_CARD_GID_DB, vfsModeRead, &fileRef ) ) == errNone )
			{
				if ( ( error = VFSFileGetDate( fileRef, vfsFileDateModified, &modDate ) ) == errNone )
				{
					if ( strModDate )
					{
						MakeTimeDateStr( modDate, &strModDateTime, NULL, NULL, false );
							
						StrCopy( strModDate, "Last Backup: " );
						StrCat( strModDate, strModDateTime );
						
						MemPtrFree( strModDateTime );		
					}
				}
				else
				{
					if ( strModDate ) StrCopy( strModDate, "           Error reading date" );
				}
			
				VFSFileClose( fileRef );
			}
			else
			{
				if ( strModDate ) StrCopy( strModDate, "           No backups found" );
			}		
			
			break;		
		}
		else
		{
			StrCopy( strModDate, "           Card error" );
			
			break;										
		}
	}

	return ( error );
	
} // GetLastBackupDate()

/*
 * LEDState()
 * set = true SETS the state
 * set = false GETS the state
 */
void LEDState( Boolean set, UInt16* countP, UInt16* stateP )
{
	if ( !set )
	{
		*countP = kIndicatorCountGetState;
		*stateP = kIndicatorStateNull;
	}
	
	HsIndicatorState( *countP, kIndicatorTypeLed, stateP );
	
	return;
		
} // LEDState()

/*
 * FlashLED()
 */
void FlashLED( Boolean blink, UInt8 BlinkType )
{
	UInt16 		count = kIndicatorCountForever;
	UInt16		onState[4] = { kIndicatorAlertAlert , kIndicatorForceRedGreen , kIndicatorForceRed , kIndicatorForceGreen } ;
	UInt16		offState[4] = { kIndicatorAlertNone , kIndicatorForceNone , kIndicatorForceNone , kIndicatorForceNone } ;
	UInt16		state = ( blink ) ? onState[BlinkType] : offState[BlinkType];
		
	HsIndicatorState( count, kIndicatorTypeLed, &state );
	
	return;
	
} // FlashLED()

/*
 * Utils.c
 */
