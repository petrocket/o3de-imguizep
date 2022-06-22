/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#pragma once

#include <AzCore/Component/Component.h>
#include <AzCore/Component/TickBus.h>
#include <ImGuiZep/ImGuiZepBus.h>

#if defined(IMGUI_ENABLED)
#include <ImGuiBus.h>
#include <imgui/imgui.h>
#endif

namespace ImGuiZep
{
    class ImGuiZepSystemComponent
        : public AZ::Component
        , protected ImGuiZepRequestBus::Handler
#if defined(IMGUI_ENABLED)
        , public ImGui::ImGuiUpdateListenerBus::Handler
#endif
    {
    public:
        AZ_COMPONENT(ImGuiZepSystemComponent, "{5D8B0B9B-F3A8-4E92-81DA-937DBF6B9D17}");

        static void Reflect(AZ::ReflectContext* context);

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

        ImGuiZepSystemComponent();
        ~ImGuiZepSystemComponent();

#if defined(IMGUI_ENABLED)
        //! ImGuiUpdateListenerBus
        void OnImGuiUpdate() override;
        void OnImGuiMainMenuUpdate() override;
#endif

    protected:

        //! AZ::Component interface implementation
        void Init() override;
        void Activate() override;
        void Deactivate() override;

    private:
        void InitZep();
        bool m_zepInitialized = false;
    };

} // namespace ImGuiZep
