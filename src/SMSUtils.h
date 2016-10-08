/*
 * SMSUtils.h
 */
#ifndef __SMSUTILS_H__
#define __SMSUTILS_H__

#include <Hs.h>
#include <HsPhone.h>
#include <Progress.h>
#include <SoundMgr.h>

#include "Global.h"
#include "AppResources.h"
#include "DispProgress.h"
#include "Prefs.h"
#include "Utils.h"
#include "NotifyReg.h"
#include "GID_DB.h"
#include "GetGID.h"
#include "RegionsDB.h"
#include "SignalLvl.h"
#include "MsgLogDB.h"

// Defines
#ifndef STRLEN_ERROR
# define STRLEN_ERROR 					32
#endif

#define ERR_STR_PHN_OFF						"Phone Off"
#define ERR_STR_NETWORK_ERR					"Network Error"
#define ERR_STR_SMS_CREATE_ERR				"Message Create Error"
#define ERR_STR_ADDRESS_ERR					"Address Error"
#define ERR_STR_SMS_ERR						"SMS Error"
#define ERR_STR_MESSAGE_SENT				"Message Sent"

typedef struct {
	lastLocation_t							loc;
	attentionMode_e 						attnMode;
} msgNotify_t;

// Prototypes
extern void 								MsgNotify( lastLocation_t* pLastLoc, attentionMode_e attnMode );	
extern Err 									HandleMsgNotification( SysNotifyParamType* notifyParamP );
extern void 								DoSMS( lastLocation_t* pLoc, attentionMode_e attnMode );
extern Err 									MakeMsgAndSend( msg_t msg, attentionMode_e attnMode, Char* locName );

#endif /* __SMSUTILS_H__ */

/*
 * SMSUtils.h
 */
 