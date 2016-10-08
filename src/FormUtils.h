/*
 * FormUtils.h
 */

#ifndef __FORMUTILS_H__
#define __FORMUTILS_H__

#include <Hs.h>
#include <PalmOS.h>
#include <Form.h>

// Prototypes
extern void 			SetPopLabelFromList3( FormType* pForm, UInt16 popID, UInt16 lstID, UInt16 idx );
extern void 			FieldSetEditable( FormType* pForm, UInt16 fldID, Boolean bEditable );
extern void 			SetCtlEnabled( FormType* pForm, UInt16 ctlID, Boolean bUsable );
extern void 			SetCtlLabelStr( FormType* pForm, UInt16 ctlID, Char* str );
extern void 			SetLabelStr( FormType* pForm, UInt16 ctlID, Char* str );
extern void 			ClearFieldStr( FormType* pForm, UInt16 fldID );
extern void 			SetFieldStr( FormType* pForm, UInt16 fldID, Char* str );
extern void 			GetFieldStr( FormType* pForm, UInt16 fldID, Char* str, UInt16 strBufferSize );
extern void 			SetCtlBoolValue( FormType* pForm, UInt16 ctlID, Boolean value );
extern Boolean 			GetCtlBoolValue( FormType* pForm, UInt16 ctlID );

#endif /* __FORMUTILS_H__ */
 
/*
 * FormUtils.h
 */