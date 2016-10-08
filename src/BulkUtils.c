/*
 * BulkUtils.c
 */
 
#include "BulkUtils.h"

// Prototypes
static void 					RebuildRecord( locations_t* pLoc ) EXTRA_SECTION_ONE;

/*
 * GIDBulkAction()
 */
Err GIDBulkAction( bulkAction_e action )
{
	Err						error = errNone;
	DmOpenRef				dbP = NULL;
	UInt16					idx = 0;
	UInt16					numRecords = 0;
	MemHandle				recordH = NULL;
	locations_t*			recordP = NULL;
	locations_t				loc;
	ProgressPtr				pProgress = NULL;
	Char					strNumRecords[maxStrIToALen] = "\0";
	
	error = DBOpen( &dbP, GIDDbName, GIDDbType, GIDDbCreator, dmModeReadWrite, GIDDbVersion, sizeof( locations_t ) );
    
    if ( dbP )
    {
    	numRecords = GetNumRecords( &dbP, dmAllCategories );
    	
    	if ( action == rebuildDB )
    	{
    		StrIToA( strNumRecords,  (Int32) numRecords );
    		pProgress = PrgStartDialog( APP_NAME, RebuildProgressCallback, strNumRecords );
    	}
    	
		for ( idx = 0 ; idx < numRecords ; idx++ )
		{
			recordH = DmGetRecord( dbP, idx );
			
			if ( recordH )
			{
				recordP = (locations_t *) MemHandleLock( recordH );
				
				if ( recordP )
				{
					MemMove( &(loc), recordP, sizeof( locations_t ) );
					
					switch ( action )
					{
						case rebuildDB:
						
							RebuildRecord( &loc );
							
							if ( ( pProgress ) && ( !( idx % 9 ) || ( idx == ( numRecords - 1 ) ) ) )
							{
								PrgUpdateDialog( pProgress, 0, idx, NULL, true );
							}
							break;
							
						case clearAlerts:
							
							MemSet( &(loc.LocAlert), sizeof( locAlert_t ), 0 );
							
							loc.LocAlert.startTime.hours = loc.LocAlert.startTime.minutes = -1; 
							loc.LocAlert.endTime.hours = loc.LocAlert.endTime.minutes = -1;
							
							break;
							
						case clearProfiles:
				
							MemSet( &(loc.AttachedProfile), sizeof( attachedProfile_t ), 0 );
							
							break;
							
						case clearRegions:
						
							MemSet( &(loc.RegionInfo), sizeof( regionInfo_t ), 0 );
							
							break;

						case clearNewFlag:
							
							loc.isNewLocation = false;
							
							break;
							
						case clearMsgs:
						
							MemSet( &(loc.Msg), sizeof( msg_t ), 0 );
							
							break;
							
						case purgeGeoInfo:
							
							MemSet( &(loc.GeoInfo), sizeof( geoInfo_t ), 0 );
							
							break;
								
						default:
							break;	
					}
					
					error = DmWrite( recordP, 0, &(loc), sizeof( locations_t ) );
					
				}
				
				MemHandleUnlock( recordH );
				recordP = NULL;
			}
	
			error = ReleaseRecord( &dbP, &idx );					
		}
	
		DmCloseDatabase( dbP );
		
		if ( ( action == rebuildDB ) && ( pProgress ) )
    	{
			PrgUpdateDialog( pProgress, 0, -1, NULL, true );
			PrgStopDialog( pProgress, true );
    	}
    }
        
	return ( error );
	
} // GIDBulkAction()

/*
 * RebuildRecord()
 */
static void RebuildRecord( locations_t* pLoc )
{
	UInt16			idx = dmMaxRecordIndex;
	Boolean			bFound = false;
	profiles_t		profile;
	regions_t		region;
	
	if ( !pLoc ) return;
	
	if ( pLoc->AttachedProfile.profileID )
	{		
		profile.uniqueID = pLoc->AttachedProfile.profileID;
		
		if ( ( !ProfilesFindRecord( &profile, &idx, true, &bFound, (DmComparF *) &GID_CompareUniqueIDF ) )
				&& ( bFound ) )
		{
			pLoc->AttachedProfile.profileIdx = idx;
			StrCopy( pLoc->AttachedProfile.profileName, profile.name );
			pLoc->AttachedProfile.profileStartTime = 0;
			pLoc->AttachedProfile.profileValidity = profile.validitySecs;
		}
		else
		{
			MemSet( &(pLoc->AttachedProfile), sizeof( attachedProfile_t ), 0 );	
		}
	}
	
	idx = dmMaxRecordIndex;
	bFound = false;
	
	if ( pLoc->RegionInfo.regionID )
	{
		region.uniqueID = pLoc->RegionInfo.regionID;
		
		if ( ( !RegionsFindRecord( &region, &idx, true, &bFound, (DmComparF *) &GID_CompareUniqueIDF ) ) 
				&& ( bFound ) )
		{		
			StrCopy( pLoc->RegionInfo.name, region.name );
		}
		else
		{
			MemSet( &(pLoc->RegionInfo), sizeof( regionInfo_t ), 0 );
		}
	}
			
	return;
	
} // RebuildRecord()

/*
 * BulkUtils.c
 */