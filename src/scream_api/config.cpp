#include <scream_api/config.hpp>

#include <koalabox/config.hpp>
#include <koalabox/logger.hpp>

namespace scream_api::config {
    Config instance; // NOLINT(cert-err58-cpp)

    bool is_dlc_unlocked(const std::string& entitlement_id, bool original_unlocked) {
        auto status = instance.default_dlc_status;

        if(instance.override_dlc_status.contains(entitlement_id)) {
            status = instance.override_dlc_status.at(entitlement_id);
        }

        bool unlocked;
        switch(status) {
        case dlc_status::UNLOCKED:
            unlocked = true;
            break;
        case dlc_status::LOCKED:
            unlocked = false;
            break;
        case dlc_status::ORIGINAL:
        case dlc_status::UNDEFINED:
            unlocked = original_unlocked;
            break;
        }

        LOG_TRACE(
            "entitlement_id: '{}', status: '{}', original_unlocked: {}, unlocked: {}",
            entitlement_id,
            nlohmann::json(status).get<std::string>(),
            original_unlocked,
            unlocked
        );

        return unlocked;
    }
}
