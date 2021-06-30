/*===================================================================
 * 
 * The Medical Imaging Interaction Toolkit (MITK)
 * 
 * Copyright (c) German Cancer Research Center,
 * Division of Medical and Biological Informatics.
 * All rights reserved.
 * 
 * This software is distributed WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.
 * 
 * See LICENSE.txt or http://www.mitk.org for details.
 * 
 * ===================================================================*/

#include "mitkFromBCToACTool.h"

#include "frombc2ac.xpm"

// us
#include <usModule.h>
#include <usModuleResource.h>
#include <usGetModuleContext.h>
#include <usModuleContext.h>

namespace mitk {
    MITK_TOOL_MACRO(AmosWorkbenchLib_EXPORT, FromBCToACTool, "FromBCToAC tool")
}

mitk::FromBCToACTool::FromBCToACTool() : MySetRegionTool(1, 1, 0)
{
}

mitk::FromBCToACTool::~FromBCToACTool()
{
}

const char** mitk::FromBCToACTool::GetXPM() const
{
    return frombc2ac_xpm;
    //   return NULL;
}

us::ModuleResource mitk::FromBCToACTool::GetIconResource() const
{
    us::Module* module = us::GetModuleContext()->GetModule();
    us::ModuleResource resource = module->GetResource("frombc2ac.png");
    return resource;
}

us::ModuleResource mitk::FromBCToACTool::GetCursorIconResource() const
{
    us::Module* module = us::GetModuleContext()->GetModule();
    us::ModuleResource resource = module->GetResource("frombc2ac_cursor.png");
    return resource;
}

const char* mitk::FromBCToACTool::GetName() const
{
    return "BC to AC";
}
