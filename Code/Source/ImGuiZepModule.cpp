/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */


#include <ImGuiZepModuleInterface.h>
#include <ImGuiZepSystemComponent.h>

namespace ImGuiZep
{
    class ImGuiZepModule
        : public ImGuiZepModuleInterface
    {
    public:
        AZ_RTTI(ImGuiZepModule, "{F093EADD-DD7A-428A-8070-A77B598C4215}", ImGuiZepModuleInterface);
        AZ_CLASS_ALLOCATOR(ImGuiZepModule, AZ::SystemAllocator, 0);
    };
}// namespace ImGuiZep

AZ_DECLARE_MODULE_CLASS(Gem_ImGuiZep, ImGuiZep::ImGuiZepModule)
