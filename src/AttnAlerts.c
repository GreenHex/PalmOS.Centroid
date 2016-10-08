/*
 * AttnAlerts.h
 */
 
#include "AttnAlerts.h" 

// Prototypes
static void 			DrawDetail( UInt32 uniqueID, RectangleType rect );
static void 			DrawList( UInt32 uniqueID, RectangleType rect );
static Boolean 			GetAlertText( UInt32 uniqueID, Char* name, Char* note, Char* time );

/*
 * DoAlert()
 */
void DoAlert( lastLocation_t* pLastLoc, attentionMode_e attnMode )
{
	UInt16 				cardNo = 0;
	LocalID				dbID = 0;
	DateTimeType		dt;
	UInt32				minsNow = 0;
	UInt32				minsStart = 0;
	UInt32				minsEnd = 0;
	Boolean				bDoAlert = false;
	locations_t			loc;
	locAlert_t*			pAlert = NULL;
	regions_t			region;
	UInt16				idx = 0;
	Boolean				bFound = false;
	
	if ( pLastLoc->regionID )
	{
		region.uniqueID = pLastLoc->regionID;
		
		if ( !RegionsFindRecord( &region, &idx, true, &bFound, (DmComparF *) &Regions_CompareUniqueIDF ) 
				&& ( bFound ) )
		{
			pAlert = &(region.LocAlert);
		}
	}
	else
	{
		loc.uniqueID = pLastLoc->uniqueID;
		
		if ( ( !GIDFindRecord( &loc, &idx, true, &bFound, (DmComparF *) &GID_CompareUniqueIDF ) )
				&& ( bFound ) )
		{
			pAlert = &(loc.LocAlert);
		}
	}
	
	if ( !( ( bFound ) && ( pAlert ) ) ) return;
	
	if ( !( ( ( pAlert->bAlertOnEntry ) && ( attnMode == onEntry ) )
			|| ( ( pAlert->bAlertOnExit ) && ( attnMode == onExit ) ) ) )
		return;
			
	TimSecondsToDateTime( TimGetSeconds(), &dt );

	if ( ( *( (UInt16 *) &(pAlert->startTime) ) == noTime ) 
					|| ( *( (UInt16 *) &(pAlert->endTime) ) == noTime ) )
	{
		bDoAlert = true;
	}
	else
	{
		minsNow = dt.minute + ( dt.hour * 60 );
		minsStart = pAlert->startTime.minutes + ( pAlert->startTime.hours * 60 );
		minsEnd = pAlert->endTime.minutes + ( pAlert->endTime.hours * 60 );
		
		if ( !( ( minsNow < minsStart ) || ( minsNow > minsEnd ) ) )
		{
			bDoAlert = true;			
		}
	}
	
	if ( bDoAlert )
	{
		if ( !SysCurAppDatabase( &cardNo, &dbID ) )
		{
			if ( !AttnUpdate( cardNo, dbID, (UInt32) pLastLoc->uniqueID, NULL, NULL, NULL, NULL ) )
			{
				AttnGetAttention( cardNo, dbID, (UInt32) pLastLoc->uniqueID, NULL, 
										kAttnLevelInsistent, kAttnFlagsUseUserSettings, 180, 3 );
			}
		}
		
		if ( attnMode == onEntry )
		{
			pAlert->bAlertOnEntry = ( !( pAlert->bDisableAfterAlert ) );
		}
		else
		{
			pAlert->bAlertOnExit = ( !( pAlert->bDisableAfterAlert ) );	
		}
		
		if ( pLastLoc->regionID )
		{
			RegionsReplaceRecordAtIdx( &idx, &region, false );
		}
		else
		{
			GIDReplaceRecordAtIdx( &idx, &loc, false );
		}
	}
			
	return;
	
} // DoAlert()

/*
 * ProcessAttention
 */
void ProcessAttention( AttnLaunchCodeArgsType* paramP )
{
	AttnCommandArgsType* 		argsP = (AttnCommandArgsType *)paramP->commandArgsP;
	GoToParamsType*				cmdPBP = NULL;	
	UInt16 						cardNo = 0;
	LocalID						dbID = 0;
			
	switch ( paramP->command ) 
	{
		case kAttnCommandDrawDetail:
			
			DrawDetail( (UInt32) paramP->userData, (RectangleType) argsP->drawDetail.bounds );

			break;
			
		case kAttnCommandDrawList:
			
			DrawList( (UInt32) paramP->userData, (RectangleType) argsP->drawDetail.bounds );
			
			break;
		
		case kAttnCommandPlaySound:
			
			 PlayAlertSound();
			
			// SndPlaySystemSound( sndInfo /* sndAlarm */ );
			
			break;  
	
		case kAttnCommandCustomEffect:
		
			// Perform a custom attention-getting effect.
			
			break;
		
		case kAttnCommandGoThere:

			if ( !SysCurAppDatabase( &cardNo, &dbID ) )
			{
				AttnForgetIt( cardNo, dbID, (UInt32) paramP->userData );
			}
			
			cmdPBP = MemPtrNew( sizeof( GoToParamsType ) );

			if ( cmdPBP )
			{
				MemSet( cmdPBP, sizeof( GoToParamsType ), 0 );
				
				if ( !SysCurAppDatabase( &cardNo, &dbID ) )
				{
					cmdPBP->matchCustom = (UInt32)( paramP->userData );
				
					MemPtrSetOwner( cmdPBP, 0 ); // set owner to system, or it will be freed on exit...
					
					SysUIAppSwitch( cardNo, dbID, sysAppLaunchCmdGoTo, cmdPBP );
				}
			}

			break; 
		
		case kAttnCommandGotIt:
			
			if ( argsP->gotIt.dismissedByUser )
			{
				if ( !SysCurAppDatabase( &cardNo, &dbID ) )
				{
					AttnForgetIt( cardNo, dbID, (UInt32) paramP->userData );		
				}
			}
			
			break;   
		
		case kAttnCommandIterate:

			if ( !SysCurAppDatabase( &cardNo, &dbID ) )
			{
				AttnForgetIt( cardNo, dbID, (UInt32) paramP->userData );		
			}

			break;
		
		default:
		
			break;
	}

} // ProcessAttention

/*
 * DrawDetail() // Large alert
 */
static void DrawDetail( UInt32 uniqueID, RectangleType rect )
{
	MemHandle 				BmpH = NULL;
	BitmapType* 			BmpP = NULL;
	Coord					BmpSizeX = 0;
	Coord					BmpSizeY = 0;
	RGBColorType 			rgb;
    IndexedColorType		StrColor;
	UInt16					strHeight = 0;
	UInt16					strWidth = 0;
	Char					name[STRLEN_GID] = "\0";
	Char					note[STRLEN_ALERT_NOTE] = "\0";
	Char					time[timeStringLength] = "\0";
	
	if ( GetAlertText( uniqueID, name, note, time ) )
	{		
		WinPushDrawState();
		
		BmpP = GetBitmap( BMP_ICON_LARGE, &BmpSizeX, &BmpSizeY, &BmpH );
		WinDrawBitmap( BmpP, rect.topLeft.x + 29 - BmpSizeX, rect.topLeft.y + 8 );
		ReleaseBitmap( &BmpH );	
		BmpH = NULL;
		BmpP = NULL;
		
		FntSetFont( largeBoldFont );
		strHeight = FntLineHeight();
		strWidth = FntLineWidth( APP_NAME, StrLen( APP_NAME ) );
		
		WinDrawChars( APP_NAME, StrLen( APP_NAME ), rect.topLeft.x + 34, rect.topLeft.y + 12 );
		
		rgb.r=250; rgb.g=0; rgb.b=51; // Red
		StrColor = WinRGBToIndex( &rgb );	
	    WinSetTextColor( StrColor );
		WinDrawChars( time, StrLen( time ), rect.topLeft.x + 34 + strWidth + 7, rect.topLeft.y + 12 );
		
		rgb.r=250; rgb.g=0; rgb.b=51; // Red
		StrColor = WinRGBToIndex( &rgb );	
	    WinSetTextColor( StrColor );
		strHeight = FntLineHeight();
		WinDrawChars( name, StrLen( name ), rect.topLeft.x + 34, rect.topLeft.y + 45 );

		if ( StrLen( note ) )
		{			
			rgb.r=0; rgb.g=51; rgb.b=200; // Blue
			StrColor = WinRGBToIndex( &rgb );	
		    WinSetTextColor( StrColor );			
		    FntSetFont( boldFont );
			WinDrawTruncChars( note, StrLen( note ),
										rect.topLeft.x + 34, 
										rect.topLeft.y + 80,
										rect.extent.x - ( 34 + 3 ) );	
		}
			
		WinPopDrawState();
	}
	
	return;
	
} // DrawDetail()

/*
 * DrawList() // Small alert.
 */
static void DrawList( UInt32 uniqueID, RectangleType rect )
{
	MemHandle 				BmpH = NULL;
	BitmapType* 			BmpP = NULL;
	Coord					BmpSizeX = 0;
	Coord					BmpSizeY = 0;
	RGBColorType 			rgb;
    IndexedColorType		StrColor;
	UInt16					strHeight = 0;
	UInt16					strWidth = 0;
	Char					name[STRLEN_GID] = "\0";
	Char					note[STRLEN_ALERT_NOTE] = "\0";
	Char					time[timeStringLength] = "\0";

	if ( GetAlertText( uniqueID, name, note, time ) )
	{	
		WinPushDrawState();
		
		BmpP = GetBitmap( BMP_ICON_SMALL, &BmpSizeX, &BmpSizeY, &BmpH );
		WinDrawBitmap( BmpP, rect.topLeft.x + 15 - BmpSizeX, rect.topLeft.y + 1 );
		ReleaseBitmap( &BmpH );	
		BmpH = NULL;
		BmpP = NULL;
		
		FntSetFont( stdFont );
		strHeight = FntLineHeight();
/*		strWidth = FntLineWidth( APP_NAME, StrLen( APP_NAME ) );	
		WinDrawChars( APP_NAME, StrLen( APP_NAME ), rect.topLeft.x + 19, rect.topLeft.y );
*/
		strWidth = FntLineWidth( time, StrLen( time ) );	
		WinDrawChars( time, StrLen( time ), rect.topLeft.x + 19, rect.topLeft.y );
			
		rgb.r=250; rgb.g=0; rgb.b=51; // Red
		StrColor = WinRGBToIndex( &rgb );	
	    WinSetTextColor( StrColor );
		WinDrawChars( name, StrLen( name ), rect.topLeft.x + 19 + strWidth + 3, rect.topLeft.y );
		
		if ( StrLen( note ) )
		{
			rgb.r=0; rgb.g=51; rgb.b=200; // Blue
			StrColor = WinRGBToIndex( &rgb );	
		    WinSetTextColor( StrColor );
			WinDrawTruncChars( note, StrLen( note ),
										rect.topLeft.x + 19, 
										rect.topLeft.y + 11,
										rect.extent.x - ( 19 + 3 ) );	
		}

		WinPopDrawState();
	}

	return;
	
} // DrawList()

/*
 * GetAlertText()
 */
static Boolean GetAlertText( UInt32 uniqueID, Char* name, Char* note, Char* time )
{
	Boolean				bFound = false;
	locations_t			loc;
	regions_t			region;
	Char*				pName = NULL;
	locAlert_t*			pAlert = NULL;
	UInt16				idx = dmMaxRecordIndex;
	
	loc.uniqueID = uniqueID;
	
	if ( ( !GIDFindRecord( &loc, &idx, true, &bFound, (DmComparF *) GID_CompareUniqueIDF ) ) 
				&& ( bFound ) )
	{	
		if ( loc.RegionInfo.regionID )
		{
			region.uniqueID = loc.RegionInfo.regionID;
			
			if ( !RegionsFindRecord( &region, &idx, true, &bFound, (DmComparF *) &Regions_CompareUniqueIDF ) 
					&& ( bFound ) )
			{
				pName = region.name;
				pAlert = &(region.LocAlert);	
			}	
		}
		else
		{
			pName = loc.LocationName;
			pAlert = &(loc.LocAlert);
		}
		
		if ( time )
		{
			MakeTimeDateStr( loc.TimeAtLoc, NULL, time, NULL, true );
			ReplaceChar( time, 'm', chrNull );
			StripSpaces( time );
		}
		
		if ( ( pName ) && ( name ) )
		{
			if ( StrLen( pName ) )
			{ 
				StrNCopy( name, pName, STRLEN_GID - 1 );
				name[STRLEN_GID - 1] = chrNull;
			}
			else
			{
				MakeGIDString( & ( loc.GID ), name );
			}	
		}
		
		if ( ( pAlert ) && ( note ) )
		{
			StrNCopy( note, pAlert->note, STRLEN_ALERT_NOTE - 1 );
			note[STRLEN_ALERT_NOTE - 1] = chrNull; 
		}				
	}
	
	return ( ( pName ) && ( pAlert ) );
		
} // GetAlertText()

/*
 * DoGoto()
 */
void DoGoto( UInt32 uniqueID )
{
	locations_t		loc; 
	UInt16			idx = dmMaxRecordIndex;
	Boolean			bFound = false;
	
	MemSet( &loc, sizeof( locations_t ), 0 );
	
	loc.uniqueID = uniqueID;

	if ( ( !GIDFindRecord( &loc, &idx, true, &bFound, (DmComparF *) GID_CompareUniqueIDF ) ) 
		&& ( bFound ) )
	{
		gLocFormOpenIdx = idx;
		 
		FrmPopupForm( LOC_FORM );
	}
		
	return;
	
} // DoGoto()

/*
 * AttnAlerts.h
 */
