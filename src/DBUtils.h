/*
 * DBUtils.h
 */
 
#ifndef __DBUTILS_H__
#define __DBUTILS_H__

#include <Hs.h>
#include <DataMgr.h>

#include "Global.h"

// Constants
// dmAllCategories 
// dmModeReadOnly | dmModeShowSecret 
/*
 * typedef enum privateRecordViewEnum {
 * showPrivateRecords = 0x00,
 * maskPrivateRecords,
 * hidePrivateRecords
 * } privateRecordViewEnum;
 * 
 * 	if ( PrefGetPreference( prefShowPrivateRecords ) == hidePrivateRecords )
	{
		dmReadMode = mode;
	}
	else
	{
		dmReadMode = mode | dmModeShowSecret;
	}
 *
 */
 
// Prototypes
extern Err 				DBOpen( DmOpenRef* dbPP, Char* dbName, UInt32 dbType, UInt32 dbCreatorID, UInt16 dbOpenMode, UInt16 dbVersion, UInt16 newRecordSize );
extern UInt16 			GetNumRecords( DmOpenRef* dbPP, UInt16 categoryIdx );
extern Err 				ReleaseRecord( DmOpenRef* dbPP, UInt16* idxP );

#endif /* __DBUTILS_H__ */

/*
 * DBUtils.h
 */