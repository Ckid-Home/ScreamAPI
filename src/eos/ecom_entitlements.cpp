#include <eos_ecom.h>

#include <koalabox/http_client.hpp>
#include <koalabox/logger.hpp>

#include "scream_api/api.hpp"
#include "scream_api/config.hpp"
#include "scream_api/scream_api.hpp"

namespace {
    scream_api::api::entitlement_map_t entitlement_map;

    // We need to respond with entitlements by index, hence we need to maintain list of IDs as well.
    std::vector<scream_api::api::entitlement_id_t> entitlement_ids;

    void auto_fetch_entitlements() {
        auto namespace_id = scream_api::namespace_id;
        if(namespace_id.empty()) {
            namespace_id = scream_api::config::instance.namespace_id;
            LOG_DEBUG("Using namespace_id from config: '{}'", namespace_id);
        }

        if(namespace_id.empty()) {
            LOG_WARN(
                "EOS_Ecom_QueryEntitlements callback: namespace_id is not set. "
                "Probable cause: ScreamAPI was injected too late.\n"
                "Try injecting it earlier, so that ScreamAPI is loaded before EOSSDK DLL.\n"
                "If that is not possible, you can manually set `namespace_id` using a config file."
            );

            return;
        }

        if(!entitlement_map.empty()) {
            // We are already making an entitlement map
            return;
        }

        static scream_api::api::entitlement_map_t entitlements_cache;

        const auto entitlements_opt = scream_api::api::fetch_entitlements(namespace_id);
        if(entitlements_opt.has_value()) {
            entitlements_cache = *entitlements_opt;
        }

        if(entitlements_cache.empty()) {
            // Failed to fetch & no cache, therefore nothing to inject
            return;
        }

        for(const auto& [id, title] : entitlements_cache) {
            if(scream_api::config::is_dlc_unlocked(id, false)) {
                LOG_DEBUG(R"(  Adding auto-fetched entitlement: {} - "{}")", id, title);
                entitlement_map[id] = title;
            }
        }
    }

    void inject_extra_entitlements() {
        for(auto& [id, title] : scream_api::config::instance.extra_entitlements) {
            if(scream_api::config::is_dlc_unlocked(id, true)) {
                LOG_DEBUG(R"(Adding entitlement from config: {} - "{}")", id, title);
                entitlement_map[id] = title;
            }
        }
    }
}

// Entitlements API is not how games should normally query DLC ownership.
// However, some games like The Escapists 2 do make use of it, instead of the Items API.
// Hence, we need to handle Entitlements API asw well.

DLL_EXPORT(void) EOS_Ecom_QueryEntitlements(
    EOS_EcomHandle* const Handle,
    const EOS_Ecom_QueryEntitlementsOptions* Options,
    void* ClientData,
    const EOS_Ecom_OnQueryEntitlementsCallback CompletionDelegate
) {
    LOG_INFO("Game queried ownership of {} entitlements:", Options->EntitlementNameCount);

    entitlement_map.clear();
    entitlement_ids.clear();

    for(auto i = 0U; i < Options->EntitlementNameCount; i++) {
        const char* const id = Options->EntitlementNames[i];
        LOG_INFO("  {}", id);

        if(scream_api::config::is_dlc_unlocked(id, true)) {
            entitlement_map[id] = std::format("Unknown Title", id);
        }
    }

    struct Container {
        void* ClientData;
        EOS_Ecom_OnQueryEntitlementsCallback CompletionDelegate;
    };

    CALL_ORIGINAL_FN(EOS_Ecom_QueryEntitlements)(
        Handle,
        Options,
        new Container{ClientData, CompletionDelegate},
        [](const EOS_Ecom_QueryEntitlementsCallbackInfo* Data) {
            try {
                const auto* const container = static_cast<Container*>(Data->ClientData);

                auto_fetch_entitlements();
                inject_extra_entitlements();

                entitlement_ids = std::vector(std::from_range, std::views::keys(entitlement_map));

                LOG_INFO("Responding with {} entitlements:", entitlement_map.size());
                for(const auto& [id, title] : entitlement_map) {
                    LOG_INFO(R"(  {} = "{}")", id, title);
                }

                auto* data = const_cast<EOS_Ecom_QueryEntitlementsCallbackInfo*>(Data);

                data->ResultCode = EOS_EResult::EOS_Success;
                data->ClientData = container->ClientData;

                container->CompletionDelegate(Data);

                delete container;
            } catch(const std::exception& e) {
                LOG_ERROR("EOS_Ecom_QueryEntitlements callback error: {}", e.what())
            }
        }
    );
}

DLL_EXPORT(uint32_t) EOS_Ecom_GetEntitlementsCount(EOS_HEcom, const EOS_Ecom_GetEntitlementsCountOptions*) {
    const auto count = entitlement_map.size();

    LOG_DEBUG("Responding with {} entitlements", count);

    return count;
}

DLL_EXPORT(EOS_EResult) EOS_Ecom_CopyEntitlementByIndex(
    EOS_HEcom,
    const EOS_Ecom_CopyEntitlementByIndexOptions* Options,
    EOS_Ecom_Entitlement** OutEntitlement
) {
    const auto index = Options->EntitlementIndex;
    if(index >= entitlement_ids.size()) {
        LOG_WARN(
            "Game requested invalid entitlement index: {}. Max size: {}",
            index, entitlement_ids.size()
        );

        return EOS_EResult::EOS_NotFound;
    }

    const auto& id = entitlement_ids[index];
    const auto& title = entitlement_map.at(id);

    LOG_DEBUG("Copying the entitlement: {} at index: {}", id, index);

    *OutEntitlement = new EOS_Ecom_Entitlement{
        .ApiVersion = EOS_ECOM_ENTITLEMENT_API_LATEST,
        .EntitlementName = title.c_str(),
        .EntitlementId = id.c_str(), // Not actually true, but so far no issues.
        .CatalogItemId = id.c_str(),
        .ServerIndex = -1,
        .bRedeemed = false,
        .EndTimestamp = -1,
    };

    return EOS_EResult::EOS_Success;
}

DLL_EXPORT(void) EOS_Ecom_Entitlement_Release(EOS_Ecom_Entitlement* Entitlement) {
    if(Entitlement) {
        LOG_DEBUG("Freeing a copy of the entitlement: {}", Entitlement->EntitlementId);
        delete Entitlement;
    } else {
        LOG_WARN("Game attempted to free a null entitlement");
    }
}
