/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#include <AzCore/Serialization/SerializeContext.h>
#include <ImGuiZepEditorSystemComponent.h>

namespace ImGuiZep
{
    void ImGuiZepEditorSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<ImGuiZepEditorSystemComponent, ImGuiZepSystemComponent>()
                ->Version(0);
        }
    }

    ImGuiZepEditorSystemComponent::ImGuiZepEditorSystemComponent() = default;

    ImGuiZepEditorSystemComponent::~ImGuiZepEditorSystemComponent() = default;

    void ImGuiZepEditorSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        BaseSystemComponent::GetProvidedServices(provided);
        provided.push_back(AZ_CRC_CE("ImGuiZepEditorService"));
    }

    void ImGuiZepEditorSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        BaseSystemComponent::GetIncompatibleServices(incompatible);
        incompatible.push_back(AZ_CRC_CE("ImGuiZepEditorService"));
    }

    void ImGuiZepEditorSystemComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        BaseSystemComponent::GetRequiredServices(required);
    }

    void ImGuiZepEditorSystemComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
        BaseSystemComponent::GetDependentServices(dependent);
    }

    void ImGuiZepEditorSystemComponent::Activate()
    {
        ImGuiZepSystemComponent::Activate();
        AzToolsFramework::EditorEvents::Bus::Handler::BusConnect();
    }

    void ImGuiZepEditorSystemComponent::Deactivate()
    {
        AzToolsFramework::EditorEvents::Bus::Handler::BusDisconnect();
        ImGuiZepSystemComponent::Deactivate();
    }

} // namespace ImGuiZep
