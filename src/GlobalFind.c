/*
 * GlobalFind.c
 */

#include "GlobalFind.h"

/*
 * DoFind()
 */
Err DoFind( FindParamsType* findParamsP )
{
	Err						error = errNone;
	DmOpenRef				dbP = NULL;
	UInt16					startIdx = ( findParamsP->continuation ) ? findParamsP->recordNum + 1 : 0;
	UInt16					idx = 0;
	UInt16					maxRecords = 0;
	MemHandle 				recordH = NULL;
	locations_t*			recordP = NULL;
	UInt16					cardNo = 0;
	LocalID					dbID = NULL;
	Char					strGID[STRLEN_GID] = "\0";
	Char* 					strToFind = findParamsP->strToFind;	
	UInt32					outPos = 0;
	UInt16					outLength = 0;
	RectangleType			rect;
	
	if ( FindDrawHeader( findParamsP, APP_NAME ) ) 
		return ( error );
	
	error = DBOpen( &dbP, GIDDbName, GIDDbType, GIDDbCreator, findParamsP->dbAccesMode, GIDDbVersion, sizeof( locations_t ) );
	
	if ( ( error == errNone ) && ( dbP ) )
	{
		DmOpenDatabaseInfo( dbP, &dbID, NULL, NULL, &cardNo, NULL );
		
		maxRecords = DmNumRecordsInCategory( dbP, dmAllCategories );

		findParamsP->more = ( startIdx < maxRecords );
		
		if ( findParamsP->more )
		{	
			for ( idx = startIdx ; idx < maxRecords ; idx++ )
			{
				if ( ( ( idx % 10 ) == 0 )
						&& EvtSysEventAvail( true ) )
				{
					break;
				}
				 
				recordH = DmQueryRecord( dbP, idx );	
				
				if ( recordH )
				{
					recordP = (locations_t *) MemHandleLock( recordH );
					
					if ( recordP )
					{
						MakeGIDString( &(recordP->GID), strGID );
						
						if ( TxtFindString( strGID, strToFind, &outPos, &outLength ) 
								|| TxtFindString( recordP->LocationName, strToFind, &outPos, &outLength )
								|| TxtFindString( recordP->RegionInfo.name, strToFind, &outPos, &outLength ) )
						{
							if ( FindSaveMatch( findParamsP, idx, 0, 0, recordP->uniqueID, cardNo, dbID ) )
							{
								MemHandleUnlock( recordH );
								recordP = NULL;
							
								break;
							}	
							
							WinPushDrawState();
							
							FindGetLineBounds( findParamsP, &rect );	
							
							DrawRow( recordP, &rect );
							
							WinPopDrawState();
							
							findParamsP->lineNumber++;
						}
					}
					
					MemHandleUnlock( recordH );
					recordP = NULL;
				}
			}
			
			findParamsP->more = ( idx < maxRecords );			
		}	
		
		DmCloseDatabase( dbP );
	}
		
	return ( error );
	
} // DoFind()


/*
 * GlobalFind.c
 */