/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#include <AzCore/Memory/SystemAllocator.h>
#include <AzCore/Module/Module.h>
#include <ImGuiZepSystemComponent.h>

namespace ImGuiZep
{
    class ImGuiZepModuleInterface
        : public AZ::Module
    {
    public:
        AZ_RTTI(ImGuiZepModuleInterface, "{80B8D46F-80EB-4626-9181-23945654670A}", AZ::Module);
        AZ_CLASS_ALLOCATOR(ImGuiZepModuleInterface, AZ::SystemAllocator, 0);

        ImGuiZepModuleInterface()
        {
            // Push results of [MyComponent]::CreateDescriptor() into m_descriptors here.
            // Add ALL components descriptors associated with this gem to m_descriptors.
            // This will associate the AzTypeInfo information for the components with the the SerializeContext, BehaviorContext and EditContext.
            // This happens through the [MyComponent]::Reflect() function.
            m_descriptors.insert(m_descriptors.end(), {
                ImGuiZepSystemComponent::CreateDescriptor(),
                });
        }

        /**
         * Add required SystemComponents to the SystemEntity.
         */
        AZ::ComponentTypeList GetRequiredSystemComponents() const override
        {
            return AZ::ComponentTypeList{
                azrtti_typeid<ImGuiZepSystemComponent>(),
            };
        }
    };
}// namespace ImGuiZep
