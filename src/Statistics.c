/*
 * Statistics.c
 */
 
#include "Statistics.h"

// Prototypes
static Boolean 				StatsFormHandleMenu( UInt16 menuID ) EXTRA_SECTION_ONE;
static void 				RefreshStatisticsForm( FormType* pForm ) EXTRA_SECTION_ONE;
static void 				DrawCount( FormType* pForm, UInt16 ctlID ) EXTRA_SECTION_ONE;
static void 				DrawBackupDate( FormType* pForm ) EXTRA_SECTION_ONE;

/*
 * StatsFormHandleMenu()
 */
static Boolean StatsFormHandleMenu( UInt16 menuID )
{
	Boolean 			handled = false;

	switch ( menuID )
	{
		case MENU_UTILS_BACKUP_DBS:
		
			BackupDBs();
											
			handled = true;
		
			break;
			
		case MENU_UTILS_REBUILD_DB:
		
			if ( FrmCustomAlert( ConfirmationOKCancelAlert, "Rebuild will correct any errors in the GID database... Continue?", "", "" ) == 0 )
			{
				GIDBulkAction( rebuildDB );
			}
											
			handled = true;
		
			break;
			
		case MENU_UTILS_CLEAR_ALERTS:
		
			if ( FrmCustomAlert( ConfirmationOKCancelAlert, "Clear all alerts?", "", "" ) == 0 )
			{
				GIDBulkAction( clearAlerts );
			}
			
			handled = true;
			
			break;

		case MENU_UTILS_CLEAR_PROFILES:
		
			if ( FrmCustomAlert( ConfirmationOKCancelAlert, "Clear profiles settings from all locations?", "", "" ) == 0 )
			{
				GIDBulkAction( clearProfiles );
			}
			
			handled = true;
			
			break;

		case MENU_UTILS_CLEAR_NEW_FLAG:
		
			if ( FrmCustomAlert( ConfirmationOKCancelAlert, "Clear all new location flags?", "", "" ) == 0 )
			{
				GIDBulkAction( clearNewFlag );
			}
			
			handled = true;
			
			break;
			
		case MENU_UTILS_CLEAR_REGIONS:
		
			if ( FrmCustomAlert( ConfirmationOKCancelAlert, "Clear all zone assignments?", "", "" ) == 0 )
			{
				GIDBulkAction( clearRegions );
			}
			
			handled = true;
			
			break;
			
		case MENU_UTILS_CLEAR_MSGS:
			
			if ( FrmCustomAlert( ConfirmationOKCancelAlert, "Clear all messaging assignments?", "", "" ) == 0 )
			{
				GIDBulkAction( clearMsgs );
			}
			
			handled = true;
			
			break;
		
			break;
			
		case MENU_UTILS_PURGE_GEOINFO:
		
			if ( FrmCustomAlert( ConfirmationOKCancelAlert, "Purge Geo-Location info from all records?", "", "" ) == 0 )
			{
				GIDBulkAction( purgeGeoInfo );
			}
			
			handled = true;
			
			break;
						
		default:
			break;
	}

	return ( handled );

} // StatsFormHandleMenu()

/*
 * StatisticsFormHandleEvent()
 */
Boolean StatisticsFormHandleEvent( EventType* pEvent )
{
	Boolean					handled = false;
	FormType* 				pForm = FrmGetActiveForm();
//	UInt32					timeNow = TimGetSeconds();
//	static UInt32			prevUpdateTime = 0;
	
	switch ( pEvent->eType )
	{		
		case frmOpenEvent:
				
			RefreshStatisticsForm( pForm ); // loc loaded here!!!
			
			handled = true;
						
			break;
			
		case ctlSelectEvent:
		
			switch ( pEvent->data.ctlSelect.controlID )
			{
				case STATS_OK_BTN:
									
					FrmReturnToForm( 0 );
					
					handled = true;

					break;
					
				case STATS_BACKUP_BTN:
				
					BackupDBs();
					
					RefreshStatisticsForm( pForm );
					
					handled = true;
					
					break;

				default:
					break;
			}
		
			break;
		
		case menuEvent:
										
			handled = StatsFormHandleMenu( pEvent->data.menu.itemID );

			if ( handled ) RefreshStatisticsForm( pForm );
			
			break;
/*			
		case nilEvent:
		
			if ( timeNow > ( prevUpdateTime + 5 ) )
			{
				prevUpdateTime = timeNow;
				
				InitializeStatisticsForm( pForm );
			}
			
			break;
*/			
		default:
								
			break;
	}
		
	return ( handled );	
	
} // StatisticsFormHandleEvent()

/*
 * RefreshStatisticsForm()
 */
static void RefreshStatisticsForm( FormType* pForm )
{
	UInt16				countGID = GIDGetNumberOfRecords();
	UInt16				countNewGID = 0;
	UInt16				countRoaming = 0;
	UInt16				countRecordsWithAlerts = 0;
	UInt16				countRecordsWithProfiles = 0;
	UInt16				countRecordsWithMsgs = 0;
	UInt16				countRecordsWithGeoInfo = 0;
	UInt16				countProfiles = ProfilesGetNumberOfRecords();
	UInt16				countRegions = RegionsGetNumberOfRecords();
	Char				strCountGID[maxStrIToALen] = "\0";
	Char				strCountRoaming[maxStrIToALen] = "\0";
	Char				strCountNewGID[maxStrIToALen] = "\0";
	Char				strCountRecordsWithAlerts[maxStrIToALen] = "\0";
	Char				strCountRecordsWithProfiles[maxStrIToALen] = "\0";
	Char				strCountRecordsWithMsgs[maxStrIToALen] = "\0";
	Char				strCountRecordsWithGeoInfo[maxStrIToALen] = "\0";
	Char				strCountProfiles[maxStrIToALen] = "\0";
	Char				strCountRegions[maxStrIToALen] = "\0";
	
	FrmSetActiveForm( pForm );
	
	FrmSetMenu( pForm, MENU_UTILS );
	
	GIDGetNumberOfNewRecords( &countNewGID, &countRoaming, &countRecordsWithAlerts, &countRecordsWithProfiles, &countRecordsWithMsgs, &countRecordsWithGeoInfo );
	
	StrIToA( strCountGID, countGID );
	StrIToA( strCountNewGID, countNewGID );
	StrIToA( strCountRoaming, countRoaming );
	StrIToA( strCountRecordsWithAlerts, countRecordsWithAlerts );
	StrIToA( strCountRecordsWithProfiles, countRecordsWithProfiles );
	StrIToA( strCountProfiles, countProfiles );
	StrIToA( strCountRecordsWithMsgs, countRecordsWithMsgs );
	StrIToA( strCountRecordsWithGeoInfo, countRecordsWithGeoInfo );
	StrIToA( strCountRegions, countRegions );

	SetLabelStr( pForm, STATS_GIDS_LBL, strCountGID );
	SetLabelStr( pForm, STATS_ROAMING_LBL, strCountRoaming );
	SetLabelStr( pForm, STATS_NEW_GIDS_LBL, strCountNewGID );
	SetLabelStr( pForm, STATS_RECORDS_ALERTS_LBL, strCountRecordsWithAlerts );
	SetLabelStr( pForm, STATS_RECORDS_PROFILES_LBL, strCountRecordsWithProfiles );
	SetLabelStr( pForm, STATS_RECORDS_MSGS_LBL, strCountRecordsWithMsgs );
	SetLabelStr( pForm, STATS_RECORDS_GEOINFO_LBL, strCountRecordsWithGeoInfo );
	SetLabelStr( pForm, STATS_PROFILES_LBL, strCountProfiles );
	SetLabelStr( pForm, STATS_RECORDS_REGIONS_LBL, strCountRegions );
	
	FrmDrawForm( pForm );
		
	DrawCount( pForm, STATS_GIDS_LBL );
	DrawCount( pForm, STATS_ROAMING_LBL );
	DrawCount( pForm, STATS_NEW_GIDS_LBL );
	DrawCount( pForm, STATS_RECORDS_ALERTS_LBL );
	DrawCount( pForm, STATS_RECORDS_PROFILES_LBL );
	DrawCount( pForm, STATS_RECORDS_MSGS_LBL );
	DrawCount( pForm, STATS_RECORDS_GEOINFO_LBL );
	DrawCount( pForm, STATS_PROFILES_LBL );	
	DrawCount( pForm, STATS_RECORDS_REGIONS_LBL );
	DrawBackupDate( pForm );
	
	return;
	
} // RefreshStatisticsForm()

/*
 * DrawCount()
 */
static void DrawCount( FormType* pForm, UInt16 ctlID )
{
	RectangleType		rect;
	
	FrmGetObjectBounds( pForm, FrmGetObjectIndex( pForm, ctlID ), &rect );
	rect.extent.x = 47;
	DrawStatusStr( (Char *) FrmGetLabel( pForm, ctlID ), &rect, boldFont, hAlignRight, vAlignMiddle, colorBlue, true );
	
	return;
		
} // DrawCount()

/*
 * DrawBackupDate()
 */
static void DrawBackupDate( FormType* pForm )
{
	RectangleType		rect;
	Char				strLastBackupDate[ 13 + timeStringLength + longDateStrLength + 3] = "\0";
	
	GetLastBackupDate( strLastBackupDate );
	
	SetLabelStr( pForm, STATS_BACKUP_DATE_LBL, strLastBackupDate );

	FrmGetObjectBounds( pForm, FrmGetObjectIndex( pForm, STATS_BACKUP_DATE_LBL ), &rect );
	rect.topLeft.x = 10;
	rect.extent.x = 142;
	DrawStatusStr( (Char *) FrmGetLabel( pForm, STATS_BACKUP_DATE_LBL ), &rect, stdFont, hAlignRight, vAlignMiddle, colorBlue, true );
	
	return;
	
} // DrawBackupDate()

/*
 * Statistics.c
 */