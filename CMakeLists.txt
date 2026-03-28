cmake_minimum_required(VERSION 3.21)

project(
    rich-input-handlr
    VERSION 1.0.1
    DESCRIPTION "Because nobody types exactly Y or N. C++20 port."
    LANGUAGES CXX
)

# Options
option(RIH_BUILD_EXAMPLES "Build example programs" ON)
option(RIH_BUILD_TESTS    "Build tests"             ON)
option(RIH_INSTALL        "Generate install targets" ON)

# Header-only interface target
add_library(rich_input_handlr INTERFACE)
add_library(rih::rich_input_handlr ALIAS rich_input_handlr)

target_include_directories(rich_input_handlr
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(rich_input_handlr INTERFACE cxx_std_20)

# Examples
if(RIH_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Install / packaging (for vcpkg port and system installs)
if(RIH_INSTALL)
    include(GNUInstallDirs)
    include(CMakePackageConfigHelpers)

    install(
        TARGETS rich_input_handlr
        EXPORT  rich_input_handlrTargets
    )

    install(
        DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )

    install(
        EXPORT      rich_input_handlrTargets
        FILE        rich_input_handlrTargets.cmake
        NAMESPACE   rih::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/rich_input_handlr
    )

    configure_package_config_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/rich_input_handlrConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/rich_input_handlrConfig.cmake
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/rich_input_handlr
    )

    write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/rich_input_handlrConfigVersion.cmake
        VERSION       ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
    )

    install(
        FILES
            ${CMAKE_CURRENT_BINARY_DIR}/rich_input_handlrConfig.cmake
            ${CMAKE_CURRENT_BINARY_DIR}/rich_input_handlrConfigVersion.cmake
        DESTINATION
            ${CMAKE_INSTALL_LIBDIR}/cmake/rich_input_handlr
    )
endif()
