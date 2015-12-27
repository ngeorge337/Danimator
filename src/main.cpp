#include "libs.h"
#include "mainWindow.h"
#include "alert.h"

IMPLEMENT_APP(DanApp);

bool DanAlert::inAlert = false;

CVAR(Bool, warn_deletesprite, true, false, true, "Warn user on attempt to delete sprite", CVAR_ARCHIVE);
CVAR(Bool, warn_deletesound, true, false, true, "Warn user on attempt to delete sound", CVAR_ARCHIVE);
CVAR(Bool, warn_badspritenames, true, false, true, "Warn user when loading improper sprite names", CVAR_ARCHIVE);
CVAR(Bool, warn_badtexturename, true, false, true, "Warn user when creating a texture with improper name", CVAR_ARCHIVE);
CVAR(Bool, warn_badexport, true, false, true, "Warn user when DECORATE export contains bad sprite names", CVAR_ARCHIVE);

CVAR(Bool, warn_bakesprite, true, false, true, "Warn user when baking sprite and explain effect", CVAR_ARCHIVE);
CVAR(Bool, warn_importtextures, true, false, true, "Warn user about requirements when importing TEXTURES code", CVAR_ARCHIVE);
