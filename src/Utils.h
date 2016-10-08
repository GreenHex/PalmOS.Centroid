/*
 * Utils.h
 */

#ifndef __UTILS_H__
#define __UTILS_H__

#include <Hs.h>
#include <HsPhone.h>
#include <HsNav.h>
#include <HsExt.h>
#include <palmOneResources.h>	
#include <Form.h>
#include <StringMgr.h>
#include <PmSysGadgetLib.h>
#include <PmSysGadgetLibCommon.h>

#include "Global.h"
#include "AppResources.h"
#include "Prefs.h"
#include "GID_DB.h"
#include "GetGID.h"
#include "SignalLvl.h"
#include "ProfilesDB.h"
#include "RegionsDB.h"

#define PROFILE_STR_LOC_X				4
#define PROFILE_STR_LOC_Y				136
#define PROFILE_STR_WIDTH				152
#define PROFILE_STR_HEIGHT				12

#define GID_STR_LOC_X					4
#define GID_STR_LOC_Y					148
#define GID_STR_WIDTH					152
#define GID_STR_HEIGHT					12

#define MIN_VERSION  					sysMakeROMVersion( 5, 0, 0, sysROMStageRelease, 0 )
#define LAUNCH_FLAGS 					( sysAppLaunchFlagNewGlobals | sysAppLaunchFlagUIApp )

#define STR_NAME_CARD_GID_DB			"/PALM/Programs/Centroid/CentroidGIDDB.PDB"
#define STR_NAME_CARD_PROFILES_DB		"/PALM/Programs/Centroid/CentroidProfilesDB.PDB"
#define STR_NAME_CARD_REGIONS_DB		"/PALM/Programs/Centroid/CentroidRegionsDB.PDB"

/*
enum fontID {
  stdFont = 0x00, 
  boldFont, 
  largeFont, 
  symbolFont, 
  symbol11Font, 
  symbol7Font, 
  ledFont, 
  largeBoldFont, 
  fntAppCustomBase = 0x80
};
*/

typedef enum {
	colorDefault						= 0,
	colorBlue 							= 0,
	colorGreen 							= 1,
	colorRed							= 2
} color_e;

typedef enum {
	hAlignLeft							= -1,
	hAlignCenter						= 0,
	hAlignRight							= 1
} hAlign_e;

typedef enum {
	vAlignTop							= -1,
	vAlignMiddle						= 0,
	vAlignBottom						= 1
} vAlign_e;

// Prototypes
extern long 					StrHexToInt(const char* pszHex);
extern Char* 					DoubleToString( double dbl, Char* strDbl );
extern double					StringToDouble( Char* strDbl );
extern void 					MakeTimeInterval( TimeType startTime, TimeType endTime, Char* strTimeInterval );
extern void 					MakeTimeDateStr( UInt32 timeSecs, Char** strTimeDatePP, Char* strTimeP, Char* strDateP, Boolean bRemoveYear );
extern void 					ReplaceDateTimeTokens( Char* str, UInt16 strLen );
extern void 					DrawStatusStr( Char* str, RectangleType* pRect, FontID fontID, hAlign_e hAlign, vAlign_e vAlign, color_e color, Boolean draw );
extern void 					GetStrDelayTime( UInt32 delayTime, Char* str );
extern Char* 					ReplaceChar(unsigned char *s, char orgChr, char newChr);
extern Char* 					StripSpaces(unsigned char *s);
extern Char* 					MakeNum( unsigned char* s );
extern Boolean 					is_display_off( void );
extern Boolean 					IsPhoneReady( void );
extern Boolean 					IsRoaming( void );
extern Err 						GetPhnOperator( Char* operatorName, UInt16 operatorNameStrLen );
extern Boolean 					IsPhoneGSM( void );
extern Boolean 					IsCentro( void );
extern Err 						RomVersionCompatible( UInt32 requiredVersion, UInt16 launchFlags );
extern void 					CommonInitGauge( FormPtr pForm );
extern Boolean 					AboutFormHandleEvent( EventType* pEvent );
extern void 					BackupDBs( void );
extern Err 						GetLastBackupDate( Char* strModDate );
extern void 					LEDState( Boolean set, UInt16* countP, UInt16* stateP );
extern void 					FlashLED( Boolean blink, UInt8 BlinkType );

#endif /* __UTILS_H__ */

/*
 * Utils.h
 */ 