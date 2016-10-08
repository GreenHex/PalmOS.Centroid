/*
 * Tables.h
 */

#ifndef __TABLES_H__
#define __TABLES_H__

#include <Hs.h>
#include <HsNav.h>
#include <HsExt.h>
#include <palmOneResources.h>
#include <Form.h>
#include <Field.h>
#include <Table.h>
#include <StringMgr.h>
#include <UIColor.h>
#include <HsExtCommon.h>

#include "Sections.h"
#include "GetGID.h"
#include "GID_DB.h"
#include "Utils.h"
#include "Bitmaps.h"
#include "Gadgets.h"

#define TABLE_ROW_HEIGHT			13

FormType*					gMainFormP;
TableType*					gGIDTableP;
UInt16						gNumTableRows;
UInt16						gTblColumnWidth;
Int16						gTopRow;
UInt16						gSelectedRow;
UInt16						gNumberGIDDBRecords;
UInt32						gModNum; 				// DB modification number
IndexedColorType			gTextColorIdx;
IndexedColorType			gSep1ColorIdx;
RGBColorType				gSep1ColorRGB;
IndexedColorType			gBackColorIdx;
RGBColorType				gBackColorRGB;

// Prototypes
extern Boolean 				ProcessScrollEvent2( FormType* pForm, EventType* pEvent, Boolean* bMenuDisplayedP ) EXTRA_SECTION_ONE;
extern void 				RedrawTable( FormType* pForm, TableType* pTbl ) EXTRA_SECTION_ONE;
extern void 				DrawFocusRing( FormType* pForm, TableType* pTbl ) EXTRA_SECTION_ONE;
extern void 				TableDrawItem( void* pTbl, Int16 row, Int16 column, RectangleType* rectP ) EXTRA_SECTION_ONE;
extern void 				DrawRow( locations_t* locP, RectangleType* rectP ); // *NO* EXTRA_SECTION_ONE
extern void 				TableInit( TableType* pTbl ) EXTRA_SECTION_ONE;

#endif /* __TABLES_H__*/
 
/*
 * Tables.h
 */