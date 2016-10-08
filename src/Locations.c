/*
 * Locations.c
 */
 
#include "Locations.h"

// Prototypes
static void 				AddNewGIDRecord( locations_t* pLoc, appPrefs_t* pPrefs );

/* 
 * DoLocation()
 */
void DoLocation( void )
{
	UInt16				idx = dmMaxRecordIndex;
	Boolean				bFound = false;
	appPrefs_t 			prefs;
	locations_t			loc;
	lastLocation_t		lastLoc;
	lastLocation_t		currentLoc;
	UInt32				timeNow = TimGetSeconds();
	UInt32				GIDValiditySecs[8] = { 0, 60, 120, 180, 300, 420, 600, 900 };
	Boolean				bDoAlert = false;
	
	MemSet( &loc, sizeof( locations_t ), 0 );
	
	getMNCMCC( &( loc.GID.MCC ), &( loc.GID.MNC ) );
	
	if ( IsCentro() )
	{
		getLACCID( &( loc.GID.LAC ), &( loc.GID.CellID ) );
	}
	else
	{
		netClose();
		getSerialLACCID( &( loc.GID.LAC ), &( loc.GID.CellID ) );
	}
	
	if ( !( ( loc.GID.MCC ) && ( loc.GID.MNC ) && ( loc.GID.LAC ) && ( loc.GID.CellID ) ) )
		return;
		
	ReadAppPrefs( &prefs );
	if ( prefs.bIgnoreCellID ) loc.GID.CellID = 0;
	
	ReadLastLocationPrefs( &lastLoc );			
	if ( !( timeNow > ( lastLoc.TimeAtLoc + GIDValiditySecs[prefs.GIDValidityIdx] ) ) )
		return;
		
	if ( CompareGIDs( &(loc.GID), &(lastLoc.GID) ) )
		return;
	
	if ( GIDFindRecord( &loc, &idx, true, &bFound, (DmComparF *) GID_CompareF ) )
		return; // error in database
	
	if ( loc.RegionInfo.regionID )
	{
		bDoAlert = ( loc.RegionInfo.regionID != lastLoc.regionID ) ;
	}
	else
	{
		bDoAlert = true;
	}
	
	currentLoc.GID = loc.GID;
	currentLoc.uniqueID = loc.uniqueID;
	currentLoc.profileID = loc.AttachedProfile.profileID;
	currentLoc.regionID = loc.RegionInfo.regionID;
	currentLoc.TimeAtLoc = loc.TimeAtLoc = timeNow;
	
	WriteLastLocationPrefs( &currentLoc );

	loc.isRoaming = IsRoaming();
	GetPhnOperator( loc.OperatorName, STRLEN_OPERATOR );
			 
	if ( bFound )
	{	
		++( loc.NumVisits );
								
		GIDReplaceRecordAtIdx( &idx, &loc, false );			
		
		if ( bDoAlert )
		{
			DoAlert( &lastLoc, onExit );
			MsgNotify( &lastLoc, onExit );
			//
			DoAlert( &currentLoc, onEntry );
			DoProfileSwitch( &currentLoc );
			MsgNotify( &currentLoc, onEntry );			
		}
	}
	else
	{
		loc.uniqueID = timeNow;
		loc.NumVisits = 1;
		loc.isNewLocation = true;
		loc.LocAlert.startTime.hours = loc.LocAlert.startTime.minutes = -1; 
		loc.LocAlert.endTime.hours = loc.LocAlert.endTime.minutes = -1;
		
		AddNewGIDRecord( &loc, &prefs );
	}		
			
	return;
	
} // DoLocation()

/*
 * AddNewGIDRecord()
 */
static void AddNewGIDRecord( locations_t* pLoc, appPrefs_t* pPrefs )
{
	UInt16			idx = dmMaxRecordIndex;
	
	if ( !pPrefs->bAddLocs ) return;
	
	GIDSaveNewRecord( pLoc, false, &idx );
	
	SndPlaySystemSound( sndClick );
		
	if ( !pPrefs->bAutoAddLocs )
	{
		ShowAddLocForm( pLoc );		
	}

	return;
	
} // AddNewGIDRecord()

/*
 * Locations.c
 */