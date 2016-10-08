/*
 * NotifyReg.c
 */
 
#include "NotifyReg.h"

/*
 * RegisterForNotifications()
 */
Err RegisterForNotifications( Boolean bRegister )
{
	Err					error = errNone;
	UInt16 				cardNo = 0; 
	LocalID  			dbID = 0;	
	UInt16 				PhoneLibRefNum = sysInvalidRefNum;

	if ( ( error = SysCurAppDatabase( &cardNo, &dbID ) ) )
		return ( error );
	
	if ( ( error = HsGetPhoneLibrary( &PhoneLibRefNum ) ) )
		return ( error );
		
	if ( ( error = PhnLibOpen( PhoneLibRefNum ) ) )
		return ( error );
		
	if ( bRegister )
	{
		error = PhnLibRegister( PhoneLibRefNum, appFileCreator, phnServiceActivation );
				/* phnServiceVoice | phnServiceActivation | phnServiceMisc | phnServiceEssentials */
				
		SysNotifyRegister( cardNo, dbID, sysNotifyTimeChangeEvent, NULL, sysNotifyNormalPriority, NULL );
		SysNotifyRegister( cardNo, dbID, msgDeferredNotifyEvent, NULL, sysNotifyNormalPriority, NULL );
	}
	else
	{
		error = PhnLibRegister( PhoneLibRefNum, appFileCreator, 0 );
		
		SysNotifyUnregister( cardNo, dbID, sysNotifyTimeChangeEvent, sysNotifyNormalPriority );
		SysNotifyUnregister( cardNo, dbID, msgDeferredNotifyEvent, sysNotifyNormalPriority );
	}
			
	PhnLibClose( PhoneLibRefNum );

	return ( error );
		
} // RegisterForNotifications()

/*
 * NotifyReg.c
 */