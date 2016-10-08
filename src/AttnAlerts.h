/*
 * AttnAlerts.h
 */
 
#ifndef __ATTNALERTS_H__
#define __ATTNALERTS_H__

#include <Hs.h>
#include <PalmOS.h>
#include <SoundMgr.h>
#include <AttentionMgr.h>

#include "Global.h"
#include "AppResources.h"
#include "GID_DB.h"
#include "GetGID.h"
#include "RegionsDB.h"
#include "Prefs.h"
#include "Bitmaps.h"
#include "Tables.h"
#include "Sounds.h"

//Prototypes
extern void				DoAlert( lastLocation_t* pLastLoc, attentionMode_e attnMode );
extern void 			ProcessAttention( AttnLaunchCodeArgsType* paramP );
extern void 			DoGoto( UInt32 uniqueID );

#endif /* __ATTNALERTS_H__ */

/*
 * AttnAlerts.h
 */