#pragma once

#include <string>

#include <nlohmann/json.hpp>

namespace scream_api::config {
    enum class dlc_status {
        UNDEFINED,
        ORIGINAL,
        UNLOCKED,
        LOCKED,
    };

    // @formatter:off
    NLOHMANN_JSON_SERIALIZE_ENUM(dlc_status, {
        { dlc_status::UNDEFINED, nullptr     },
        { dlc_status::ORIGINAL,  "original"  },
        { dlc_status::UNLOCKED,  "unlocked"  },
        { dlc_status::LOCKED,    "locked"    },
    }) // @formatter:on

    struct Config {
        uint32_t $version = 3;
        bool logging = false;
        bool log_eos = false;
        bool block_metrics = false;
        std::string namespace_id;
        dlc_status default_dlc_status = dlc_status::UNLOCKED;
        std::map<std::string, dlc_status> override_dlc_status;
        // Key is id, Value is title
        std::vector<std::string> extra_graphql_endpoints;
        std::map<std::string, std::string> extra_entitlements;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(
            Config,
            $version,
            logging,
            log_eos,
            block_metrics,
            namespace_id,
            default_dlc_status,
            override_dlc_status,
            extra_graphql_endpoints,
            extra_entitlements
        );
    };

    extern Config instance;

    bool is_dlc_unlocked(const std::string& entitlement_id, bool original_unlocked);

    inline bool is_logging_eos() {
        return instance.logging && instance.log_eos;
    }
}
