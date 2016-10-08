/*
 * BulkUtils.h
 */

#ifndef __BULKUTILS_H__
#define __BULKUTILS_H__

#include <Hs.h>
#include <DataMgr.h>

#include "Sections.h"
#include "Global.h"
#include "DBUtils.h"
#include "GID_DB.h"
#include "ProfilesDB.h"
#include "RegionsDB.h"
#include "DispProgress.h"

typedef enum {
	none								= 0,
	clearAlerts							= 1,
	clearProfiles						= 2,
	clearNewFlag						= 3,
	clearRegions						= 4,
	clearMsgs							= 5,
	purgeGeoInfo						= 6,
	rebuildDB							= 7
} bulkAction_e;

// Prototypes
extern Err 					GIDBulkAction( bulkAction_e action ) EXTRA_SECTION_ONE;

#endif /* __BULKUTILS_H__ */

 
/*
 * BulkUtils.h
 */