/*
 * PhoneProfilesForm.c
 */
 
#include "ProfilesForm.h"

static void 			InitializePhoneProfilesForm( FormType* pForm ) EXTRA_SECTION_ONE;
static void 			SetProfile(	profiles_t*	profileP, attachedProfile_t* lastProfileP ) EXTRA_SECTION_ONE;

/*
 * ProfilesFormHandleEvent()
 */
Boolean ProfilesFormHandleEvent( EventType* pEvent )
{
	Boolean					handled = false;
	FormType* 				pForm = FrmGetActiveForm();
	ListType*				pList = FrmGetObjectPtr( pForm, FrmGetObjectIndex( pForm, PROFILES_LST ) );
	profiles_t				profile;
	attachedProfile_t 		lastProfile;
	UInt16 					cardNo = 0;
   	LocalID 				dbID = 0;
	
	switch ( pEvent->eType )
	{		
		case frmOpenEvent:
			
			gProfileIdx = noListSelection;
			gProfilesListP = pList;
							
			InitializePhoneProfilesForm( pForm ); // loc loaded here!!!
						
			FrmDrawForm( pForm );
	
			FrmNavObjectTakeFocus( pForm, PROFILES_OK_BTN );
			
			handled = true;
						
			break;
		
		case frmCloseEvent:
		
			gProfileIdx = noListSelection;
			gProfilesListP = pList;
			
			// NO handled = true;
			
			break;
							
		case lstSelectEvent:
		
			if ( pEvent->data.lstSelect.listID == PROFILES_LST )
			{
				gProfileIdx = LstGetSelection( pEvent->data.lstSelect.pList );
				
				switch ( LstPopupList( FrmGetObjectPtr( pForm, FrmGetObjectIndex( pForm, PROFILES_ACTION_LST ) ) ) )
				{
					case 1: // Edit
						
						if ( pList ) // this IS the bigger list, NOT the popup!!!
						{
							gProfileIdx = LstGetSelection( pList );
							
							if ( gProfileIdx != noListSelection )
							{
								gProfilesListP = pList;
								FrmPopupForm( PROFILE_EDIT_FORM );							
							}
						}
						
						break;
						
					case 2: // Activate
						
						CtlSetEnabled( pEvent->data.ctlSelect.pControl, false );
							
						if ( !SysCurAppDatabase( &cardNo, &dbID ) )
						{
							AlmSetAlarm( cardNo, dbID, 0, 0, false );
						}

						SetProfile( &profile, &lastProfile);
						
						CtlSetEnabled( pEvent->data.ctlSelect.pControl, true );
					
						break;
						
					case 3: // Set Default
						
						if ( pList )
						{
							gProfileIdx = LstGetSelection( pList );
							
							if ( ProfilesGetRecordAtIdx( &gProfileIdx, &profile ) == errNone )
							{
								ProfilesDBSetUniqueID( profile.uniqueID );
								
								gDefaultListItemIdx = gProfileIdx;
								
								LstDrawList( pList );
							}
						}
						
						break;
						
					case 4: // New
					
						if ( pList )
						{
							gProfileIdx = noListSelection;
							gProfilesListP = pList;
							
							FrmPopupForm( PROFILE_EDIT_FORM );	
						}
						
						break;
						
					default: // Cancel
						break;	
				}
			}
			
			handled = true;
			
			break;
		
		case ctlSelectEvent:
		
			switch ( pEvent->data.ctlSelect.controlID )
			{
				case PROFILES_RESTORE_SOUND_BTN:
				
					RestoreSoundPrefs();
	
					handled = true;
					
					break;
					
				case PROFILES_OK_BTN:
					
					FrmReturnToForm( 0 );
					
					handled = true;

					break;
/*
				case PROFILES_CANCEL_BTN:
				
					FrmReturnToForm( 0 );
			
					handled = true;
					
					break;
*/					
				case PROFILES_NEW_BTN:
					
					if ( pList )
					{
						gProfileIdx = noListSelection;
						gProfilesListP = pList;
						
						FrmPopupForm( PROFILE_EDIT_FORM );	
					}
						
					handled = true;
				
					break;
					
				case PROFILES_EDIT_BTN:
				
					if ( pList )
					{
						gProfileIdx = LstGetSelection( pList );
						
						if ( gProfileIdx != noListSelection )
						{
							gProfilesListP = pList;
							FrmPopupForm( PROFILE_EDIT_FORM );							
						}
					}
								
					handled = true;
					
					break;
					
				case PROFILES_ACTIVATE_BTN:
					
					CtlSetEnabled( pEvent->data.ctlSelect.pControl, false );
					
					SetProfile( &profile, &lastProfile);
					
					CtlSetEnabled( pEvent->data.ctlSelect.pControl, true );
					
					handled = true;
					
					break;
/*
				case PROFILES_DELETE_BTN:
					
					handled = true;
					
					break;
*/											
				default:
					break;
			}
		
			break;
			
		default:
								
			break;
	}
		
	return ( handled );	
	
} // ProfilesFormHandleEvent()

/*
 * InitializePhoneProfilesForm()
 */
static void InitializePhoneProfilesForm( FormType* pForm )
{
	ListType*			pList = FrmGetObjectPtr( pForm, FrmGetObjectIndex( pForm, PROFILES_LST ) );
	ListType*			pActionList = FrmGetObjectPtr( pForm, FrmGetObjectIndex( pForm, PROFILES_ACTION_LST ) );
	
	gProfileIdx = noListSelection;
			
	if ( pList )
	{
		LoadProfilesList( pList, false );
		LstSetDrawFunction( pList, (ListDrawDataFuncPtr) ProfilesListDrawFunction );
		LstSetSelection( pList, noListSelection );
	}
	
	if ( pActionList )
	{
		LstSetSelection( pList, noListSelection );
	}
	
	return;
	
} // InitializePhoneProfilesForm()

/*
 * SetProfile()
 */
static void SetProfile(	profiles_t*	profileP, attachedProfile_t* lastProfileP )
{
	if ( ( !profileP ) || ( !lastProfileP ) ) return;
	
	if ( ( gProfileIdx != noListSelection ) 
				&& ( !ProfilesGetRecordAtIdx( &gProfileIdx, profileP ) ) )
	{
		ActivateProfile( profileP );
		
		lastProfileP->profileID = profileP->uniqueID;
		lastProfileP->profileIdx = gProfileIdx;
		lastProfileP->profileStartTime = TimGetSeconds();
		lastProfileP->profileValidity = profileP->validitySecs;
		StrCopy( lastProfileP->profileName, profileP->name );
		
		WriteLastProfilePrefs( lastProfileP );
	}
	
	return;
} // SetProfile()

/*
 * ProfilesListDrawFunction() // callback
 */
void ProfilesListDrawFunction( Int16 itemNum, RectangleType* bounds, Char** itemsText )
{
	UInt16			strHeight = FntLineHeight();

	if ( gDefaultListItemIdx == itemNum ) WinDrawChar( 0x0095, bounds->topLeft.x, 
															bounds->topLeft.y + ( bounds->extent.y - strHeight ) / 2 );
	
	WinDrawTruncChars( *( itemsText + itemNum ), StrLen( *( itemsText + itemNum ) ), 
							bounds->topLeft.x + 6,
							bounds->topLeft.y + ( bounds->extent.y - strHeight ) / 2,
							bounds->extent.x - 6 );
	
	return;
		
} // ProfilesListDrawFunction()

/*
 * ProfilesSetPopupTextFromList()
 */
Boolean ProfilesSetPopupTextFromList( ControlType* pCtl, ListType* pList, UInt16 selection )
{
	Boolean 		retVal = false;
	profiles_t		profile;
	UInt16			idx = selection - 1; // adjust for "-None-"
	Char*			strPopupP = NULL;
	UInt32			defaultProfileID = 0;
	
	if ( ProfilesGetRecordAtIdx( &idx, &profile ) == errNone )
	{
		defaultProfileID = ProfilesDBGetUniqueID();
		
		strPopupP = LstGetSelectionText( pList, selection );
		
		if ( profile.uniqueID == defaultProfileID )
		{
			gStrPopup[0] = 0x0095;
			gStrPopup[1] = ' ';
			gStrPopup[2] = chrNull;
			StrCat( gStrPopup, strPopupP );
		}
		else
		{
			StrCopy( gStrPopup, strPopupP );
		}
	
		CtlSetLabel( pCtl, gStrPopup );	
	}	
	else
	{
		CtlSetLabel( pCtl, LstGetSelectionText( pList, selection ) );
	}
	
	retVal = true;
	
	return ( retVal );

} // ProfilesSetPopupTextFromList()

/*
 * SetListFromProfileID()
 */
void SetListFromProfileID( ListType* pList, UInt32 profileID )
{
	profiles_t			profile;
	UInt16				profileIdx = dmMaxRecordIndex;
	Boolean				bFound = false;
	
	if ( ( pList) && ( profileID ) )
	{
		profile.uniqueID = profileID;
		
		ProfilesFindRecord( &profile, &profileIdx, false, &bFound, (DmComparF *) &Profiles_CompareUniqueIDF );
	
		if ( bFound )
		{
			LstSetSelection( pList, profileIdx + 1 ); // adjust for "- None -"
		}	
	}

	return;
	
} // SetListFromProfileID()

/*
 * PhoneProfilesForm.c
 */