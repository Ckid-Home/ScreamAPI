#include <eos_metrics.h>

#include <koalabox/logger.hpp>

#include <scream_api/config.hpp>
#include <scream_api/scream_api.hpp>

DLL_EXPORT(EOS_EResult) EOS_Metrics_BeginPlayerSession(
    EOS_MetricsHandle* const Handle,
    const EOS_Metrics_BeginPlayerSessionOptions* Options
) {
    if(scream_api::config::instance.block_metrics) {
        LOG_INFO("Blocking {}", __func__);
        return EOS_EResult::EOS_Success;
    }

    CALL_ORIGINAL_FN(EOS_Metrics_BeginPlayerSession, return)(Handle, Options);
}

DLL_EXPORT(EOS_EResult) EOS_Metrics_EndPlayerSession(
    EOS_MetricsHandle* const Handle,
    const EOS_Metrics_EndPlayerSessionOptions* Options
) {
    if(scream_api::config::instance.block_metrics) {
        LOG_INFO("Blocking {}", __func__);
        return EOS_EResult::EOS_Success;
    }

    CALL_ORIGINAL_FN(EOS_Metrics_EndPlayerSession, return)(Handle, Options);
}
