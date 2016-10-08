/*
 * AppMain.c
 */
 
#include <Hs.h>
#include <HsPhone.h>
#include <HsPhoneGSM.h>
#include <HsNav.h>
#include <HsExt.h>
#include <palmOneResources.h>
#include <PalmTypes.h>
#include <Form.h>

#include "Sections.h"
#include "Global.h"
#include "AppResources.h"
#include "Utils.h"
#include "FormUtils.h"
#include "NotifyReg.h"
#include "GetGID.h"
#include "GID_DB.h"
#include "Locations.h"
#include "Tables.h"
#include "LocForm.h"
#include "AttnAlerts.h"
#include "ProfilesDB.h"
#include "ProfileEditForm.h"
#include "ProfilesForm.h"
#include "Gadgets.h"
#include "Statistics.h"
#include "GlobalFind.h"
#include "GeoInfo.h"
#include "RegionsForm.h"
#include "RegionEditForm.h"
#include "MessagingForm.h"
#include "MsgLogForm.h"

// Prototypes
static void 			InitializeMainForm( FormType* pForm ) EXTRA_SECTION_ONE;
static Boolean 			MainFormHandleMenu( UInt16 command ) EXTRA_SECTION_ONE;
static Boolean 			MainFormHandleEvent( EventType* pEvent ) EXTRA_SECTION_ONE;
static Boolean 			AppHandleEvent( EventType* pEvent ) EXTRA_SECTION_ONE;
static Err 				AppStart( void );
static void 			AppStop( void );
static void 			AppEventLoop( void );
//
UInt32 					PilotMain( UInt16 cmd, MemPtr cmdPBP, UInt16 launchFlags );

/*
 * InitializeMainForm()
 */
static void InitializeMainForm( FormType* pForm )
{
	TableType*				pTbl = FrmGetObjectPtr( pForm, FrmGetObjectIndex( pForm, MAIN_TBL ) );
	appPrefs_t 				prefs;
	
	if ( !pForm ) return;
	
	gMainFormP = pForm;
	
	gTopRow = 0;
	gSelectedRow = -1;
						
	CommonInitGauge( pForm );
	InitializeGadgets( pForm );
	
	FrmDrawForm( pForm );
	
	TableInit( pTbl );	
	
	ReadAppPrefs( &prefs );
	
	SetCtlBoolValue( pForm, MAIN_ENABLE_CBX, prefs.bEnabled );

	return;
	
} // InitializeMainForm()

/*
 * MainFormHandleMenu()
 */
Boolean MainFormHandleMenu( UInt16 menuID )
{
	Boolean 			handled = false;
	EventType			newEvent;

	switch ( menuID )
	{
		case MENU_MAIN_EXIT:
		
			newEvent.eType = appStopEvent;
									
			EvtAddEventToQueue( &newEvent );	
					
			handled = true;
		
			break;
			
		case MENU_MAIN_STATS:
		
			FrmPopupForm( STATS_FORM );
			
			handled = true;
			
			break;
		
		case MENU_MAIN_PREFS:
		
			FrmPopupForm( PREFS_FORM );
			
			handled = true;
			
			break;
			
		case MENU_MAIN_ABOUT:

			FrmPopupForm( ABOUT_FORM );
			
			handled = true;
			
			break;
			
		case MENU_MAIN_PROFILES:
				
			FrmPopupForm( PROFILES_FORM );
			
			handled = true;
			
			break;
			
		case MENU_MAIN_REGIONS:
				
			FrmPopupForm( REGIONS_FORM );
			
			handled = true;
			
			break;
			
		case MENU_MAIN_MSG_LOG:
				
			FrmPopupForm( LOG_FORM );
			
			handled = true;
			
			break;					
			
		default:
			break;
	}

	return ( handled );

} // MainFormHandleMenu()

/*
 * MainFormHandleEvent()
 */
static Boolean MainFormHandleEvent( EventType* pEvent )
{
	Boolean 			handled = false;
	FormType* 			pForm = FrmGetActiveForm();
	appPrefs_t			prefs;
	static Boolean		bMenuDisplayed = false;
	
	switch ( pEvent->eType )
	{
		case frmOpenEvent:
					
			InitializeMainForm( pForm );
					
			handled = true;
						
			break;
			
		case frmCloseEvent:
			
			ReadAppPrefs( &prefs );
						
			RegisterForNotifications( prefs.bEnabled );
			
			if ( prefs.bGIDDBRebuildOnExit )
			{
				GIDBulkAction( rebuildDB );
			}
			
			break;
				
		case ctlSelectEvent:
		
			switch (pEvent->data.ctlSelect.controlID)
			{
				case MAIN_ENABLE_CBX:
		
					ReadAppPrefs( &prefs );
								
					prefs.bEnabled = ( CtlGetValue(pEvent->data.ctlSelect.pControl ) == 1 );
					
					WriteAppPrefs( &prefs );
					
					RegisterForNotifications( prefs.bEnabled );
/*					
					if ( prefs.bEnabled )
					{
						SetCtlLabelStr( pForm, pEvent->data.ctlSelect.controlID, "ON" );
					}
					else
					{
						SetCtlLabelStr( pForm, pEvent->data.ctlSelect.controlID, "OFF" );
					}
*/					
					UpdateGadgets();
					
					handled = true;

					break;
					
				default:
					break;
			}
		
			break;
		
		case menuEvent:
										
			handled = MainFormHandleMenu( pEvent->data.menu.itemID );

			bMenuDisplayed = false;
			
			break;
			
		case menuOpenEvent:
		
			bMenuDisplayed = true;
			
			break;
			
		default:
			
			handled = ProcessScrollEvent2( pForm, pEvent, &bMenuDisplayed );
			
			break;
	}
	
	return ( handled );
	
} // MainFormHandleEvent()

/*
 * AppHandleEvent()
 */
static Boolean AppHandleEvent( EventType* pEvent )
{
	UInt16 		formId = 0;
	FormType* 	pForm = NULL;
	Boolean		handled = false;

	if ( pEvent->eType == frmLoadEvent )
	{
		formId = pEvent->data.frmLoad.formID;
		
		pForm = FrmInitForm( formId );
		FrmSetActiveForm( pForm );

		switch ( formId )
		{
			case MAIN_FORM:
			
				FrmSetEventHandler( pForm, MainFormHandleEvent );
			
				break;
				
			case PREFS_FORM:
			
				FrmSetEventHandler( pForm, PrefsFormHandleEvent );
			
				break;
				
			case ABOUT_FORM:
			
				FrmSetEventHandler( pForm, AboutFormHandleEvent );
				
				break;
				
			case LOC_FORM:
				
				FrmSetEventHandler( pForm, LocFormHandleEvent );
				
				break;
				
			case GEO_INFO_FORM:
			
				FrmSetEventHandler( pForm, GeoInfoFormHandleEvent );
				
				break;
					
			case PROFILES_FORM:
			
				FrmSetEventHandler( pForm, ProfilesFormHandleEvent );
				
				break;
				
			case PROFILE_EDIT_FORM:
			
				FrmSetEventHandler( pForm, ProfileEditFormHandleEvent );
			
				break;
	
			case STATS_FORM:
			
				FrmSetEventHandler( pForm, StatisticsFormHandleEvent );
			
				break;
				
			case REGIONS_FORM:
			
				FrmSetEventHandler( pForm, RegionsFormHandleEvent );
			
				break;
				
			case REGION_EDIT_FORM:
			
				FrmSetEventHandler( pForm, RegionEditFormHandleEvent );
			
				break;
				
			case MSG_FORM:
			
				FrmSetEventHandler( pForm, MsgFormHandleEvent );
			
				break;
				
			case LOG_FORM:
				
				FrmSetEventHandler( pForm, LogFormHandleEvent );
				
				break;

			default:
				break;
		}
		
		handled = true;
	}
	
	return ( handled );

} // AppHandleEvent()


/*
 * AppStart()
 */
static Err AppStart( void )
{
	gProfilesH = NULL;
	gThemesH = NULL;
	gTimeZoneH = NULL;
	gTonesH = NULL;
	
	FrmGotoForm( MAIN_FORM );

	return ( errNone );

} // AppStart()

/*
 * AppStop()
 */
static void AppStop( void )
{
	FrmCloseAllForms();
	
	if ( gProfilesH )
	{
        MemHandleUnlock( gProfilesH );
        MemHandleFree( gProfilesH );
        gProfilesH = NULL;
    }

	if ( gThemesH )
	{
        MemHandleUnlock( gThemesH );
        MemHandleFree( gThemesH );
        gThemesH = NULL;
    }
	
	if ( gTimeZoneH )
	{
        MemHandleUnlock( gTimeZoneH );
        MemHandleFree( gTimeZoneH );
        gTimeZoneH = NULL;
    }
    
    if ( gTonesH )
    {
    	MemHandleUnlock( gTonesH );
    	MemHandleFree( gTonesH );
    	gTonesH = NULL;
    }
    
	return;
	
} // AppStop()


/*
 * AppEventLoop()
 */
static void AppEventLoop( void )
{
	Err					error;
	EventType			event;
	
	do {

		EvtGetEvent( &event, (Int32) SysTicksPerSecond() /* evtWaitForever */ );

		if ( SysHandleEvent( &event ) )
			continue;
			
		if ( MenuHandleEvent( 0, &event, &error ) )
			continue;
			
		if ( AppHandleEvent( &event ) )
			continue;

		FrmDispatchEvent( &event );

	} while ( event.eType != appStopEvent );
	
	return;

} // AppEventLoop()

/*
 * PilotMain
 */
UInt32 PilotMain( UInt16 cmd, MemPtr cmdPBP, UInt16 launchFlags )
{
	Err 						error = errNone;
	PhnEventPtr 				phoneEvent = NULL;
	appPrefs_t*					pPrefs = NULL;
	UInt16 						cardNo = 0;
   	LocalID 					dbID = 0;
   	UInt32						alarmProfileID = 0;

	switch ( cmd )
	{
		case phnLibLaunchCmdEvent:
			
			phoneEvent = (PhnEventPtr) cmdPBP;
			
			switch ( phoneEvent->eventType )
			{					
				case phnEvtRegistration:
					
					if ( ( phoneEvent->data.registration.status == registrationHome )
								|| ( phoneEvent->data.registration.status == registrationRoaming ) )
					{
						DoLocation();
					}						
					
					// phoneEvent->acknowledge = true;
					
					break;
						
				default:
					break;
			}	

			break;
			
		case sysAppLaunchCmdNotify:
			
			if ( ((SysNotifyParamType *) cmdPBP)->notifyType == msgDeferredNotifyEvent )
			{
				HandleMsgNotification( (SysNotifyParamType *) cmdPBP );
				
				((SysNotifyParamType *) cmdPBP)->handled = true;
			}
			else if ( ((SysNotifyParamType *) cmdPBP)->notifyType == sysNotifyTimeChangeEvent )
			{
				if ( !SysCurAppDatabase( &cardNo, &dbID ) )
				{
					AlmSetAlarm( cardNo, dbID, 0, 0, false );
				}
				
				((SysNotifyParamType *) cmdPBP)->handled = true;
			}
			
			break;
		
		case sysAppLaunchCmdSystemReset:
		
			if ( ( error = RomVersionCompatible( MIN_VERSION, launchFlags ) == errNone )
					&& IsPhoneGSM() )
			{
				pPrefs = MemPtrNew( sizeof( appPrefs_t ) );
				if ( pPrefs )
				{
					MemSet( pPrefs, sizeof( appPrefs_t ), 0 );			
					ReadAppPrefs( pPrefs );
					RegisterForNotifications( pPrefs->bEnabled );
					MemPtrFree( pPrefs );
				}
			}
					
			break;

		case sysAppLaunchCmdSaveData:
			// nothing
			break;
			 
		case sysAppLaunchCmdFind:
		
			DoFind( (FindParamsType *) cmdPBP );
			
			break;
			
		case sysAppLaunchCmdGoTo:
			
			gHasNewGlobals = ( launchFlags & sysAppLaunchFlagNewGlobals );
			
			DoGoto( ( (GoToParamsType *) cmdPBP)->matchCustom );
			
			if ( gHasNewGlobals )
			{
				AppEventLoop();
				AppStop();
			}
			
			break;
			
		case sysAppLaunchCmdAttention:

			ProcessAttention( ( AttnLaunchCodeArgsType *) cmdPBP );
					
			break;		

		case sysAppLaunchCmdAlarmTriggered:
		
			((SysAlarmTriggeredParamType *) cmdPBP)->purgeAlarm = ( ((SysAlarmTriggeredParamType *)cmdPBP)->ref == 0 );
			
			if ( !SysCurAppDatabase( &cardNo, &dbID ) )
			{
				AlmSetAlarm( cardNo, dbID, 0, 0, false );
			}
			
			WriteWakePrefs( is_display_off() );
			
			break;
			
		case sysAppLaunchCmdDisplayAlarm:
		
			if ( ( alarmProfileID = ((SysDisplayAlarmParamType *)cmdPBP)->ref ) == 0 ) break;
			
			((SysDisplayAlarmParamType *)cmdPBP)->ref = 0;
			
			DoAlarmProfileCancel( alarmProfileID );
			
			if ( ReadWakePrefs() )
			{
				EvtEnqueueKey( (WChar) vchrPowerOff, 0, commandKeyMask );
			}
					
			break;
					
		case sysAppLaunchCmdNormalLaunch:
			
			gHasNewGlobals = false;
		
			if ( ( error = RomVersionCompatible( MIN_VERSION, launchFlags ) == errNone )
					&& IsPhoneGSM() )
			{
				if ( ( error = AppStart() ) == 0 )
				{
					AppEventLoop();
					AppStop();				
				}
	
				pPrefs = MemPtrNew( sizeof( appPrefs_t ) );			
				if ( pPrefs )
				{
					MemSet( pPrefs, sizeof( appPrefs_t ), 0 );						
					ReadAppPrefs( pPrefs );
					RegisterForNotifications( pPrefs->bEnabled );
					MemPtrFree( pPrefs );
				}
			}
			else
			{
				FrmCustomAlert( ErrOKAlert, "Incompatible Device...\nRequire GSM Treo or Centro.", "", "" );	
			}
			
			break;
							
		default:
			break;
	}
	
	return ( (UInt32) error );
	
	
} // PilotMain()

/*
 * AppMain.c
 */

