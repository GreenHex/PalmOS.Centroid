/*
 * PhoneProfilesForm.h
 */
 
#ifndef __PHONEPROFILESFORM_H__
#define __PHONEPROFILESFORM_H__

#include <Hs.h>
#include <PalmOS.h>

#include "Sections.h"
#include "Global.h"
#include "AppResources.h"
#include "Utils.h"
#include "Tables.h"
#include "GID_DB.h"
#include "FormUtils.h"
#include "ProfilesDB.h"
#include "ProfileSwitch.h"

// Globals
UInt16						gProfileIdx;
ListType*					gProfilesListP;
UInt16						gDefaultListItemIdx;

// Prototypes
extern Boolean 				ProfilesFormHandleEvent( EventType* pEvent ) EXTRA_SECTION_ONE;
extern void 				ProfilesListDrawFunction( Int16 itemNum, RectangleType* bounds, Char** itemsText ) EXTRA_SECTION_ONE;
extern Boolean 				ProfilesSetPopupTextFromList( ControlType* pCtl, ListType* pList, UInt16 selection );
extern void 				SetListFromProfileID( ListType* pList, UInt32 profileID );

#endif /* __PHONEPROFILESFORM_H__ */

/*
 * PhoneProfilesForm.h
 */