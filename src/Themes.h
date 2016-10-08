/*
 * Themes.h
 */

#ifndef __THEMES_H__
#define __THEMES_H__

#include <Hs.h>
#include <PalmOS.h>
#include <palmOneResources.h>
#include <PalmTypes.h>
#include <Form.h>

#include "Global.h"
#include "AppResources.h"
#include "ProfilesDB.h"

// Defines
#define STR_LEN_THEME_STR						100

// Prototypes
extern Err 				GetThemeInfoFromIdx( UInt16 idx, themes_t* pTheme );
extern UInt16 			FindThemeIdx( Char* str );
extern Err 				LoadThemesListFromResource( ListType* pList );

#endif /* __THEMES_H__ */
 
/*
 * Themes.h
 */