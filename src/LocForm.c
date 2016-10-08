/*
 * LocForm.c
 */
 
#include "LocForm.h"

// Prototypes
static void 			InitializeLocForm( FormType* pForm, UInt16* idxP, locations_t* pLoc ) EXTRA_SECTION_ONE;
static void 			CopyAndSaveLocInfo( FormType* pForm, UInt16* idxP, locations_t* pLoc ) EXTRA_SECTION_ONE;

/*
 * LocFormHandleEvent()
 */
Boolean LocFormHandleEvent( EventType* pEvent )
{
	Boolean					handled = false;
	FormType* 				pForm = FrmGetActiveForm();
	Boolean					bFound = false;
	static UInt16			idx = 0;
	static locations_t		loc;
	static msg_t			msg;
	static TimeType 		startTime = { -1, -1 };
	static TimeType 		endTime = { -1, -1 };
	static UInt32			regionID = 0;
	static Char				regionName[STRLEN_PROFILE_NAME] = "\0";
	regions_t				region;
	UInt16					regionIdx;
	ListType*				pRegionsList = FrmGetObjectPtr( pForm, FrmGetObjectIndex( pForm, LOC_REGION_SEL_LST ) );
	EventType				newEvent;
	
	switch ( pEvent->eType )
	{		
		case frmOpenEvent:
				
			idx = gLocFormOpenIdx; 
			
			gLocForm = NULL;
			gLocP = NULL;
						
			InitializeLocForm( pForm, &idx, &loc ); // loc loaded here!!!
			
			msg = loc.Msg;
			startTime = loc.LocAlert.startTime;
			endTime = loc.LocAlert.endTime;
			regionID = loc.RegionInfo.regionID;
			StrCopy( regionName, loc.RegionInfo.name );
			
			FrmDrawForm( pForm );
			
			handled = true;
						
			break;
		
		case frmCloseEvent:
			
			gLocForm = NULL;
			gLocP = NULL;
			
			// NO handled = true;
			
			break;
			
		case popSelectEvent:
		
			if ( pEvent->data.popSelect.controlID == LOC_PHN_OPT_POP )
			{
				if ( pEvent->data.popSelect.selection > 0 )
				{
					handled = ProfilesSetPopupTextFromList( pEvent->data.popSelect.controlP, pEvent->data.popSelect.listP, pEvent->data.popSelect.selection );					
				}
			}
			break;
			
		case ctlSelectEvent:
		
			switch ( pEvent->data.ctlSelect.controlID )
			{
				case LOC_REGION_SEL_BTN:
					
					LoadRegionsList( pRegionsList, true );
					
					// get idx
					region.uniqueID = regionID;
					if ( !RegionsFindRecord( &region, &regionIdx, true, &bFound, (DmComparF *) &Regions_CompareUniqueIDF ) 
							&& ( bFound ) )
					{
						++regionIdx;
						LstSetSelection( pRegionsList, regionIdx );
					}

					regionIdx = LstPopupList( pRegionsList );
					
					switch ( regionIdx )
					{
						case -1:
							// nothing			
							break;

						case 0:
							
							regionID = loc.RegionInfo.regionID = 0;
							regionName[0] = chrNull;
							
							break;
														
						default:

							--regionIdx;
							
							if ( !RegionsGetRecordAtIdx( &regionIdx, &region ) )
							{
								regionID = loc.RegionInfo.regionID = region.uniqueID;
								StrCopy( regionName, region.name );
								StrCopy( loc.RegionInfo.name, region.name );
							}

							break;						
					}
					
					SetFrmView( pForm, &loc );
					
					FldDrawField( FrmGetObjectPtr( pForm, FrmGetObjectIndex( pForm, LOC_DESC_FLD ) ) );
					FldDrawField( FrmGetObjectPtr( pForm, FrmGetObjectIndex( pForm, LOC_NOTE_FLD ) ) );
					
					FrmNavObjectTakeFocus( pForm, LOC_REGION_SEL_BTN );
					 				
					handled = true;
					
					break;
					
				case LOC_REGION_EDIT_BTN:
				
					region.uniqueID = loc.RegionInfo.regionID;
				
					if ( !RegionsFindRecord( &region, &regionIdx, true, &bFound, (DmComparF *) &Regions_CompareUniqueIDF ) 
							&& ( bFound ) )
					{
						gLocForm = pForm;
						gLocP = &loc;
						gRegionIdx = regionIdx;
						FrmPopupForm( REGION_EDIT_FORM );
					}
				
					handled = true;
					
					break;
					
				case LOC_TIME_INTERVAL_SEL:
					{
						Boolean			bUntimed = ( ( *( (UInt16 *) &startTime ) == noTime ) || ( *( (UInt16 *) &endTime ) == noTime ) );
						
						if ( bUntimed ) 
						{
							startTime.hours = startTime.minutes = 0;
							endTime.hours = endTime.minutes = 0;
						}
						
						if ( SelectTime( (TimeType *) &startTime, (TimeType *) &endTime, bUntimed, "Centroid [Alert Time]", 8, 18, 9 ) )
						{
							Char			strTimeInterval[STRLEN_TIME_INTERVAL] = "\0";
							
							loc.LocAlert.startTime = startTime;
							loc.LocAlert.endTime = endTime;
									
							MakeTimeInterval( startTime, endTime, strTimeInterval );
							SetCtlLabelStr( pForm, pEvent->data.ctlSelect.controlID, strTimeInterval );
							
							FrmShowObject( pForm, FrmGetObjectIndex( pForm, pEvent->data.ctlSelect.controlID ) );
							// CtlDrawControl( pEvent->data.ctlSelect.pControl );
						}
						else if ( bUntimed ) 
						{
							startTime.hours = startTime.minutes = -1;
							endTime.hours = endTime.minutes = -1;
						}
					}
					handled = true;
					
					break;
					
				case LOC_OK_BTN:
					
					if ( GIDFindRecord( &loc, &idx, true, &bFound, (DmComparF *) &GID_CompareF ) == errNone )
					{	
						if ( bFound )
						{
							loc.Msg = msg;	
							loc.LocAlert.startTime = startTime;
							loc.LocAlert.endTime = endTime;
							loc.RegionInfo.regionID = regionID;
							StrCopy( loc.RegionInfo.name, regionName );
							
						 	CopyAndSaveLocInfo( pForm, &idx, &loc );
						}
					}
					
					if ( !bFound )
					{
						FrmCustomAlert( ErrOKAlert, "Something wrong... Could not save record.", "", "" );
					}
				
					if ( gHasNewGlobals )
					{
						newEvent.eType = appStopEvent;						
						EvtAddEventToQueue( &newEvent );
					}
					else
					{
						FrmReturnToForm( 0 );
						RedrawTable( gMainFormP, gGIDTableP );
					}
					
					handled = true;

					break;

				case LOC_CANCEL_BTN:
					
					if ( gHasNewGlobals )
					{
						newEvent.eType = appStopEvent;						
						EvtAddEventToQueue( &newEvent );							
					}
					else
					{
						FrmReturnToForm( 0 );	
						DrawFocusRing( gMainFormP, gGIDTableP );
					}
				
					handled = true;
					
					break;
				
				case LOC_MESSAGING_BTN:
					
					gMsgP = &(msg);
					FrmPopupForm( MSG_FORM );
					
					handled = true;
					
					break;
					
				case LOC_DELETE_BTN:
				
					if ( FrmCustomAlert( ConfirmationOKCancelAlert, "Delete record?", "", "" ) == 0 )
					{
						if ( GIDFindRecord( &loc, &idx, false, &bFound, (DmComparF *) &GID_CompareF ) == errNone )
						{	
							if ( bFound )
							{	
								GIDRemoveRecordAtIdx( &idx );
								gSelectedRow = -1; // invalidate
							}							
						}
						
						if ( !bFound )
						{
							FrmCustomAlert( ErrOKAlert, "Something wrong... Could not delete record.", "", "" );
						}
					}
					
					if ( gHasNewGlobals )
					{
						newEvent.eType = appStopEvent;						
						EvtAddEventToQueue( &newEvent );	
					}
					else
					{
						FrmReturnToForm( 0 );
						RedrawTable( gMainFormP, gGIDTableP );
					}
					
					handled = true;
					
					break;
					
				case LOC_GEO_LOC_BTN:
				
					gGID = loc.GID;
					
					FrmPopupForm( GEO_INFO_FORM );
				
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
	
} // LocFormHandleEvent()

/*
 * InitializeLocForm()
 */
static void InitializeLocForm( FormType* pForm, UInt16* idxP, locations_t* pLoc )
{
	Char			strGID[STRLEN_GID] = "\0";
	Char*			strDateTime = NULL;
	Char			strVisits[maxStrIToALen] = "\0";
	Char*			strDateTimeVisits = NULL;
	
	if ( GIDGetRecordAtIdx( idxP, pLoc ) == errNone )
	{
		if ( !( pLoc->isRoaming ) )
		{
			FrmHideObject( pForm, FrmGetObjectIndex( pForm, BMP_ICON_ROAMING ) );	
		}
		
		MakeGIDString( &( pLoc->GID ), strGID );
		SetLabelStr( pForm, LOC_GID_LBL, strGID );
		
		MakeTimeDateStr( pLoc->TimeAtLoc, &strDateTime, NULL, NULL, false );
		StrIToA( strVisits, pLoc->NumVisits );

		strDateTimeVisits = MemPtrNew ( StrLen( strDateTime ) + StrLen( strVisits ) + StrLen( "  []" ) + 1 );
		
		if ( strDateTimeVisits )
		{
			MemSet( strDateTimeVisits, StrLen( strDateTime ) + StrLen( strVisits ) + StrLen( "  []" ) + 1, 0 );
			
			StrCopy( strDateTimeVisits, strDateTime );
			StrCat( strDateTimeVisits, "  [" );
			StrCat( strDateTimeVisits, strVisits );
			StrCat( strDateTimeVisits, "]" );
			
			SetLabelStr( pForm, LOC_TIME_LBL, strDateTimeVisits );
		}
	
		SetFrmView( pForm, pLoc );
	}

	if ( gHasNewGlobals )
	{
		FrmHideObject( pForm, FrmGetObjectIndex( pForm, LOC_DELETE_BTN ) );
	}
							
	FrmSetMenu( pForm, MENU_EDIT );
	
	if ( strDateTime ) MemPtrFree( strDateTime );
	
	if ( strDateTimeVisits ) MemPtrFree( strDateTimeVisits );
	
	return;
	
} // InitializeLocForm()
	
/*
 * CopyAndSaveLocInfo()
 */
static void CopyAndSaveLocInfo( FormType* pForm, UInt16* idxP, locations_t* pLoc )
{	
	ListType*			pList = FrmGetObjectPtr( pForm, FrmGetObjectIndex( pForm, LOC_PHN_OPT_LST ) );
	profiles_t			profile;
	
	pLoc->isNewLocation = false;
	
	if ( !pLoc->RegionInfo.regionID )
	{
		pLoc->LocAlert.bAlertOnEntry = GetCtlBoolValue( pForm, LOC_ALERT_ON_ENTRY_CBX );	
		pLoc->LocAlert.bAlertOnExit = GetCtlBoolValue( pForm, LOC_ALERT_ON_EXIT_CBX );
		pLoc->LocAlert.bDisableAfterAlert = GetCtlBoolValue( pForm, LOC_DISABLE_ALERT_CBX );
		
		GetFieldStr( pForm, LOC_DESC_FLD, pLoc->LocationName, STRLEN_LOCATION_NAME );
		GetFieldStr( pForm, LOC_NOTE_FLD, pLoc->LocAlert.note, STRLEN_ALERT_NOTE );
		
		if ( pList )
		{
			pLoc->AttachedProfile.profileIdx = LstGetSelection( pList );
	
			if ( pLoc->AttachedProfile.profileIdx )
			{
				--( pLoc->AttachedProfile.profileIdx ); // adjust for "- None -"
				
				MemSet( &profile, sizeof( profiles_t ), 0 );
				
				if ( ! ProfilesGetRecordAtIdx( &( pLoc->AttachedProfile.profileIdx ), &profile ) )
				{
					pLoc->AttachedProfile.profileID = profile.uniqueID;
					StrCopy( pLoc->AttachedProfile.profileName, profile.name);
					pLoc->AttachedProfile.profileValidity = profile.validitySecs;
				}
			}
			else
			{
				MemSet( &(pLoc->AttachedProfile), sizeof( attachedProfile_t ), 0 );
			}
		}	
	}
	
	GIDReplaceRecordAtIdx( idxP, pLoc, false );
	
	return;
		
} // CopyAndSaveLocInfo()

/*
 * SetFrmView()
 */
void SetFrmView( FormType* pForm, locations_t* pLoc )
{
	Boolean 		bFound = false;
	UInt16			regionIdx = dmMaxRecordIndex;
	regions_t		region;
	Char			strTimeInterval[STRLEN_TIME_INTERVAL] = "\0";
	ListType*		pList = FrmGetObjectPtr( pForm, FrmGetObjectIndex( pForm, LOC_PHN_OPT_LST ) );
	ControlType*	pCtl = FrmGetObjectPtr( pForm, FrmGetObjectIndex( pForm, LOC_PHN_OPT_POP ) );
	
	region.uniqueID = pLoc->RegionInfo.regionID;
	
	if ( pList )
	{
		LoadProfilesList( pList, true );
		LstSetDrawFunction( pList, (ListDrawDataFuncPtr) ProfilesListDrawFunction );
		LstSetSelection( pList, 0 ); // preset
	}
		
	if ( ( region.uniqueID )
			&& ( !RegionsFindRecord( &region, &regionIdx, true, &bFound, (DmComparF *) &Regions_CompareUniqueIDF ) )
			&& ( bFound ) )
	{
		SetFieldStr( pForm, LOC_DESC_FLD, region.name );	
		SetCtlBoolValue( pForm, LOC_ALERT_ON_ENTRY_CBX, region.LocAlert.bAlertOnEntry );
		SetCtlBoolValue( pForm, LOC_ALERT_ON_EXIT_CBX, region.LocAlert.bAlertOnExit );	
		SetCtlBoolValue( pForm, LOC_DISABLE_ALERT_CBX, region.LocAlert.bDisableAfterAlert );
		MakeTimeInterval( region.LocAlert.startTime, region.LocAlert.endTime, strTimeInterval );
		SetCtlLabelStr( pForm, LOC_TIME_INTERVAL_SEL, strTimeInterval );
		SetFieldStr( pForm, LOC_NOTE_FLD, region.LocAlert.note );			
		SetListFromProfileID( pList, region.AttachedProfile.profileID );
		
		FieldSetEditable( pForm, LOC_DESC_FLD, false );
		SetCtlEnabled( pForm, LOC_ALERT_ON_ENTRY_CBX, false );
		SetCtlEnabled( pForm, LOC_ALERT_ON_EXIT_CBX, false );
		SetCtlEnabled( pForm, LOC_DISABLE_ALERT_CBX, false );
		SetCtlEnabled( pForm, LOC_TIME_INTERVAL_SEL, false );
		FieldSetEditable( pForm, LOC_NOTE_FLD, false );
		SetCtlEnabled( pForm, LOC_PHN_OPT_POP, false );
		SetCtlEnabled( pForm, LOC_MESSAGING_BTN, false );
		
		FrmShowObject( pForm, FrmGetObjectIndex( pForm, LOC_REGION_EDIT_BTN ) );
		FrmShowObject( pForm, FrmGetObjectIndex( pForm, BMP_ICON_REGION_ASSIGNED ) );
		FrmShowObject( pForm, FrmGetObjectIndex( pForm, BMP_ICON_REGION_ASSIGNED + 1 ) );
		FrmShowObject( pForm, FrmGetObjectIndex( pForm, BMP_ICON_REGION_ASSIGNED + 2 ) );
	}
	else
	{
		SetFieldStr( pForm, LOC_DESC_FLD, pLoc->LocationName );
		SetCtlBoolValue( pForm, LOC_ALERT_ON_ENTRY_CBX, pLoc->LocAlert.bAlertOnEntry );
		SetCtlBoolValue( pForm, LOC_ALERT_ON_EXIT_CBX, pLoc->LocAlert.bAlertOnExit );	
		SetCtlBoolValue( pForm, LOC_DISABLE_ALERT_CBX, pLoc->LocAlert.bDisableAfterAlert );
		MakeTimeInterval( pLoc->LocAlert.startTime, pLoc->LocAlert.endTime, strTimeInterval );
		SetCtlLabelStr( pForm, LOC_TIME_INTERVAL_SEL, strTimeInterval );
		SetFieldStr( pForm, LOC_NOTE_FLD, pLoc->LocAlert.note );			
		SetListFromProfileID( pList, pLoc->AttachedProfile.profileID );
		
		FieldSetEditable( pForm, LOC_DESC_FLD, true );
		SetCtlEnabled( pForm, LOC_ALERT_ON_ENTRY_CBX, true );
		SetCtlEnabled( pForm, LOC_ALERT_ON_EXIT_CBX, true );
		SetCtlEnabled( pForm, LOC_DISABLE_ALERT_CBX, true );
		SetCtlEnabled( pForm, LOC_TIME_INTERVAL_SEL, true );
		FieldSetEditable( pForm, LOC_NOTE_FLD, true );
		SetCtlEnabled( pForm, LOC_PHN_OPT_POP, true );
		SetCtlEnabled( pForm, LOC_MESSAGING_BTN, true );
		
		FrmHideObject( pForm, FrmGetObjectIndex( pForm, LOC_REGION_EDIT_BTN ) );
		FrmHideObject( pForm, FrmGetObjectIndex( pForm, BMP_ICON_REGION_ASSIGNED ) );
		FrmHideObject( pForm, FrmGetObjectIndex( pForm, BMP_ICON_REGION_ASSIGNED + 1 ) );
		FrmHideObject( pForm, FrmGetObjectIndex( pForm, BMP_ICON_REGION_ASSIGNED + 2 ) );
		FrmShowObject( pForm, FrmGetObjectIndex( pForm, BMP_BG_SQUARE ) );
		FrmShowObject( pForm, FrmGetObjectIndex( pForm, BMP_BG_SQUARE + 1) ); 
		FrmShowObject( pForm, FrmGetObjectIndex( pForm, BMP_ICON_PROFILE ) ); 
		FrmShowObject( pForm, FrmGetObjectIndex( pForm, BMP_ICON_ALERT ) );
		FrmShowObject( pForm, FrmGetObjectIndex( pForm, LOC_MESSAGING_BTN ) );
	}

	if ( ( pList ) && ( pCtl ) )
	{
		ProfilesSetPopupTextFromList( pCtl, pList, LstGetSelection( pList ) );
	}
	
	return;
		
} // SetFrmView()

/*
 * LocForm.c
 */