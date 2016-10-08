/*
 * Bitmap.c
 */
 
#ifndef __BITMAPS_H__
#define __BITMAPS_H__

#include <Hs.h>
#include <HsPhone.h>
#include <HsNav.h>
#include <HsExt.h>
#include <palmOneResources.h>
#include <PalmTypes.h>

#include "AppResources.h"

/*
 * USAGE
 */
/* 
	UInt16					bmpID = 0;
	MemHandle 				bmpH = NULL;
	BitmapType* 			bmpP = NULL;
	Coord					bmpSizeX = 0;
	Coord					bmpSizeY = 0;
	
	bmpP = GetBitmap( bmpID, &bmpSizeX, &bmpSizeY, &bmpH );
	WinDrawBitmap( bmpP, X - bmpSizeX, Y - bmpSizeZ );
	ReleaseBitmap( &bmpH );	
	bmpH = NULL;
	bmpID = 0;
*/

// Prototypes
extern BitmapType* 				GetBitmap( DmResID resID, Coord* widthP, Coord* heightP, MemHandle* bmpHP );
extern void 					ReleaseBitmap( MemHandle* bmpHP );

#endif /* __BITMAPS_H__ */

/*
 * Bitmap.c
 */