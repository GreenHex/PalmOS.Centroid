/*
 * MessagingForm.h
 */
 
#ifndef __MESSAGINGFORM_H__
#define __MESSAGINGFORM_H__

#include <Hs.h>
#include <Form.h>
#include <Field.h>

#include "Sections.h"
#include "Prefs.h"
#include "GID_DB.h"
#include "AppResources.h"
#include "FormUtils.h"
#include "SMSUtils.h"

// Globals
msg_t*						gMsgP;

// Prototypes
extern Boolean 				MsgFormHandleEvent( EventType* pEvent ) EXTRA_SECTION_ONE;


#endif /* __MESSAGINGFORM_H__ */
 
/*
 * MessagingForm.h
 */