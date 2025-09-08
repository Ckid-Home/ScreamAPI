#include <eos_ecom.h>

#include <koalabox/logger.hpp>

#include "scream_api/scream_api.hpp"
#include "scream_api/config.hpp"

DLL_EXPORT(void) EOS_Ecom_QueryOwnership(
    EOS_EcomHandle* const Handle,
    const EOS_Ecom_QueryOwnershipOptions* Options,
    void* ClientData,
    const EOS_Ecom_OnQueryOwnershipCallback CompletionDelegate
) {
    LOG_INFO("Game requested ownership of {} items:", Options->CatalogItemIdCount);

    for(uint32_t i = 0; i < Options->CatalogItemIdCount; i++) {
        LOG_INFO("  {}", Options->CatalogItemIds[i]);
    }

    struct Container {
        void* ClientData;
        EOS_Ecom_OnQueryOwnershipCallback CompletionDelegate;
    };

    CALL_ORIGINAL_FN(EOS_Ecom_QueryOwnership)(
        Handle,
        Options,
        new Container{ClientData, CompletionDelegate},
        [](const EOS_Ecom_QueryOwnershipCallbackInfo* Data) {
            const auto* const container = static_cast<Container*>(Data->ClientData);

            LOG_INFO("Responding with {} items:", Data->ItemOwnershipCount);

            for(uint32_t i = 0; i < Data->ItemOwnershipCount; i++) {
                const auto* const item = Data->ItemOwnership + i;

                const auto unlocked = scream_api::config::is_dlc_unlocked(
                    item->Id,
                    item->OwnershipStatus == EOS_EOwnershipStatus::EOS_OS_Owned
                );

                const auto status =
                    unlocked
                        ? EOS_EOwnershipStatus::EOS_OS_Owned
                        : EOS_EOwnershipStatus::EOS_OS_NotOwned;

                const std::string unlock_status = unlocked ? "Owned" : "Not Owned";

                const_cast<EOS_Ecom_ItemOwnership*>(item)->OwnershipStatus = status;

                LOG_INFO("  {} [{}]", item->Id, unlock_status);
            }

            const_cast<EOS_Ecom_QueryOwnershipCallbackInfo*>(Data)->ClientData = container->ClientData;

            container->CompletionDelegate(Data);

            delete container;
        }
    );
}
