#include <eos_ecom.h>
#include <eos_init.h>
#include <eos_logging.h>
#include <eos_metrics.h>

#include <koalabox/config.hpp>
#include <koalabox/dll_monitor.hpp>
#include <koalabox/globals.hpp>
#include <koalabox/hook.hpp>
#include <koalabox/loader.hpp>
#include <koalabox/path.hpp>
#include <koalabox/paths.hpp>
#include <koalabox/win.hpp>

#include "build_config.h"

#include "scream_api/scream_api.hpp"
#include "scream_api/config.hpp"

#define HOOK(FUNC) \
    koalabox::hook::detour_or_warn( \
        scream_api::eossdk_handle, \
        koalabox::loader::get_decorated_function(scream_api::eossdk_handle, #FUNC), \
        reinterpret_cast<void*>(FUNC) \
    )

namespace {
    namespace kb = koalabox;

    bool on_eossdk_loaded(const HMODULE eossdk_handle) {
        scream_api::eossdk_handle = eossdk_handle;

        HOOK(EOS_Initialize);
        HOOK(EOS_Platform_Create);

        HOOK(EOS_Ecom_CopyEntitlementByIndex);
        HOOK(EOS_Ecom_CopyEntitlementByNameAndIndex);
        HOOK(EOS_Ecom_CopyItemById);
        HOOK(EOS_Ecom_Entitlement_Release);
        HOOK(EOS_Ecom_GetEntitlementsByNameCount);
        HOOK(EOS_Ecom_GetEntitlementsCount);
        HOOK(EOS_Ecom_GetItemReleaseCount);
        HOOK(EOS_Ecom_QueryEntitlementToken);
        HOOK(EOS_Ecom_QueryEntitlements);
        HOOK(EOS_Ecom_QueryOwnership);
        HOOK(EOS_Ecom_QueryOwnershipBySandboxIds);
        HOOK(EOS_Ecom_QueryOwnershipToken);
        HOOK(EOS_Ecom_RedeemEntitlements);


        if(scream_api::config::instance.block_metrics) {
            HOOK(EOS_Metrics_BeginPlayerSession);
            HOOK(EOS_Metrics_EndPlayerSession);
        }

        if(scream_api::config::is_logging_eos()) {
            HOOK(EOS_Logging_SetCallback);
            HOOK(EOS_Logging_SetLogLevel);
        }

        // Note: We should not shut down the listener because some games may mistakenly load EOS SDK DLL more than once.
        return false;
    }

    void init_proxy_mode() {
        LOG_INFO("Detected proxy mode");

        const auto self_path = kb::paths::get_self_dir();
        scream_api::eossdk_handle = kb::loader::load_original_library(self_path, EOSSDK_DLL);
    }

    void init_hook_mode() {
        LOG_INFO("Detected hook mode");

        kb::dll_monitor::init_listener(
            {{EOSSDK_DLL, on_eossdk_loaded}}
        );
    }
}

namespace scream_api {
    std::string namespace_id;

    HMODULE eossdk_handle = nullptr;

    bool hook_mode = false;

    void init(const HMODULE module_handle) {
        try {
            kb::globals::init_globals(module_handle, PROJECT_NAME);

            config::instance = kb::config::parse<config::Config>();

            if(config::instance.logging) {
                kb::logger::init_file_logger(kb::paths::get_log_path());
            }

            LOG_INFO("{} v{} | Built at '{}'", PROJECT_NAME, PROJECT_VERSION, __TIMESTAMP__);
            LOG_DEBUG("Parsed config:\n{}", nlohmann::ordered_json(config::instance).dump(2));

            const auto exe_path = kb::win::get_module_path(nullptr);
            const auto exe_name = kb::path::to_str(exe_path.filename());

            LOG_DEBUG("Process name: '{}' [{}-bit]", exe_name, kb::util::BITNESS);

            // No harm in initializing it even if proxy mode doesn't use it
            kb::hook::init(true);

            if(kb::hook::is_hook_mode(module_handle, EOSSDK_DLL)) {
                hook_mode = true;
                init_hook_mode();
            } else {
                init_proxy_mode();
            }

            LOG_INFO("Initialization complete");
        } catch(const std::exception& e) {
            kb::util::panic(fmt::format("Initialization error: {}", e.what()));
        }
    }

    void shutdown() {
        try {
            if(!hook_mode) {
                kb::win::free_library(eossdk_handle);
                eossdk_handle = nullptr;
            }

            kb::logger::shutdown();

            LOG_INFO("Shutdown complete");
        } catch(const std::exception& e) {
            const auto msg = std::format("Shutdown error: {}", e.what());
            LOG_ERROR(msg);
        }
    }
}
