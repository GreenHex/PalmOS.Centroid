/*
 * Themes.c
 */

#include "Themes.h"

/*
 * GetThemeInfoFromIdx()
 */
Err GetThemeInfoFromIdx( UInt16 idx, themes_t* pTheme )
{
	Err						error = errNone;
	DmOpenRef				dbP;
	DmSearchStateType 		stateInfo; 
	Boolean 				latestVer = false; 
	UInt16 					cardNo; 
	LocalID 				localID = 0;
	MemHandle				recordH = NULL;
	Char* 					recordP = NULL;
	UInt16					numItems = 0;
	Char					nameStr[STR_LEN_THEME_STR] = "\0";
	Char*					separatorLocation = NULL;
	
	if ( idx == noListSelection ) return ( dmErrResourceNotFound );
	
	error = DmGetNextDatabaseByTypeCreator( true, &stateInfo, 0, 'colP', latestVer, &cardNo, &localID );
	
	if ( error ) return ( error );
	
	dbP = DmOpenDatabase( cardNo, localID, dmModeReadOnly );

	if ( dbP )
	{
		recordH = DmGetResource( 'tSTL', 1100 );

		if ( recordH )
		{		
			recordP = (Char *) MemHandleLock( recordH );	

			if ( recordP )
			{
				MemMove( &numItems, recordP + StrLen( recordP ) + 1, sizeof( UInt16 ) ); // !!!
				
				if ( numItems > idx )
				{
					SysStringByIndex( 1100, idx, nameStr, STR_LEN_THEME_STR );
				
					if ( ( separatorLocation = StrStr( nameStr, "|" ) ) )
					{
						if ( pTheme )
						{
							pTheme->themeID = StrAToI( separatorLocation + 1 );
							
							*separatorLocation = chrNull;
						
							StrNCopy( pTheme->name, nameStr, STRLEN_THEME_NAME - 1 );
							pTheme->name[STRLEN_THEME_NAME - 1] = chrNull;	
						}
					}		
				}
			}
		
			MemHandleUnlock( recordH );
			DmReleaseResource( recordH );
		}
		
		DmCloseDatabase( dbP );
	}
	
	return ( error );
	
} // GetThemeInfoFromIdx() 

/*
 * FindThemeIdx()
 */
UInt16 FindThemeIdx( Char* str )
{
	UInt16					retVal = noListSelection;
	Err						error = errNone;
	DmOpenRef				dbP;
	DmSearchStateType 		stateInfo; 
	Boolean 				latestVer = false; 
	UInt16 					cardNo; 
	LocalID 				localID = 0;
	MemHandle				recordH = NULL;
	Char* 					recordP = NULL;
	UInt16					numItems = 0;
	UInt16					i = 0;
	Char					nameStr[STR_LEN_THEME_STR] = "\0";
	
	if ( !StrLen( str ) ) return ( retVal );
	
	error = DmGetNextDatabaseByTypeCreator( true, &stateInfo, 0, 'colP', latestVer, &cardNo, &localID );
	
	if ( error ) return ( retVal );
	
	dbP = DmOpenDatabase( cardNo, localID, dmModeReadOnly );

	if ( dbP )
	{
		recordH = DmGetResource( 'tSTL', 1100 );

		if ( recordH )
		{		
			recordP = (Char *) MemHandleLock( recordH );	

			if ( recordP )
			{
				MemMove( &numItems, recordP + StrLen( recordP ) + 1, sizeof( UInt16 ) ); // !!!
				
				if ( numItems )
				{
					for ( i = 0 ; i < numItems ; i++ )
					{	
						SysStringByIndex( 1100, i, nameStr, STR_LEN_THEME_STR );
						
						if ( StrStr( nameStr, "|" ) )
						{
							*StrStr(nameStr, "|") = chrNull;
						}
						
						if ( StrCompare( nameStr, str ) == 0 )
						{
							retVal = i;
							break;
						}
					}
				}
			}
		
			MemHandleUnlock( recordH );
			DmReleaseResource( recordH );
		}
		
		DmCloseDatabase( dbP );
	}
	
	return ( retVal );
	
} // FindThemeIdx()

/*
 * LoadThemesListFromResource
 */
extern Err LoadThemesListFromResource( ListType* pList )
{
	Err						error = errNone;
	DmOpenRef				dbP;
	DmSearchStateType 		stateInfo; 
	Boolean 				latestVer = false; 
	UInt16 					cardNo; 
	LocalID 				localID = 0;
	MemHandle				recordH = NULL;
	Char* 					recordP = NULL;
	UInt16					numItems = 0;
	UInt16					i = 0;
	Char					nameStr[STR_LEN_THEME_STR] = "\0";
	UInt16					nameStrSize = 0;
	UInt16					nameListSize = 0;
	Char**					listOptions;
	
	if ( gThemesH )
	{
        MemHandleUnlock( gThemesH );
        MemHandleFree( gThemesH );
        gThemesH = NULL;
    }
  
	error = DmGetNextDatabaseByTypeCreator( true, &stateInfo, 0, 'colP', latestVer, &cardNo, &localID );
	
	if ( error ) return ( error );
	
	dbP = DmOpenDatabase( cardNo, localID, dmModeReadOnly );

	if ( dbP )
	{
		recordH = DmGetResource( 'tSTL', 1100 );

		if ( recordH )
		{		
			recordP = (Char *) MemHandleLock( recordH );	

			if ( recordP )
			{
				MemMove( &numItems, recordP + StrLen( recordP ) + 1, sizeof( UInt16 ) ); // !!!
				
				if ( !numItems )
				{
					DmCloseDatabase( dbP );
					LstSetListChoices( pList, NULL, numItems );

					error = dmErrResourceNotFound;
					
					return ( error );
				}
				
				for ( i = 0 ; i < numItems ; i++ )
				{	
					SysStringByIndex( 1100, i, nameStr, STR_LEN_THEME_STR);
					
					if ( StrStr( nameStr, "|" ) )
					{
						*StrStr(nameStr, "|") = chrNull;
					}
					
					nameStrSize = StrLen( nameStr ) + 1;
					MemMove( gThemesList + nameListSize, nameStr, nameStrSize );
					nameListSize = nameListSize + nameStrSize;
				}
				
				// gThemesH = SysFormPointerArrayToStrings( recordP + StrLen( recordP ) + 1 + sizeof( UInt16 ), numItems );
				gThemesH = SysFormPointerArrayToStrings( gThemesList, numItems );		
				listOptions = MemHandleLock( gThemesH ); // to be unlocked somewhere...
				LstSetListChoices( pList, listOptions, numItems ); 		
			}
		
			MemHandleUnlock( recordH );
		
			DmReleaseResource( recordH );
		}
		
		DmCloseDatabase( dbP );
	}
	
	return ( error );

} // LoadThemesListFromResource()

/*
 * Themes.c
 */ 