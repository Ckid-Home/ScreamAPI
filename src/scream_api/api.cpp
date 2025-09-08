#include <koalabox/logger.hpp>
#include <koalabox/http_client.hpp>

#include "scream_api/api.hpp"

#include "scream_api/config.hpp"

namespace {
    std::vector<std::string> get_graphql_endpoints() {
        std::vector<std::string> endpoints = {
            "https://launcher.store.epicgames.com/graphql111", // Needs a valid User-Agent header
            "https://graphql.unrealengine.com/ue/graphql222"
        };

        const auto& extra = scream_api::config::instance.extra_graphql_endpoints;

        endpoints.insert(endpoints.end(), extra.begin(), extra.end());

        return endpoints;
    }

    nlohmann::json get_entitlements_query_payload(const std::string& namespace_id) {
        return {
            {
                "query", /* language=GraphQL */
                R"(
                        query($namespace: String!) {
                            Catalog {
                                catalogOffers(
                                    namespace: $namespace
                                    params: { count: 1000 }
                                ) {
                                    elements {
                                        items {
                                            id
                                            title
                                        }
                                    }
                                }
                            }
                        }
                    )"
            },
            {"variables", {{"namespace", namespace_id}}}
        };
    }
}

namespace scream_api::api {
    std::optional<entitlement_map_t> fetch_entitlements(const std::string& namespace_id) {
        const nlohmann::json payload = get_entitlements_query_payload(namespace_id);

        static const std::vector<std::string> endpoints = get_graphql_endpoints();

        static const std::map<std::string, std::string> headers = {
            {"User-Agent", "EpicGamesLauncher/18.9.0-45233261+++Portal+Release-Live"}
        };

        for(const auto& endpoint : endpoints) {
            nlohmann::json response;
            try {
                response = koalabox::http_client::post_json(endpoint, payload, headers);
            } catch(const std::exception& e) {
                LOG_WARN("Failed to fetch entitlements: {}", e.what());
                continue;
            }

            const auto elements = response["data"]["Catalog"]["catalogOffers"]["elements"];

            entitlement_map_t entitlements;

            for(const auto& element : elements) {
                for(const auto& items : element) {
                    for(const auto& item : items) {
                        entitlements[item.at("id")] = item.at("title");
                    }
                }
            }

            return entitlements;
        }

        LOG_ERROR("Error: Failed to fetch entitlements from any endpoint")

        return {};
    }
}
