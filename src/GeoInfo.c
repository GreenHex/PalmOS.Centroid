/*
 * GeoInfo.c
 */

#include "GeoInfo.h"

// Prototypes
static Boolean 				GeoInfoFormHandleMenu( FormType* pForm, UInt16 menuID ) EXTRA_SECTION_ONE;
static void 				InitializeGeoInfoForm( FormType* pForm, UInt16* idxP, locations_t* pLoc ) EXTRA_SECTION_ONE;
static void 				CopyAndSaveGeoInfo( FormType* pForm, UInt16* idxP, locations_t* pLoc ) EXTRA_SECTION_ONE;
static void 				GeoInfoCopyAllFields( FormType* pForm, geoInfo_t* pGeoInfo ) EXTRA_SECTION_ONE;
static void 				GeoInfoPasteAllFields( FormType* pForm, geoInfo_t* pGeoInfo ) EXTRA_SECTION_ONE;
static void 				GeoInfoClearAllFields( FormType* pForm ) EXTRA_SECTION_ONE;

/*
 * GeoInfoFormHandleMenu()
 */
static Boolean GeoInfoFormHandleMenu( FormType* pForm, UInt16 menuID )
{
	Boolean 				handled = false;
	static geoInfo_t		geoInfo;
	
	switch ( menuID )
	{
		case MENU_GEOINFO_COPY_ALL:

			GeoInfoCopyAllFields( pForm, &geoInfo );
			
			FrmDrawForm( pForm );
			
			handled = true;
		
			break;
			
		case MENU_GEOINFO_PASTE_ALL:
		
			GeoInfoPasteAllFields( pForm, &geoInfo );
			
			FrmDrawForm( pForm );
		
			handled = true;
			
			break;

		case MENU_GEOINFO_CLEAR_ALL:
			
			GeoInfoClearAllFields( pForm );
			
			FrmDrawForm( pForm );
						
			handled = true;
			
			break;

		default:
			break;
	}

	return ( handled );

} // GeoInfoFormHandleMenu()

/*
 * GeoInfoFormHandleEvent()
 */
Boolean GeoInfoFormHandleEvent( EventType* pEvent )
{
	Boolean					handled = false;
	FormType* 				pForm = FrmGetActiveForm();
	Boolean					bFound = false;
	static UInt16			idx = 0;
	static locations_t		loc;
	static double			latitude = 0;
	static double			longitude = 0;
	static UInt32			altitude = 0;
	
	switch ( pEvent->eType )
	{		
		case frmOpenEvent:
				
			idx = gLocFormOpenIdx; 
			
			loc.GID = gGID;
			
			if ( GIDFindRecord( &loc, &idx, true, &bFound, (DmComparF *) &GID_CompareF ) == errNone )
			{	
				if ( bFound )
				{	
					InitializeGeoInfoForm( pForm, &idx, &loc ); // loc loaded here!!!
				
					latitude = loc.GeoInfo.latitude;
					longitude = loc.GeoInfo.longitude;
					altitude = loc.GeoInfo.altitude;
				}
			}
			
			FrmDrawForm( pForm );
			
			handled = true;
						
			break;
			
		case ctlSelectEvent:
		
			switch ( pEvent->data.ctlSelect.controlID )
			{					
				case GEO_INFO_OK_BTN:
					
					if ( GIDFindRecord( &loc, &idx, true, &bFound, (DmComparF *) &GID_CompareF ) == errNone )
					{	
						if ( bFound )
						{	
							loc.GeoInfo.latitude = latitude;
							loc.GeoInfo.longitude = longitude;
							loc.GeoInfo.altitude = altitude;
							
						 	CopyAndSaveGeoInfo( pForm, &idx, &loc );
						}
					}
					
					if ( !bFound )
					{
						FrmCustomAlert( ErrOKAlert, "Something wrong... Could not save record.", "", "" );
					}
				
					FrmReturnToForm( 0 );
					
					handled = true;

					break;

				case GEO_INFO_CANCEL_BTN:
					
					FrmReturnToForm( 0 );	
				
					handled = true;
					
					break;
					
				case GEO_INFO_CLEAR_BTN:
				
					GeoInfoClearAllFields( pForm );
					
					FrmDrawForm( pForm );
					
					handled = true;
					
					break;
										
				default:
					break;
			}
		
			break;
			
		case menuEvent:
										
			handled = GeoInfoFormHandleMenu( pForm, pEvent->data.menu.itemID );

			break;
			
		default:
								
			break;
	}
		
	return ( handled );	
	
} // GeoInfoFormHandleEvent()

/*
 * InitializeGeoInfoForm()
 */
static void InitializeGeoInfoForm( FormType* pForm, UInt16* idxP, locations_t* pLoc )
{
	if ( ( !pForm ) || ( !pLoc ) ) return;

	if ( GIDGetRecordAtIdx( idxP, pLoc ) == errNone )
	{
		GeoInfoPasteAllFields( pForm, &( pLoc->GeoInfo ) );
	}
						
	FrmSetMenu( pForm, MENU_GEOINFO );
	
	return;
	
} // InitializeGeoInfoForm()

/*
 * CopyAndSaveGeoInfo()
 */
static void CopyAndSaveGeoInfo( FormType* pForm, UInt16* idxP, locations_t* pLoc )
{	
	if ( ( !pForm ) || ( !pLoc ) ) return;
	
	GeoInfoCopyAllFields( pForm, &( pLoc->GeoInfo ) );
		
	GIDReplaceRecordAtIdx( idxP, pLoc, false );
	
	return;
		
} // CopyAndSaveGeoInfo()
	
/*
 * GeoInfoCopyAllFields()
 */
static void GeoInfoCopyAllFields( FormType* pForm, geoInfo_t* pGeoInfo )
{	
	Char			strDbl[maxStrIToALen * 5] = "\0";
	Char			strAltitude[maxStrIToALen] = "\0";
	
	if ( ( !pForm ) || ( !pGeoInfo ) ) return;
	
	GetFieldStr( pForm, GEO_INFO_LAT_FLD, strDbl, maxStrIToALen * 5 );
	pGeoInfo->latitude = StringToDouble( strDbl );

	GetFieldStr( pForm, GEO_INFO_LON_FLD, strDbl, maxStrIToALen * 5 );
	pGeoInfo->longitude = StringToDouble( strDbl );

	GetFieldStr( pForm, GEO_INFO_ALT_FLD, strAltitude, maxStrIToALen );
	pGeoInfo->altitude = StrAToI( strAltitude );

	GetFieldStr( pForm, GEO_INFO_COUNTRY_FLD, pGeoInfo->Country, STRLEN_COUNTRY_CODE );
	GetFieldStr( pForm, GEO_INFO_REGION_FLD, pGeoInfo->Region, STRLEN_REGION );
	GetFieldStr( pForm, GEO_INFO_CITY_FLD, pGeoInfo->City, STRLEN_CITY );
	GetFieldStr( pForm, GEO_INFO_ZIP_FLD, pGeoInfo->ZIP, STRLEN_ZIP );
	GetFieldStr( pForm, GEO_INFO_STREET_FLD, pGeoInfo->Street, STRLEN_STREET );
	
	return;
		
} // GeoInfoCopyAllFields()


/*
 * GeoInfoPasteAllFields()
 */
static void GeoInfoPasteAllFields( FormType* pForm, geoInfo_t* pGeoInfo )
{
	Char			strDbl[maxStrIToALen * 5] = "\0";
	Char			strAltitude[maxStrIToALen] = "\0";

	if ( ( !pForm ) || ( !pGeoInfo ) ) return;
	
	DoubleToString( pGeoInfo->latitude, strDbl );
	SetFieldStr( pForm, GEO_INFO_LAT_FLD, strDbl );

	DoubleToString( pGeoInfo->longitude, strDbl );
	SetFieldStr( pForm, GEO_INFO_LON_FLD, strDbl );
	
	StrIToA ( strAltitude, pGeoInfo->altitude );
	SetFieldStr( pForm, GEO_INFO_ALT_FLD, strAltitude );

	SetFieldStr( pForm, GEO_INFO_COUNTRY_FLD, pGeoInfo->Country );
	SetFieldStr( pForm, GEO_INFO_REGION_FLD, pGeoInfo->Region );
	SetFieldStr( pForm, GEO_INFO_CITY_FLD, pGeoInfo->City );
	SetFieldStr( pForm, GEO_INFO_ZIP_FLD, pGeoInfo->ZIP );
	SetFieldStr( pForm, GEO_INFO_STREET_FLD, pGeoInfo->Street );
	
	return;
	
} // GeoInfoPasteAllFields

/* 
 * GeoInfoClearAllFields()
 */ 
static void GeoInfoClearAllFields( FormType* pForm )
{
	if ( !pForm ) return;
	
	ClearFieldStr( pForm, GEO_INFO_LAT_FLD );
	ClearFieldStr( pForm, GEO_INFO_LON_FLD );
	ClearFieldStr( pForm, GEO_INFO_ALT_FLD );
	ClearFieldStr( pForm, GEO_INFO_COUNTRY_FLD );
	ClearFieldStr( pForm, GEO_INFO_REGION_FLD );
	ClearFieldStr( pForm, GEO_INFO_CITY_FLD );
	ClearFieldStr( pForm, GEO_INFO_ZIP_FLD );	
	ClearFieldStr( pForm, GEO_INFO_STREET_FLD );

	return;

} // GeoInfoClearAllFields()

/*
 * GeoInfo.c
 */