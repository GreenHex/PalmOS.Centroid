/*
 * DispProgress.c
 */
 
#include "DispProgress.h"

/*
 * ProfilesProgressCallback()
 */
Boolean ProfilesProgressCallback( PrgCallbackDataPtr pCallbackData )
{	
	pCallbackData->bitmapId = BMP_ICON_PROGRESS;
	pCallbackData->canceled = true;
		
	switch ( pCallbackData->stage )
	{
		case 0:
			
			StrCopy( pCallbackData->textP, "Profile: " );
			StrCat( pCallbackData->textP, (Char *)( pCallbackData->userDataP ) );
			StrCat( pCallbackData->textP, "\n\nComplete." );
			pCallbackData->delay = true;
			
			break;
			
		case 1:

			StrCopy( pCallbackData->textP, "Profile: " );
			StrCat( pCallbackData->textP, (Char *)( pCallbackData->userDataP ) );
			StrCat( pCallbackData->textP, "\n\n" );
			StrCat( pCallbackData->textP, pCallbackData->message );
			pCallbackData->delay = true;
					
			break;
			
		default:

			StrCopy( pCallbackData->textP, "Profile: " );
			StrCat( pCallbackData->textP, (Char *)( pCallbackData->userDataP ) );			
			StrCat( pCallbackData->textP, "]\n\nPlease wait..." );
			pCallbackData->delay = true;

			break;
				
	}

	pCallbackData->textChanged = true;

	return ( true ); 

} // ProfilesProgressCallback()

/*
 * RebuildProgressCallback()
 */
Boolean RebuildProgressCallback( PrgCallbackDataPtr pCallbackData )
{	
	Char		str[maxStrIToALen] = "\0";
	
	pCallbackData->bitmapId = BMP_ICON_PROGRESS;
	pCallbackData->canceled = true;
	pCallbackData->textChanged = false;
		
	switch ( pCallbackData->stage )
	{
		case -1:
			
			StrCopy( pCallbackData->textP, "Rebuild Complete.\n\n" );
			StrCat( pCallbackData->textP, pCallbackData->userDataP );
			StrCat( pCallbackData->textP, " of " );
			StrCat( pCallbackData->textP, pCallbackData->userDataP );
			pCallbackData->delay = true;
			pCallbackData->textChanged = true;
			
			break;
						
		default:
		
			StrIToA( str, (Int32) ( pCallbackData->stage + 1 ) );
			StrCopy( pCallbackData->textP, "Rebuilding database...\n\n" );
			StrCat( pCallbackData->textP, str );
			StrCat( pCallbackData->textP, " of " );
			StrCat( pCallbackData->textP, pCallbackData->userDataP );			
			pCallbackData->delay = false;
			pCallbackData->textChanged = true;
				
			break;
	}

	return ( true ); 

} // RebuildProgressCallback()

/*
 * MsgProgressCallback()
 */
Boolean MsgProgressCallback( PrgCallbackDataPtr pCallbackData )
{	
	pCallbackData->bitmapId = BMP_MSG_PROGRESS;
	pCallbackData->canceled = true;
	pCallbackData->delay = true;
			
	switch ( pCallbackData->stage )
	{
		case 0:
		
			StrCopy( pCallbackData->textP, pCallbackData->message );
			StrCat( pCallbackData->textP, "\n\nConfiguring SMS..." );
			
			break;
				
		case 1:

			StrCopy( pCallbackData->textP, pCallbackData->message );			
			StrCat( pCallbackData->textP, "\n\nSending message..." );
			pCallbackData->delay = false;
							
			break;

		case 2:

			StrCopy( pCallbackData->textP, pCallbackData->message );			
			StrCat( pCallbackData->textP, "\n\nMessage sent." );
			
			break;
			
		case 3:

			StrCopy( pCallbackData->textP, pCallbackData->message );		
			StrCat( pCallbackData->textP, "\n" );
			if ( pCallbackData->error )
			{
				char 		errStr[maxStrIToALen];
				
				StrPrintF(errStr, "[0x%x]", pCallbackData->error );	
				StrCat( pCallbackData->textP, errStr );
			}
			StrCat( pCallbackData->textP, "\nMessage not sent." );
			
			pCallbackData->error = 0; // dismiss the pop-up automatically
			
			break;
			
		default:
			break;
	}

	pCallbackData->textChanged = true;

	return ( true ); 

} // MsgProgressCallback()

/*
 * DispProgress.c
 */