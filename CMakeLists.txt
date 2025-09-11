cmake_minimum_required(VERSION 3.24)

project(ScreamAPI VERSION 4.0.0)

include(KoalaBox/cmake/KoalaBox.cmake)

add_subdirectory(KoalaBox)

set(
    SCREAM_API_SOURCES
    src/eos/ecom_entitlements.cpp
    src/eos/ecom_items.cpp
    src/eos/init.cpp
    src/eos/logging.cpp
    src/eos/metrics.cpp
    src/eos/stubs.cpp
    src/scream_api/api.cpp
    src/scream_api/api.hpp
    src/scream_api/config.cpp
    src/scream_api/config.hpp
    src/scream_api/scream_api.cpp
    src/scream_api/scream_api.hpp
    src/legacy_linker_exports.h
    src/main.cpp
)

add_library(${PROJECT_NAME} SHARED ${SCREAM_API_SOURCES} ${VERSION_RESOURCE})
target_link_libraries(${PROJECT_NAME} PRIVATE KoalaBox)

target_compile_definitions(${PROJECT_NAME} PUBLIC EOS_BUILDING_SDK=1)

configure_version_resource(
    TARGET ScreamAPI
    FILE_DESC "Epic Online Services DLC Unlocker"
    ORIG_NAME ScreamAPI
)

set_32_and_64(SCREAMAPI_DLL ScreamAPI32 ScreamAPI64)
set_32_and_64(EOSSDK_DLL EOSSDK-Win32-Shipping EOSSDK-Win64-Shipping)

configure_output_name(${SCREAMAPI_DLL})
configure_build_config(extra_build_config)
configure_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/res/eossdk/headers)

configure_linker_exports(
    TARGET ${PROJECT_NAME}
    HEADER_NAME "linker_exports_for_eossdk"
    FORWARDED_DLL "${EOSSDK_DLL}_o"
    INPUT_SOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/eos"
    DLL_FILES_GLOB "${CMAKE_CURRENT_SOURCE_DIR}/res/eossdk/binaries/${EOSSDK_DLL}.dll"
    UNDECORATE true
)

configure_linker_exports(
    TARGET ${PROJECT_NAME}
    HEADER_NAME "linker_exports_for_version"
    FORWARDED_DLL "C:/Windows/System32/version.dll"
    INPUT_SOURCES_DIR ""
    DLL_FILES_GLOB "C:/Windows/System32/version.dll"
)
