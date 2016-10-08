/*
 * FormUtils.c
 */
 
#include "FormUtils.h"

/*
 * FieldSetEditable()
 */
void FieldSetEditable( FormType* pForm, UInt16 fldID, Boolean bEditable )
{
	FieldType*			pField = FrmGetObjectPtr( pForm, FrmGetObjectIndex( pForm, fldID ) );
	FieldAttrType		fldAttr;
	
	if ( pField )
	{
		FldGetAttributes( pField, &fldAttr );
		
		fldAttr.editable = bEditable;
		
		if ( bEditable )
		{
			fldAttr.underlined = grayUnderline;
		}
		else
		{
			fldAttr.underlined = noUnderline;
		}
		
		FldSetAttributes( pField, &fldAttr );
		
		FldReleaseFocus( pField );
	}
	
	return;
	
} // FieldSetEditable()
			
/*
 * SetCtlEnabled()
 */
void SetCtlEnabled( FormType* pForm, UInt16 ctlID, Boolean bUsable )
{	
	CtlSetEnabled( FrmGetObjectPtr( pForm, FrmGetObjectIndex( pForm, ctlID ) ), bUsable );
	
	return;
	
} // SetCtlEnabled()

/*
 * SetCtlLabelStr()
 */
void SetCtlLabelStr( FormType* pForm, UInt16 ctlID, Char* str )
{
	Char*		strLabel = NULL;
	if ( !( ( pForm ) && ( ctlID ) && ( str ) && StrLen( str )) ) return;
	
	strLabel = (Char *) CtlGetLabel( FrmGetObjectPtr( pForm, FrmGetObjectIndex( pForm, ctlID ) ) );
			
	StrNCopy( strLabel, str, StrLen( strLabel ) );
	strLabel[StrLen( strLabel )] = chrNull;
	
	CtlSetLabel( FrmGetObjectPtr( pForm, FrmGetObjectIndex( pForm, ctlID ) ), strLabel );

	return;
		
} // SetCtlLabelStr()


/*
 * SetLabelStr()
 */
void SetLabelStr( FormType* pForm, UInt16 ctlID, Char* str )
{
	if ( !( ( pForm ) && ( ctlID ) && ( str ) && StrLen( str ) ) ) return;
	
	if ( StrLen( str ) >  StrLen( (Char *) FrmGetLabel( pForm, ctlID ) ) )
	{
		str[StrLen( (Char *) FrmGetLabel( pForm, ctlID ) )] = chrNull;
	}
	
	FrmCopyLabel( pForm, ctlID, str );
	
	return;
		
} // SetLabelStr()

/*
 * ClearFieldStr()
 */
void ClearFieldStr( FormType* pForm, UInt16 fldID )
{
	SetFieldStr( pForm, fldID, "" );
		
	return;	
	
} // ClearFieldStr()

/*
 * SetFieldStr()
 */
void SetFieldStr( FormType* pForm, UInt16 fldID, Char* str )
{
	MemHandle		memH = NULL;
	Char*			strP = NULL;
	FieldType*		pField = NULL;
	UInt16			fldStrLen = 0;
	
	if ( !( ( pForm ) && ( fldID ) && ( str ) /* && StrLen( str ) */ ) ) return;
	
	pField = FrmGetObjectPtr( pForm, FrmGetObjectIndex( pForm, fldID ) );
		
	fldStrLen = FldGetMaxChars( pField ) + 1;
	
	if ( pField ) 
	{
		memH = FldGetTextHandle( pField );
		
		if ( memH )
		{
			if ( MemHandleResize( memH, fldStrLen ) )
			{
				fldStrLen = MemHandleSize( memH );	
			}	
		}
		else
		{
			memH = MemHandleNew( fldStrLen );
		}
		
		strP = MemHandleLock( memH );
		StrNCopy( strP, str, fldStrLen - 1 );
		strP[fldStrLen - 1] = chrNull;
		
		FldSetTextHandle( pField, NULL );			
		FldSetTextHandle( pField, memH );
		
		MemHandleUnlock( memH );

		FldSetDirty( pField, true );		
	}
	
	return;
	
} // SetFieldStr()

/*
 * GetFieldStr()
 */
void GetFieldStr( FormType* pForm, UInt16 fldID, Char* str, UInt16 strBufferSize )
{
	Char*	strP = NULL;
	
	if ( !( ( pForm ) && ( fldID ) && ( str ) && ( strBufferSize ) ) ) return;
	
	if ( str )
	{
		str[0] = chrNull;
	}
	
	strP = (Char *) FldGetTextPtr( FrmGetObjectPtr( pForm, FrmGetObjectIndex( pForm, fldID ) ) );

	if ( ( strP ) && ( str ) )
	{
		MemMove( str, strP, strBufferSize - 1 );	
		str[strBufferSize - 1] = chrNull;
	}
	
	return;	

} // GetFieldStr()

/*
 * SetCtlBoolValue()
 */
void SetCtlBoolValue( FormType* pForm, UInt16 ctlID, Boolean value )
{
	ControlType*		pCtl = NULL;
	
	if ( !( ( pForm ) && ( ctlID ) ) ) return;
	
	pCtl = FrmGetObjectPtr( pForm, FrmGetObjectIndex( pForm, ctlID ) );
	
	if ( pCtl ) CtlSetValue( pCtl, value );

	return;
	
} // SetCtlBoolValue

/*
 * SetPopLabelFromList3()
 */
void SetPopLabelFromList3( FormType* pForm, UInt16 popID, UInt16 lstID, UInt16 idx )
{
	ControlType*	pCtl = FrmGetObjectPtr( pForm, FrmGetObjectIndex( pForm, popID ) );
	ListType*		pList = FrmGetObjectPtr( pForm, FrmGetObjectIndex( pForm, lstID ) );
	
	if ( ( pCtl ) && ( pList ) )
	{
		LstSetSelection( pList, ( LstGetNumberOfItems( pList ) > idx ) ? idx : 0 );
		CtlSetLabel( pCtl, LstGetSelectionText( pList, LstGetSelection( pList ) ) );
	}

	return;
	
} // SetPopLabelFromList3()

/*
 * GetCtlBoolValue()
 */
Boolean GetCtlBoolValue( FormType* pForm, UInt16 ctlID )
{
	Boolean				retVal = false;
	ControlType*		pCtl = NULL;
	
	if ( !( ( pForm ) && ( ctlID ) ) ) return ( retVal );
	
	pCtl = FrmGetObjectPtr( pForm, FrmGetObjectIndex( pForm, ctlID ) );
	
	if ( pCtl ) 
	{
		retVal = ( CtlGetValue( pCtl ) == 1 );
	}
	
	return ( retVal );
	
} // GetCtlBoolValue

/*
 * FormUtils.c
 */