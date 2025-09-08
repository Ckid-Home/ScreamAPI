#include <koalabox/win.hpp>

#include "linker_exports_for_eossdk.h"
#include "linker_exports_for_version.h"

#include "legacy_linker_exports.h"
#include "scream_api/scream_api.hpp"

EXTERN_C BOOLEAN WINAPI DllMain(const HMODULE handle, const DWORD reason, LPVOID) {
    if(reason == DLL_PROCESS_ATTACH) {
        scream_api::init(handle);
    } else if(reason == DLL_PROCESS_DETACH) {
        scream_api::shutdown();
    }

    return TRUE;
}
