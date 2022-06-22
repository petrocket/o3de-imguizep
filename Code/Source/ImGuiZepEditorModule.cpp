/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#include <ImGuiZepModuleInterface.h>
#include <ImGuiZepEditorSystemComponent.h>

namespace ImGuiZep
{
    class ImGuiZepEditorModule
        : public ImGuiZepModuleInterface
    {
    public:
        AZ_RTTI(ImGuiZepEditorModule, "{F093EADD-DD7A-428A-8070-A77B598C4215}", ImGuiZepModuleInterface);
        AZ_CLASS_ALLOCATOR(ImGuiZepEditorModule, AZ::SystemAllocator, 0);

        ImGuiZepEditorModule()
        {
            // Push results of [MyComponent]::CreateDescriptor() into m_descriptors here.
            // Add ALL components descriptors associated with this gem to m_descriptors.
            // This will associate the AzTypeInfo information for the components with the the SerializeContext, BehaviorContext and EditContext.
            // This happens through the [MyComponent]::Reflect() function.
            m_descriptors.insert(m_descriptors.end(), {
                ImGuiZepEditorSystemComponent::CreateDescriptor(),
            });
        }

        /**
         * Add required SystemComponents to the SystemEntity.
         * Non-SystemComponents should not be added here
         */
        AZ::ComponentTypeList GetRequiredSystemComponents() const override
        {
            return AZ::ComponentTypeList {
                azrtti_typeid<ImGuiZepEditorSystemComponent>(),
            };
        }
    };
}// namespace ImGuiZep

AZ_DECLARE_MODULE_CLASS(Gem_ImGuiZep, ImGuiZep::ImGuiZepEditorModule)
