/*
 * LocForm.h
 */
 
#ifndef __LOCFORM_H__
#define __LOCFORM_H__

#include <Hs.h>
#include <PalmOS.h>
#include <Form.h>
#include <DataMgr.h>

#include "Sections.h"
#include "Global.h"
#include "AppResources.h"
#include "Utils.h"
#include "Tables.h"
#include "GID_DB.h"
#include "FormUtils.h"
#include "ProfilesDB.h"
#include "GeoInfo.h"
#include "RegionsDB.h"
#include "RegionEditForm.h"
#include "MessagingForm.h"

#define STRLEN_TIME_INTERVAL		20
UInt16								gLocFormOpenIdx; // idx of db record to open
Char								gStrPopup[STRLEN_PROFILE_NAME + 4];
FormType*							gLocForm;
locations_t*						gLocP;

// Prototypes
extern Boolean 						LocFormHandleEvent( EventType* pEvent ) EXTRA_SECTION_ONE;
extern void 						SetFrmView( FormType* pForm, locations_t* pLoc ) EXTRA_SECTION_ONE;

#endif /* __LOCFORM_H__ */
 
/*
 * LocForm.h
 */