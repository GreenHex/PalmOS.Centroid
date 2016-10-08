/*
 * MsgLogDB.h
 */
 
#ifndef __MSGLOGDB_H__
#define __MSGLOGDB_H__

#include <Hs.h>
#include <DataMgr.h>
#include <Form.h>
#include <Window.h>

#include "Global.h"
#include "GID_DB.h"
#include "Prefs.h"
// #include "SMSUtils.h"
#include "DBUtils.h"
#include "Utils.h"

#define CentroidMsgLogDbCreator			appFileCreator
#define CentroidMsgLogDbName			"CentroidMsgLogDB"
#define CentroidMsgLogDbType			'DCMS'
#define CentroidMsgLogDbVersion			0x01

#define STRLEN_TRUNCMSG_LEN				20

#define POS_X_RIGHT_ERR					31
#define POS_X_RIGHT_TIME				59
#define POS_X_RIGHT_DATE				84
#define POS_X_LEFT_PHN_NUM				88

#ifndef STRLEN_ERROR
# define STRLEN_ERROR 					32
#endif

typedef struct {
	Err						error;
	Char					strError[STRLEN_ERROR];
	UInt16					numAttempts;
	UInt32					timeSecs;
	attentionMode_e 		attnMode;
	Char					Address[STRLEN_PHN_NUMBER];
	Char					CallerID[STRLEN_CLIR];
	Char					Message[STRLEN_TRUNCMSG_LEN];
} log_t;

// Prototypes
extern Err					MsgSaveLog( Err error, Char* strError, UInt16 numAttempts, attentionMode_e attnMode, msg_t* pMsg );
extern void 				DrawLogsListItem( Int16 itemNum, RectangleType* pRect, Char **itemsText );
extern UInt16 				InitializeLogList( ListType* pList );
extern Err 					DeleteLogDB( void );
extern Err					LogRemoveRecordAtIdx( UInt16* idxP );
extern Err 					LogGetRecordAtIdx( UInt16* idxP, log_t* pLog );

#endif /* __MSGLOGDB_H__ */

/*
 * MsgLogDB.h
 */