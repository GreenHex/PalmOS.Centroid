/*
 * Tables.c
 */

#include "Tables.h"

// Prototypes

/*
 * ProcessScrollEvent()
 */
Boolean ProcessScrollEvent2( FormType* pForm, EventType* pEvent, Boolean* bMenuDisplayedP )
{
	Boolean 				handled = false;
	TableType*				pTbl = FrmGetObjectPtr( pForm, FrmGetObjectIndex( pForm, MAIN_TBL ) );
	static UInt16			oldTopRow = 0;
	
	switch ( pEvent->eType )
	{
		case keyDownEvent:

			switch ( pEvent->data.keyDown.chr )
			{
				case chrVerticalTabulation:
				case vchrRockerUp:
				case vchrRockerLeft:
					
					if ( gSelectedRow == -1 )
					{
						if ( gNumberGIDDBRecords < gNumTableRows)
						{
							gSelectedRow = gNumberGIDDBRecords - 1;
						}
						else
						{
							gSelectedRow = gNumTableRows - 1 ;
						}
					}
					else if ( gSelectedRow > 0 )
					{
						--gSelectedRow;
					}
					else
					{
						--gTopRow;
					}
					
					handled = true;
					
					break;
					
				case chrFormFeed:
				case vchrRockerDown:
				case vchrRockerRight:

					if ( gSelectedRow == -1 )
					{
						gSelectedRow = 0 ;
					}
					else if ( ( gSelectedRow < ( gNumTableRows - 1 ) ) 
								&& ( gSelectedRow < gNumberGIDDBRecords - 1 ) )
					{
						++gSelectedRow;
					}
					else
					{
						++gTopRow;
					}
					
					handled = true;
								
					break;

				case chrSpace:
					
					++gTopRow;
									
					break;
					
					handled = true;
					
				case chrBackspace:
					
					if ( pEvent->data.keyDown.modifiers & shiftKeyMask )
					{
						gTopRow = gNumberGIDDBRecords - gNumTableRows; // handled below...
					}
					else
					{
						gTopRow = 0;
					}
					
					handled = true;
					
					break;
					
				case chrDigitZero:
				case chrFullStop:
					
					gSelectedRow = 0;
					
					handled = true;
				
					break;

				case vchrRockerCenter:
				case hsChrSide:
				case chrLineFeed:
					
					if ( gTopRow + gSelectedRow < gNumberGIDDBRecords  )
					{
						gLocFormOpenIdx = gTopRow + ( ( gSelectedRow == -1 ) ? 0 : gSelectedRow);
						
						FrmPopupForm( LOC_FORM );				
					}
					
					handled = true;
						
					break;
				
				default:
					break;			
			}			

			// validation...
			if ( ( gTopRow < 0 ) || ( gNumTableRows >= gNumberGIDDBRecords ) )
			{
				gTopRow = 0;
			}
			else if ( ( gTopRow + gNumTableRows ) > gNumberGIDDBRecords )
			{
				gTopRow = ( gNumberGIDDBRecords - gNumTableRows );
			}

			if ( handled )
			{
				*bMenuDisplayedP = false;
				
				if ( oldTopRow != gTopRow ) 
				{
					oldTopRow = gTopRow;
					
					TblMarkTableInvalid( pTbl );
					TblRedrawTable( pTbl );	
				}
				
				DrawFocusRing( pForm, pTbl );
			}		
			
			break;					
/*	
		case penDownEvent:
			
			break;
			
		case penUpEvent:
		
			break;
*/			
		case tblSelectEvent:
		
			TblUnhighlightSelection( pTbl );
		
			gSelectedRow = pEvent->data.tblSelect.row;
			
			if ( ( gTopRow + gSelectedRow ) < gNumberGIDDBRecords  )
			{
				gLocFormOpenIdx = gTopRow + ( ( gSelectedRow == -1 ) ? 0 : gSelectedRow);
				
				FrmPopupForm( LOC_FORM );
			}
						
			handled = true;
			
			break;
			
		case tblEnterEvent:
		
			gSelectedRow = pEvent->data.tblEnter.row;
			
			*bMenuDisplayedP = false;
			
			if ( gSelectedRow > gNumberGIDDBRecords - 1 )
			{
				gSelectedRow = gNumberGIDDBRecords - 1;
			}
			
			DrawFocusRing( pForm, pTbl );
			
			TblUnhighlightSelection( pTbl );
			
			// handled = true; // no longer receive the pen-up event!
		
			break;
			
		case tblExitEvent:

			handled = false;
		
			break;
		
		case nilEvent:
		
			if ( *bMenuDisplayedP ) break;
			
			RedrawTable( pForm, pTbl );
			
			UpdateGadgets();
		
			handled  = true;
			
			break;
									
		default:
			break;	
	}
	
	return ( handled );
	
} // ProcessScrollEvent2()

/*
 * RedrawTable()
 */
void RedrawTable( FormType* pForm, TableType* pTbl )
{
	UInt32					modNum = 0;
	static UInt32			oldModNum = 0;
	static UInt16			oldNumberGIDDBRecords = 0;
	
	GIDGetDBModificationInfo( NULL, &modNum );
	gNumberGIDDBRecords = GIDGetNumberOfRecords();
	
	if ( ( oldModNum != modNum ) || ( oldNumberGIDDBRecords != gNumberGIDDBRecords ) )
	{
		oldModNum = modNum;
		oldNumberGIDDBRecords = gNumberGIDDBRecords;
		
		TblMarkTableInvalid( pTbl );
		TblRedrawTable( pTbl );
		
		DrawFocusRing( pForm, pTbl );	
	}

	return;
	
} // RedrawTable()

/*
 * DrawFocusRing()
 */
void DrawFocusRing( FormType* pForm, TableType* pTbl )
{
	RectangleType			rowRect;
	 
	if ( ( gSelectedRow != -1 ) && ( gNumberGIDDBRecords > 0 ) )
	{
		TblGetItemBounds( pTbl, gSelectedRow, 0, &rowRect );
			
		FrmNavDrawFocusRing( pForm, MAIN_TBL, 
								gSelectedRow, &rowRect, 
								frmNavFocusRingStyleSquare, false );
	}
 	
 	return;
 	
} // DrawFocusRing()
 		
/*
 * TableDrawItem
 */
void TableDrawItem( void* pTbl, Int16 row, Int16 column, RectangleType* rectP )
{
	UInt16					idx = row + gTopRow;
	locations_t				loc;

	WinPushDrawState();
		
    WinSetForeColor( gSep1ColorIdx );
	WinSetTextColor( gTextColorIdx );

	if ( row % 2 )
	{
		WinDrawRectangle( rectP, 0 );
		WinSetBackColor( gSep1ColorIdx );
	}
	else
	{
		WinSetBackColor( gBackColorIdx );	
	}
						
  	if ( GIDGetRecordAtIdx( &idx, &loc ) == errNone )
	{
		DrawRow( &loc, rectP );
	} 

	WinPopDrawState();
	
	return;
	
} // TableDrawItem

/*
 * DrawRow()
 */
void DrawRow( locations_t* locP, RectangleType* rectP )
{
	Char					strLoc[STRLEN_GID];
	Char					strTime[timeStringLength] = "\0";
	Char					strDate[longDateStrLength] = "\0";
	UInt16					strHeight = 0;
    UInt16					strWidth = 0;
    RGBColorType 			rgb;
    IndexedColorType		StrColor;
    UInt16					bmpID = 0;
    MemHandle 				bmpH = NULL;
	BitmapType* 			bmpP = NULL;
	Coord					bmpSizeX = 0;
	Coord					bmpSizeY = 0;
	UInt32					defaultProfileID = 0;
	regions_t				region;
	locAlert_t				alert;
	attachedProfile_t		attachedProfile;
	msg_t					msg;
	UInt16					regionIdx;
	Boolean					bFound = false;
	
	region.uniqueID = locP->RegionInfo.regionID;
		
	if ( ( locP->RegionInfo.regionID )		
			&& ( !RegionsFindRecord( &region, &regionIdx, true, &bFound, (DmComparF *) &Regions_CompareUniqueIDF ) )
			&& ( bFound ) )
	{
		alert = region.LocAlert;
		attachedProfile = region.AttachedProfile;
		msg = region.Msg;
		
		bmpID = BMP_TBL_REGION_INDICATOR;
		
		bmpP = GetBitmap( bmpID, &bmpSizeX, &bmpSizeY, &bmpH );
		WinDrawBitmap( bmpP, rectP->topLeft.x, rectP->topLeft.y );
		ReleaseBitmap( &bmpH );	
		bmpH = NULL;
		bmpID = 0;
	}
	else
	{
		alert = locP->LocAlert;
		attachedProfile = locP->AttachedProfile;
		msg = locP->Msg;
	}
	
	if ( ( alert.bAlertOnEntry )
			|| ( alert.bAlertOnExit ) )
	{
		if ( alert.bDisableAfterAlert )
		{
			bmpID = BMP_ICON_ALERT_ONCE;	
		}
		else
		{
			bmpID = BMP_ICON_ALERT;
		}
		
		bmpP = GetBitmap( bmpID, &bmpSizeX, &bmpSizeY, &bmpH );
		WinDrawBitmap( bmpP, rectP->topLeft.x + 1, rectP->topLeft.y + 1 );
		ReleaseBitmap( &bmpH );	
		bmpH = NULL;
		bmpID = 0;
		
		if ( alert.bAlertOnEntry )
		{
			bmpID = BMP_ICON_ALERT_ENTRY;
			bmpP = GetBitmap( bmpID, &bmpSizeX, &bmpSizeY, &bmpH );
			WinDrawBitmap( bmpP, rectP->topLeft.x + 1, rectP->topLeft.y + 1 );
			ReleaseBitmap( &bmpH );	
			bmpH = NULL;
			bmpID = 0;
		}
		
		if ( alert.bAlertOnExit )
		{
			bmpID = BMP_ICON_ALERT_EXIT;
			bmpP = GetBitmap( bmpID, &bmpSizeX, &bmpSizeY, &bmpH );
			WinDrawBitmap( bmpP, rectP->topLeft.x + 1, rectP->topLeft.y + 1 );
			ReleaseBitmap( &bmpH );	
			bmpH = NULL;
			bmpID = 0;
		}
	}

	if ( attachedProfile.profileID )
	{
		if ( attachedProfile.profileValidity )
		{
			bmpID = BMP_ICON_PROFILE_TIMED;	
		}
		else
		{
			bmpID = BMP_ICON_PROFILE;
		}
		
		bmpP = GetBitmap( bmpID, &bmpSizeX, &bmpSizeY, &bmpH );
		WinDrawBitmap( bmpP, rectP->topLeft.x + 7, rectP->topLeft.y + 1 );
		ReleaseBitmap( &bmpH );	
		bmpH = NULL;
		bmpID = 0;
		
		defaultProfileID = ProfilesDBGetUniqueID();
		
		if ( defaultProfileID == attachedProfile.profileID )
		{
			bmpID = BMP_ICON_PROFILE_DEFAULT;
			bmpP = GetBitmap( bmpID, &bmpSizeX, &bmpSizeY, &bmpH );
			WinDrawBitmap( bmpP, rectP->topLeft.x + 7, rectP->topLeft.y + 1 );
			ReleaseBitmap( &bmpH );	
			bmpH = NULL;
			bmpID = 0;
		}
	}
	
	if ( StrLen( msg.Address ) )
	{
		bmpID = BMP_ICON_MSG_TABLE;	
		
		bmpP = GetBitmap( bmpID, &bmpSizeX, &bmpSizeY, &bmpH );
		WinDrawBitmap( bmpP, rectP->topLeft.x + 14, rectP->topLeft.y + 1 );
		ReleaseBitmap( &bmpH );	
		bmpH = NULL;
		bmpID = 0;
		
		if ( msg.bDisableAfterAlert )
		{
			bmpID = BMP_ICON_MSG_ONCE_TABLE;
			bmpP = GetBitmap( bmpID, &bmpSizeX, &bmpSizeY, &bmpH );
			WinDrawBitmap( bmpP, rectP->topLeft.x + 14, rectP->topLeft.y + 1 );
			ReleaseBitmap( &bmpH );	
			bmpH = NULL;
		}
		
		if ( msg.bAlertOnEntry )
		{
			bmpID = BMP_ICON_ALERT_ENTRY;
			bmpP = GetBitmap( bmpID, &bmpSizeX, &bmpSizeY, &bmpH );
			WinDrawBitmap( bmpP, rectP->topLeft.x + 14, rectP->topLeft.y + 1 );
			ReleaseBitmap( &bmpH );	
			bmpH = NULL;
			bmpID = 0;
		}
		
		if ( msg.bAlertOnExit )
		{
			bmpID = BMP_ICON_ALERT_EXIT;
			bmpP = GetBitmap( bmpID, &bmpSizeX, &bmpSizeY, &bmpH );
			WinDrawBitmap( bmpP, rectP->topLeft.x + 13, rectP->topLeft.y + 1 );
			ReleaseBitmap( &bmpH );	
			bmpH = NULL;
			bmpID = 0;
		}
	} 	
	
	// GID
	if ( ( locP->RegionInfo.regionID ) && ( StrLen( locP->RegionInfo.name ) ) )
	{
		MemMove( strLoc, locP->RegionInfo.name, STRLEN_GID - 1);	
		strLoc[STRLEN_GID - 1] = chrNull;
	}
	else if ( StrLen( locP->LocationName ) )
	{
	 	MemMove( strLoc, locP->LocationName, STRLEN_GID - 1);	
		strLoc[STRLEN_GID - 1] = chrNull;
	}
	else
	{
		MakeGIDString( & ( locP->GID ), strLoc );
	}
	
	// Time
	MakeTimeDateStr( locP->TimeAtLoc, NULL, strTime, strDate, true );
	ReplaceChar( strTime, 'm', chrNull );
	StripSpaces( strTime );
		
	FntSetFont( stdFont );
	strHeight = FntLineHeight();
	
	strWidth = FntLineWidth( strTime, StrLen( strTime ) );		
	WinDrawChars( strTime, StrLen( strTime ),
						rectP->topLeft.x + 40 + 6 - strWidth,
						rectP->topLeft.y + ( rectP->extent.y - strHeight ) / 2 );

	strWidth = FntLineWidth( strDate, StrLen( strDate ) );
	WinDrawChars( strDate, StrLen( strDate ),
						rectP->topLeft.x + /* 77 */ 67 + 6 - strWidth,
						rectP->topLeft.y + ( rectP->extent.y - strHeight ) / 2 );

	if ( locP->isNewLocation )
	{
		bmpID = BMP_ICON_NEW_ENTRY;
		bmpP = GetBitmap( bmpID, &bmpSizeX, &bmpSizeY, &bmpH );
		WinDrawBitmap( bmpP, rectP->topLeft.x + /* 78 */ 69 + 6, rectP->topLeft.y + 1 );
		ReleaseBitmap( &bmpH );	
		bmpH = NULL;
		bmpID = 0;
	}
	
	if ( locP->isRoaming )
	{
		rgb.r=153; rgb.g=51; rgb.b=0; // Dark brown
	}
	else
	{
		rgb.r=0; rgb.g=51; rgb.b=153; // Dark Blue
	}
	
	StrColor = WinRGBToIndex( &rgb );	
    WinSetTextColor( StrColor );		
	strWidth = FntLineWidth( strLoc, StrLen( strLoc ) );					
	WinDrawTruncChars( strLoc, StrLen( strLoc ), 
						rectP->topLeft.x + /* 82 */ 74 + 6,
						rectP->topLeft.y + ( rectP->extent.y - strHeight ) / 2,
						rectP->extent.x - /* 83 */ 74 - 6 );

	return;
	
} // DrawRow()

/*
 * TableInit
 */
void TableInit( TableType* pTbl )
{ 
	UInt16				row = 0;
	RGBColorType 		rgb;
	
	if ( !pTbl ) return;
	
	gGIDTableP = pTbl;
	gNumTableRows = TblGetNumberOfRows( pTbl );
	gTblColumnWidth = TblGetColumnWidth( pTbl, 0 );

	rgb.r=0;	rgb.g=0;	rgb.b=0;	gTextColorIdx = WinRGBToIndex( &rgb );
	rgb.r=255;	rgb.g=255;	rgb.b=255;	gSep1ColorIdx = WinRGBToIndex( &rgb );
	gSep1ColorRGB = rgb;
	rgb.r=255;	rgb.g=255;	rgb.b=204;	gBackColorIdx = WinRGBToIndex( &rgb );
	gBackColorRGB = rgb;
	
	UIColorSetTableEntry( UIFieldBackground, &gBackColorRGB );	
	
	for ( row = 0; row < gNumTableRows; row++ )
	{		
		TblSetItemStyle( pTbl, row, 0, tallCustomTableItem );
		
		TblSetRowHeight( pTbl, row, TABLE_ROW_HEIGHT );
		TblSetRowUsable( pTbl, row, true );
		TblSetRowMasked( pTbl, row, false );
		TblSetRowSelectable( pTbl, row, true );
	} 
	 
	TblSetColumnUsable( pTbl, 0, true );
	TblSetColumnMasked( pTbl, 0, false );
	TblSetColumnEditIndicator( pTbl, 0, false );
	
	TblSetCustomDrawProcedure( pTbl, 0, TableDrawItem );
	
	TblHasScrollBar( pTbl, false ); 
	// TblMarkTableInvalid( pTbl );
	
	return;
						
} // TableInit

/*
 * Tables.c
 */