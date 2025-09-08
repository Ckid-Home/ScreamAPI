#pragma once

#include <map>

namespace scream_api::api {
    using entitlement_id_t = std::string;
    using entitlement_title_t = std::string;

    using entitlement_map_t = std::map<entitlement_id_t, entitlement_title_t>;

    std::optional<entitlement_map_t> fetch_entitlements(const std::string& namespace_id);
}
