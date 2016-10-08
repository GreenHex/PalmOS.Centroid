/*
 * TimeZone.h
 */

#ifndef __TIMEZONE_H__
#define __TIMEZONE_H__

#include <Hs.h>

#include "Sections.h"
#include "Global.h"
#include "AppResources.h"
#include "ProfilesDB.h"

// Prototypes
extern Err 				GetTimeZoneInfoFromIdx( UInt16 idx, timezone_t* pTimeZone ) EXTRA_SECTION_ONE;
extern UInt16 			FindTimeZoneIdx( Char* str ) EXTRA_SECTION_ONE;
extern Err 				LoadTimeZoneListFromDB( ListType* pList ) EXTRA_SECTION_ONE;

#endif /* __TIMEZONE_H__ */
 
/*
 * TimeZone.h
 */