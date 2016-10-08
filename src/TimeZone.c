 /*
 * TimeZone.c
 */
 
#include "TimeZone.h"

/*
 * GetTimeZoneInfo()
 */
Err GetTimeZoneInfoFromIdx( UInt16 idx, timezone_t* pTimeZone )
{
	Err						error = errNone;
	UInt16					libRef = sysInvalidRefNum;
	DmOpenRef				defDBp = NULL;
	DmOpenRef				cusDBp = NULL;
	LocalID					localID = 0;
	LocDBInfoType			DBInfo;
	LocationType			locType;
	UInt16					numRecords = 0;
	LocationType**			locTypeListPP;
	
	error = SysLibFind( kLocsLibName, &libRef );
	if ( error )
	{
		SysLibLoad( sysFileTLibrary, kLocsFileCLocsLib, &libRef );
	}
		
	error = LocsLibOpen( libRef );
	if ( error ) return ( error );
	
	localID = LocDBOpenDefault( libRef, &defDBp );
	if ( !localID ) return ( dmErrCantOpen );
	
	DBInfo.dbName = kLocFileNCustomDB;
	DBInfo.dbCreatorID = kLocFileCDatabase;
	DBInfo.dbType = kLocFileTCusLocDB;
	DBInfo.dbCardNum = 0;

	localID = LocDBOpenCustom( libRef, &DBInfo, dmModeReadOnly, &cusDBp, defDBp ); // dmModeReadOnly
	if ( !localID ) return ( dmErrCantOpen );

	locTypeListPP =  LocDBGetRecordListV20( libRef, cusDBp, &numRecords, NULL, NULL );

	if ( numRecords > idx )
	{	
		LocationInitV20( libRef, &locType );
		LocDBConvertRecordV20( libRef, &locType, locTypeListPP[idx] );
		
		pTimeZone->uTC = locType.uTC;
		pTimeZone->dSTStart = locType.dSTStart;
		pTimeZone->dSTEnd = locType.dSTEnd;
		pTimeZone->dSTAdjustmentInMinutes = locType.dSTAdjustmentInMinutes;
		pTimeZone->country = locType.country;
		StrCopy(pTimeZone->name, locType.name);
		pTimeZone->position = locType.position;
		
		LocationFinalV20( libRef, &locType );
	}
	
	error = DmCloseDatabase( cusDBp );
	error = DmCloseDatabase( defDBp );
	error = LocsLibClose( libRef );
	
	return ( error );
	
} // GetTimeZoneInfo()

/*
 * FindTimeZoneIdx()
 */
UInt16 FindTimeZoneIdx( Char* str )
{
	UInt16					retVal = noListSelection;
	Err						error = errNone;
	UInt16					libRef = sysInvalidRefNum;
	DmOpenRef				defDBp = NULL;
	DmOpenRef				cusDBp = NULL;
	LocalID					localID = 0;
	LocDBInfoType			DBInfo;
	LocationType			locType;
	UInt16					numRecords = 0;
	LocationType**			locTypeListPP;	
	UInt16					i = 0;
	
    error = SysLibFind( kLocsLibName, &libRef );
    
	if ( error )
	{
		SysLibLoad( sysFileTLibrary, kLocsFileCLocsLib, &libRef );
	}
		
	error = LocsLibOpen( libRef );
	if ( error ) return ( retVal );
	
	localID = LocDBOpenDefault( libRef, &defDBp );
	if ( !localID ) return ( dmErrCantOpen );
	
	DBInfo.dbName = kLocFileNCustomDB;
	DBInfo.dbCreatorID = kLocFileCDatabase;
	DBInfo.dbType = kLocFileTCusLocDB;
	DBInfo.dbCardNum = 0;

	localID = LocDBOpenCustom( libRef, &DBInfo, dmModeReadOnly, &cusDBp, defDBp ); // dmModeReadOnly
	if ( !localID ) return ( dmErrCantOpen );

	locTypeListPP =  LocDBGetRecordListV20( libRef, cusDBp, &numRecords, NULL, NULL );

	if ( numRecords )
	{
		for ( i = 0 ; i < numRecords ; i++ )
		{
			LocationInitV20( libRef, &locType );
			LocDBConvertRecordV20( libRef, &locType, locTypeListPP[i] );
			
			if ( StrCompare( locType.name, str ) == 0 )
			{
				retVal = i;
				break;
			}
			
			LocationFinalV20( libRef, &locType );
		}
	}
	error = DmCloseDatabase( cusDBp );
	error = DmCloseDatabase( defDBp );
	error = LocsLibClose( libRef );
	
	return ( retVal );
	
} // FindTimeZoneIdx()

/*
 * LoadLocsTZ
 */
Err LoadTimeZoneListFromDB( ListType* pList )
{
	Err						error = errNone;	
	UInt16					libRef = sysInvalidRefNum;
	DmOpenRef				defDBp = NULL;
	DmOpenRef				cusDBp = NULL;
	LocalID					localID = 0;
	LocDBInfoType			DBInfo;
	LocationType			locType;
	UInt16					numRecords = 0;
	LocationType**			locTypeListPP;
	UInt16					listItemLen = 0;
	UInt16					listLen = 0;
	Char**					listOptions;
	UInt16					i = 0;
	
	if ( gTimeZoneH ) // unlock... // don't want problems
	{
        MemHandleUnlock( gTimeZoneH );
        MemHandleFree( gTimeZoneH );
        gTimeZoneH = NULL;
    }
	
    error = SysLibFind( kLocsLibName, &libRef );
	if (error)
	{
		SysLibLoad( sysFileTLibrary, kLocsFileCLocsLib, &libRef );
	}
		
	error = LocsLibOpen( libRef );
	if ( error ) return ( error );

	localID = LocDBOpenDefault( libRef, &defDBp );
	if ( !localID ) return ( dmErrCantOpen );
	
	DBInfo.dbName = kLocFileNCustomDB;
	DBInfo.dbCreatorID = kLocFileCDatabase;
	DBInfo.dbType = kLocFileTCusLocDB;
	DBInfo.dbCardNum = 0;

	localID = LocDBOpenCustom( libRef, &DBInfo, dmModeReadOnly, &cusDBp, defDBp );
	if ( !localID ) return ( dmErrCantOpen );

	locTypeListPP =  LocDBGetRecordListV20( libRef, cusDBp, &numRecords, NULL, NULL );

	if ( numRecords )
	{
		for ( i = 0 ; i < numRecords ; i++ )
		{
			LocationInitV20( libRef, &locType );
			LocDBConvertRecordV20( libRef, &locType, locTypeListPP[i] );
			
			listItemLen = StrLen( locType.name ) + 1;
			MemMove( gTimeZoneList + listLen, locType.name, listItemLen );
			listLen += listItemLen;
			
			LocationFinalV20( libRef, &locType );
		}
	}
	error = DmCloseDatabase( cusDBp );
	error = DmCloseDatabase( defDBp );
	error = LocsLibClose( libRef );
	
	gTimeZoneH = SysFormPointerArrayToStrings( gTimeZoneList, numRecords );		
	listOptions = MemHandleLock( gTimeZoneH ); // to be unlocked somewhere...
	LstSetListChoices( pList, listOptions, numRecords ); 
	
	return ( error );

} // LoadLocsTZ

/*
 * TimeZone.c
 */