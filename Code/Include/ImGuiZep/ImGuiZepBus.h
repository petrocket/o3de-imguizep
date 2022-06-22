/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#pragma once

#include <AzCore/EBus/EBus.h>
#include <AzCore/Interface/Interface.h>

namespace ImGuiZep
{
    class ImGuiZepRequests
    {
    public:
        AZ_RTTI(ImGuiZepRequests, "{FAB30BB1-428F-4B9D-A82C-BE6CDCD81E5D}");
        virtual ~ImGuiZepRequests() = default;
        // Put your public methods here
    };
    
    class ImGuiZepBusTraits
        : public AZ::EBusTraits
    {
    public:
        //////////////////////////////////////////////////////////////////////////
        // EBusTraits overrides
        static constexpr AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
        static constexpr AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;
        //////////////////////////////////////////////////////////////////////////
    };

    using ImGuiZepRequestBus = AZ::EBus<ImGuiZepRequests, ImGuiZepBusTraits>;
    using ImGuiZepInterface = AZ::Interface<ImGuiZepRequests>;

} // namespace ImGuiZep
