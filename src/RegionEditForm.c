/*
 * RegionEditForm.c
 */
 
#include "RegionEditForm.h"

static void 			InitializeRegionEditForm( FormType* pForm, UInt16* idxP, regions_t* pRegion ) EXTRA_SECTION_ONE;
static Boolean 			CopyAndSaveRegionEditForm( FormType* pForm, UInt16* idxP, regions_t* pRegion ) EXTRA_SECTION_ONE;
static void 			RedrawRegionsList( void ) EXTRA_SECTION_ONE;
static void 			RedrawLocEditForm( void ) EXTRA_SECTION_ONE;

/*
 * RegionEditFormHandleEvent()
 */
Boolean RegionEditFormHandleEvent( EventType* pEvent )
{
	Boolean					handled = false;
	FormType* 				pForm = FrmGetActiveForm();
	static UInt16			idx = 0;
	static regions_t		region;
	static TimeType 		startTime = { -1, -1 };
	static TimeType 		endTime = { -1, -1 };
	
	switch ( pEvent->eType )
	{		
		case frmOpenEvent:
			
			idx = gRegionIdx;
				
			InitializeRegionEditForm( pForm, &idx, &region ); // loc loaded here!!!
					
			handled = true;
						
			break;
			
		case popSelectEvent:
		
			if ( pEvent->data.popSelect.controlID == REGION_EDIT_PHN_OPT_POP )
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
				case REGION_EDIT_TIME_INTERVAL_SEL:
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
							
							region.LocAlert.startTime = startTime;
							region.LocAlert.endTime = endTime;
									
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
				
				case REGION_EDIT_OK_BTN:

					region.LocAlert.startTime = startTime;
					region.LocAlert.endTime = endTime;
							
					if ( CopyAndSaveRegionEditForm( pForm, &idx, &region ) )
					{
						FrmReturnToForm( 0 );
						
						RedrawRegionsList();
						RedrawLocEditForm();
					}
					
					handled = true;

					break;

				case REGION_EDIT_CANCEL_BTN:
				
					FrmReturnToForm( 0 );
			
					RedrawRegionsList();
					RedrawLocEditForm();
					
					handled = true;
					
					break;
					
				case REGION_EDIT_MESSAGING_BTN:
					
					gMsgP = &(region.Msg);
					FrmPopupForm( MSG_FORM );
					
					handled = true;
					
					break;
					
				case REGION_EDIT_DELETE_BTN:
				
					if ( gRegionIdx == noListSelection ) break;
					
					if ( FrmCustomAlert( ConfirmationOKCancelAlert, "Delete selected record?", "", "" ) == 0 )
					{
						if ( RegionsRemoveRecordAtIdx( &gRegionIdx ) != errNone )
						{
							FrmCustomAlert( ErrOKAlert, "Unable to delete record.", "", "" );	
						}
						else
						{
							FrmReturnToForm( 0 );
							
							RedrawRegionsList();
							RedrawLocEditForm();
						}
					}
				
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
	
} // RegionEditFormHandleEvent()

/*
 * InitializeRegionEditForm()
 */
static void InitializeRegionEditForm( FormType* pForm, UInt16* idxP, regions_t* pRegion )
{
	ListType*		pList = FrmGetObjectPtr( pForm, FrmGetObjectIndex( pForm, REGION_EDIT_PHN_OPT_LST ) );
	ControlType*	pCtl = FrmGetObjectPtr( pForm, FrmGetObjectIndex( pForm, REGION_EDIT_PHN_OPT_POP ) );
	Char			strTimeInterval[STRLEN_TIME_INTERVAL] = "\0";
	UInt16			numGIDs = 0;
	Char			strNumGIDs[maxStrIToALen] = "None";
	
	if ( pList )
	{
		LoadProfilesList( pList, true );
		LstSetDrawFunction( pList, (ListDrawDataFuncPtr) ProfilesListDrawFunction );
		LstSetSelection( pList, 0 ); // preset
	}
	
	if ( gRegionIdx == noListSelection )
	{
		MemSet( pRegion, sizeof( regions_t ), 0 );
		
		pRegion->uniqueID = TimGetSeconds();
		pRegion->LocAlert.startTime.hours = pRegion->LocAlert.startTime.minutes = -1;
		pRegion->LocAlert.endTime.hours = pRegion->LocAlert.endTime.minutes = -1;
	}
	else
	{	
		RegionsGetRecordAtIdx( idxP, pRegion );
		numGIDs = RegionGetNumberOfGIDs( pRegion->uniqueID );
		if ( numGIDs )
		{
			StrIToA( strNumGIDs, (Int32) numGIDs );
		}
	}
	
	SetFieldStr( pForm, REGION_EDIT_NAME_FLD, pRegion->name );
	SetLabelStr( pForm, REGION_EDIT_GID_COUNT, strNumGIDs );
		
	SetCtlBoolValue( pForm, REGION_EDIT_ALERT_ON_ENTRY_CBX, pRegion->LocAlert.bAlertOnEntry );
	SetCtlBoolValue( pForm, REGION_EDIT_ALERT_ON_EXIT_CBX, pRegion->LocAlert.bAlertOnExit );	
	SetCtlBoolValue( pForm, REGION_EDIT_DISABLE_ALERT_CBX, pRegion->LocAlert.bDisableAfterAlert );

	MakeTimeInterval( pRegion->LocAlert.startTime, pRegion->LocAlert.endTime, strTimeInterval );
	SetCtlLabelStr( pForm, REGION_EDIT_TIME_INTERVAL_SEL, strTimeInterval );
	
	SetFieldStr( pForm, REGION_EDIT_NOTE_FLD, pRegion->LocAlert.note );
		
	SetListFromProfileID( pList, pRegion->AttachedProfile.profileID );
	
	if ( ( pList ) && ( pCtl ) )
	{
		ProfilesSetPopupTextFromList( pCtl, pList, LstGetSelection( pList ) );
	}
	
	FrmDrawForm( pForm );
	
	if ( ( gRegionIdx == noListSelection )
			|| ( gLocForm ) )
	{
		FrmHideObject( pForm, FrmGetObjectIndex( pForm, REGION_EDIT_DELETE_BTN ) );
	}
	
	FrmSetMenu( pForm, MENU_EDIT );
	
	return;
	
} // InitializeRegionEditForm()

	
/*
 * CopyAndSaveRegionEditForm()
 */
static Boolean CopyAndSaveRegionEditForm( FormType* pForm, UInt16* idxP, regions_t* pRegion )
{	
	Boolean 		retVal = false;
	Boolean			bFound = false;
	ListType*		pList = FrmGetObjectPtr( pForm, FrmGetObjectIndex( pForm, REGION_EDIT_PHN_OPT_LST ) );
	profiles_t		profile;
	
	GetFieldStr( pForm, REGION_EDIT_NAME_FLD, pRegion->name, STRLEN_PROFILE_NAME );
	
	if ( !StrLen( pRegion->name ) )
	{
		FrmCustomAlert( ErrOKAlert, "Zone name is empty.", "", "" );	
	}
	else
	{
		pRegion->LocAlert.bAlertOnEntry = GetCtlBoolValue( pForm, REGION_EDIT_ALERT_ON_ENTRY_CBX );	
		pRegion->LocAlert.bAlertOnExit = GetCtlBoolValue( pForm, REGION_EDIT_ALERT_ON_EXIT_CBX );
		pRegion->LocAlert.bDisableAfterAlert = GetCtlBoolValue( pForm, REGION_EDIT_DISABLE_ALERT_CBX );
		
		GetFieldStr( pForm, REGION_EDIT_NOTE_FLD, pRegion->LocAlert.note, STRLEN_ALERT_NOTE );
		
		if ( pList )
		{
			pRegion->AttachedProfile.profileIdx = LstGetSelection( pList );
	
			if ( pRegion->AttachedProfile.profileIdx )
			{
				--( pRegion->AttachedProfile.profileIdx ); // adjust for "- None -"
				
				MemSet( &profile, sizeof( profiles_t ), 0 );
				
				if ( ! ProfilesGetRecordAtIdx( &( pRegion->AttachedProfile.profileIdx ), &profile ) )
				{
					pRegion->AttachedProfile.profileID = profile.uniqueID;
					StrCopy( pRegion->AttachedProfile.profileName, profile.name);
					pRegion->AttachedProfile.profileValidity = profile.validitySecs;
				}
			}
			else
			{
				MemSet( &(pRegion->AttachedProfile), sizeof( attachedProfile_t ), 0 );
			}
		}	

		if ( ( RegionsFindRecord( pRegion, &gRegionIdx, false, &bFound, (DmComparF *) &Regions_CompareUniqueIDF ) == errNone )
				&& ( bFound ) )
		{
			retVal = ( RegionsReplaceRecordAtIdx( &gRegionIdx, pRegion, false ) == errNone );
		}
		else
		{
			retVal = ( RegionsSaveNewRecord( pRegion, false, &gRegionIdx ) == errNone );
		}
		
		if ( !retVal )
		{
		 	FrmCustomAlert( ErrOKAlert, "Could not save record.", "", "" );	
		}
	}
	
	return ( retVal );
		
} // CopyAndSaveRegionEditForm()

/* 
 * RedrawRegionsList()
 */
static void RedrawRegionsList( void )
{
	if ( gRegionsListP )
	{
		LoadRegionsList( gRegionsListP, false );
										
		LstSetSelection( gRegionsListP, gRegionIdx );	
	
		LstDrawList( gRegionsListP );
		
		gRegionsListP = NULL;
	}
	
	return;	
	
} // RedrawRegionsList()

/* 
 * RedrawLocEditForm()
 */
static void RedrawLocEditForm( void )
{
	if ( ( gLocForm ) && ( gLocP ) )
	{
		SetFrmView( gLocForm, gLocP );
		
		gLocForm = NULL;
		gLocP = NULL;
	}
	
	return;	
	
} // RedrawLocEditForm()

/*
 * RegionEditForm.c
 */