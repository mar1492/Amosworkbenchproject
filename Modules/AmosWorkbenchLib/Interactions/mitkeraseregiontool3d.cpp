#include "mitkeraseregiontool3d.h"

#include "mitkEraseRegionTool.xpm"

// us
#include <usModule.h>
#include <usModuleResource.h>
#include <usGetModuleContext.h>
#include <usModuleContext.h>

namespace mitk {
    MITK_TOOL_MACRO(AmosWorkbenchLib_EXPORT, EraseRegionTool3D, "Erase tool")
}

EraseRegionTool3D::EraseRegionTool3D() : MySetRegionTool(0, 0, 0)
{
}

EraseRegionTool3D::~EraseRegionTool3D()
{
}

const char** EraseRegionTool3D::GetXPM() const
{
    return mitkEraseRegionTool_xpm;
}

us::ModuleResource EraseRegionTool3D::GetCursorIconResource() const
{
    us::Module* module = us::GetModuleContext()->GetModule();
    us::ModuleResource resource = module->GetResource("Erase_Cursor_32x32.png");
    return resource;
}

us::ModuleResource EraseRegionTool3D::GetIconResource() const
{
    us::Module* module = us::GetModuleContext()->GetModule();
    us::ModuleResource resource = module->GetResource("Erase_48x48.png");
    return resource;
}

const char* EraseRegionTool3D::GetName() const
{
    return "Erase23D";
}
