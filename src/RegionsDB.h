/*
 * RegionsDB.h
 */
 
#ifndef __REGIONSDB_H__
#define __REGIONSDB_H__

#include <Hs.h>

#include "Sections.h"
#include "Global.h"
#include "AppResources.h"
#include "DBUtils.h"
#include "GID_DB.h"

#define RegionsDbCreator				appFileCreator
#define RegionsDbName					"CentroidRegionsDB"
#define RegionsDbType					'DRGN'
#define RegionsDbVersion				0x02

// #define STRLEN_REGION_NAME				STRLEN_PROFILE_NAME

typedef struct {
	UInt32								uniqueID;
	Char								name[STRLEN_PROFILE_NAME];
	locAlert_t							LocAlert;
	attachedProfile_t					AttachedProfile;
	msg_t								Msg; // v2
} regions_t;

// Prototypes
extern UInt16 				RegionGetNumberOfGIDs( UInt32 uniqueID ) EXTRA_SECTION_ONE;
extern UInt16 				RegionsGetNumberOfRecords( void );
extern Err 					RegionsSaveNewRecord( regions_t* pRegion, Boolean isSecret, UInt16* idxP );
extern Err 					RegionsReplaceRecordAtIdx( UInt16* idxP, regions_t* pRegion, Boolean isSecret );
extern Err 					RegionsRemoveRecordAtIdx( UInt16* idxP );
extern Err 					RegionsGetRecordAtIdx( UInt16* idxP, regions_t* pRegion );
extern Err 					RegionsFindRecord( regions_t* pRegion, UInt16* idxP, Boolean bLoadRecord, Boolean* bFound, DmComparF* comparFP );
extern Int16 				Regions_SortNameF( regions_t* recP1, regions_t* recP2, Int16 other, SortRecordInfoPtr rec1SortInfo, SortRecordInfoPtr rec2SortInfo, MemHandle appInfoH );
extern Int16 				Regions_CompareUniqueIDF( regions_t* recP1, regions_t* recP2, Int16 other, SortRecordInfoPtr rec1SortInfo, SortRecordInfoPtr rec2SortInfo, MemHandle appInfoH );
extern Boolean 				LoadRegionsList( ListType* pList, Boolean bMakeWithNone );

#endif /* __REGIONSDB_H__ */

/*
 * RegionsDB.h
 */