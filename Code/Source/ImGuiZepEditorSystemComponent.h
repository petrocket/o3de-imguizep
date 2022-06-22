/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#pragma once

#include <ImGuiZepSystemComponent.h>

#include <AzToolsFramework/Entity/EditorEntityContextBus.h>

namespace ImGuiZep
{
    /// System component for ImGuiZep editor
    class ImGuiZepEditorSystemComponent
        : public ImGuiZepSystemComponent
        , private AzToolsFramework::EditorEvents::Bus::Handler
    {
        using BaseSystemComponent = ImGuiZepSystemComponent;
    public:
        AZ_COMPONENT(ImGuiZepEditorSystemComponent, "{12FBED37-593C-46F6-9759-2DB60F0E68A8}", BaseSystemComponent);
        static void Reflect(AZ::ReflectContext* context);

        ImGuiZepEditorSystemComponent();
        ~ImGuiZepEditorSystemComponent();

    private:
        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

        // AZ::Component
        void Activate() override;
        void Deactivate() override;
    };
} // namespace ImGuiZep
