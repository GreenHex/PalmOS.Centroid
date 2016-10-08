/*
 * ProfilesDB.h
 */
 
#ifndef __PROFILESDB_H__
#define __PROFILESDB_H__

#include <Hs.h>
#include <PalmOS.h>
#include <LocsLib.h>
#include <List.h>

#include "Global.h"
#include "AppResources.h"
#include "GID_DB.h"

#define ProfilesDbCreator			appFileCreator
#define ProfilesDbName				"CentroidProfilesDB"
#define ProfilesDbType				'DPRF'
#define ProfilesDbVersion			0x00

#define STRLEN_PHONE_NUMBER			kMaxPhoneNumberLen
#define STRLEN_THEME_NAME			36
#define STRLEN_TZ_CITY_NAME			kLocNameMaxLength // 100 bytes

UInt16							gDefaultListItemIdx;

typedef struct {
	UInt32						defaultUniqueID;
} profilesInfoBlock_t;

typedef struct {
	Boolean						bMutePhone;
	Boolean						bMuteMessging;
	Boolean						bMuteCalendar;
	Boolean						bMuteSystem;
	Boolean						bMuteAlarm;
	Boolean						bMuteGame;
	Boolean						bVibrateON;
} mute_t;

typedef struct {
	Int16       				uTC;
    DSTType     				dSTStart;
    DSTType     				dSTEnd;
    Int16       				dSTAdjustmentInMinutes;
    DaylightSavingsTypes		dSTType;
    CountryType 				country;
    Char						name[STRLEN_TZ_CITY_NAME];
    PosType						position;
} timezone_t;

typedef struct {
	DmResID						themeID;
	Char						name[STRLEN_THEME_NAME];
} themes_t;

typedef struct {
	Boolean						bFwdCalls;
	Char						strFwdCallsNum[STRLEN_PHONE_NUMBER];
	mute_t						Mute;
	Boolean						bTimeZone;
	timezone_t					TimeZone;
	Boolean						bTheme;
	themes_t					Theme;
} profileSettings_t;

typedef struct {
	UInt32						uniqueID;
	Char						name[STRLEN_PROFILE_NAME];
	Boolean						bBetweenTime; // unused
	TimeType					startTime;
	TimeType					endTime;
	UInt16						validityIdx; // unused
	UInt32						validitySecs;
	profileSettings_t			Settings;
} profiles_t;

// Prototypes
extern UInt16 				ProfilesGetNumberOfRecords( void );
extern Err 					ProfilesSaveNewRecord( profiles_t* pProfile, Boolean isSecret, UInt16* idxP );
extern Err 					ProfilesReplaceRecordAtIdx( UInt16* idxP, profiles_t* pProfile, Boolean isSecret );
extern Err 					ProfilesRemoveRecordAtIdx( UInt16* idxP );
extern Err 					ProfilesGetRecordAtIdx( UInt16* idxP, profiles_t* pProfile );
extern Err 					ProfilesFindRecord( profiles_t* pProfile, UInt16* idxP, Boolean bLoadRecord, Boolean* bFound, DmComparF* comparFP );
extern Int16 				Profiles_SortNameF( profiles_t* recP1, profiles_t* recP2, Int16 other, SortRecordInfoPtr rec1SortInfo, SortRecordInfoPtr rec2SortInfo, MemHandle appInfoH );
extern Int16 				Profiles_CompareUniqueIDF( profiles_t* recP1, profiles_t* recP2, Int16 other, SortRecordInfoPtr rec1SortInfo, SortRecordInfoPtr rec2SortInfo, MemHandle appInfoH );
extern Boolean 				LoadProfilesList( ListType* pList, Boolean bMakeWithNone );
extern Err 					ProfilesDBSetUniqueID( UInt32 uniqueID );
extern UInt32 				ProfilesDBGetUniqueID( void );

#endif /* __PROFILESDB_H__ */

/*
 * ProfilesDB.h
 */