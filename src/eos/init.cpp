#include <eos_init.h>
#include <eos_logging.h>

#include <koalabox/logger.hpp>

#include "scream_api/config.hpp"
#include "scream_api/scream_api.hpp"

DLL_EXPORT(EOS_EResult) EOS_Initialize(const EOS_InitializeOptions* Options) {
    if(Options) {
        LOG_DEBUG(R"({} -> ProductName: "{}")", __func__, Options->ProductName);
    } else {
        LOG_DEBUG("{} -> null options", __func__);
    }

    if(scream_api::config::is_logging_eos()) {
        EOS_Logging_SetLogLevel(
            EOS_ELogCategory::EOS_LC_ALL_CATEGORIES,
            EOS_ELogLevel::EOS_LOG_VeryVerbose
        );

        EOS_Logging_SetCallback(
            [](const EOS_LogMessage* Message) {
                LOG_DEBUG("[EOS_Logging] <{}>\t{}", Message->Category, Message->Message);
            }
        );
    }

    CALL_ORIGINAL_FN(EOS_Initialize, return)(Options);
}

DLL_EXPORT(EOS_HPlatform) EOS_Platform_Create(const EOS_Platform_Options* Options) {
    if(Options) {
        LOG_DEBUG("{} -> ApiVersion: {}", __func__, Options->ApiVersion);

        scream_api::namespace_id = Options->SandboxId;

        static std::once_flag flag;
        std::call_once(
            flag, [&] {
                LOG_INFO("DLC database: https://scream-db.web.app/games/{}", Options->SandboxId);
            }
        );
    } else {
        LOG_DEBUG("{} -> null options", __func__);
    }

    CALL_ORIGINAL_FN(EOS_Platform_Create, return)(Options);
}
