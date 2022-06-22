/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#include <ImGuiZepSystemComponent.h>

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Serialization/EditContextConstants.inl>
#include <AzCore/IO/FileIO.h>
#include <AzCore/Utils/Utils.h>
#include <AzCore/IO/Path/Path.h>

#include <AzFramework/Gem/GemInfo.h>

#ifdef IMGUI_ENABLED
#include <zep.h>
#include <zep/editor.h>
#include <functional>
#include <filesystem>
//#include "config_app.h"
#endif

namespace ImGuiZep
{

#ifdef IMGUI_ENABLED
    namespace fs = std::filesystem;

    using namespace Zep;

    using cmdFunc = std::function<void(const std::vector<std::string>&)>;
    class ZepCmd : public ZepExCommand
    {
    public:
        ZepCmd(ZepEditor& editor, const std::string name, cmdFunc fn)
            : ZepExCommand(editor)
            , m_name(name)
            , m_func(fn)
        {
        }

        virtual void Run(const std::vector<std::string>& args) override
        {
            m_func(args);
        }

        virtual const char* ExCommandName() const override
        {
            return m_name.c_str();
        }

    private:
        std::string m_name;
        cmdFunc m_func;
    };

    struct ZepWrapper : public Zep::IZepComponent
    {
        ZepWrapper(const fs::path& root_path, const Zep::NVec2f& pixelScale, std::function<void(std::shared_ptr<Zep::ZepMessage>)> fnCommandCB)
            : zepEditor(Zep::ZepPath(root_path.string()), pixelScale)
            , Callback(fnCommandCB)
        {
            zepEditor.RegisterCallback(this);

        }

        virtual Zep::ZepEditor& GetEditor() const override
        {
            return (Zep::ZepEditor&)zepEditor;
        }

        virtual void Notify(std::shared_ptr<Zep::ZepMessage> message) override
        {
            Callback(message);

            return;
        }

        virtual void HandleInput()
        {
            zepEditor.HandleInput();
        }

        Zep::ZepEditor_ImGui zepEditor;
        std::function<void(std::shared_ptr<Zep::ZepMessage>)> Callback;
    };

    std::shared_ptr<ZepWrapper> spZep;

    AZ::IO::Path GetGemAssetsPath(const AZStd::string& gemName)
    {
        if (auto settingsRegistry = AZ::Interface<AZ::SettingsRegistryInterface>::Get(); settingsRegistry != nullptr)
        {
            AZ::IO::Path gemSourceAssetDirectories;
            AZStd::vector<AzFramework::GemInfo> gemInfos;
            if (AzFramework::GetGemsInfo(gemInfos, *settingsRegistry))
            {
                auto FindGemByName = [gemName](const AzFramework::GemInfo& gemInfo)
                {
                    return gemInfo.m_gemName == gemName;
                };

                // Gather unique list of Gem Paths from the Settings Registry
                auto foundIt = AZStd::find_if(gemInfos.begin(), gemInfos.end(), FindGemByName);
                if (foundIt != gemInfos.end())
                {
                    const AzFramework::GemInfo& gemInfo = *foundIt;
                    for (const AZ::IO::Path& absoluteSourcePath : gemInfo.m_absoluteSourcePaths)
                    {
                        gemSourceAssetDirectories = (absoluteSourcePath / gemInfo.GetGemAssetFolder());
                    }

                    return gemSourceAssetDirectories;
                }
            }
        }
        return AZ::IO::Path();
    }

    void zep_init(const Zep::NVec2f& pixelScale)
    {
        // Initialize the editor and watch for changes
        auto gemPath = GetGemAssetsPath("ImGuiZep");
        spZep = std::make_shared<ZepWrapper>(std::filesystem::path(gemPath.c_str()), Zep::NVec2f(pixelScale.x, pixelScale.y), [](std::shared_ptr<ZepMessage> spMessage) -> void {
            });

        auto& display = spZep->GetEditor().GetDisplay();
        auto pImFont = ImGui::GetIO().Fonts[0].Fonts[0];
        auto pixelHeight = pImFont->FontSize;
        display.SetFont(ZepTextType::UI, std::make_shared<ZepFont_ImGui>(display, pImFont, int(pixelHeight)));
        display.SetFont(ZepTextType::Text, std::make_shared<ZepFont_ImGui>(display, pImFont, int(pixelHeight)));
        display.SetFont(ZepTextType::Heading1, std::make_shared<ZepFont_ImGui>(display, pImFont, int(pixelHeight * 1.5)));
        display.SetFont(ZepTextType::Heading2, std::make_shared<ZepFont_ImGui>(display, pImFont, int(pixelHeight * 1.25)));
        display.SetFont(ZepTextType::Heading3, std::make_shared<ZepFont_ImGui>(display, pImFont, int(pixelHeight * 1.125)));
    }

    void zep_update()
    {
        if (spZep)
        {
            spZep->GetEditor().RefreshRequired();
        }
    }

    void zep_destroy()
    {
        spZep.reset();
    }

    ZepEditor& zep_get_editor()
    {
        return spZep->GetEditor();
    }

    void zep_load(const Zep::ZepPath& file)
    {
        //auto pBuffer = zep_get_editor().InitWithFileOrDir(file);
        zep_get_editor().InitWithFileOrDir(file);
    }

    void zep_show(const Zep::NVec2i& displaySize)
    {
        bool show = true;
        ImGui::SetNextWindowSize(ImVec2(static_cast<float>(displaySize.x), static_cast<float>(displaySize.y)), ImGuiCond_FirstUseEver);
        if (!ImGui::Begin("Zep", &show, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_MenuBar))
        {
            ImGui::End();
            return;
        }

        auto min = ImGui::GetCursorScreenPos();
        auto max = ImGui::GetContentRegionAvail();
        if (max.x <= 0)
            max.x = 1;
        if (max.y <= 0)
            max.y = 1;
        ImGui::InvisibleButton("ZepContainer", max);

        // Fill the window
        max.x = min.x + max.x;
        max.y = min.y + max.y;

        spZep->zepEditor.SetDisplayRegion(Zep::NVec2f(min.x, min.y), Zep::NVec2f(max.x, max.y));
        spZep->zepEditor.Display();
        bool zep_focused = ImGui::IsWindowFocused();
        if (zep_focused)
        {
            spZep->zepEditor.HandleInput();
        }

        // TODO: A Better solution for this; I think the audio graph is creating a new window and stealing focus
        static int focus_count = 0;
        if (focus_count++ < 2)
        {
            ImGui::SetWindowFocus();
        }
        ImGui::End();
    }
#endif

    void ImGuiZepSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (AZ::SerializeContext* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<ImGuiZepSystemComponent, AZ::Component>()
                ->Version(0)
                ;

            if (AZ::EditContext* ec = serialize->GetEditContext())
            {
                ec->Class<ImGuiZepSystemComponent>("ImGuiZep", "[Description of functionality provided by this System Component]")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                        ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("System"))
                        ->Attribute(AZ::Edit::Attributes::AutoExpand, true)
                    ;
            }
        }
    }

    void ImGuiZepSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("ImGuiZepService"));
    }

    void ImGuiZepSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC_CE("ImGuiZepService"));
    }

    void ImGuiZepSystemComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
    }

    void ImGuiZepSystemComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
    }

    ImGuiZepSystemComponent::ImGuiZepSystemComponent()
    {
        if (ImGuiZepInterface::Get() == nullptr)
        {
            ImGuiZepInterface::Register(this);
        }
    }

    ImGuiZepSystemComponent::~ImGuiZepSystemComponent()
    {
        if (ImGuiZepInterface::Get() == this)
        {
            ImGuiZepInterface::Unregister(this);
        }
    }

    void ImGuiZepSystemComponent::Init()
    {
    }

    void ImGuiZepSystemComponent::Activate()
    {
        ImGuiZepRequestBus::Handler::BusConnect();
#ifdef IMGUI_ENABLED
        ImGui::ImGuiUpdateListenerBus::Handler::BusConnect();
#endif // IMGUI_ENABLED
    }

    void ImGuiZepSystemComponent::Deactivate()
    {
        ImGuiZepRequestBus::Handler::BusDisconnect();
#ifdef IMGUI_ENABLED
        ImGui::ImGuiUpdateListenerBus::Handler::BusDisconnect();

        if (m_zepInitialized)
        {
            zep_destroy();
        }
#endif // IMGUI_ENABLED
    }

#if defined(IMGUI_ENABLED)
    void ImGuiZepSystemComponent::InitZep()
    {
        zep_init({ 1.0f, 1.0f });

        auto gemPath = GetGemAssetsPath("ImGuiZep");
        auto filePath = AZ::IO::Path(gemPath) / "ZepExample.lua";
        zep_load(filePath.c_str());
        m_zepInitialized = true;
    }

    void ImGuiZepSystemComponent::OnImGuiUpdate()
    {
        if (!m_zepInitialized)
        {
            InitZep();
        }
        zep_update();
        zep_show({ 640, 480 });
    }

    void ImGuiZepSystemComponent::OnImGuiMainMenuUpdate()
    {
        if (!m_zepInitialized)
        {
            InitZep();
        }

        if (ImGui::BeginMenu("Zep"))
        {
            ImGui::EndMenu();
        }

    }
#endif

} // namespace ImGuiZep
