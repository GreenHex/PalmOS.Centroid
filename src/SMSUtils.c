/*
 * SMSUtils.c
 */

#include "SMSUtils.h"

// Prototypes
static Err 			SendMessage( msg_t* msgP, ProgressPtr pProgress, Char* strError );
static void 		ReplaceLocTypeTokens( attentionMode_e attnMode, Char* locName, Char* str, UInt16 strLen );

/*
 * MsgNotify()
 */
void MsgNotify( lastLocation_t* pLastLoc, attentionMode_e attnMode )
{
	SysNotifyParamType* 		msgNotifyParamP = NULL;
	msgNotify_t					msgNotifyDetails;
		
	msgNotifyParamP = MemPtrNew( sizeof( SysNotifyParamType ) );
				
	if ( msgNotifyParamP )
	{
		MemSet( msgNotifyParamP, sizeof( SysNotifyParamType ), 0 );
	
		msgNotifyDetails.loc = *pLastLoc;
		msgNotifyDetails.attnMode = attnMode;
			
		msgNotifyParamP->notifyType = msgDeferredNotifyEvent;
		msgNotifyParamP->broadcaster = appFileCreator;
		msgNotifyParamP->notifyDetailsP = &msgNotifyDetails;
		msgNotifyParamP->handled = false;
			
		SysNotifyBroadcastDeferred( msgNotifyParamP, sizeof( msgNotify_t ) );
		
		MemPtrFree( msgNotifyParamP );
	}
	
	return;
	
} // MsgNotify()

/*
 * HandleMsgNotification()
 */
Err HandleMsgNotification( SysNotifyParamType* notifyParamP )
{
	Err					error = errNone;
	msgNotify_t*		msgNotifyDetailsP = (msgNotify_t *) notifyParamP->notifyDetailsP;
	
	if ( notifyParamP->notifyType != msgDeferredNotifyEvent ) return ( error );
	
	DoSMS( &(msgNotifyDetailsP->loc), msgNotifyDetailsP->attnMode );
	
	return ( error );
	
} // HandleMsgNotification()

/*
 * DoSMS()
 */
void DoSMS( lastLocation_t* pLoc, attentionMode_e attnMode )
{
	locations_t			loc;
	regions_t			region;
	msg_t*				pMsg = NULL;
	UInt16				idx = 0;
	Boolean				bFound = false;
	Char				locName[STRLEN_LOCATION_NAME] = "\0";
	
	if ( pLoc->regionID )
	{
		region.uniqueID = pLoc->regionID;
		
		if ( !RegionsFindRecord( &region, &idx, true, &bFound, (DmComparF *) &Regions_CompareUniqueIDF ) 
				&& ( bFound ) )
		{
			pMsg = &(region.Msg);
			StrCopy( locName, region.name );
		}
	}
	else
	{
		loc.uniqueID = pLoc->uniqueID;
		
		if ( ( !GIDFindRecord( &loc, &idx, true, &bFound, (DmComparF *) &GID_CompareUniqueIDF ) )
				&& ( bFound ) )
		{
			pMsg = &(loc.Msg);
			
			if ( !StrLen( loc.LocationName ) )
			{
				MakeGIDString( &(loc.GID), locName );
			}
			else
			{
				StrCopy( locName, loc.LocationName );
			}
		}
	}
	
	if ( ( !bFound ) || ( !pMsg ) ) return;
	
	if ( !( ( ( pMsg->bAlertOnEntry ) && ( attnMode == onEntry ) )
			|| ( ( pMsg->bAlertOnExit ) && ( attnMode == onExit ) ) ) )
		return;
	
	if ( MakeMsgAndSend( *pMsg, attnMode, locName ) != errNone ) return;

	if ( attnMode == onEntry )
	{
		pMsg->bAlertOnEntry = ( !( pMsg->bDisableAfterAlert ) );
	}
	else
	{
		pMsg->bAlertOnExit = ( !( pMsg->bDisableAfterAlert ) );	
	}
	
	if ( pLoc->regionID )
	{
		RegionsReplaceRecordAtIdx( &idx, &region, false );
	}
	else
	{
		GIDReplaceRecordAtIdx( &idx, &loc, false );
	}
			
	return;
	
} // DoSMS()


/*
 * MakeMsgAndSend()
 */
Err MakeMsgAndSend( msg_t msg, attentionMode_e attnMode, Char* locName )
{
	Err							error = errNone;
	ProgressPtr					pProgress = NULL;
	appPrefs_t					prefs;
	UInt16						numAttempts = 1; // idx starts from zero
	UInt16						signalQuality = 0;
	Char						strError[STRLEN_ERROR] = "\0";

	if ( !StrLen( msg.Address ) ) return ( dmErrInvalidParam );
	
	ReadAppPrefs( &prefs );
	
	if ( !StrLen( msg.Message ) )
	{	
		if ( StrLen( prefs.DefaultMsg ) )
		{
			StrCopy( msg.Message, prefs.DefaultMsg );	
		}
		else
		{
			StrCopy( msg.Message, STR_DEFAULT_MSG );
		}	
	}
	
	ReplaceDateTimeTokens( msg.Message, STRLEN_MSG );		
	ReplaceLocTypeTokens( attnMode, locName, msg.Message, STRLEN_MSG );
	
	StrCopy( msg.CallerID, msg.Address );

	MakeNum( msg.Address );
	
	numAttempts += prefs.numMsgAttemptsIdx;
	
	pProgress = PrgStartDialog( APP_NAME, MsgProgressCallback, NULL );
	
	if ( pProgress )
	{
		PrgUpdateDialog( pProgress, 0, 0, msg.CallerID, true );
	}
	
	while ( numAttempts )
	{	
		--numAttempts;
		
		if ( !GetSignalInfo( &signalQuality, NULL ) )
		{
			if ( ( signalQuality > 0 ) 
					&& ( signalQuality < 32 ) )
			{
				SndPlaySystemSound( sndClick );
				
				if ( ( error = SendMessage( &msg, pProgress, strError ) ) == errNone )
				{
					break;
				}
			}
		}
			
		SysTaskDelay( (Int32) SysTicksPerSecond() );
	}

	if ( prefs.bEnableLogging )
	{
		MsgSaveLog( error, strError, (UInt16) prefs.numMsgAttemptsIdx - numAttempts, attnMode, &msg );
	}
			
	if ( pProgress ) 
	{
		PrgHandleEvent(pProgress, NULL);
		PrgStopDialog( pProgress, true );
		pProgress = NULL;										
	}
			
	return ( error );
	
} // MakeMsgAndSend()

/*
 * SendMessage
 */
static Err SendMessage( msg_t* pMsg, ProgressPtr pProgress, Char* strErrorP )
{
	Err						error = phnErrUnknownError;
	Char					strError[STRLEN_ERROR] = ERR_STR_SMS_ERR;
	UInt32 					msgID = 0;
	PhnAddressList 			addList;
  	PhnAddressHandle 		hAddress;
  	// DmOpenRef 			smsRefNum = 0;      // CDMA???
  	UInt16					PhoneLibRefNum = sysInvalidRefNum;
  	UInt16					ledOldState = kIndicatorStateNull;
	UInt16					ledOldCount = kIndicatorCountGetState;
	UInt16					ledNewState;
	UInt16					ledNewCount = kIndicatorCountForever;
  	
	if ( ( error = HsGetPhoneLibrary( &PhoneLibRefNum ) ) != errNone )
		return ( error );

	if ( ( error = PhnLibOpen( PhoneLibRefNum ) ) != errNone )
		return ( error );
/*		
	if ( PhnLibModulePowered( PhoneLibRefNum ) != phnPowerOn )
	{
		StrCopy( strError, ERR_STR_PHN_OFF );
	  	goto CloseAndRelease;
	}
	
	if ( !PhnLibRegistered( PhoneLibRefNum ) )
	{
		StrCopy( strError, ERR_STR_NETWORK_ERR );
	  	goto CloseAndRelease;
	}
*/	
	// smsRefNum = PhnLibGetDBRef( PhoneLibRefNum );    // CDMA???

	msgID = PhnLibNewMessage( PhoneLibRefNum, kMTOutgoing );
  	if ( !msgID )
  	{
  		StrCopy( strError, ERR_STR_SMS_CREATE_ERR );
		goto SendMessage_CloseAndRelease;
  	}
  	
  	error = PhnLibSetOwner( PhoneLibRefNum, msgID, appFileCreator );
	error |= PhnLibSetDate( PhoneLibRefNum, msgID, TimGetSeconds() );
  	error |= PhnLibSetText( PhoneLibRefNum, msgID, pMsg->Message, (short) StrLen( pMsg->Message ) );
  	
  	// fill in the address
  	addList = PhnLibNewAddressList( PhoneLibRefNum );
  	if ( !addList )
  	{
  		StrCopy( strError, ERR_STR_ADDRESS_ERR );
		goto SendMessage_CloseAndRelease;
  	}
  		
  	hAddress = PhnLibNewAddress( PhoneLibRefNum, pMsg->Address, phnLibUnknownID );
  	if ( !hAddress )
	{
		StrCopy( strError, ERR_STR_ADDRESS_ERR );
		goto AddressErr_FreeAndRelease;
	}
			
  	if ( ( error = PhnLibAddAddress( PhoneLibRefNum, addList, hAddress ) ) )
  	{
  		StrCopy( strError, ERR_STR_ADDRESS_ERR );
  		goto AddressErr_FreeAndRelease;
  	}
  	
  	MemHandleFree( hAddress );
  	
  	if ( ( error = PhnLibSetAddresses( PhoneLibRefNum, msgID, addList ) ) )
  	{
  		StrCopy( strError, ERR_STR_ADDRESS_ERR );
  		goto AddressErr_FreeAndRelease;
  	}	
	
	if ( pProgress )
	{
		PrgUpdateDialog( pProgress, 0, 1, pMsg->CallerID, true );
	}

	LEDState( false, &ledOldCount, &ledOldState );
	ledNewState = ledOldState & 0xFF00;
	LEDState( true, &ledNewCount, &ledNewState );
	FlashLED( true, 3 );
	
  	if ( ( error = PhnLibSendMessage( PhoneLibRefNum, msgID, true ) ) )
  	{
		StrCopy( strError, ERR_STR_SMS_ERR );
	}
	else
	{
		StrCopy( strError, ERR_STR_MESSAGE_SENT );
	}
  	
  	FlashLED( false, 3 );
	ledOldCount = kIndicatorCountForever;
	LEDState( false, &ledOldCount, &ledOldState );
	
AddressErr_FreeAndRelease:
	PhnLibDisposeAddressList( PhoneLibRefNum, addList );
	
SendMessage_CloseAndRelease:
	// PhnLibReleaseDBRef( PhoneLibRefNum, smsRefNum ); // CDMA???

// CloseAndRelease:
	
	PhnLibClose( PhoneLibRefNum );
	
	if ( pProgress )
	{
		if ( error == errNone )
		{
			PrgUpdateDialog( pProgress, 0, 2, pMsg->CallerID, true ); // it's done			
		}
		else
		{
			PrgUpdateDialog( pProgress, error, 3, strError, true ); // some error
		}
	}
	
	if ( strErrorP )
	{
		StrCopy( strErrorP, strError );	
	}
				
	return ( error );
	
} // SendMessage

/*
 * ReplaceLocTypeTokens()
 */
static void ReplaceLocTypeTokens( attentionMode_e attnMode, Char* locName, Char* str, UInt16 strLen )
{
	TxtReplaceStr( str, strLen, ( attnMode == onEntry ) ? "Enter" : "Exit", 2 ); // ^2
	
	if ( locName )
	{
		TxtReplaceStr( str, strLen, locName, 3 ); // ^3
	}
	else
	{
		TxtReplaceStr( str, strLen, " ", 3 );
	}
	
	return;
	
} // ReplaceLocTypeTokens()


/*
 * SMSUtils.c
 */
