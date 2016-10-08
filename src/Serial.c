/*
 * Serial.c
 */
 
#include "Serial.h"

/*
 * getSerialLACCID
 */
Err getSerialLACCID( UInt32* pLAC, UInt32* pCellID ) 
{
	Err				error = errNone;
	UInt16 			SerialPortID;
	Char			buf[100];
	UInt16			numBytesReceived = 0;
	Char			sLAC[maxStrIToALen] = "\0";
	Char			sCellID[maxStrIToALen] = "\0";
	
/*	
#define hsFileCVirtualModemSerLib		'FakM'	  // Yes  : Virtual Modem
#define hsFileCVirtualGSMSerLib			'VGSM'	  // Yes  : Virtual GSM
#define hsFileCDirectRadioSerLib		'DirR'	  // Yes  : Direct Radio access SdrvDirectRadio
*/
	if ( pLAC ) *pLAC = 0;
	if ( pCellID ) *pCellID = 0;
	
	error = SrmOpen( hsFileCVirtualModemSerLib, 115200, &SerialPortID );
	
	if ( ( !error ) && ( SerialPortID ) )
	{
		Boolean		streamEndFlag	= false;
		UInt32		timeoutTime = TimGetSeconds() + AT_CMD_RESPONSE_TIMEOUT_SECONDS; // not more than 5 seconds...
		Char*		chrLoc = NULL; // location of search string in modem response
		
		SrmSend( SerialPortID, AT_CMD_QUERY_STRING, 10, &error );
		
		if ( error )
			goto CLOSE_SERIAL;
				
		error = SrmSendWait( SerialPortID ); // wait until all data is transmitted... (blocking)
		if ( error )
			goto CLOSE_SERIAL;
		
		do {
			
			UInt32		numBytesInBuffer = 0; // re-initialize every time.
					
			error = SrmReceiveCheck( SerialPortID, &numBytesInBuffer );
			
			if ( error ) // can't recover here, yet...
				goto CLOSE_SERIAL;
				
			if ( numBytesInBuffer > 0 )
			{
				UInt16	i = 0;
				
				for ( i = 0 ; i < numBytesInBuffer ; i++ )
				{
					SrmReceive( SerialPortID, (Char *) buf + numBytesReceived, 1, 0, &error ); // get one byte...

					++numBytesReceived; // ready for next char
				}
				
				if ( StrStr( buf, AT_CMD_RESPONSE_SEARCH_STRING ) ) // found what we require
					streamEndFlag = true;
			}
				
		} while ( ( !error ) 
						&& ( !streamEndFlag ) && ( timeoutTime > TimGetSeconds() ) 
						&& ( numBytesReceived < 100 ) ); // artificial exit... have to fix it.
		
CLOSE_SERIAL:		
		if (error == serErrLineErr)
		{
			SrmClearErr( SerialPortID );
		}
		
		error = SrmClose( SerialPortID );

		if ( !error )
		{			
			chrLoc = StrStr( buf, AT_CMD_RESPONSE_SEARCH_STRING );
	
			if ( chrLoc )
			{	
				MemMove( sLAC, chrLoc + StrLen( AT_CMD_RESPONSE_SEARCH_STRING ), 4 ); // 35
				MemMove( sCellID, chrLoc + StrLen( AT_CMD_RESPONSE_SEARCH_STRING ) + 7, 4 );
				sLAC[4] = chrNull;
				sCellID[4] = chrNull;
				
				if ( pLAC ) *pLAC = (UInt32) StrHexToInt( sLAC );
				if ( pCellID ) *pCellID = (UInt32) StrHexToInt( sCellID );
			}
		}
	}
	
	return ( error );
	
} // getSerialLACCID
 
/*
 * netClose
 */
Err netClose()
{
	Err										error = errNone;
	UInt16									NetMasterRefNum = 0;
	Boolean									isLibLoaded = false;
	struct NetPrefContextTypeTag* 			cxtP = NULL;
	Int16									maxNumID = 5;
	NetMasterDataSessionIDType 				sessionIDs[maxNumID];
	Int16									numID = 0;
	Boolean									isActive = false;
					
	error = SysLibFind( netMasterLibName, &NetMasterRefNum );
	
	if ( error )
	{
		error = SysLibLoad( netPrefLibTypeID, netPrefLibCreatorID, &NetMasterRefNum );		
		isLibLoaded = ( !error );
	}
	
	if ( !error )
	{
		error = NetPrefLibOpen( NetMasterRefNum, &cxtP );
	}
	
	if ( ( !error ) && ( NetMasterRefNum ) && ( cxtP ) )
	{
		error = NetMasterSCSessionsEnumerate( NetMasterRefNum, sessionIDs, maxNumID, &numID );
		
		if ( ( !error ) && ( numID ) )
		{
			error = NetMasterSCSessionIsActive( NetMasterRefNum, sessionIDs[0], &isActive ); // only sessionIDs[0] is returned by PalmOS
		
			if ( ( !error ) && ( isActive ) )
			{
				error = NetMasterSCSessionShutDown( NetMasterRefNum, sessionIDs[0],
											netMasterNetIFShutDownOptDismissPrgDialog | netMasterNetIFShutDownOptResetNetGuard );					
			}
		}
		
		NetPrefLibClose( NetMasterRefNum, cxtP );
	}
	
	if ( isLibLoaded ) SysLibRemove( NetMasterRefNum );
	
	cxtP = NULL;
	NetMasterRefNum = 0;
	
	return ( error );	

} // netClose

/*
 * Serial.c
 */