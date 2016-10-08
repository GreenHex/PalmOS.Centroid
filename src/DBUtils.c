/*
 * DBUtils.c
 */
 
#include "DBUtils.h"

// Prototypes
static Err 			ResizeDatabaseRecords( UInt32 dbType, UInt32 dbCreatorID, UInt16 dbVersion, UInt16 newRecordSize );

/*
 * DBOpen
 */
Err DBOpen( DmOpenRef* dbPP, Char* dbName, UInt32 dbType, UInt32 dbCreatorID, UInt16 dbOpenMode, UInt16 dbVersion, UInt16 newRecordSize )
{
	Err 			error = dmErrCantOpen;
	DmOpenRef 		hdbP = NULL;
	UInt16			cardNo = 0;
	LocalID  		dbID = 0;
	UInt16			attributes = dmHdrAttrCopyPrevention | dmHdrAttrHidden | dmHdrAttrBackup;
	UInt16			version = dbVersion;
	UInt16			openMode = dmModeReadOnly; // junk initial value
			
	*dbPP = NULL;
	
	hdbP = DmOpenDatabaseByTypeCreator( dbType, dbCreatorID, dbOpenMode );

	if ( hdbP )
	{
		if ( newRecordSize ) // special case when DB needs to be upgraded
		{
			if ( DmOpenDatabaseInfo( hdbP, &dbID, NULL, &openMode, &cardNo, NULL ) == errNone )
			{	
				if ( DmDatabaseInfo( cardNo, dbID, NULL, &attributes, &version, 
								NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL ) == errNone )
				{
					if ( version != dbVersion )
					{
						DmCloseDatabase( hdbP );
						hdbP = NULL;
						
						if ( ( error = ResizeDatabaseRecords( dbType, dbCreatorID, dbVersion, newRecordSize ) ) == errNone )					
						{
							hdbP = DmOpenDatabaseByTypeCreator( dbType, dbCreatorID, dbOpenMode );
						}
					}						
				}
			}
		}
	}	
	else // if ( !hdbP ) // create the database...
	{	
		error = DmCreateDatabase( cardNo, dbName, dbCreatorID, dbType, false ); // "true" to create a resource DB
		
		if ( error )
		{
			return ( error );
		}
					
		hdbP = DmOpenDatabaseByTypeCreator( dbType, dbCreatorID, dbOpenMode );
		if ( !hdbP )
		{
			return ( DmGetLastErr() );
		}	
		
		dbID = DmFindDatabase( cardNo, dbName );
		
		// this returns error, don't know why
		DmSetDatabaseInfo( cardNo, dbID, NULL, &attributes, &version, 
							NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL );				
	}

	if ( hdbP )
	{
		error = errNone;	
		*dbPP = hdbP;
	}

	return ( error );
	
} // DBOpen()

/*
 * ResizeDatabaseRecords()
 */
static Err ResizeDatabaseRecords( UInt32 dbType, UInt32 dbCreatorID, UInt16 dbVersion, UInt16 newRecordSize )
{
	Err					error = errNone;
	DmOpenRef			dbP = NULL;
	UInt16				cardNo = 0;
	LocalID  			dbID = 0;
	UInt16				idx = 0;
	UInt16				maxRecords = 0;
	
	if ( !newRecordSize ) return ( error );
	
	dbP = DmOpenDatabaseByTypeCreator( dbType, dbCreatorID, dmModeExclusive | dmModeReadWrite );
	
	if ( dbP )
	{
		maxRecords = DmNumRecordsInCategory( dbP, dmAllCategories );
		
		if ( maxRecords ) 
		{
			for ( idx = 0 ; idx < maxRecords ; idx++ )
			{
				if ( ! DmResizeRecord( dbP, idx, newRecordSize ) )
				{	
					return ( DmGetLastErr() );
				}
			}
		}
		
		if ( DmOpenDatabaseInfo( dbP, &dbID, NULL, NULL, &cardNo, NULL ) == errNone )
		{
			DmSetDatabaseInfo( cardNo, dbID, NULL, NULL, &dbVersion, 
									NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL );
		}
	}
	else
	{
		error = DmGetLastErr();
	}
	
	DmCloseDatabase( dbP );
	
	return ( error );
	
} // ResizeDatabaseRecords()

/*
 * GetNumRecords()
 */
UInt16 GetNumRecords( DmOpenRef* dbPP, UInt16 categoryIdx )
{
	UInt16				retVal = 0;
	
	if ( *dbPP )
	{
		retVal = DmNumRecordsInCategory( *dbPP, categoryIdx );		
	}
	
	return ( retVal );
	
} // GetNumRecords()


/*
 * releaseRecord()
 */
Err ReleaseRecord( DmOpenRef* dbPP, UInt16* idxP )
{
	Err			error = errNone;
	UInt16		attrP = 0;
		
	if ( ( error = DmRecordInfo( *dbPP, *idxP, &attrP, NULL, NULL ) ) == errNone )
	{
		if ( attrP & dmRecAttrBusy )
		{							
			error = DmReleaseRecord( *dbPP, *idxP, false );
		}
	}
	
	return ( error );
	
} // releaseRecord()
 
/*
 * DBUtils.c
 */