#ifndef __MAIN_H__
#define __MAIN_H__

#include <windows.h>
#include <fstream>
#include <string>
#include <vector>
#include <time.h>
#include <list>
#include <map>
#include <algorithm>
#include <gl/GL.h>
#include <gl/GLU.h>

using namespace std;

#include "ValveSDK/engine/wrect.h"
#include "ValveSDK/engine/cl_dll.h"
#include "ValveSDK/engine/r_studioint.h"
#include "ValveSDK/engine/cl_entity.h"
#include "ValveSDK/misc/com_model.h"
#include "ValveSDK/engine/triangleapi.h"
#include "ValveSDK/engine/pmtrace.h"
#include "ValveSDK/engine/pm_defs.h"
#include "ValveSDK/engine/pm_info.h"
#include "ValveSDK/common/ref_params.h"
#include "ValveSDK/common/studio_event.h"
#include "ValveSDK/common/net_api.h"
#include "ValveSDK/common/r_efx.h"
#include "ValveSDK/engine/cvardef.h"
#include "ValveSDK/engine/util_vector.h"
#include "ValveSDK/misc/parsemsg.h"
#include "ValveSDK/engine/studio.h"
#include "ValveSDK/engine/event_args.h"
#include "ValveSDK/engine/event_flags.h"
#include "ValveSDK/common/event_api.h"
#include "ValveSDK/common/screenfade.h"
#include "ValveSDK/engine/keydefs.h"
#include "ValveSDK/common/engine_launcher_api.h"

#include "detours.h"
#include "struct.h"
#include "CvarUtils.h"
#include "UserMsgs.h"
#include "ModuleSecurity.h"
#include "AutoOffset.h"
#include "SvcMessage.h"
#include "Utils.h"
#include "Drawing.h"
#include "Client.h"
#include "cFont.h"

void HookOpenGL();
void add_log(const char *fmt, ...);

#endif