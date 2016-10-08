/*
 * GetGID.c
 */

#include "GetGID.h"

/*
 * getLACCID()
 */
Err getLACCID( UInt32* pLAC, UInt32* pCellID )
{
	Err					error = errNone;
	UInt16 				PhoneLibRefNum = sysInvalidRefNum;
	
	if ( !( ( pLAC ) && ( pCellID ) ) )  return ( error = dmErrInvalidParam );
	
	*pLAC = 0;
	*pCellID = 0;
	
	if ( ( error = HsGetPhoneLibrary( &PhoneLibRefNum ) ) )
		return ( error );
	
	if ( ( error = PhnLibOpen( PhoneLibRefNum ) ) )
		return ( error );

	error = PhnLibGetGSMCellInfo( PhoneLibRefNum, pLAC, pCellID );
	
	PhnLibClose(PhoneLibRefNum);
	
 	return ( error );
 
} // getLACCID()

/*
 * getMNCMCC()
 */
Err getMNCMCC( UInt32* pMCC, UInt32* pMNC )
{
	Err					error = errNone;
	UInt16				PhoneLibRefNum = sysInvalidRefNum;
  	Int16				carrierIDBufSize = CARRIER_ID_BUFFER_SIZE;	
	char				carrierID[carrierIDBufSize];	
	char				sMCC[maxStrIToALen] = "\0";
	char				sMNC[maxStrIToALen] = "\0";

	if ( !( ( pMCC ) && ( pMNC ) ) )  return ( error = dmErrInvalidParam );
	
	*pMCC = 0;
	*pMNC = 0;
	
	if ( ( error = HsGetPhoneLibrary( &PhoneLibRefNum ) ) )
		return ( error );

	if ( ( error = PhnLibOpen( PhoneLibRefNum ) ) )
		return ( error );
	
	if ( ( error = PhnLibCurrentOperatorID( PhoneLibRefNum, carrierID, &carrierIDBufSize ) ) == errNone )
	{
		MemMove( sMCC, carrierID, 3 );
		sMCC[3] = chrNull;
		
		MemMove( sMNC, carrierID + 3, 3 );
		sMNC[2] = chrNull;
		
		*pMCC = StrAToI( sMCC );
		*pMNC = StrAToI( sMNC );
		
	}

	PhnLibClose(PhoneLibRefNum);
	
	return ( error );
	
} // getMNCMCC()

/*
 * MakeGIDString()
 */
void MakeGIDString( GID_t* pGID, Char* sGID )
{
	char				sMCC[maxStrIToALen] = "\0";
	char				sMNC[maxStrIToALen] = "\0";
	char				sLAC[maxStrIToALen] = "\0";
	char				sCellID[maxStrIToALen] = "\0";
	
	StrIToA( sMCC, pGID->MCC );
	StrIToA( sMNC, pGID->MNC );
	StrIToH( sLAC, pGID->LAC );
	StrIToH( sCellID, pGID->CellID );

	MemMove( sLAC, sLAC + 4, 5 );
	MemMove( sCellID, sCellID + 4, 5 );
	
	StrCopy( sGID, sMCC );
	StrCat( sGID, ":" );
	StrCat( sGID, sMNC );
	StrCat( sGID, ":" );
	StrCat( sGID, sLAC );
	StrCat( sGID, ":" );
	StrCat( sGID, sCellID );
		
	return;
	
} // MakeGIDString()

/*
 * GetGID.c
 */