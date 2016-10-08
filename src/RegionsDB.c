/*
 * RegionsDB.c
 */
 
#include "RegionsDB.h"

// Prototypes
static Boolean 		RegionsSeekRecord( DmOpenRef* dbPP, regions_t* pRegion, UInt16* idxP, Boolean bLoadRecord, DmComparF* comparFP );

/*
 * RegionGetNumberOfGIDs()
 */
UInt16 RegionGetNumberOfGIDs( UInt32 uniqueID )
{
	UInt16			retVal = 0;
	Err				error = errNone;
	DmOpenRef		dbP = NULL;
	UInt16			numRecords = 0;
	UInt16			idx = 0;
	MemHandle		recordH = NULL;
	locations_t*	recordP = NULL;

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
					if ( recordP->RegionInfo.regionID == uniqueID )
					{
						++retVal;	
					}
				}
				
				MemHandleUnlock( recordH );
				recordP = NULL;
			}	
		}
	
		DmCloseDatabase( dbP );
    }
    
	return ( retVal );
	
} // RegionGetNumberOfGIDs()

/*
 * RegionsGetNumberOfRecords()
 */
UInt16 RegionsGetNumberOfRecords( void )
{
	Err 			error = errNone;
	UInt16			retVal = 0;
	DmOpenRef 		dbP = NULL;
	
	error = DBOpen( &dbP, RegionsDbName, RegionsDbType, RegionsDbCreator, dmModeReadOnly, RegionsDbVersion, sizeof( regions_t ) );
    
    if ( dbP )
    {
	    retVal = DmNumRecordsInCategory( dbP, dmAllCategories );
		
		DmCloseDatabase( dbP );
    }
    
	return ( retVal );
	
} // RegionsGetNumberOfRecords()

/*
 * RegionsSaveNewRecord()
 */
Err RegionsSaveNewRecord( regions_t* pRegion, Boolean isSecret, UInt16* idxP )
{
	Err 			error = errNone;
	DmOpenRef 		dbP = NULL;
	
	error = DBOpen( &dbP, RegionsDbName, RegionsDbType, RegionsDbCreator, dmModeReadWrite, RegionsDbVersion, sizeof( regions_t ) );
	
	if ( ( error == errNone) && ( dbP ) )
	{
		MemHandle		recordH = NULL;
		regions_t*		recordP = NULL;
		UInt16			idx = dmMaxRecordIndex;
		UInt16			recordAttributes = 0;		
		
		recordH = DmNewRecord( dbP, &idx, sizeof( regions_t ) );
		if (recordH)
		{
			recordP = MemHandleLock( recordH );
			
			if ( recordP )
			{
				error = DmWrite( recordP, 0, pRegion, sizeof( regions_t ) );
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
			
			if ( idxP )
			{
				*idxP = idx;
			}
		}		
    	
    	DmQuickSort( dbP, (DmComparF *) &Regions_SortNameF, 0 );
	    		
		DmCloseDatabase( dbP );
	}
		
	return ( error );
	
} // RegionsSaveNewRecord()

/*
 * RegionsReplaceRecordAtIdx()
 */
Err RegionsReplaceRecordAtIdx( UInt16* idxP, regions_t* pRegion, Boolean isSecret )
{
	Err				error = errNone;
	DmOpenRef 		dbP = NULL;
	MemHandle		recordH;
	regions_t*		recordP;
	
	error = DBOpen( &dbP, RegionsDbName, RegionsDbType, RegionsDbCreator, dmModeReadWrite, RegionsDbVersion, sizeof( regions_t ) );
	
	if ( ( error == errNone) && ( dbP ) )
	{
		recordH = DmGetRecord( dbP, *idxP );
		
		if ( recordH )
		{
			recordP = (regions_t *) MemHandleLock( recordH );
			if ( recordP )
			{	
				error = DmWrite( recordP, 0, pRegion, sizeof( regions_t ) );
			}
				
			MemHandleUnlock( recordH );			
			recordP = NULL;
		}
		
		error = ReleaseRecord( &dbP, idxP );		
		
		DmQuickSort( dbP, (DmComparF *) &Regions_SortNameF, 0 );
	    
		DmCloseDatabase( dbP );
	}
	
	return ( error );

} // RegionsReplaceRecordAtIdx()

/*
 * RegionsRemoveRecordAtIdx()
 */
Err RegionsRemoveRecordAtIdx( UInt16* idxP )
{
	Err				error = errNone;
	DmOpenRef 		dbP = NULL;

	error = DBOpen( &dbP, RegionsDbName, RegionsDbType, RegionsDbCreator, dmModeReadWrite, RegionsDbVersion, sizeof( regions_t ) );
	
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
	
} // RegionsRemoveRecordAtIdx()

/*
 * RegionsGetRecordAtIdx
 */
Err RegionsGetRecordAtIdx( UInt16* idxP, regions_t* pRegion )
{
	Err				error = errNone;
	DmOpenRef 		dbP = NULL;
	
	if ( ( !idxP ) || ( !pRegion ) )
	{ 
		return ( error = dmErrInvalidParam  );
	}

	MemSet( pRegion, sizeof( regions_t ), 0 );

	error = DBOpen( &dbP, RegionsDbName, RegionsDbType, RegionsDbCreator, dmModeReadOnly, RegionsDbVersion, sizeof( regions_t ) );
	
	if ( ( error == errNone ) && ( dbP ) )
	{
		MemHandle		recordH = NULL;
		regions_t*		recordP = NULL;
		UInt16			numRecords = GetNumRecords( &dbP, dmAllCategories );
		
		if ( *idxP < numRecords )
		{
			recordH = DmQueryRecord( dbP, *idxP );
			
			if ( recordH )
			{		
				recordP = (regions_t *) MemHandleLock( recordH );

				if ( recordP )
				{
					MemMove( pRegion, recordP, sizeof( regions_t ) );
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
	
} // RegionsGetRecordAtIdx()

/*
 * RegionsSeekRecord()
 */
static Boolean RegionsSeekRecord( DmOpenRef* dbPP, regions_t* pRegion, UInt16* idxP, Boolean bLoadRecord, DmComparF* comparFP )
{
	Boolean 		retVal = false;
	UInt16			numRecords = GetNumRecords( dbPP, dmAllCategories );
	MemHandle		recordH = NULL;
	regions_t*		recordP = NULL;
		
	for ( *idxP = 0 ; *idxP < numRecords ; (*idxP)++)
	{
		recordH = DmQueryRecord( *dbPP, *idxP );
		
		if ( recordH )
		{
			recordP = (regions_t *) MemHandleLock( recordH );
			
			if ( recordP )
			{
				retVal = ( comparFP( recordP, pRegion, 0, NULL, NULL, NULL ) == 0 ); 
				
				if ( retVal )
				{
					if ( ( pRegion ) && ( bLoadRecord ) )
					{
						MemMove( pRegion, recordP, sizeof( regions_t ) );	
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
	
} // RegionsSeekRecord()

/*
 * RegionsFindRecord
 */
Err RegionsFindRecord( regions_t* pRegion, UInt16* idxP, Boolean bLoadRecord, Boolean* bFound, DmComparF* comparFP )
{
	Err				error = errNone;
	DmOpenRef 		dbP = NULL;
	
	if ( bFound )
	{
		*bFound = false;
	}
	
	if ( !( ( pRegion ) && ( idxP ) ) )
	{ 
		return ( error = dmErrInvalidParam  );
	}

	error = DBOpen( &dbP, RegionsDbName, RegionsDbType, RegionsDbCreator, dmModeReadOnly, RegionsDbVersion, sizeof( regions_t ) );
	
	if ( ( error == errNone ) && ( dbP ) )
	{
		*bFound = RegionsSeekRecord( &dbP, pRegion, idxP, bLoadRecord, comparFP );
					
		DmCloseDatabase( dbP );
	}
	
	return ( error );
	
} // RegionsFindRecord()

/*
 * Regions_SortNameF()
 */
Int16 Regions_SortNameF( regions_t* recP1, regions_t* recP2,
				Int16 other, SortRecordInfoPtr rec1SortInfo, SortRecordInfoPtr rec2SortInfo, MemHandle appInfoH )
{	
	Int16		retVal = 0;
	
	retVal = StrCompare( recP1->name, recP2->name );
	
	return ( retVal );
	
} // Regions_SortNameF()

/*
 * Regions_CompareUniqueIDF()
 */
Int16 Regions_CompareUniqueIDF( regions_t* recP1, regions_t* recP2,
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
	
} // Regions_CompareUniqueIDF()
 
/*
 * LoadRegionsList
 */
Boolean LoadRegionsList( ListType* pList, Boolean bMakeWithNone )
{
	Boolean			retVal = false;
	Err				error = errNone;
	UInt16			itemCount = 0;
	Char			nameStr[STRLEN_PROFILE_NAME];
	UInt16 			nameListSize = 0;
	UInt16			nameStrSize = 0;
	Char**          listOptions = NULL;

	if ( gProfilesH != NULL ) // unlock... // don't want problems
	{
        MemHandleUnlock( gProfilesH );
        MemHandleFree( gProfilesH );
        gProfilesH = NULL;
    }
	    
	if ( pList )
	{
		DmOpenRef		dbP;

		if ( bMakeWithNone )
		{
			StrCopy( nameStr, "- None -" );
			nameStrSize = StrLen( nameStr ) + 1;
			MemMove( gProfilesList + nameListSize, nameStr, nameStrSize );
			nameListSize = nameListSize + nameStrSize;
		
			++itemCount;
		}
		
		error = DBOpen( &dbP, RegionsDbName, RegionsDbType, RegionsDbCreator, dmModeReadOnly, RegionsDbVersion, sizeof( regions_t ) );
		
		if ( dbP )	
		{
			MemHandle		recordH = NULL;
			regions_t*		recordP = NULL;
			UInt16			numRecords = DmNumRecordsInCategory( dbP, dmAllCategories );
			UInt16 			i = 0;
			
			if ( numRecords )
			{
				for (i = 0 ; i < numRecords ; i++)
				{
					recordH = DmQueryRecord( dbP, i );
					if ( recordH )
					{	
						recordP = (regions_t *) MemHandleLock( recordH );
						
						if ( recordP )
						{
							StrCopy( nameStr, recordP->name );
				
							nameStrSize = StrLen( nameStr ) + 1;
							MemMove( gProfilesList + nameListSize, nameStr, nameStrSize );
							nameListSize = nameListSize + nameStrSize;
														
							++itemCount;
						}
	
						MemHandleUnlock( recordH );
					}
				}

				retVal = true; // indicates DB was not empty...
			}
					    
	    	if ( itemCount )
	    	{
		    	gProfilesH = SysFormPointerArrayToStrings( gProfilesList, itemCount);
			    listOptions = MemHandleLock( gProfilesH ); // to be unlocked somewhere...
	    	}
	    	
		    LstSetListChoices( pList, listOptions, itemCount );

			DmCloseDatabase( dbP );	
		}
	}
	
	return ( retVal );
	
}  // LoadRegionsList

/*
 * RegionsDB.c
 */