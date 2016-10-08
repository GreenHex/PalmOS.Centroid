/*
 * AddForm.c
 */

#include "AddForm.h"

// Prototypes
static void 			InitializeLocForm( FormType* pForm, locations_t* pLoc );

/*
 * ShowAddLocForm()
 */
void ShowAddLocForm( locations_t* pLoc )
{
	FormType*					pOldForm = FrmGetActiveForm();
	FormType*					pForm = FrmInitForm( ADD_FORM );
	FormActiveStateType			frmState;
	EventType					event;
	UInt32						endTime = TimGetSeconds() + 7;
	UInt16						idx = dmMaxRecordIndex;
	Boolean						bFound = false;
					
	FrmSaveActiveState( &frmState );
	
	if ( !pForm )
		return;
		
	FrmSetActiveForm( pForm );
	FrmSetEventHandler( pForm, AddLocFormHandleEvent );	
	InitializeLocForm( pForm, pLoc );
	
	FrmNavObjectTakeFocus( pForm, ADD_DESC_FLD /* ADD_OK_BTN */ );
		
	SndPlaySystemSound( sndInfo );
	
	// FrmPopupForm( ADD_FORM );
	FrmDrawForm( pForm );
					
	do // to prevent PalmOS from getting control...
	{
		EvtGetEvent( &event, SysTicksPerSecond() );

		FrmDispatchEvent( &event );	

		if ( event.eType == ctlSelectEvent )
		{	
			if ( event.data.ctlSelect.controlID == ADD_OK_BTN )
			{
				break;
			}
		}
			
		if ( event.eType == keyDownEvent )
		{
			endTime = TimGetSeconds() + 7;
			EvtResetAutoOffTimer();
			
			if ( ( event.data.keyDown.chr == vchrHardPower )
						|| ( event.data.keyDown.chr == vchrHard4 ) 
						|| ( event.data.keyDown.chr == vchrRockerCenter ) )
			{
				break;
			}
		}
		
	} while ( TimGetSeconds() < endTime );
				
	GetFieldStr( pForm, ADD_DESC_FLD, pLoc->LocationName, STRLEN_LOCATION_NAME );
	
	if ( ( !GIDFindRecord( pLoc, &idx, false, &bFound, (DmComparF *) GID_CompareF ) )
			&& ( bFound ) )
	{
		GIDReplaceRecordAtIdx( &idx, pLoc, false );		
	}
	
	FrmEraseForm( pForm );
	FrmDeleteForm( pForm );

	FrmRestoreActiveState( &frmState );
	
	if ( pOldForm ) FrmSetActiveForm( pOldForm );
		
	return;
	
} // ShowAddLocForm

/*
 * AddLocFormHandleEvent()
 */
Boolean AddLocFormHandleEvent( EventType* pEvent )
{
	Boolean					handled = false;
	FormType* 				pForm = FrmGetActiveForm();
	
	switch ( pEvent->eType )
	{		
		case frmOpenEvent:
	
			FrmDrawForm( pForm );
	
			handled = true;
												
			break;
			
		case ctlSelectEvent:
		
			switch ( pEvent->data.ctlSelect.controlID )
			{
				default:
					break;
			}
		
			break;
			
		default:
								
			break;
	}
		
	return ( handled );	
	
} // AddLocFormHandleEvent()

/*
 * InitializeAddLocForm()
 */
static void InitializeLocForm( FormType* pForm, locations_t* pLoc )
{
	Char			strGID[STRLEN_GID] = "\0";
	
	MakeGIDString( &( pLoc->GID ), strGID );
	SetLabelStr( pForm, ADD_GID_LBL, strGID );

	return;
	
} // InitializeAddLocForm()
 
/*
 * AddForm.c
 */