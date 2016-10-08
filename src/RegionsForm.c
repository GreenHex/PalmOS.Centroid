/*
 * RegionsForm.c
 */
 
#include "RegionsForm.h"

static void 			InitializeRegionsForm( FormType* pForm ) EXTRA_SECTION_ONE;

/*
 * RegionsFormHandleEvent()
 */
Boolean RegionsFormHandleEvent( EventType* pEvent )
{
	Boolean					handled = false;
	FormType* 				pForm = FrmGetActiveForm();
	ListType*				pList = FrmGetObjectPtr( pForm, FrmGetObjectIndex( pForm, REGIONS_LST ) );
	
	switch ( pEvent->eType )
	{		
		case frmOpenEvent:
			
			gRegionIdx = noListSelection;
			gRegionsListP = NULL;
				
			InitializeRegionsForm( pForm ); // loc loaded here!!!

			FrmDrawForm( pForm );
	
			FrmNavObjectTakeFocus( pForm, REGIONS_OK_BTN );
			
			handled = true;
						
			break;
			
		case frmCloseEvent:
			
			gRegionIdx = noListSelection;
			gRegionsListP = NULL;
							
			// NO handled = true;
			
			break;
		
		case lstSelectEvent:
		
			if ( pEvent->data.lstSelect.listID == REGIONS_LST )
			{
				gRegionIdx = LstGetSelection( pEvent->data.lstSelect.pList );
				
				switch ( LstPopupList( FrmGetObjectPtr( pForm, FrmGetObjectIndex( pForm, REGIONS_ACTION_LST ) ) ) )
				{
					
					case 1: // Edit
						
						if ( pList ) // this IS the bigger list, NOT the popup!!!
						{
							gRegionIdx = LstGetSelection( pList );
							
							if ( gRegionIdx != noListSelection )
							{
								gRegionsListP = pList;
								FrmPopupForm( REGION_EDIT_FORM );							
							}
						}
						
						break;
												
					case 2: // New
					
						if ( pList )
						{
							gRegionIdx = noListSelection;
							gRegionsListP = pList;
							FrmPopupForm( REGION_EDIT_FORM );	
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
				case REGIONS_OK_BTN:
					
					FrmReturnToForm( 0 );
					
					handled = true;

					break;

				case REGIONS_NEW_BTN:
					
					if ( pList )
					{
						gRegionIdx = noListSelection;
						gRegionsListP = pList;
						FrmPopupForm( REGION_EDIT_FORM );	
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
	
} // RegionsFormHandleEvent()

/*
 * InitializeRegionsForm()
 */
static void InitializeRegionsForm( FormType* pForm )
{
	ListType*			pList = FrmGetObjectPtr( pForm, FrmGetObjectIndex( pForm, REGIONS_LST ) );
	ListType*			pActionList = FrmGetObjectPtr( pForm, FrmGetObjectIndex( pForm, REGIONS_ACTION_LST ) );
			
	if ( pList )
	{
		LoadRegionsList( pList, false );
		LstSetSelection( pList, noListSelection );
	}
	
	if ( pActionList )
	{
		LstSetSelection( pList, noListSelection );
	}
	
	return;
	
} // InitializeRegionsForm()
 
/*
 * RegionsForm.c
 */