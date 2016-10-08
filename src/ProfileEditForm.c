/*
 * ProfileEditForm.c
 */
 
#include "ProfileEditForm.h"

static void 			InitializeProfileEditForm( FormType* pForm, UInt16* idxP, profiles_t* pProfile ) EXTRA_SECTION_ONE;
static Boolean 			CopyAndSaveProfileEditInfo( FormType* pForm, UInt16* idxP, profiles_t* pProfile ) EXTRA_SECTION_ONE;
static void 			RedrawProfilesList( void ) EXTRA_SECTION_ONE;

/*
 * ProfileEditFormHandleEvent()
 */
Boolean ProfileEditFormHandleEvent( EventType* pEvent )
{
	Boolean					handled = false;
	FormType* 				pForm = FrmGetActiveForm();
	ListType*				pList = NULL;
	ControlType*			pCtl = NULL;
	static UInt16			idx = 0;
	static profiles_t 		profile;
	
	switch ( pEvent->eType )
	{		
		case frmOpenEvent:
			
			idx = gProfileIdx;
				
			InitializeProfileEditForm( pForm, &idx, &profile ); // loc loaded here!!!
					
			handled = true;
						
			break;
		
		case frmCloseEvent:
		
			// NO handled = true;
			
			break;
			
		case ctlSelectEvent:
		
			switch ( pEvent->data.ctlSelect.controlID )
			{
				case PROFILE_EDIT_DEFAULT_PBT:
					
					pList = FrmGetObjectPtr( pForm,  FrmGetObjectIndex( pForm, PROFILE_EDIT_VALIDITY_LST ) );
					pCtl = FrmGetObjectPtr( pForm,  FrmGetObjectIndex( pForm, PROFILE_EDIT_VALIDITY_POP ) );
					
					if ( ( pList ) && ( pCtl ) )
					{
						if ( pEvent->data.ctlSelect.on )
						{
							FrmHideObject ( pForm, FrmGetObjectIndex( pForm, PROFILE_EDIT_VALIDITY_POP ) );
						}
						else
						{
							FrmShowObject( pForm, FrmGetObjectIndex( pForm, PROFILE_EDIT_VALIDITY_POP ) );
						}
					}
					
					handled = true;
					
					break;
					
				case PROFILE_EDIT_OK_BTN:

					if ( CopyAndSaveProfileEditInfo( pForm, &idx, &profile ) )
					{
						FrmReturnToForm( 0 );
						
						RedrawProfilesList();
					}
					
					handled = true;

					break;

				case PROFILE_EDIT_CANCEL_BTN:
				
					FrmReturnToForm( 0 );
			
					handled = true;
					
					break;
					
				case PROFILE_EDIT_DELETE_BTN:
				
					if ( gProfileIdx == noListSelection ) break;
					
					if ( FrmCustomAlert( ConfirmationOKCancelAlert, "Delete selected record?", "", "" ) == 0 )
					{
						if ( ProfilesRemoveRecordAtIdx( &gProfileIdx ) != errNone )
						{
							FrmCustomAlert( ErrOKAlert, "Unable to delete record.", "", "" );	
						}
						else
						{
							FrmReturnToForm( 0 );
							
							RedrawProfilesList();
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
	
} // ProfileEditFormHandleEvent()

/*
 * InitializeProfileEditForm()
 */
static void InitializeProfileEditForm( FormType* pForm, UInt16* idxP, profiles_t* pProfile )
{
	ListType*			pList = NULL;
	ControlType* 		pCtl = NULL;
	UInt32				defaultUniqueID = ProfilesDBGetUniqueID();

	// Validity
	pList = FrmGetObjectPtr( pForm,  FrmGetObjectIndex( pForm, PROFILE_EDIT_VALIDITY_LST ) );
	LstSetSelection( pList, 0 );
	
	// TimeZones
	pList = FrmGetObjectPtr( pForm,  FrmGetObjectIndex( pForm, PROFILE_EDIT_TZ_LST ) );
	LoadTimeZoneListFromDB( pList );
	LstSetSelection( pList, 0 );
	
	// Themes
	pList = FrmGetObjectPtr( pForm,  FrmGetObjectIndex( pForm, PROFILE_EDIT_THEME_LST ) );
	LoadThemesListFromResource( pList );
	LstSetSelection( pList, 0 );
	
	if ( gProfileIdx == noListSelection )
	{
		pProfile->uniqueID = TimGetSeconds();
	}
	else
	{	
		if ( ProfilesGetRecordAtIdx( idxP, pProfile ) == errNone )
		{
			SetCtlBoolValue( pForm, PROFILE_EDIT_DEFAULT_PBT, ( defaultUniqueID == pProfile->uniqueID ) );
		
			SetFieldStr( pForm, PROFILE_EDIT_NAME_FLD, pProfile->name );	
		
			SetCtlBoolValue( pForm, PROFILE_EDIT_FWD_CALLS_CHK, pProfile->Settings.bFwdCalls );
			SetFieldStr( pForm, PROFILE_EDIT_FWD_CALLS_FLD, pProfile->Settings.strFwdCallsNum );
			
			SetCtlBoolValue( pForm, PROFILE_EDIT_MUTE_PHN_CHK, pProfile->Settings.Mute.bMutePhone );
			SetCtlBoolValue( pForm, PROFILE_EDIT_MUTE_MSG_CHK, pProfile->Settings.Mute.bMuteMessging );
			SetCtlBoolValue( pForm, PROFILE_EDIT_MUTE_CAL_CHK, pProfile->Settings.Mute.bMuteCalendar );
			SetCtlBoolValue( pForm, PROFILE_EDIT_MUTE_SYS_CHK, pProfile->Settings.Mute.bMuteSystem );
			SetCtlBoolValue( pForm, PROFILE_EDIT_MUTE_ALM_CHK, pProfile->Settings.Mute.bMuteAlarm );
			SetCtlBoolValue( pForm, PROFILE_EDIT_MUTE_GAM_CHK, pProfile->Settings.Mute.bMuteGame );
			SetCtlBoolValue( pForm, PROFILE_EDIT_MUTE_VIB_PBT, pProfile->Settings.Mute.bVibrateON );
			
			SetCtlBoolValue( pForm, PROFILE_EDIT_TZ_CHK, pProfile->Settings.bTimeZone);
			pList = FrmGetObjectPtr( pForm,  FrmGetObjectIndex( pForm, PROFILE_EDIT_TZ_LST ) );
			LstSetSelection( pList, FindTimeZoneIdx( pProfile->Settings.TimeZone.name ) );
			
			SetCtlBoolValue( pForm, PROFILE_EDIT_THEME_CHK, pProfile->Settings.bTheme );
			pList = FrmGetObjectPtr( pForm,  FrmGetObjectIndex( pForm, PROFILE_EDIT_THEME_LST ) );
			LstSetSelection( pList, FindThemeIdx( pProfile->Settings.Theme.name ) ); // ???
		}
	}
	
	FrmDrawForm( pForm );
	
	if ( gProfileIdx == noListSelection )
	{
		FrmHideObject( pForm, FrmGetObjectIndex( pForm, PROFILE_EDIT_DELETE_BTN ) );
	}
	else
	{		
		pList = FrmGetObjectPtr( pForm,  FrmGetObjectIndex( pForm, PROFILE_EDIT_VALIDITY_LST ) );
		pCtl = FrmGetObjectPtr( pForm,  FrmGetObjectIndex( pForm, PROFILE_EDIT_VALIDITY_POP ) );
		
		if ( ( pList ) && ( pCtl ) )
		{
			if ( defaultUniqueID == pProfile->uniqueID )
			{
				LstSetSelection( pList, 0 );			
				FrmHideObject( pForm, FrmGetObjectIndex( pForm, PROFILE_EDIT_VALIDITY_POP ) );
			}
			else
			{
				LstSetSelection( pList, pProfile->validityIdx );			
				FrmShowObject( pForm, FrmGetObjectIndex( pForm, PROFILE_EDIT_VALIDITY_POP ) );
			}
		}	
	}
	
	// Validity
	pList = FrmGetObjectPtr( pForm,  FrmGetObjectIndex( pForm, PROFILE_EDIT_VALIDITY_LST ) );
	pCtl = FrmGetObjectPtr( pForm,  FrmGetObjectIndex( pForm, PROFILE_EDIT_VALIDITY_POP ) );
	CtlSetLabel( pCtl, LstGetSelectionText( pList, LstGetSelection( pList ) ) );
	
	// TimeZone
	pList = FrmGetObjectPtr( pForm,  FrmGetObjectIndex( pForm, PROFILE_EDIT_TZ_LST ) );
	pCtl = FrmGetObjectPtr( pForm,  FrmGetObjectIndex( pForm, PROFILE_EDIT_TZ_POP ) );
	CtlSetLabel( pCtl, LstGetSelectionText( pList, LstGetSelection( pList ) ) );	
	
	// Themes
	pList = FrmGetObjectPtr( pForm,  FrmGetObjectIndex( pForm, PROFILE_EDIT_THEME_LST ) );
	pCtl = FrmGetObjectPtr( pForm,  FrmGetObjectIndex( pForm, PROFILE_EDIT_THEME_POP ) );
	CtlSetLabel( pCtl, LstGetSelectionText( pList, LstGetSelection( pList ) ) );
	
	return;
	
} // InitializeProfileEditForm()

	
/*
 * CopyAndSaveProfileEditInfo()
 */
static Boolean CopyAndSaveProfileEditInfo( FormType* pForm, UInt16* idxP, profiles_t* pProfile )
{	
	Boolean 		retVal = false;
	Boolean			bFound = false;
	ListType*		pList = NULL;
	UInt32			validitySecs[9] = { 0, 600, 1200, 1800, 3600, 5400, 7200, 10800, 21600 }; 
	
	GetFieldStr( pForm, PROFILE_EDIT_NAME_FLD, pProfile->name, STRLEN_PROFILE_NAME );
	
	if ( !StrLen( pProfile->name ) )
	{
		FrmCustomAlert( ErrOKAlert, "Profile name is empty.", "", "" );	
	}
	else
	{
		if ( GetCtlBoolValue( pForm, PROFILE_EDIT_DEFAULT_PBT ) )
		{
			pProfile->validityIdx = 0;
			pProfile->validitySecs = validitySecs[0];
		}
		else
		{
			pList = FrmGetObjectPtr( pForm,  FrmGetObjectIndex( pForm, PROFILE_EDIT_VALIDITY_LST ) );
			pProfile->validityIdx = LstGetSelection( pList );
			pProfile->validitySecs = validitySecs[LstGetSelection( pList )];
		}
/*		
		pList = FrmGetObjectPtr( pForm,  FrmGetObjectIndex( pForm, PROFILE_EDIT_VALIDITY_LST ) );
		pProfile->validityIdx = LstGetSelection( pList );
		pProfile->validitySecs = validitySecs[LstGetSelection( pList )];
*/		
		pProfile->Settings.bFwdCalls = GetCtlBoolValue( pForm, PROFILE_EDIT_FWD_CALLS_CHK );
		GetFieldStr( pForm, PROFILE_EDIT_FWD_CALLS_FLD, pProfile->Settings.strFwdCallsNum, STRLEN_PHONE_NUMBER );	
		
		pProfile->Settings.Mute.bMutePhone = GetCtlBoolValue( pForm, PROFILE_EDIT_MUTE_PHN_CHK );
		pProfile->Settings.Mute.bMuteMessging = GetCtlBoolValue( pForm, PROFILE_EDIT_MUTE_MSG_CHK );
		pProfile->Settings.Mute.bMuteCalendar = GetCtlBoolValue( pForm, PROFILE_EDIT_MUTE_CAL_CHK );
		pProfile->Settings.Mute.bMuteSystem = GetCtlBoolValue( pForm, PROFILE_EDIT_MUTE_SYS_CHK );
		pProfile->Settings.Mute.bMuteAlarm = GetCtlBoolValue( pForm, PROFILE_EDIT_MUTE_ALM_CHK );
		pProfile->Settings.Mute.bMuteGame = GetCtlBoolValue( pForm, PROFILE_EDIT_MUTE_GAM_CHK );
		pProfile->Settings.Mute.bVibrateON = GetCtlBoolValue( pForm, PROFILE_EDIT_MUTE_VIB_PBT );
		
		pProfile->Settings.bTimeZone = GetCtlBoolValue( pForm, PROFILE_EDIT_TZ_CHK );
		pList = FrmGetObjectPtr( pForm,  FrmGetObjectIndex( pForm, PROFILE_EDIT_TZ_LST ) );
		GetTimeZoneInfoFromIdx( LstGetSelection( pList ), (timezone_t *) &( pProfile->Settings.TimeZone ) );
		
		pProfile->Settings.bTheme = GetCtlBoolValue( pForm, PROFILE_EDIT_THEME_CHK );	
		pList = FrmGetObjectPtr( pForm,  FrmGetObjectIndex( pForm, PROFILE_EDIT_THEME_LST ) );
		GetThemeInfoFromIdx( LstGetSelection( pList ), (themes_t *) &( pProfile->Settings.Theme ) );		

		if ( ( ProfilesFindRecord( pProfile, &gProfileIdx, false, &bFound, (DmComparF *) &Profiles_CompareUniqueIDF ) == errNone )
				&& ( bFound ) )
		{
			retVal = ( ProfilesReplaceRecordAtIdx( &gProfileIdx, pProfile, false ) == errNone );
		}
		else
		{
			retVal = ( ProfilesSaveNewRecord( pProfile, false, &gProfileIdx ) == errNone );
		}
		
		if ( ( ProfilesDBGetUniqueID() == pProfile->uniqueID ) 
				&& ( !GetCtlBoolValue( pForm, PROFILE_EDIT_DEFAULT_PBT ) ) )
		{
			ProfilesDBSetUniqueID( -1 );
		}
			
		if ( GetCtlBoolValue( pForm, PROFILE_EDIT_DEFAULT_PBT ) )
		{
			ProfilesDBSetUniqueID( pProfile->uniqueID );
		}
		
		if ( !retVal )
		{
		 	FrmCustomAlert( ErrOKAlert, "Could not save record.", "", "" );	
		}
	}
	
	return ( retVal );
		
} // CopyAndSaveProfileEditInfo()

/* 
 * RedrawProfilesList()
 */
static void RedrawProfilesList( void )
{
	if ( gProfilesListP )
	{
		LoadProfilesList( gProfilesListP, false );
								
		gProfileIdx = ( ProfilesGetNumberOfRecords() ) ? gProfileIdx : noListSelection;
		
		LstSetSelection( gProfilesListP, gProfileIdx );	
	
		LstDrawList( gProfilesListP );
		
		gProfilesListP = NULL;
	}
	
	return;	
	
} // RedrawProfilesList()

/*
 * ProfileOptionsForm.c
 */