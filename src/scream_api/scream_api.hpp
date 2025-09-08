#pragma once

#include <string>

#include <eos_base.h>

// ReSharper disable CppUnusedIncludeDirective
#include <koalabox/hook.hpp>
#include <koalabox/loader.hpp>
#include <koalabox/win.hpp>
// ReSharper restore CppUnusedIncludeDirective

#define DLL_EXPORT(TYPE) EOS_DECLARE_FUNC(TYPE)

/** Second argument can be used to add `return` or `const auto result = ` */
#define CALL_ORIGINAL_FN(FUNC, ...) \
    static const auto $decorated_func = koalabox::loader::get_decorated_function(scream_api::eossdk_handle, #FUNC); \
    static const auto FUNC##$ = scream_api::hook_mode \
        ? koalabox::hook::get_hooked_function($decorated_func, FUNC) \
        : koalabox::win::get_proc(scream_api::eossdk_handle, $decorated_func.c_str(), FUNC); \
    __VA_ARGS__ FUNC##$

namespace scream_api {
    extern std::string namespace_id;

    extern HMODULE eossdk_handle;

    extern bool hook_mode;

    void init(HMODULE module_handle);

    void shutdown();
}
