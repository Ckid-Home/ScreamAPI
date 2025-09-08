#include <eos_ecom.h>

#include <koalabox/logger.hpp>

#include "scream_api/config.hpp"
#include "scream_api/scream_api.hpp"

using namespace koalabox;

/// This file contains stub implementation of functions
/// for the purposes of logging and potential future improvements

#define IMPLEMENT(FUNC, ...) \
    LOG_DEBUG("{}", __func__); \
    CALL_ORIGINAL_FN(FUNC, return)

DLL_EXPORT(void) EOS_Ecom_QueryOwnershipBySandboxIds(
    EOS_EcomHandle* const Handle,
    const EOS_Ecom_QueryOwnershipBySandboxIdsOptions* Options,
    void* ClientData,
    const EOS_Ecom_OnQueryOwnershipBySandboxIdsCallback CompletionDelegate
) {
    IMPLEMENT(EOS_Ecom_QueryOwnershipBySandboxIds)(Handle, Options, ClientData, CompletionDelegate);
}

DLL_EXPORT(void) EOS_Ecom_QueryOwnershipToken(
    EOS_EcomHandle* const Handle,
    const EOS_Ecom_QueryOwnershipTokenOptions* Options,
    void* ClientData,
    const EOS_Ecom_OnQueryOwnershipTokenCallback CompletionDelegate
) {
    IMPLEMENT(EOS_Ecom_QueryOwnershipToken)(Handle, Options, ClientData, CompletionDelegate);
}

DLL_EXPORT(void) EOS_Ecom_QueryEntitlementToken(
    EOS_EcomHandle* const Handle,
    const EOS_Ecom_QueryEntitlementTokenOptions* Options,
    void* ClientData,
    const EOS_Ecom_OnQueryEntitlementTokenCallback CompletionDelegate
) {
    IMPLEMENT(EOS_Ecom_QueryEntitlementToken)(Handle, Options, ClientData, CompletionDelegate);
}

DLL_EXPORT(void) EOS_Ecom_RedeemEntitlements(
    EOS_EcomHandle* const Handle,
    const EOS_Ecom_RedeemEntitlementsOptions* Options,
    void* ClientData,
    const EOS_Ecom_OnRedeemEntitlementsCallback CompletionDelegate
) {
    IMPLEMENT(EOS_Ecom_RedeemEntitlements)(Handle, Options, ClientData, CompletionDelegate);
}

DLL_EXPORT(uint32_t) EOS_Ecom_GetEntitlementsByNameCount(
    EOS_EcomHandle* const Handle,
    const EOS_Ecom_GetEntitlementsByNameCountOptions* Options
) {
    IMPLEMENT(EOS_Ecom_GetEntitlementsByNameCount)(Handle, Options);
}

DLL_EXPORT(EOS_EResult) EOS_Ecom_CopyEntitlementByNameAndIndex(
    EOS_EcomHandle* const Handle,
    const EOS_Ecom_CopyEntitlementByNameAndIndexOptions* Options,
    EOS_Ecom_Entitlement** OutEntitlement
) {
    IMPLEMENT(EOS_Ecom_CopyEntitlementByNameAndIndex)(Handle, Options, OutEntitlement);
}

DLL_EXPORT(EOS_EResult) EOS_Ecom_CopyItemById(
    EOS_EcomHandle* const Handle,
    const EOS_Ecom_CopyItemByIdOptions* Options,
    EOS_Ecom_CatalogItem** OutItem
) {
    IMPLEMENT(EOS_Ecom_CopyItemById)(Handle, Options, OutItem);
}

DLL_EXPORT(uint32_t) EOS_Ecom_GetItemReleaseCount(
    EOS_EcomHandle* const Handle,
    const EOS_Ecom_GetItemReleaseCountOptions* Options
) {
    IMPLEMENT(EOS_Ecom_GetItemReleaseCount)(Handle, Options);
}
