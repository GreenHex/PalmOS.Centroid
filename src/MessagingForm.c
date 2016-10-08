/*
 * MessagingForm.c
 */

#include "MessagingForm.h"
 
// Prototypes
static void 				InitializeMsgForm( FormType* pForm, msg_t* pMsg ) EXTRA_SECTION_ONE;
static void 				CopyMsgInfo( FormType* pForm, msg_t* pMsg ) EXTRA_SECTION_ONE;
static void 				MsgClearAllFields( FormType* pForm ) EXTRA_SECTION_ONE;

/*
 * MsgFormHandleEvent()
 */
Boolean MsgFormHandleEvent( EventType* pEvent )
{
	Boolean					handled = false;
	FormType* 				pForm = FrmGetActiveForm();
	FieldType*				pAddrField = FrmGetObjectPtr( pForm, FrmGetObjectIndex( pForm, MSG_PHN_NUM_FLD ) );
	AddrLookupParamsType 	AddrParams;
	msg_t					Msg;
	appPrefs_t				prefs;
	
	switch ( pEvent->eType )
	{		
		case frmOpenEvent:
			
			InitializeMsgForm( pForm, gMsgP );
			
			FrmDrawForm( pForm );
			
			handled = true;
						
			break;
			
		case ctlSelectEvent:
		
			switch ( pEvent->data.ctlSelect.controlID )
			{	
				case MSG_PHN_NUM_LOOKUP_BTN:
	
					if ( pAddrField )
					{
						AddrParams.title = "Centroid [Message To]";
						AddrParams.pasteButtonText = "Paste";
						AddrParams.field1 = addrLookupSortField;
						AddrParams.field2 = addrLookupAnyPhones;
						AddrParams.field2Optional = false;
						AddrParams.userShouldInteract = true;  
						AddrParams.formatStringP = "^smsphone"; // "^matchphone"; // "^smsphone"; // "^listphone, ";
						
						PhoneNumberLookupCustom( pAddrField, &AddrParams, false );
					}
					
					handled = true;
					
					break;
									
				case MSG_OK_BTN:
					
					CopyMsgInfo( pForm, gMsgP );
									
					FrmReturnToForm( 0 );
					
					handled = true;

					break;

				case MSG_CANCEL_BTN:
					
					FrmReturnToForm( 0 );	
				
					handled = true;
					
					break;
					
				case MSG_CLR_ALL_BTN:
				
					MsgClearAllFields( pForm );
					
					FrmDrawForm( pForm );
					
					handled = true;
					
					break;
					
				case MSG_MSG_TEST_BTN:
					
					GetFieldStr( pForm, MSG_PHN_NUM_FLD, Msg.Address, STRLEN_PHN_NUMBER );
					GetFieldStr( pForm, MSG_MSG_FLD, Msg.Message, STRLEN_MSG );
					
					MakeMsgAndSend( Msg, onEntry, "Test" );
					
					handled = true;
					
					break;
					
				case MSG_MSG_MAKE_DEFAULT_BTN:
				
					ReadAppPrefs( &prefs);
					GetFieldStr( pForm, MSG_MSG_FLD, prefs.DefaultMsg, STRLEN_MSG );
						
					if ( ( StrLen( prefs.DefaultMsg ) ) 
							&& ( FrmCustomAlert( ConfirmationOKCancelAlert, "Replace default message with current message?", "", "" ) == 0 ) )
					{
						WriteAppPrefs( &prefs);
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
	
} // MsgFormHandleEvent()

/*
 * InitializeMsgForm()
 */
static void InitializeMsgForm( FormType* pForm, msg_t* pMsg )
{
	if ( ( !pForm ) || ( !pMsg ) ) return;

	SetFieldStr( pForm, MSG_PHN_NUM_FLD, pMsg->Address );
	SetFieldStr( pForm, MSG_MSG_FLD, pMsg->Message );
	SetCtlBoolValue( pForm, MSG_MSG_ON_ENTRY_CBX, pMsg->bAlertOnEntry );
	SetCtlBoolValue( pForm, MSG_MSG_ON_EXIT_CBX, pMsg->bAlertOnExit );
	SetCtlBoolValue( pForm, MSG_MSG_ONLY_ONCE_CBX, pMsg->bDisableAfterAlert );
							
	return;
	
} // InitializeMsgForm()

/*
 * CopyAndSaveMsg()
 */
static void CopyMsgInfo( FormType* pForm, msg_t* pMsg )
{	
	if ( ( !pForm ) || ( !pMsg ) ) return;
	
	GetFieldStr( pForm, MSG_PHN_NUM_FLD, pMsg->Address, STRLEN_PHN_NUMBER );
	GetFieldStr( pForm, MSG_MSG_FLD, pMsg->Message, STRLEN_MSG );
	pMsg->bAlertOnEntry = GetCtlBoolValue( pForm, MSG_MSG_ON_ENTRY_CBX );
	pMsg->bAlertOnExit = GetCtlBoolValue( pForm, MSG_MSG_ON_EXIT_CBX );
	pMsg->bDisableAfterAlert = GetCtlBoolValue( pForm, MSG_MSG_ONLY_ONCE_CBX );
	
	return;
		
} // CopyAndSaveMsg()
	
/* 
 * MsgClearAllFields()
 */ 
static void MsgClearAllFields( FormType* pForm )
{
	if ( !pForm ) return;
	
	ClearFieldStr( pForm, MSG_PHN_NUM_FLD );
	ClearFieldStr( pForm, MSG_MSG_FLD );
	SetCtlBoolValue( pForm, MSG_MSG_ON_ENTRY_CBX, false );
	SetCtlBoolValue( pForm, MSG_MSG_ON_EXIT_CBX, false );
	SetCtlBoolValue( pForm, MSG_MSG_ONLY_ONCE_CBX, false );

	return;

} // MsgClearAllFields()

/*
 * MessagingForm.c
 */