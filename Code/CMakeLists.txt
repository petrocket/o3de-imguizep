#
# Copyright (c) Contributors to the Open 3D Engine Project.
# For complete copyright and license terms please see the LICENSE at the root of this distribution.
#
# SPDX-License-Identifier: Apache-2.0 OR MIT
#
#

# Currently we are in the Code folder: ${CMAKE_CURRENT_LIST_DIR}
# Get the platform specific folder ${pal_dir} for the current folder: ${CMAKE_CURRENT_LIST_DIR}/Platform/${PAL_PLATFORM_NAME}
# Note: o3de_pal_dir will take care of the details for us, as this may be a restricted platform
#       in which case it will see if that platform is present here or in the restricted folder.
#       i.e. It could here in our gem : Gems/ImGuiZep/Code/Platform/<platorm_name>  or
#            <restricted_folder>/<platform_name>/Gems/ImGuiZep/Code
o3de_pal_dir(pal_dir ${CMAKE_CURRENT_LIST_DIR}/Platform/${PAL_PLATFORM_NAME} "${gem_restricted_path}" "${gem_path}" "${gem_parent_relative_path}")

# Now that we have the platform abstraction layer (PAL) folder for this folder, thats where we will find the
# traits for this platform. Traits for a platform are defines for things like whether or not something in this gem
# is supported by this platform.
include(${pal_dir}/PAL_${PAL_PLATFORM_NAME_LOWERCASE}.cmake)

# Add Zep dependency using FetchContent
include(FetchContent)
FetchContent_Declare(
        Zep
        GIT_REPOSITORY "https://github.com/Rezonality/zep"
        GIT_TAG "46a783d6bc14f2dad2660c98ffcfddec5500b085" # master branch
        GIT_SHALLOW ON
        PATCH_COMMAND git checkout . && git apply --ignore-space-change --ignore-whitespace ${CMAKE_CURRENT_SOURCE_DIR}/zep.patch 
)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(BUILD_IMGUI OFF)
set(BUILD_TESTS OFF)
set(BUILD_DEMOS OFF)
set(ZEP_FEATURE_CPP_FILE_SYSTEM ON)
set(ZEP_SINGLE_HEADER_BUILD OFF)
get_property(
    compile_options
    DIRECTORY
    PROPERTY COMPILE_OPTIONS
)
include(${pal_dir}/PAL_zep_${PAL_PLATFORM_NAME_LOWERCASE}.cmake)
FetchContent_MakeAvailable(Zep)
# Restore compile options
set_property(
    DIRECTORY
    PROPERTY COMPILE_OPTIONS ${compile_options}
)
unset(compile_options)

# get_property(imguizep_gem_root GLOBAL PROPERTY "@GEMROOT:ImGuiZep@")
# ly_get_engine_relative_source_dir(${imguizep_gem_root} relative_imguizep_gem_root)
# set_property(TARGET Zep PROPERTY FOLDER "${relative_imguizep_gem_root}/External")
set_property(TARGET Zep PROPERTY FOLDER "External")

set_property(DIRECTORY APPEND PROPERTY O3DE_SUBDIRECTORY_INSTALL_CODE [[
    add_library(Zep::Zep STATIC IMPORTED GLOBAL)

    set_target_properties(Zep::Zep} 
        PROPERTIES 
            IMPORTED_LOCATION 
                "${LY_ROOT_FOLDER}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}Zep${CMAKE_STATIC_LIBRARY_SUFFIX}")

    ly_target_include_system_directories(TARGET Zep::Zep
        INTERFACE 
            "${LY_ROOT_FOLDER}/include/zep")
]]
)

ly_add_target(
    NAME ImGuiZep.API INTERFACE
    NAMESPACE Gem
    FILES_CMAKE
        imguizep_api_files.cmake
        ${pal_dir}/imguizep_api_files.cmake
    INCLUDE_DIRECTORIES
        INTERFACE
            Include
    BUILD_DEPENDENCIES
        INTERFACE
           AZ::AzCore
)

ly_add_target(
    NAME ImGuiZep.Private.Object STATIC
    NAMESPACE Gem
    FILES_CMAKE
        imguizep_private_files.cmake
        ${pal_dir}/imguizep_private_files.cmake
    TARGET_PROPERTIES
        O3DE_PRIVATE_TARGET TRUE
    COMPILE_DEFINITIONS
        PRIVATE
            IMGUIZEP_ROOT ${CMAKE_CURRENT_LIST_DIR}
    INCLUDE_DIRECTORIES
        PRIVATE
            Include
                ${Zep_SOURCE_DIR}/include
            Source
                ${Zep_SOURCE_DIR}/src
    BUILD_DEPENDENCIES
        PUBLIC
            Zep::Zep
            AZ::AzCore
            AZ::AzFramework
            Gem::ImGui.imguilib
            Gem::ImGui.Static
)

ly_add_target(
    NAME ImGuiZep ${PAL_TRAIT_MONOLITHIC_DRIVEN_MODULE_TYPE}
    NAMESPACE Gem
    FILES_CMAKE
        imguizep_shared_files.cmake
        ${pal_dir}/imguizep_shared_files.cmake
    INCLUDE_DIRECTORIES
        PUBLIC
            Include
        PRIVATE
            Source
    BUILD_DEPENDENCIES
        PUBLIC
            Gem::ImGuiZep.API
        PRIVATE
            Gem::ImGuiZep.Private.Object
)

# By default, we will specify that the above target ImGuiZep would be used by
# Client and Server type targets when this gem is enabled.  If you don't want it
# active in Clients or Servers by default, delete one of both of the following lines:
ly_create_alias(NAME ImGuiZep.Clients NAMESPACE Gem TARGETS Gem::ImGuiZep)
ly_create_alias(NAME ImGuiZep.Servers NAMESPACE Gem TARGETS Gem::ImGuiZep)

# For the Client and Server variants of ImGuiZep Gem, an alias to the ImGuiZep.API target will be made
ly_create_alias(NAME ImGuiZep.Clients.API NAMESPACE Gem TARGETS Gem::ImGuiZep.API)
ly_create_alias(NAME ImGuiZep.Servers.API NAMESPACE Gem TARGETS Gem::ImGuiZep.API)

# If we are on a host platform, we want to add the host tools targets like the ImGuiZep.Editor MODULE target
if(PAL_TRAIT_BUILD_HOST_TOOLS)
    # The ImGuiZep.Editor.API target can be used by other gems that want to interact with the ImGuiZep.Editor module
    ly_add_target(
        NAME ImGuiZep.Editor.API INTERFACE
        NAMESPACE Gem
        FILES_CMAKE
            imguizep_editor_api_files.cmake
            ${pal_dir}/imguizep_editor_api_files.cmake
        INCLUDE_DIRECTORIES
            INTERFACE
                Include
        BUILD_DEPENDENCIES
            INTERFACE
                AZ::AzToolsFramework
    )

    # The ImGuiZep.Editor.Private.Object target is an internal target
    # which is only to be used by this gems CMakeLists.txt and any subdirectories
    # Other gems should not use this target
    ly_add_target(
        NAME ImGuiZep.Editor.Private.Object STATIC
        NAMESPACE Gem
        FILES_CMAKE
            imguizep_editor_private_files.cmake
        TARGET_PROPERTIES
            O3DE_PRIVATE_TARGET TRUE
        INCLUDE_DIRECTORIES
            PRIVATE
                Include
                Source
        BUILD_DEPENDENCIES
            PUBLIC
                AZ::AzToolsFramework
                Gem::ImGui.imguilib
                Gem::ImGui.Static
                Gem::ImGui.Editor
                Zep::Zep
                $<TARGET_OBJECTS:Gem::ImGuiZep.Private.Object>
    )

    ly_add_target(
        NAME ImGuiZep.Editor GEM_MODULE
        NAMESPACE Gem
        AUTOMOC
        FILES_CMAKE
            imguizep_editor_shared_files.cmake
        INCLUDE_DIRECTORIES
            PRIVATE
                Source
            PUBLIC
                Include
        BUILD_DEPENDENCIES
            PUBLIC
                Gem::ImGuiZep.Editor.API
            PRIVATE
                Gem::ImGuiZep.Editor.Private.Object
    )

    # By default, we will specify that the above target ImGuiZep would be used by
    # Tool and Builder type targets when this gem is enabled.  If you don't want it
    # active in Tools or Builders by default, delete one of both of the following lines:
    ly_create_alias(NAME ImGuiZep.Tools    NAMESPACE Gem TARGETS Gem::ImGuiZep.Editor)
    ly_create_alias(NAME ImGuiZep.Builders NAMESPACE Gem TARGETS Gem::ImGuiZep.Editor)

    # For the Tools and Builders variants of ImGuiZep Gem, an alias to the ImGuiZep.Editor API target will be made
    ly_create_alias(NAME ImGuiZep.Tools.API NAMESPACE Gem TARGETS Gem::ImGuiZep.Editor.API)
    ly_create_alias(NAME ImGuiZep.Builders.API NAMESPACE Gem TARGETS Gem::ImGuiZep.Editor.API)

endif()

################################################################################
# Tests
################################################################################
# See if globally, tests are supported
if(PAL_TRAIT_BUILD_TESTS_SUPPORTED)
    # We globally support tests, see if we support tests on this platform for ImGuiZep.Tests
    if(PAL_TRAIT_IMGUIZEP_TEST_SUPPORTED)
        # We support ImGuiZep.Tests on this platform, add dependency on the Private Object target
        ly_add_target(
            NAME ImGuiZep.Tests ${PAL_TRAIT_TEST_TARGET_TYPE}
            NAMESPACE Gem
            FILES_CMAKE
                imguizep_tests_files.cmake
            INCLUDE_DIRECTORIES
                PRIVATE
                    Tests
                    Source
            BUILD_DEPENDENCIES
                PRIVATE
                    AZ::AzTest
                    AZ::AzFramework
                    Gem::ImGuiZep.Private.Object
        )

        # Add ImGuiZep.Tests to googletest
        ly_add_googletest(
            NAME Gem::ImGuiZep.Tests
        )
    endif()

    # If we are a host platform we want to add tools test like editor tests here
    if(PAL_TRAIT_BUILD_HOST_TOOLS)
        # We are a host platform, see if Editor tests are supported on this platform
        if(PAL_TRAIT_IMGUIZEP_EDITOR_TEST_SUPPORTED)
            # We support ImGuiZep.Editor.Tests on this platform, add ImGuiZep.Editor.Tests target which depends on
            # private ImGuiZep.Editor.Private.Object target
            ly_add_target(
                NAME ImGuiZep.Editor.Tests ${PAL_TRAIT_TEST_TARGET_TYPE}
                NAMESPACE Gem
                FILES_CMAKE
                    imguizep_editor_tests_files.cmake
                INCLUDE_DIRECTORIES
                    PRIVATE
                        Tests
                        Source
                BUILD_DEPENDENCIES
                    PRIVATE
                        AZ::AzTest
                        Gem::ImGuiZep.Private.Object
            )

            # Add ImGuiZep.Editor.Tests to googletest
            ly_add_googletest(
                NAME Gem::ImGuiZep.Editor.Tests
            )
        endif()
    endif()
endif()
