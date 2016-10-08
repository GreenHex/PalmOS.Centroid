/*
 * Gadgets.h
 */
 
#ifndef __GADGETS_H__
#define __GADGETS_H__

#include <Hs.h>
#include <Form.h>
#include <SoundMgr.h>

#include "Sections.h"
#include "Global.h"
#include "AppResources.h"
#include "Utils.h"
#include "SignalLvl.h"
#include "Bitmaps.h"
#include "LocForm.h"
#include "ProfilesForm.h"

// Prototypes
extern void 				InitializeGadgets( FormType* pForm ) EXTRA_SECTION_ONE;
extern Boolean				LocationGadgetHandler( FormGadgetTypeInCallback *gadgetP, UInt16 cmd, void *paramP ) EXTRA_SECTION_ONE;
extern Boolean				ProfileGadgetHandler( FormGadgetTypeInCallback *gadgetP, UInt16 cmd, void *paramP ) EXTRA_SECTION_ONE;
extern Boolean 				SignalLvlGadgetHandler( FormGadgetTypeInCallback *gadgetP, UInt16 cmd, void *paramP ) EXTRA_SECTION_ONE;
extern Boolean 				RoamingGadgetHandler( FormGadgetTypeInCallback *gadgetP, UInt16 cmd, void *paramP ) EXTRA_SECTION_ONE;
extern void 				UpdateGadgets( void ) EXTRA_SECTION_ONE;

#endif /* __GADGETS_H__ */
  
/*
 * Gadgets.h
 */