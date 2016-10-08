/*
 * Gadgets.c
 */
 
#include "Gadgets.h"

// Prototypes
static void 				BlinkRectangle( RectangleType* rectP ) EXTRA_SECTION_ONE;

/*
 * InitializeGadgets()
 */
void InitializeGadgets( FormType* pForm )
{
	if ( !pForm ) return;
	
	FrmSetActiveForm( pForm );
 			
	FrmSetGadgetHandler( pForm, FrmGetObjectIndex( pForm, MAIN_LOCATION_GDT ), (FormGadgetHandlerType *) LocationGadgetHandler );
	FrmSetGadgetHandler( pForm, FrmGetObjectIndex( pForm, MAIN_PROFILE_GDT ), (FormGadgetHandlerType *) ProfileGadgetHandler );
	FrmSetGadgetHandler( pForm, FrmGetObjectIndex( pForm, MAIN_SIGNALLVL_GDT ), (FormGadgetHandlerType *) SignalLvlGadgetHandler );
	FrmSetGadgetHandler( pForm, FrmGetObjectIndex( pForm, MAIN_ROAMING_GDT ), (FormGadgetHandlerType *) RoamingGadgetHandler );
	
	FrmSetGadgetData( pForm, FrmGetObjectIndex( pForm, MAIN_LOCATION_GDT ), NULL );
	FrmSetGadgetData( pForm, FrmGetObjectIndex( pForm, MAIN_PROFILE_GDT ), NULL );
	FrmSetGadgetData( pForm, FrmGetObjectIndex( pForm, MAIN_SIGNALLVL_GDT ), NULL );
	FrmSetGadgetData( pForm, FrmGetObjectIndex( pForm, MAIN_ROAMING_GDT ), NULL );
	
	return;
	
} // InitializeGadgets()

/* 
 * LocationGadgetHandler()
 */
Boolean LocationGadgetHandler( FormGadgetTypeInCallback *gadgetP, UInt16 cmd, void *paramP )
{
	Boolean 				handled = false;
	FormType*				pForm = FrmGetActiveForm();
	RectangleType			rect = gadgetP->rect;
	color_e 				color = colorGreen;
	appPrefs_t				prefs;
	lastLocation_t			loc;
	Char					strLoc[STRLEN_GID] = "\0";
	Char*					strTimeDate = NULL;
	Char					str[STRLEN_GID + timeStringLength + longDateStrLength + 6] = "\0";
	static UInt32			lastUniqueID = -1;
	
	if ( gadgetP->id != MAIN_LOCATION_GDT ) return ( handled );
	
	switch ( cmd )
	{
		case formGadgetDeleteCmd:

			handled = true;

			break;
			
		case formGadgetEraseCmd:

			handled = false;

			break;		

		case formGadgetHandleEventCmd:
		
			
			if ( ((EventType *) paramP)->eType == frmGadgetEnterEvent )
			{
				ReadAppPrefs( &prefs );
				
				if ( prefs.bEnabled )
				{
					ReadLastLocationPrefs( &loc );
				
					if ( loc.uniqueID )
					{
						gLocFormOpenIdx = 0;
												
						FrmNavRemoveFocusRing( pForm );
						
						BlinkRectangle( &rect );
						
						FrmPopupForm( LOC_FORM );
					}
				}
				
				handled = true;
				
				break;
			}
			
			// NO break;
			
		case formGadgetDrawCmd:
	
			FrmSetActiveForm( pForm );
			
			gadgetP->attr.visible = true;
							
			ReadAppPrefs( &prefs );
	
			if ( !prefs.bEnabled )
			{				
				StrCopy( str, "Centroid Disabled" );
				color = colorRed;
				
				lastUniqueID = -1;
			}
			else
			{
				ReadLastLocationPrefs( &loc );
				
				if ( loc.uniqueID )
				{
					// GID
					MakeGIDString( & ( loc.GID ), strLoc );
							
					// time & date	
					MakeTimeDateStr( loc.TimeAtLoc, &strTimeDate, NULL, NULL, false );
					
					StrCopy( str, strLoc );
					StrCat( str, "  [" );
					StrCat( str, strTimeDate );
					StrCat( str, "]" );
					
					lastUniqueID = loc.uniqueID;
				}
			}
				
			if ( strTimeDate ) MemPtrFree( strTimeDate );
			
			if ( !StrLen( str ) )
			{
				StrCopy( str, "Waiting for registration..." );		
			}
			
			DrawStatusStr( str, &rect, stdFont, hAlignCenter, vAlignMiddle, color, true );
			
			handled = true; 
					
			break;
			
		default:
			break;
		
	}
	
	return ( handled );
	
} // LocationGadgetHandler()

/* 
 * ProfileGadgetHandler()
 */
Boolean ProfileGadgetHandler( FormGadgetTypeInCallback *gadgetP, UInt16 cmd, void *paramP )
{
	Boolean 				handled = false;
	FormType*				pForm = FrmGetActiveForm();
	RectangleType			rect = gadgetP->rect;
	color_e 				color = colorRed;
	attachedProfile_t 		lastProfile;
	appPrefs_t				prefs;
	UInt16 					cardNo = 0;
	LocalID 				dbID = 0;
	UInt32					ref = 0;
	UInt32					nextAlarmTime = 0;
	UInt32					nextAlarmSecs = 0;
	Char					strTime[STRLEN_PROFILE_NAME] = "\0";	
	Char					str[StrLen( "Profile: " ) + STRLEN_PROFILE_NAME + StrLen( "  9999999d 99h 99m" ) ];
	static UInt32			lastProfileID = -1;
	
	if ( gadgetP->id != MAIN_PROFILE_GDT ) return ( handled );
	
	switch ( cmd )
	{
		case formGadgetDeleteCmd:

			handled = true;

			break;
			
		case formGadgetEraseCmd:

			handled = false;

			break;		
		
		case formGadgetHandleEventCmd:
		
			if ( ((EventType *) paramP)->eType == frmGadgetEnterEvent )
			{
				ReadAppPrefs( &prefs );
	
				if ( prefs.bEnabled )
				{		
					ReadLastProfilePrefs( &lastProfile );
					
					if ( ( lastProfile.profileID ) 
							&& ( lastProfile.profileIdx != dmMaxRecordIndex ) )
					{
						BlinkRectangle( &rect );
						
						gProfileIdx = lastProfile.profileIdx;
						FrmPopupForm( PROFILE_EDIT_FORM );
					}
				
					handled = true;
					
					break;
				}
			}
			
			// NO break;
			
		case formGadgetDrawCmd:
	
			FrmSetActiveForm( pForm );
			
			ReadAppPrefs( &prefs );
			
			if ( !prefs.bEnabled )
			{
				DrawStatusStr( str, &rect, stdFont, hAlignCenter, vAlignMiddle, color, false );
				
				lastProfileID = -1;
			}
			else
			{
				gadgetP->attr.visible = true;
										
				ReadLastProfilePrefs( &lastProfile );
				
				if ( !SysCurAppDatabase( &cardNo, &dbID ) )
				{
					if ( ( nextAlarmTime = AlmGetAlarm( cardNo, dbID, &ref ) ) != 0 )
					{
						nextAlarmSecs = nextAlarmTime - TimGetSeconds();
						
						if ( nextAlarmSecs > 0 )
						{
							GetStrDelayTime( nextAlarmSecs, strTime );
						}
					}
				}
					
				if ( StrLen( lastProfile.profileName ) )
				{
					StrCopy( str, "Profile: ");
					StrCat( str, lastProfile.profileName );
					StrCat( str, strTime );			
				
					DrawStatusStr( str, &rect, stdFont, hAlignCenter, vAlignMiddle, color, true );
					
				}
				
				lastProfileID = lastProfile.profileID;
			}
			
			handled = true; 
					
			break;
			
		default:
			break;
		
	}
	
	return ( handled );
	
} // ProfileGadgetHandler()

/* 
 * SignalLvlGadgetHandler()
 */
Boolean SignalLvlGadgetHandler( FormGadgetTypeInCallback *gadgetP, UInt16 cmd, void *paramP )
{
	Boolean 				handled = false;
	FormType*				pForm = FrmGetActiveForm();
	RectangleType			rect = gadgetP->rect;
	UInt16					signalLvl = 0;
	Char					strSignalLvl[maxStrIToALen] = "\0";
	Coord 					posX = 0;
	Coord 					posY = 0;
	Int16					strHeight = 0;
	Int16					strWidth = 0;	
    RGBColorType 			rgb;
    IndexedColorType		colorIdx;
	UInt16					bmpID = BMP_ICON_SIGNALLVL_GADGET;
	MemHandle 				bmpH = NULL;
	BitmapType* 			bmpP = NULL;
	Coord					bmpSizeX = 0;
	Coord					bmpSizeY = 0;
	static UInt16			oldSignalLvl = -1;
	
	if ( gadgetP->id != MAIN_SIGNALLVL_GDT ) return ( handled );
	
	switch ( cmd )
	{
		case formGadgetDeleteCmd:

			handled = true;

			break;
			
		case formGadgetEraseCmd:

			handled = false;

			break;		
		
		case formGadgetHandleEventCmd:
		case formGadgetDrawCmd:
	
			FrmSetActiveForm( pForm );
			
			gadgetP->attr.visible = true;
							
			if ( !GetSignalInfo( &signalLvl, strSignalLvl ) )
			{	
				if ( oldSignalLvl == signalLvl ) break;
				
				oldSignalLvl = signalLvl;
				
				WinPushDrawState();
				
				bmpP = GetBitmap( bmpID, &bmpSizeX, &bmpSizeY, &bmpH );
				WinDrawBitmap( bmpP, rect.topLeft.x, rect.topLeft.y );
				ReleaseBitmap( &bmpH );	
				bmpH = NULL;
				bmpID = 0;
							
				rgb.r=221; rgb.g=221; rgb.b=221; // Red
				colorIdx = WinRGBToIndex( &rgb );	
	    		WinSetBackColor( colorIdx );
					
				rgb.r=250; rgb.g=0; rgb.b=51; // Red
				colorIdx = WinRGBToIndex( &rgb );	
	    		WinSetTextColor( colorIdx );
	    
				FntSetFont( stdFont );
				strHeight = FntLineHeight();
				strWidth = FntLineWidth( strSignalLvl, StrLen( strSignalLvl ) );
		
				posX = rect.topLeft.x + ( ( rect.extent.x - strWidth ) / 2 );
				posY = rect.topLeft.y + ( ( rect.extent.y - strHeight ) / 2 );
		
				WinDrawChars( strSignalLvl, StrLen( strSignalLvl ), posX, posY );
		
				WinPopDrawState();
			}
							
			handled = true; 
					
			break;
			
		default:
			break;
		
	}
	
	return ( handled );
	
} // SignalLvlGadgetHandler()

/* 
 * RoamingGadgetHandler()
 */
Boolean RoamingGadgetHandler( FormGadgetTypeInCallback *gadgetP, UInt16 cmd, void *paramP )
{
	Boolean 				handled = false;
	FormType*				pForm = FrmGetActiveForm();
	RectangleType			rect = gadgetP->rect;
	MemHandle 				bmpH = NULL;
	BitmapType* 			bmpP = NULL;
	Coord					bmpSizeX = 0;
	Coord					bmpSizeY = 0;
	Boolean					roamingStatus = false;
	static Boolean			oldRoamingStatus = false;
	
	if ( gadgetP->id != MAIN_ROAMING_GDT ) return ( handled );
	
	switch ( cmd )
	{
		case formGadgetDeleteCmd:

			handled = true;

			break;
			
		case formGadgetEraseCmd:

			handled = false;

			break;		
		
		case formGadgetHandleEventCmd:
		case formGadgetDrawCmd:
	
			FrmSetActiveForm( pForm );
			
			gadgetP->attr.visible = true;
			
			roamingStatus = IsRoaming();
							
			if ( oldRoamingStatus == roamingStatus ) break;
			
			oldRoamingStatus = roamingStatus;

			WinPushDrawState();
						
			if ( roamingStatus )
			{	
				bmpP = GetBitmap( BMP_ICON_ROAMING, &bmpSizeX, &bmpSizeY, &bmpH );
				WinDrawBitmap( bmpP, rect.topLeft.x + ( ( rect.extent.x - bmpSizeX ) / 2 ), 
										rect.topLeft.y + ( ( rect.extent.y - bmpSizeY ) / 2 ) );
				ReleaseBitmap( &bmpH );	
				bmpH = NULL;					
			}
			else
			{
				WinEraseRectangle( &rect, 0 );
			}
				
			WinPopDrawState();
						
			handled = true; 
					
			break;
			
		default:
			break;
		
	}
	
	return ( handled );
	
} // RoamingGadgetHandler()

/*
 * UpdateGadgets()
 */
void UpdateGadgets( void )
{
	EventType		event;
	
	event.eType = frmGadgetMiscEvent;
	event.data.gadgetMisc.dataP = NULL;
	
	event.data.gadgetMisc.gadgetID = MAIN_LOCATION_GDT;
	EvtAddEventToQueue( &event );
	
	event.data.gadgetMisc.gadgetID = MAIN_PROFILE_GDT;
	EvtAddEventToQueue( &event );
	
	event.data.gadgetMisc.gadgetID = MAIN_SIGNALLVL_GDT;
	EvtAddEventToQueue( &event );

	event.data.gadgetMisc.gadgetID = MAIN_ROAMING_GDT;
	EvtAddEventToQueue( &event );
	
	return;	
	
} // UpdateGadgets()

/* 
 * BlinkRectangle()
 */
static void BlinkRectangle( RectangleType* rectP )
{
	if ( !rectP ) return;
	
	SndPlaySystemSound( sndClick );
						
	WinInvertRectangle( rectP, 2 );
	SysTaskDelay( SysTicksPerSecond() / 2 );
	WinInvertRectangle( rectP, 2 );
	
	return;
		
} // BlinkRectangle()

/*
 * Gadgets.c
 */