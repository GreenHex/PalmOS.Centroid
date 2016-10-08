/*
 * ProfilesDB.c
 */

#include "ProfilesDB.h"

// Prototypes
static Boolean 		ProfilesSeekRecord( DmOpenRef* dbPP, profiles_t* pProfile, UInt16* idxP, Boolean bLoadRecord, DmComparF* comparFP );
static Err 			ProfilesDBInitializeAppInfoBlock( DmOpenRef dbP,  UInt16 cardNo, LocalID dbID, LocalID* pAppInfoID );

/*
 * ProfilesGetNumberOfRecords()
 */
UInt16 ProfilesGetNumberOfRecords( void )
{
	Err 			error = errNone;
	UInt16			retVal = 0;
	DmOpenRef 		dbP = NULL;
	
	error = DBOpen( &dbP, ProfilesDbName, ProfilesDbType, ProfilesDbCreator, dmModeReadOnly, ProfilesDbVersion, sizeof( profiles_t ) );
    
    if ( dbP )
    {
	    retVal = DmNumRecordsInCategory( dbP, dmAllCategories );
		
		DmCloseDatabase( dbP );
    }
    
	return ( retVal );
	
} // ProfilesGetNumberOfRecords()

/*
 * ProfilesSaveNewRecord()
 */
Err ProfilesSaveNewRecord( profiles_t* pProfile, Boolean isSecret, UInt16* idxP )
{
	Err 			error = errNone;
	DmOpenRef 		dbP = NULL;
	
	error = DBOpen( &dbP, ProfilesDbName, ProfilesDbType, ProfilesDbCreator, dmModeReadWrite, ProfilesDbVersion, sizeof( profiles_t ) );
	
	if ( ( error == errNone) && ( dbP ) )
	{
		MemHandle		recordH = NULL;
		profiles_t*		recordP = NULL;
		UInt16			idx = dmMaxRecordIndex;
		UInt16			recordAttributes = 0;		
		
		recordH = DmNewRecord( dbP, &idx, sizeof( profiles_t ) );
		if (recordH)
		{
			recordP = MemHandleLock( recordH );
			
			if ( recordP )
			{
				error = DmWrite( recordP, 0, pProfile, sizeof( profiles_t ) );
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
    	
    	DmQuickSort( dbP, (DmComparF *) &Profiles_SortNameF, 0 );
	    		
		DmCloseDatabase( dbP );
	}
		
	return ( error );
	
} // ProfilesSaveNewRecord()

/*
 * ProfilesReplaceRecordAtIdx()
 */
Err ProfilesReplaceRecordAtIdx( UInt16* idxP, profiles_t* pProfile, Boolean isSecret )
{
	Err				error = errNone;
	DmOpenRef 		dbP = NULL;
	MemHandle		recordH;
	profiles_t*		recordP;
	
	error = DBOpen( &dbP, ProfilesDbName, ProfilesDbType, ProfilesDbCreator, dmModeReadWrite, ProfilesDbVersion, sizeof( profiles_t ) );
	
	if ( ( error == errNone) && ( dbP ) )
	{
		recordH = DmGetRecord( dbP, *idxP );
		
		if ( recordH )
		{
			recordP = (profiles_t *) MemHandleLock( recordH );
			if ( recordP )
			{	
				error = DmWrite( recordP, 0, pProfile, sizeof( profiles_t ) );
			}
				
			MemHandleUnlock( recordH );			
			recordP = NULL;
		}
		
		error = ReleaseRecord( &dbP, idxP );		
		
		DmQuickSort( dbP, (DmComparF *) &Profiles_SortNameF, 0 );
	    
		DmCloseDatabase( dbP );
	}
	
	return ( error );

} // ProfilesReplaceRecordAtIdx()

/*
 * ProfilesRemoveRecordAtIdx()
 */
Err ProfilesRemoveRecordAtIdx( UInt16* idxP )
{
	Err				error = errNone;
	DmOpenRef 		dbP = NULL;

	error = DBOpen( &dbP, ProfilesDbName, ProfilesDbType, ProfilesDbCreator, dmModeReadWrite, ProfilesDbVersion, sizeof( profiles_t ) );
	
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
	
} // ProfilesRemoveRecordAtIdx()

/*
 * ProfilesGetRecordAtIdx
 */
Err ProfilesGetRecordAtIdx( UInt16* idxP, profiles_t* pProfile )
{
	Err				error = errNone;
	DmOpenRef 		dbP = NULL;
	
	if ( ( !idxP ) || ( !pProfile ) )
	{ 
		return ( error = dmErrInvalidParam  );
	}

	MemSet( pProfile, sizeof( profiles_t ), 0 );

	error = DBOpen( &dbP, ProfilesDbName, ProfilesDbType, ProfilesDbCreator, dmModeReadOnly, ProfilesDbVersion, sizeof( profiles_t ) );
	
	if ( ( error == errNone ) && ( dbP ) )
	{
		MemHandle		recordH = NULL;
		profiles_t*		recordP = NULL;
		UInt16			numRecords = GetNumRecords( &dbP, dmAllCategories );
		
		if ( *idxP < numRecords )
		{
			recordH = DmQueryRecord( dbP, *idxP );
			
			if ( recordH )
			{		
				recordP = (profiles_t *) MemHandleLock( recordH );

				if ( recordP )
				{
					MemMove( pProfile, recordP, sizeof( profiles_t ) );
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
	
} // ProfilesGetRecordAtIdx()

/*
 * ProfilesSeekRecord()
 */
static Boolean ProfilesSeekRecord( DmOpenRef* dbPP, profiles_t* pProfile, UInt16* idxP, Boolean bLoadRecord, DmComparF* comparFP )
{
	Boolean 		retVal = false;
	UInt16			numRecords = GetNumRecords( dbPP, dmAllCategories );
	MemHandle		recordH = NULL;
	profiles_t*		recordP = NULL;
		
	for ( *idxP = 0 ; *idxP < numRecords ; (*idxP)++)
	{
		recordH = DmQueryRecord( *dbPP, *idxP );
		
		if ( recordH )
		{
			recordP = (profiles_t *) MemHandleLock( recordH );
			
			if ( recordP )
			{
				retVal = ( comparFP( recordP, pProfile, 0, NULL, NULL, NULL ) == 0 ); 
				
				if ( retVal )
				{
					if ( ( pProfile ) && ( bLoadRecord ) )
					{
						MemMove( pProfile, recordP, sizeof( profiles_t ) );	
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
	
} // ProfilesSeekRecord()

/*
 * ProfilesFindRecord
 */
Err ProfilesFindRecord( profiles_t* pProfile, UInt16* idxP, Boolean bLoadRecord, Boolean* bFound, DmComparF* comparFP )
{
	Err				error = errNone;
	DmOpenRef 		dbP = NULL;
	
	if ( bFound )
	{
		*bFound = false;
	}
	
	if ( !( ( pProfile ) && ( idxP ) ) )
	{ 
		return ( error = dmErrInvalidParam  );
	}

	error = DBOpen( &dbP, ProfilesDbName, ProfilesDbType, ProfilesDbCreator, dmModeReadOnly, ProfilesDbVersion, sizeof( profiles_t ) );
	
	if ( ( error == errNone ) && ( dbP ) )
	{
		*bFound = ProfilesSeekRecord( &dbP, pProfile, idxP, bLoadRecord, comparFP );
					
		DmCloseDatabase( dbP );
	}
	
	return ( error );
	
} // ProfilesFindRecord()

/*
 * Profiles_SortNameF()
 */
Int16 Profiles_SortNameF( profiles_t* recP1, profiles_t* recP2,
				Int16 other, SortRecordInfoPtr rec1SortInfo, SortRecordInfoPtr rec2SortInfo, MemHandle appInfoH )
{	
	Int16		retVal = 0;
	
	retVal = StrCompare( recP1->name, recP2->name );
	
	return ( retVal );
	
} // Profiles_SortNameF()

/*
 * Profiles_CompareUniqueIDF()
 */
Int16 Profiles_CompareUniqueIDF( profiles_t* recP1, profiles_t* recP2,
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
	
} // Profiles_CompareUniqueIDF()
 
/*
 * LoadProfilesList
 */
Boolean LoadProfilesList( ListType* pList, Boolean bMakeWithNone )
{
	Boolean			retVal = false;
	Err				error = errNone;
	UInt16			itemCount = 0;
	Char			nameStr[STRLEN_PROFILE_NAME];
	UInt16 			nameListSize = 0;
	UInt16			nameStrSize = 0;
	Char**          listOptions = NULL;
	UInt32			defaultUniqueID = ProfilesDBGetUniqueID();

	if ( gProfilesH != NULL ) // unlock... // don't want problems
	{
        MemHandleUnlock( gProfilesH );
        MemHandleFree( gProfilesH );
        gProfilesH = NULL;
    }

	gDefaultListItemIdx = -1;
	    
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
		
		error = DBOpen( &dbP, ProfilesDbName, ProfilesDbType, ProfilesDbCreator, dmModeReadOnly, ProfilesDbVersion, sizeof( profiles_t ) );
		
		if ( dbP )	
		{
			MemHandle		recordH = NULL;
			profiles_t*		recordP = NULL;
			UInt16			numRecords = DmNumRecordsInCategory( dbP, dmAllCategories );
			UInt16 			i = 0;
			
			if ( numRecords )
			{
				for (i = 0 ; i < numRecords ; i++)
				{
					recordH = DmQueryRecord( dbP, i );
					if ( recordH )
					{	
						recordP = (profiles_t *) MemHandleLock( recordH );
						
						if ( recordP )
						{
							StrCopy( nameStr, recordP->name );
				
							nameStrSize = StrLen( nameStr ) + 1;
							MemMove( gProfilesList + nameListSize, nameStr, nameStrSize );
							nameListSize = nameListSize + nameStrSize;
							
							if ( recordP->uniqueID == defaultUniqueID )				
							{
								gDefaultListItemIdx = itemCount;
							}
							
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
	
}  // LoadProfilessList

/*
 * ProfilesDBSetUniqueID()
 */
Err ProfilesDBSetUniqueID( UInt32 uniqueID )
{
	Err 					error = 0;
	UInt16 					cardNo = 0;
 	LocalID 				dbID = 0;
 	DmOpenRef				dbP; 
 	LocalID  				appInfoID;
	profilesInfoBlock_t* 	pProfilesInfo;
	profilesInfoBlock_t 	tmpProfilesInfo;

	error = DBOpen( &dbP, ProfilesDbName, ProfilesDbType, ProfilesDbCreator, dmModeReadOnly, ProfilesDbVersion, sizeof( profiles_t ) );
	if ( error ) return ( error );
	
	error = DmOpenDatabaseInfo( dbP, &dbID, NULL, NULL, &cardNo, NULL );	

	if ( !error )
	{
		error  = DmDatabaseInfo( cardNo, dbID, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &appInfoID, NULL, NULL, NULL );
			
		if ( !error )
		{
			if ( !appInfoID )
			{		
				ProfilesDBInitializeAppInfoBlock( dbP, cardNo, dbID, &appInfoID );
			}
		
			pProfilesInfo = (profilesInfoBlock_t *)MemLocalIDToLockedPtr( appInfoID, cardNo );				
			tmpProfilesInfo.defaultUniqueID = uniqueID;
			DmWrite( pProfilesInfo, 0, &tmpProfilesInfo, sizeof( profilesInfoBlock_t ) );
			MemPtrUnlock( pProfilesInfo );	

		}
	}
	
	DmCloseDatabase( dbP );
		
	return ( error );
	
} // ProfilesDBSetUniqueID()

/*
 * ProfilesDBGetUniqueID()
 */
UInt32 ProfilesDBGetUniqueID( void )
{
	UInt32					retVal = -1;
	Err 					error = 0;
	UInt16 					cardNo = 0;
 	LocalID 				dbID = 0;
 	DmOpenRef				dbP; 
 	LocalID  				appInfoID;
	profilesInfoBlock_t* 	pProfilesInfo;

	error = DBOpen( &dbP, ProfilesDbName, ProfilesDbType, ProfilesDbCreator, dmModeReadOnly, ProfilesDbVersion, sizeof( profiles_t ) );
	if ( error ) return ( retVal );
	
	error = DmOpenDatabaseInfo( dbP, &dbID, NULL, NULL, &cardNo, NULL );	

	if ( !error )
	{
		error  = DmDatabaseInfo( cardNo, dbID, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &appInfoID, NULL, NULL, NULL );
			
		if ( !error )
		{
			if ( !appInfoID )
			{		
				ProfilesDBInitializeAppInfoBlock( dbP, cardNo, dbID, &appInfoID );	
			}
			
			pProfilesInfo = (profilesInfoBlock_t *)MemLocalIDToLockedPtr( appInfoID, cardNo );
				
			retVal = pProfilesInfo->defaultUniqueID;
				
			MemPtrUnlock( pProfilesInfo );	
		}
	}
	
	DmCloseDatabase( dbP );
		
	return ( retVal );
	
} // ProfilesDBGetUniqueID()

/*
 * ProfilesDBInitializeAppInfoBlock()
 */
static Err ProfilesDBInitializeAppInfoBlock( DmOpenRef dbP,  UInt16 cardNo, LocalID dbID, LocalID* pAppInfoID )
{
	Err 					error = 0;
	MemHandle				memH = 0;
	profilesInfoBlock_t* 	pProfilesInfo;

	if ( !( *pAppInfoID ) )
	{		
		memH = DmNewHandle( dbP, sizeof( profilesInfoBlock_t ) );
		
		if ( !memH ) return ( dmErrMemError );
		
		*pAppInfoID = MemHandleToLocalID( memH );
		
		DmSetDatabaseInfo( cardNo, dbID, NULL, NULL, NULL, NULL, NULL, NULL, NULL, pAppInfoID, NULL, NULL, NULL );	
	}	
	
	pProfilesInfo = MemLocalIDToLockedPtr( *pAppInfoID, cardNo );
		
	DmSet( pProfilesInfo, 0, sizeof( profilesInfoBlock_t ), -1 );
		
	MemPtrUnlock( pProfilesInfo );
		
	return ( error );
	
} // ProfilesDBInitializeAppInfoBlock()

/*
 * ProfilesDB.c
 */