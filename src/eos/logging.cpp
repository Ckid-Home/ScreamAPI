#include <set>

#include <eos_logging.h>

#include <koalabox/logger.hpp>

#include "scream_api/config.hpp"
#include "scream_api/scream_api.hpp"

DLL_EXPORT(EOS_EResult) EOS_Logging_SetCallback(const EOS_LogMessageFunc Callback) {
    LOG_DEBUG("{} -> Callback: {}", __func__, reinterpret_cast<void*>(Callback));

    if(scream_api::config::is_logging_eos()) {
        // To prevent the game override our logging callback, we store and invoke callbacks ourselves
        static std::set<EOS_LogMessageFunc> callbacks;

        callbacks.insert(Callback);

        CALL_ORIGINAL_FN(EOS_Logging_SetCallback, return)(
            [](const EOS_LogMessage* Message) {
                for(const auto& callback : callbacks) {
                    callback(Message);
                }
            }
        );
    }

    CALL_ORIGINAL_FN(EOS_Logging_SetCallback, return)(Callback);
}

DLL_EXPORT(EOS_EResult) EOS_Logging_SetLogLevel(EOS_ELogCategory LogCategory, EOS_ELogLevel LogLevel) {
    LOG_DEBUG(
        "{} -> Category: 0x{:X}, Level: {}",
        __func__, static_cast<int32_t>(LogCategory), static_cast<int32_t>(LogLevel)
    );

    if(scream_api::config::is_logging_eos()) {
        // Override game's preferences
        LogCategory = EOS_ELogCategory::EOS_LC_ALL_CATEGORIES;
        LogLevel = EOS_ELogLevel::EOS_LOG_VeryVerbose;
    }

    CALL_ORIGINAL_FN(EOS_Logging_SetLogLevel, return)(LogCategory, LogLevel);
}
