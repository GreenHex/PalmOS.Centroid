/*
 * MsgLogForm.c
 */
 
#include "MsgLogForm.h"

// Prototypes
static void 			InitializeLogForm( FormType* pForm ) EXTRA_SECTION_ONE;

/*
 * LogFormHandleEvent()
 */
Boolean LogFormHandleEvent( EventType* pEvent )
{
	Boolean					handled = false;
	FormType* 				pForm = FrmGetActiveForm();
	ListType* 				pList = FrmGetObjectPtr( pForm, FrmGetObjectIndex( pForm, LOG_LST ) );
	UInt16					listIdx = noListSelection;
	log_t					log;
	
	switch ( pEvent->eType )
	{		
		case frmOpenEvent:
	
			InitializeLogForm( pForm );
			
			FrmDrawForm( pForm );
	
			handled = true;
												
			break;
		
		case lstSelectEvent:
			
			if ( pEvent->data.lstSelect.listID == LOG_LST )
			{
				listIdx = pEvent->data.lstSelect.selection;
				
				if ( listIdx != noListSelection )
				{
					switch ( LstPopupList( FrmGetObjectPtr( pForm, FrmGetObjectIndex( pForm, LOG_ACTION_LST ) ) ) )
					{
						case 1: // dial
						
							if ( LogGetRecordAtIdx( &listIdx, &log ) == errNone )
							{
								HsOpenDialNumberDialog( log.Address, log.CallerID, true );			
							}
							
							break;
	
						case 2: // delete
					
							if ( !FrmCustomAlert( ConfirmationOKCancelAlert, "Delete selected item?", "", "" ) )
							{
								if ( LogRemoveRecordAtIdx( &listIdx ) == errNone )
								{
									InitializeLogForm( pForm );
								
									LstDrawList( pList );
									
									LstSetSelection( pList, listIdx );
								}
							}			
							break;
													
						default:
							break;
					}
				}
								
				handled = true;	
			}
			
			break;	
			
		case ctlSelectEvent:
		
			switch ( pEvent->data.ctlSelect.controlID )
			{
				case LOG_OK_BTN:
			
					FrmReturnToForm( 0 );
						
					handled = true;
					
					break;
				
				case LOG_CLEAR_BTN:
					
					if ( LstGetNumberOfItems( pList ) )
					{
						if ( !FrmCustomAlert( ConfirmationOKCancelAlert, "Delete all entries from Centroid messaging log?", "", "" ) )
						{
							DeleteLogDB();
							
							InitializeLogForm( pForm );
							
							LstDrawList( pList );
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
	
} // LogFormHandleEvent()

/*
 * InitializeLogForm()
 */
static void InitializeLogForm( FormType* pForm )
{
	ListType* 		pList = FrmGetObjectPtr( pForm, FrmGetObjectIndex( pForm, LOG_LST ) );
	UInt16			numRecords = 0;
	Char			strNumRecords[maxStrIToALen] = "\0";
	
	if ( pList )
	{
		numRecords = InitializeLogList( pList );
		
		StrIToA( strNumRecords, numRecords );
		SetLabelStr( pForm, LOG_NUM_RECORDS_LBL, strNumRecords );
			
		LstSetDrawFunction( pList, (ListDrawDataFuncPtr) DrawLogsListItem );
	}
	
	return;
	
} // InitializeLogForm()

/*
 * MsgLogForm.c
 */