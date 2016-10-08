/*
 * GID_DB.h
 */
 
#ifndef __GID_DB_H__
#define __GID_DB_H__

#include <Hs.h>
#include <HsPhone.h>
#include <DataMgr.h>
#include <FloatMgr.h>
#include <HsPhoneNetworkTypes.h>

#include "Global.h"
#include "DBUtils.h"

#define GIDDbCreator					appFileCreator
#define GIDDbName						"CentroidGIDDB"
#define GIDDbType						'DGID'
#define GIDDbVersion					0x07	/* 0x07 -> Added "msg_t" to locations_t */
												/* 0x06 -> Added "RegionName" to locations_t */
												/* 0x05 -> Added "RegionID" to locations_t */
												/* 0x04 -> Added "PhnOperatorType" to locations_t */
											 	/* 0x03 -> Added "isValid" to geoInfo */ 
												/* 0x02 -> Adjusted geoInfo structure string lenghts */ 
												/* 0x01 -> Added "geoInfo_t" to locations_t */ 
												/* 0x00 -> Initial */

// Location
#define STRLEN_GID						20
#define STRLEN_LOCATION_NAME			STRLEN_GID // 20
#define STRLEN_ALERT_NOTE				64
// Geo Location
#define STRLEN_COUNTRY_CODE				3
#define STRLEN_REGION					24
#define STRLEN_CITY						24
#define STRLEN_STREET					24
#define STRLEN_ZIP						16
#define STRLEN_OPERATOR					MAX_LONGOPERATORNAME_LENGTH + 1
// Profiles
#define STRLEN_PROFILE_NAME				STRLEN_LOCATION_NAME // 20
// Messaging
#define STRLEN_PHN_NUMBER				kMaxPhoneNumberLen + 1
#define STRLEN_CLIR						24 + 1
#define STRLEN_MSG						160 + 1

typedef struct {
	char 								Address[STRLEN_PHN_NUMBER];
	char 								Message[STRLEN_MSG];
	char								CallerID[STRLEN_CLIR];
	Boolean								bAlertOnEntry;
	Boolean								bAlertOnExit;
	Boolean								bDisableAfterAlert;
} msg_t;

typedef struct {
	UInt32								MCC;
	UInt32								MNC;
	UInt32								LAC;
	UInt32								CellID;
} GID_t;

typedef struct {
	double								latitude;
	double								longitude;
	UInt32								altitude;
	UInt32								hor_error;
	UInt32								ver_error;
	Char		 						Country[STRLEN_COUNTRY_CODE];
	Char								Region[STRLEN_REGION];
	Char								City[STRLEN_CITY];
	Char								Street[STRLEN_STREET];
	Char								ZIP[STRLEN_ZIP];
	Boolean								isValid;
} geoInfo_t; // v1 database only

typedef struct {
	Boolean								bAlertOnEntry;
	Boolean								bAlertOnExit;
	Boolean								bDisableAfterAlert;
	TimeType							startTime;
	TimeType							endTime;
	Char								note[STRLEN_ALERT_NOTE];
} locAlert_t;

typedef struct {
	UInt32								profileID; // profile uniqueID
	UInt16								profileIdx; // idx, not used... always searched from "profileID"
	Char								profileName[STRLEN_PROFILE_NAME];
	UInt32								profileStartTime;
	UInt32								profileValidity; // seconds profile is valid...
} attachedProfile_t;

typedef struct {
	UInt32								regionID; // v5
	Char								name[STRLEN_PROFILE_NAME]; // v6 // used for search
} regionInfo_t;
	
typedef struct {
	UInt32								uniqueID; // For attention manager...
	GID_t								GID;
	UInt32								TimeAtLoc;
	Char								LocationName[STRLEN_LOCATION_NAME];
	UInt32								NumVisits;
	Boolean								isRoaming;
	Char								OperatorName[STRLEN_OPERATOR]; // v4
	Boolean								isNewLocation;
	locAlert_t							LocAlert;
	attachedProfile_t					AttachedProfile;
	geoInfo_t							GeoInfo; // v1
	regionInfo_t						RegionInfo; // v5, v6
	msg_t								Msg; // v7
} locations_t;
 
//
//	error = DBOpen( &dbP, GIDDbName, GIDDbType, GIDDbCreator, dmModeReadWrite );
//	error = DmCloseDatabase( dbP );
//
// Prototypes
extern Err 					GIDGetDBModificationInfo( UInt32* pModDate, UInt32* pModNum );
extern UInt16 				GIDGetNumberOfRecords( void );
extern Err	 				GIDGetNumberOfNewRecords( UInt16* newRecordsCountP, UInt16* roamingCountP, UInt16* recordsWithAlarmCountP, UInt16* recordsWithProfileCountP, UInt16* recordsWithMsgCountP, UInt16* recordsWithGeoInfoP );
extern Err 					GIDSaveNewRecord( locations_t* pLoc, Boolean isSecret, UInt16* idxP );
extern Err 					GIDReplaceRecordAtIdx( UInt16* idxP, locations_t* pLoc, Boolean isSecret );
extern Err 					GIDRemoveRecordAtIdx( UInt16* idxP );
extern Err 					GIDGetRecordAtIdx( UInt16* idxP, locations_t* pLoc );
extern Err 					GIDFindRecord( locations_t* pLoc, UInt16* idxP, Boolean bLoadRecord, Boolean* bFound, DmComparF* comparFP );
extern Boolean 				CompareGIDs( GID_t* pGID1, GID_t* pGID2 );
extern Int16 				GID_CompareF( locations_t* recP1, locations_t* recP2, Int16 other, SortRecordInfoPtr rec1SortInfo, SortRecordInfoPtr rec2SortInfo, MemHandle appInfoH );
extern Int16 				GID_SortTimeF( locations_t* recP1, locations_t* recP2, Int16 other, SortRecordInfoPtr rec1SortInfo, SortRecordInfoPtr rec2SortInfo, MemHandle appInfoH );
extern Int16 				GID_CompareUniqueIDF( locations_t* recP1, locations_t* recP2, Int16 other, SortRecordInfoPtr rec1SortInfo, SortRecordInfoPtr rec2SortInfo, MemHandle appInfoH );

#endif /* __GID_DB_H__ */

/*
 * GID_DB.h
 */