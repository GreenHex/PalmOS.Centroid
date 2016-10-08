/*
 * GID_DB.c
 */

#include "GID_DB.h"

// Prototypes
static Boolean 		GIDSeekRecord( DmOpenRef* dbPP, locations_t* pLoc, UInt16* idxP, Boolean bLoadRecord, DmComparF* comparFP );

/*
 * GIDGetDBModificationInfo()
 */
Err GIDGetDBModificationInfo( UInt32* pModDate, UInt32* pModNum )
{
	Err 					error = 0;
	UInt16 					cardNo = 0;
 	LocalID 				dbID = 0;
 	DmSearchStateType 		stateInfo;

	error = DmGetNextDatabaseByTypeCreator( true, &stateInfo, GIDDbType, GIDDbCreator, true, &cardNo, &dbID );
	
	if ( error == errNone )
	{	
		error  = DmDatabaseInfo( cardNo, dbID, NULL, NULL, NULL, NULL, pModDate, NULL, pModNum, NULL, NULL, NULL, NULL );
	}
	
	return ( error );
	
} // GIDGetDBModificationInfo()

/*
 * GIDGetNumberOfRecords()
 */
UInt16 GIDGetNumberOfRecords( void )
{
	Err 			error = errNone;
	UInt16			retVal = 0;
	DmOpenRef 		dbP = NULL;
	
	error = DBOpen( &dbP, GIDDbName, GIDDbType, GIDDbCreator, dmModeReadOnly, GIDDbVersion, sizeof( locations_t ) );
    
    if ( dbP )
    {
	    retVal = DmNumRecordsInCategory( dbP, dmAllCategories );
		
		DmCloseDatabase( dbP );
    }
    
	return ( retVal );
	
} // GIDGetNumberOfRecords()

/*
 * GIDGetNumberOfNewRecords()
 */
Err GIDGetNumberOfNewRecords( UInt16* newRecordsCountP, UInt16* roamingCountP, UInt16* recordsWithAlarmCountP, UInt16* recordsWithProfileCountP, UInt16* recordsWithMsgCountP, UInt16* recordsWithGeoInfoP )
{
	Err				error = errNone;
	DmOpenRef		dbP = NULL;
	UInt16			idx = 0;
	UInt16			numRecords = 0;
	MemHandle		recordH = NULL;
	locations_t*	recordP = NULL;
	UInt16			newRecordsCount = 0;
	UInt16			roamingCount = 0;
	UInt16			recordsWithAlarmCount = 0;
	UInt16			recordsWithProfileCount = 0;
	UInt16			recordsWithMsgCount = 0;
	UInt16			recordsWithGeoInfo = 0;
			
	error = DBOpen( &dbP, GIDDbName, GIDDbType, GIDDbCreator, dmModeReadOnly, GIDDbVersion, sizeof( locations_t ) );
    
    if ( dbP )
    {
    	numRecords = GetNumRecords( &dbP, dmAllCategories );
    	
		for ( idx = 0 ; idx < numRecords ; idx++ )
		{
			recordH = DmQueryRecord( dbP, idx );
			
			if ( recordH )
			{
				recordP = (locations_t *) MemHandleLock( recordH );
				
				if ( recordP )
				{
					if ( recordP->isNewLocation )
					{
						++newRecordsCount;	
					}
					
					if ( recordP->isRoaming )
					{
						++roamingCount;	
					}
					
					if ( ( recordP->LocAlert.bAlertOnEntry ) 
								|| ( recordP->LocAlert.bAlertOnExit ) ) 
					{
						++recordsWithAlarmCount;
					}
					
					if ( recordP->AttachedProfile.profileID )
					{
						++recordsWithProfileCount;
					}
					
					if ( StrLen( recordP->Msg.Address ) )
					{
						++recordsWithMsgCount;
					}
					
					if ( recordP->GeoInfo.isValid )
					{
						++recordsWithGeoInfo;
					}
				}
				
				MemHandleUnlock( recordH );
				recordP = NULL;
			}	
		}
	
		DmCloseDatabase( dbP );
    }
    
    if ( newRecordsCountP ) *newRecordsCountP = newRecordsCount;
    if ( roamingCountP ) *roamingCountP = roamingCount;
    if ( recordsWithAlarmCountP ) *recordsWithAlarmCountP = recordsWithAlarmCount;
    if ( recordsWithProfileCountP ) *recordsWithProfileCountP = recordsWithProfileCount;
    if ( recordsWithMsgCountP ) *recordsWithMsgCountP = recordsWithMsgCount;
    if ( recordsWithGeoInfoP ) *recordsWithGeoInfoP = recordsWithGeoInfo;
    
	return ( error );
	
} // GIDGetNumberOfNewRecords()


/*
 * GIDSaveNewRecord()
 */
Err GIDSaveNewRecord( locations_t* pLoc, Boolean isSecret, UInt16* idxP )
{
	Err 			error = errNone;
	DmOpenRef 		dbP = NULL;
	
	error = DBOpen( &dbP, GIDDbName, GIDDbType, GIDDbCreator, dmModeReadWrite, GIDDbVersion, sizeof( locations_t ) );
	
	if ( ( error == errNone) && ( dbP ) )
	{
		MemHandle		recordH = NULL;
		locations_t*	recordP = NULL;
		UInt16			idx = dmMaxRecordIndex;
		UInt16			recordAttributes = 0;		
		
		recordH = DmNewRecord( dbP, &idx, sizeof( locations_t ) );
		if (recordH)
		{
			recordP = MemHandleLock( recordH );
			
			if ( recordP )
			{
				error = DmWrite( recordP, 0, pLoc, sizeof( locations_t ) );
			}
			
			MemHandleUnlock( recordH );
			recordP = NULL;
		
			DmRecordInfo( dbP, idx, &recordAttributes, NULL, NULL );
			if (isSecret)
			{
				recordAttributes |= dmRecAttrSecret;
			}
			else
			{
				recordAttributes &= ~dmRecAttrSecret;
			}
			error = DmSetRecordInfo( dbP, idx, &recordAttributes, NULL );
			
			error = ReleaseRecord( &dbP, &idx );
			
			if (idxP)
			{
				*idxP = idx;
			}
		}		
    	
    	DmQuickSort( dbP, (DmComparF *) &GID_SortTimeF, 0 );
	    		
		DmCloseDatabase( dbP );
	}
		
	return ( error );
	
} // GIDSaveNewRecord()

/*
 * GIDReplaceRecordAtIdx()
 */
Err GIDReplaceRecordAtIdx( UInt16* idxP, locations_t* pLoc, Boolean isSecret )
{
	Err				error = errNone;
	DmOpenRef 		dbP = NULL;
	MemHandle		recordH;
	locations_t*	recordP;
	
	error = DBOpen( &dbP, GIDDbName, GIDDbType, GIDDbCreator, dmModeReadWrite, GIDDbVersion, sizeof( locations_t ) );
	
	if ( ( error == errNone) && ( dbP ) )
	{
		recordH = DmGetRecord( dbP, *idxP );
		
		if ( recordH )
		{
			recordP = (locations_t *) MemHandleLock( recordH );
			if ( recordP )
			{	
				error = DmWrite( recordP, 0, pLoc, sizeof( locations_t ) );
			}
				
			MemHandleUnlock( recordH );			
			recordP = NULL;
		}
		
		error = ReleaseRecord( &dbP, idxP );		
		
		DmQuickSort( dbP, (DmComparF *) &GID_SortTimeF, 0 );
	    
		DmCloseDatabase( dbP );
	}
	
	return ( error );

} // GIDReplaceRecordAtIdx()

/*
 * GIDRemoveRecordAtIdx()
 */
Err GIDRemoveRecordAtIdx( UInt16* idxP )
{
	Err				error = errNone;
	DmOpenRef 		dbP = NULL;

	error = DBOpen( &dbP, GIDDbName, GIDDbType, GIDDbCreator, dmModeReadWrite, GIDDbVersion, sizeof( locations_t ) );
	
	if ( ( error == errNone ) && ( dbP ) )
	{
		if ( DmNumRecords( dbP ) > 0 )
		{
			if ( ( error = DmRemoveRecord( dbP, *idxP ) ) == errNone )
			{	
				*idxP = ( ( *idxP ) > 0 ) ? --( *idxP ) : 0;				
			}
		}
		
		DmCloseDatabase( dbP );
	}
	
	return ( error );
	
} // GIDRemoveRecordAtIdx()

/*
 * GIDGetRecordAtIdx
 */
Err GIDGetRecordAtIdx( UInt16* idxP, locations_t* pLoc )
{
	Err				error = errNone;
	DmOpenRef 		dbP = NULL;
	
	if ( ! ( idxP ) && ( pLoc ) )
	{ 
		return ( error = dmErrInvalidParam  );
	}

	MemSet( pLoc, sizeof( locations_t ), 0 );

	error = DBOpen( &dbP, GIDDbName, GIDDbType, GIDDbCreator, dmModeReadOnly, GIDDbVersion, sizeof( locations_t ) );
	
	if ( ( error == errNone ) && ( dbP ) )
	{
		MemHandle		recordH = NULL;
		locations_t*	recordP = NULL;
		UInt16			numRecords = GetNumRecords( &dbP, dmAllCategories );
		
		if ( *idxP < numRecords )
		{
			recordH = DmQueryRecord( dbP, *idxP );
			
			if ( recordH )
			{		
				recordP = (locations_t *) MemHandleLock( recordH );

				if ( recordP )
				{
					MemMove( pLoc, recordP, sizeof( locations_t ) );
				}
				
				MemHandleUnlock( recordH );
				recordP = NULL;
			}
			else
			{
				error =  DmGetLastErr();
			}
		}
		else
		{
			error = dmErrIndexOutOfRange;
		}
		
		DmCloseDatabase( dbP );
	}
	
	return ( error );
	
} // GIDGetRecordAtIdx()

/*
 * GIDSeekRecord()
 */
static Boolean GIDSeekRecord( DmOpenRef* dbPP, locations_t* pLoc, UInt16* idxP, Boolean bLoadRecord, DmComparF* comparFP )
{
	Boolean 		retVal = false;
	UInt16			numRecords = GetNumRecords( dbPP, dmAllCategories );
	MemHandle		recordH = NULL;
	locations_t*	recordP = NULL;
		
	for ( *idxP = 0 ; *idxP < numRecords ; (*idxP)++)
	{
		recordH = DmQueryRecord( *dbPP, *idxP );
		
		if ( recordH )
		{
			recordP = (locations_t *) MemHandleLock( recordH );
			
			if ( recordP )
			{
				retVal = ( comparFP( recordP, pLoc, 0, NULL, NULL, NULL ) == 0 ); 
				
				if ( retVal )
				{
					if ( ( pLoc ) && ( bLoadRecord ) )
					{
						MemMove( pLoc, recordP, sizeof( locations_t ) );	
					}

					// What a stupid mistake!!!
					MemHandleUnlock( recordH );
					recordP = NULL;
										
					break;	
				}	
			}
			
			MemHandleUnlock( recordH );
			recordP = NULL;
		}	
	}
	
	return ( retVal );
	
} // GIDSeekRecord()

/*
 * GIDFindRecord
 */
Err GIDFindRecord( locations_t* pLoc, UInt16* idxP, Boolean bLoadRecord, Boolean* bFound, DmComparF* comparFP )
{
	Err				error = errNone;
	DmOpenRef 		dbP = NULL;
	
	if ( bFound )
	{
		*bFound = false;
	}
	
	if ( !( ( pLoc ) && ( idxP ) ) )
	{ 
		return ( error = dmErrInvalidParam  );
	}

	error = DBOpen( &dbP, GIDDbName, GIDDbType, GIDDbCreator, dmModeReadOnly, GIDDbVersion, sizeof( locations_t ) );
	
	if ( ( error == errNone ) && ( dbP ) )
	{
		*bFound = GIDSeekRecord( &dbP, pLoc, idxP, bLoadRecord, comparFP );
					
		DmCloseDatabase( dbP );
	}
	
	return ( error );
	
} // GIDFindRecord()

/*
 * CompareGIDs()
 */
Boolean CompareGIDs( GID_t* pGID1, GID_t* pGID2 )
{
	return ( ( pGID1->MCC == pGID2->MCC ) 
				&& ( pGID1->MNC == pGID2->MNC )
				&& ( pGID1->LAC == pGID2->LAC )
				&& ( pGID1->CellID == pGID2->CellID ) );
	
} // CompareGIDs()

/*
 * GID_CompareF()
 */
Int16 GID_CompareF( locations_t* recP1, locations_t* recP2,
				Int16 other, SortRecordInfoPtr rec1SortInfo, SortRecordInfoPtr rec2SortInfo, MemHandle appInfoH )
{	
	if ( !( ( recP1 ) && ( recP2 ) ) ) 
		return ( -1 );
	
	if ( CompareGIDs( &(recP1->GID), &(recP2->GID) ) )
	{
		return ( 0 );
	}
	
	return ( -1 );
	
} // GID_CompareF()

/*
 * GID_SortTimeF()
 */
Int16 GID_SortTimeF( locations_t* recP1, locations_t* recP2,
				Int16 other, SortRecordInfoPtr rec1SortInfo, SortRecordInfoPtr rec2SortInfo, MemHandle appInfoH )
{	
	Int16		retVal = 0;
	
	/* 
	 * other = 0 -> ascending, 
	 * other = 1 -> desceding
	 */
	
	if ( recP1->TimeAtLoc == recP2->TimeAtLoc )
	{
		retVal = 0;
	}
	else if ( recP1->TimeAtLoc < recP2->TimeAtLoc )
	{
		retVal = other ? -1 : 1;
	}
	else if ( recP1->TimeAtLoc > recP2->TimeAtLoc ) 
	{
		retVal = other ? 1 : -1;
	}
	
	return ( retVal );
	
} // GID_SortTimeF()

/*
 * GID_CompareUniqueIDF()
 */
Int16 GID_CompareUniqueIDF( locations_t* recP1, locations_t* recP2,
				Int16 other, SortRecordInfoPtr rec1SortInfo, SortRecordInfoPtr rec2SortInfo, MemHandle appInfoH )
{	
	Int16 		retVal = 0;
	
	if ( recP1->uniqueID < recP2->uniqueID )
	{
		retVal = -1;
	}		
	else if ( recP1->uniqueID > recP2->uniqueID )
	{
		retVal = 1;
	}
	
	return ( retVal );
	
} // GID_CompareUniqueIDF()

/*
 * GID_DB.c
 */