﻿#include "pch.h"

namespace CBP
{
    const std::unordered_map<MiscHelpText, const char*> UIBase::m_helpText =
    {
        {MiscHelpText::timeStep, "Update rate in Hz. Higher values produce smoother motion but cost more CPU time. It's not recommended to set this below 60."},
        {MiscHelpText::timeScale, "Simulation rate, speeds up or slows down time"},
        {MiscHelpText::colMaxPenetrationDepth, "Maximum penetration depth during collisions"},
        {MiscHelpText::showAllActors, "Checked: Show all known actors\nUnchecked: Only show actors currently simulated"},
        {MiscHelpText::clampValues, "Clamp slider values to the default range."},
        {MiscHelpText::syncMinMax, "Move weight 0/100 sliders together."},
        {MiscHelpText::rescanActors, "Scan for nearby actors and update list."},
        {MiscHelpText::resetConfOnActor, "Clear configuration for currently selected actor."},
        {MiscHelpText::resetConfOnRace, "Clear configuration for currently selected race."},
        {MiscHelpText::showEDIDs, "Show editor ID's instead of names."},
        {MiscHelpText::playableOnly, "Show playable races only."},
    };

    static const keyDesc_t comboKeyDesc({
        {0, "None"},
        {DIK_LSHIFT, "Left shift"},
        {DIK_RSHIFT, "Right shift"},
        {DIK_LCONTROL, "Left control"},
        {DIK_RCONTROL, "Right control"},
        {DIK_LALT, "Left alt"},
        {DIK_RALT, "Right alt"}
        });

    static const keyDesc_t keyDesc({
        {DIK_INSERT,"Insert"},
        {DIK_DELETE,"Delete"},
        {DIK_HOME,"Home"},
        {DIK_END,"End"},
        {DIK_PGUP,"Page up"},
        {DIK_PGDN,"Page down"},
        {DIK_BACKSPACE, "Backspace"},
        {DIK_RETURN,"Return"},
        {DIK_PAUSE, "Pause"},
        {DIK_CAPSLOCK, "Caps lock"},
        {DIK_LEFT, "Left"},
        {DIK_RIGHT, "Right"},
        {DIK_UP, "Up"},
        {DIK_DOWN, "Down"},
        {DIK_TAB, "Tab"},
        {DIK_F1, "F1"},
        {DIK_F2, "F2"},
        {DIK_F3, "F3"},
        {DIK_F4, "F4"},
        {DIK_F5, "F5"},
        {DIK_F6, "F6"},
        {DIK_F7, "F7"},
        {DIK_F8, "F8"},
        {DIK_F9, "F9"},
        {DIK_F10, "F10"},
        {DIK_F11, "F11"},
        {DIK_F12, "F12"},
        {DIK_F13, "F13"},
        {DIK_F14, "F14"},
        {DIK_F15, "F15"},
        {DIK_NUMPAD0, "Num 0"},
        {DIK_NUMPAD1, "Num 1"},
        {DIK_NUMPAD2, "Num 2"},
        {DIK_NUMPAD3, "Num 3"},
        {DIK_NUMPAD4, "Num 4"},
        {DIK_NUMPAD5, "Num 5"},
        {DIK_NUMPAD6, "Num 6"},
        {DIK_NUMPAD7, "Num 7"},
        {DIK_NUMPAD8, "Num 8"},
        {DIK_NUMPAD9, "Num 9"},
        {DIK_NUMPADSLASH, "Num /"},
        {DIK_NUMPADSTAR, "Num *"},
        {DIK_NUMPADMINUS, "Num -"},
        {DIK_NUMPADPLUS, "Num +"},
        {DIK_NUMPADENTER, "Num Enter"},
        {DIK_NUMPADCOMMA, "Num ,"},
        {DIK_PERIOD, "."},
        {DIK_COMMA, ","},
        {DIK_MINUS, "-"},
        {DIK_BACKSLASH, "\\"},
        {DIK_COLON, ":"},
        {DIK_SEMICOLON, ";"},
        {DIK_SLASH, "/"},
        {DIK_0,"0"},
        {DIK_1,"1"},
        {DIK_2,"2"},
        {DIK_3,"3"},
        {DIK_4,"4"},
        {DIK_5,"5"},
        {DIK_6,"6"},
        {DIK_7,"7"},
        {DIK_8,"8"},
        {DIK_9,"9"},
        {DIK_A,"A"},
        {DIK_B,"B"},
        {DIK_C,"C"},
        {DIK_D,"D"},
        {DIK_E,"E"},
        {DIK_F,"F"},
        {DIK_G,"G"},
        {DIK_H,"H"},
        {DIK_I,"I"},
        {DIK_J,"J"},
        {DIK_K,"K"},
        {DIK_L,"L"},
        {DIK_M,"M"},
        {DIK_N,"N"},
        {DIK_O,"O"},
        {DIK_P,"P"},
        {DIK_Q,"Q"},
        {DIK_R,"R"},
        {DIK_S,"S"},
        {DIK_T,"T"},
        {DIK_U,"U"},
        {DIK_V,"V"},
        {DIK_W,"W"},
        {DIK_X,"X"},
        {DIK_Y,"Y"},
        {DIK_Z,"Z"}
        });

    bool UIBase::CollapsingHeader(
        const std::string& a_key,
        const char* a_label,
        bool a_default) const
    {
        auto& globalConfig = IConfig::GetGlobalConfig();

        bool& state = globalConfig.GetColState(a_key, a_default);
        bool newState = ImGui::CollapsingHeader(a_label,
            state ? ImGuiTreeNodeFlags_DefaultOpen : 0);

        if (state != newState) {
            state = newState;
            DCBP::MarkGlobalsForSave();
        }

        return newState;
    }

    void UIBase::HelpMarker(MiscHelpText a_id) const
    {
        ImGui::SameLine();
        UICommon::HelpMarker(m_helpText.at(a_id));
    }

    template <class T>
    void UIProfileBase<T>::DrawCreateNew()
    {
        if (UICommon::TextInputDialog("New profile", "Enter the profile name:",
            state.new_input, sizeof(state.new_input)))
        {
            if (strlen(state.new_input))
            {
                T profile;

                auto& pm = GlobalProfileManager::GetSingleton<T>();

                if (pm.CreateProfile(state.new_input, profile))
                {
                    std::string name(profile.Name());
                    if (pm.AddProfile(std::move(profile))) {
                        state.selected = std::move(name);
                    }
                    else {
                        state.lastException = pm.GetLastException();
                        ImGui::OpenPopup("Add Error");
                    }
                }
                else {
                    state.lastException = pm.GetLastException();
                    ImGui::OpenPopup("Create Error");
                }
            }
        }

        UICommon::MessageDialog("Create Error", "Could not create the profile\n\n%s", state.lastException.what());
        UICommon::MessageDialog("Add Error", "Could not add the profile\n\n%s", state.lastException.what());

    }

    template <class T>
    UIProfileEditorBase<T>::UIProfileEditorBase(const char* a_name) :
        m_name(a_name)
    {
    }

    template <class T>
    void UIProfileEditorBase<T>::Draw(bool* a_active)
    {
        auto& io = ImGui::GetIO();

        ImGui::SetNextWindowPos(ImVec2(min(420.0f, io.DisplaySize.x - 40.0f), 20.0f), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(450.0f, io.DisplaySize.y), ImGuiCond_FirstUseEver);

        ImVec2 sizeMin(min(300.0f, io.DisplaySize.x - 40.0f), min(200.0f, io.DisplaySize.y - 40.0f));
        ImVec2 sizeMax(min(450.0f, io.DisplaySize.x), max(io.DisplaySize.y - 40.0f, sizeMin.y));

        ImGui::SetNextWindowSizeConstraints(sizeMin, sizeMax);

        ImGui::PushID(static_cast<const void*>(a_active));

        if (ImGui::Begin(m_name, a_active))
        {
            ImGui::PushItemWidth(ImGui::GetFontSize() * -15.0f);

            auto& data = GlobalProfileManager::GetSingleton<T>().Data();

            const char* curSelName = nullptr;
            if (state.selected) {
                if (data.contains(*state.selected))
                {
                    curSelName = state.selected->c_str();

                    if (!m_filter.Test(*state.selected))
                    {
                        for (const auto& e : data)
                        {
                            if (!m_filter.Test(e.first))
                                continue;

                            state.selected = e.first;
                            curSelName = e.first.c_str();

                            break;
                        }
                    }
                }
                else {
                    state.selected.Clear();
                }
            }
            else {
                if (data.size()) {
                    state.selected = data.begin()->first;
                    curSelName = (*state.selected).c_str();
                }
            }

            ImGui::PushItemWidth(ImGui::GetFontSize() * -9.0f);

            if (ImGui::BeginCombo("Profile", curSelName, ImGuiComboFlags_HeightLarge))
            {
                for (const auto& e : data)
                {
                    if (!m_filter.Test(e.first))
                        continue;

                    ImGui::PushID(reinterpret_cast<const void*>(std::addressof(e.second)));

                    bool selected = e.first == *state.selected;
                    if (selected)
                        if (ImGui::IsWindowAppearing()) ImGui::SetScrollHereY();

                    if (ImGui::Selectable(e.second.Name().c_str(), selected))
                        state.selected = e.first;

                    ImGui::PopID();
                }
                ImGui::EndCombo();
            }

            ImGui::SameLine();
            m_filter.DrawButton();

            ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 30.0f);
            if (ImGui::Button("New")) {
                ImGui::OpenPopup("New profile");
                state.new_input[0] = 0;
            }

            m_filter.Draw();

            ImGui::PopItemWidth();

            DrawCreateNew();

            if (state.selected)
            {
                auto& profile = data.at(*state.selected);

                ImGui::Spacing();

                if (ImGui::Button("Save")) {
                    if (!profile.Save()) {
                        ImGui::OpenPopup("Save");
                        state.lastException = profile.GetLastException();
                    }
                }

                ImGui::SameLine();
                if (ImGui::Button("Delete")) {
                    ImGui::OpenPopup("Delete");
                }

                ImGui::SameLine();
                if (ImGui::Button("Rename")) {
                    ImGui::OpenPopup("Rename");
                    _snprintf_s(ex_state.ren_input, _TRUNCATE, "%s", (*state.selected).c_str());
                }

                if (UICommon::ConfirmDialog(
                    "Delete",
                    "Are you sure you want to delete profile '%s'?\n\n", curSelName))
                {
                    auto& pm = GlobalProfileManager::GetSingleton<T>();
                    if (pm.DeleteProfile(*state.selected)) {
                        state.selected.Clear();
                    }
                    else {
                        state.lastException = pm.GetLastException();
                        ImGui::OpenPopup("Delete failed");
                    }
                }
                else if (UICommon::TextInputDialog("Rename", "Enter the new profile name:",
                    ex_state.ren_input, sizeof(ex_state.ren_input)))
                {
                    auto& pm = GlobalProfileManager::GetSingleton<T>();
                    std::string newName(ex_state.ren_input);

                    if (pm.RenameProfile(*state.selected, newName)) {
                        state.selected = newName;
                    }
                    else {
                        state.lastException = pm.GetLastException();
                        ImGui::OpenPopup("Rename failed");
                    }
                }
                else {

                    UICommon::MessageDialog("Save", "Saving profile '%s' to '%s' failed\n\n%s",
                        profile.Name().c_str(), profile.Path().string().c_str(), state.lastException.what());

                    ImGui::Separator();

                    DrawItem(profile);
                }

                UICommon::MessageDialog("Delete failed",
                    "Could not delete the profile\n\n%s", state.lastException.what());
                UICommon::MessageDialog("Rename failed",
                    "Could not rename the profile\n\n%s", state.lastException.what());
            }

            ImGui::PopItemWidth();
        }

        ImGui::End();

        ImGui::PopID();
    }

    void UIProfileEditorSim::DrawItem(SimProfile& a_profile) {
        DrawSimComponents(0, a_profile.Data());
    }

    void UIProfileEditorSim::OnSimSliderChange(
        int,
        SimProfile::base_type& a_data,
        SimProfile::base_type::value_type& a_pair,
        const componentValueDescMap_t::vec_value_type& a_desc,
        float* a_val)
    {
        auto& globalConfig = IConfig::GetGlobalConfig();

        if (globalConfig.ui.clampValuesMain)
            *a_val = std::clamp(*a_val, a_desc.second.min, a_desc.second.max);

        Propagate(a_data, nullptr, a_pair.first, a_desc.first, *a_val);

        if (a_desc.second.counterpart.size() && globalConfig.ui.syncWeightSlidersMain) {
            a_pair.second.Set(a_desc.second.counterpart, *a_val);
            Propagate(a_data, nullptr, a_pair.first, a_desc.second.counterpart, *a_val);
        }
    }

    void UIProfileEditorNode::DrawItem(NodeProfile& a_profile)
    {
        DrawNodes(0, a_profile.Data());
    }

    void UIProfileEditorNode::UpdateNodeData(
        int,
        const std::string&,
        const NodeProfile::base_type::mapped_type&)
    {
    }

    UIRaceEditor::UIRaceEditor() noexcept :
        m_currentRace(0),
        m_nextUpdateRaceList(true),
        m_changed(false),
        m_firstUpdate(false)
    {
    }

    void UIRaceEditor::Reset() {
        m_nextUpdateRaceList = true;
        m_changed = false;
        m_firstUpdate = false;
    }

    auto UIRaceEditor::GetSelectedEntry()
        -> raceListValue_t*
    {
        if (m_currentRace != 0) {
            auto it = m_raceList.find(m_currentRace);
            return std::addressof(*it);
        }

        if (m_raceList.size()) {
            auto it = m_raceList.begin();
            m_currentRace = it->first;
            return std::addressof(*it);
        }

        return nullptr;
    }

    void UIRaceEditor::Draw(bool* a_active)
    {
        if (m_nextUpdateRaceList) {
            m_nextUpdateRaceList = false;
            UpdateRaceList();
        }

        auto& io = ImGui::GetIO();

        ImGui::SetNextWindowPos(ImVec2(min(820.0f, io.DisplaySize.x - 40.0f), 20.0f), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(450.0f, io.DisplaySize.y), ImGuiCond_FirstUseEver);

        ImVec2 sizeMin(min(300.0f, io.DisplaySize.x - 40.0f), min(200.0f, io.DisplaySize.y - 40.0f));
        ImVec2 sizeMax(min(450.0f, io.DisplaySize.x), max(io.DisplaySize.y - 40.0f, sizeMin.y));

        ImGui::SetNextWindowSizeConstraints(sizeMin, sizeMax);

        ImGui::PushID(static_cast<const void*>(a_active));

        if (ImGui::Begin("Race Editor", a_active))
        {
            auto entry = GetSelectedEntry();

            const char* curSelName;

            if (entry) {
                if (!m_filter.Test(entry->second.first))
                {
                    m_currentRace = 0;
                    entry = nullptr;
                    curSelName = nullptr;

                    for (auto& e : m_raceList)
                    {
                        if (!m_filter.Test(e.second.first))
                            continue;

                        m_currentRace = e.first;
                        entry = std::addressof(e);
                        curSelName = e.second.first.c_str();

                        break;
                    }
                }
                else
                    curSelName = entry->second.first.c_str();
            }
            else
                curSelName = nullptr;

            ImGui::PushItemWidth(ImGui::GetFontSize() * -5.0f);

            if (ImGui::BeginCombo("Race", curSelName, ImGuiComboFlags_HeightLarge))
            {
                for (auto& e : m_raceList)
                {
                    if (!m_filter.Test(e.second.first))
                        continue;

                    ImGui::PushID(reinterpret_cast<const void*>(std::addressof(e.second)));

                    bool selected = e.first == m_currentRace;
                    if (selected)
                        if (ImGui::IsWindowAppearing()) ImGui::SetScrollHereY();

                    if (ImGui::Selectable(e.second.first.c_str(), selected)) {
                        m_currentRace = e.first;
                        entry = std::addressof(e);
                    }

                    ImGui::PopID();
                }

                ImGui::EndCombo();
            }

            ImGui::SameLine();
            m_filter.DrawButton();
            m_filter.Draw();

            ImGui::PopItemWidth();

            ImGui::Spacing();

            if (m_currentRace)
            {
                ImGui::PushItemWidth(ImGui::GetFontSize() * -15.0f);

                auto& rlEntry = IData::GetRaceListEntry(m_currentRace);
                ImGui::Text("Playable: %s", rlEntry.playable ? "yes" : "no");

                auto& globalConfig = IConfig::GetGlobalConfig();

                ImGui::Spacing();
                if (ImGui::Checkbox("Playable only", &globalConfig.ui.rlPlayableOnly)) {
                    QueueUpdateRaceList();
                    DCBP::MarkGlobalsForSave();
                }
                HelpMarker(MiscHelpText::playableOnly);

                ImGui::Spacing();
                if (ImGui::Checkbox("Editor IDs", &globalConfig.ui.rlShowEditorIDs)) {
                    QueueUpdateRaceList();
                    DCBP::MarkGlobalsForSave();
                }
                HelpMarker(MiscHelpText::showEDIDs);

                ImGui::Spacing();

                if (ImGui::Checkbox("Clamp values", &globalConfig.ui.clampValuesRace))
                    DCBP::MarkGlobalsForSave();
                HelpMarker(MiscHelpText::clampValues);

                ImGui::Spacing();
                if (ImGui::Checkbox("Sync min/max weight sliders", &globalConfig.ui.syncWeightSlidersRace))
                    DCBP::MarkGlobalsForSave();
                HelpMarker(MiscHelpText::syncMinMax);

                ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 43.0f);
                if (ImGui::Button("Reset"))
                    ImGui::OpenPopup("Reset");
                HelpMarker(MiscHelpText::resetConfOnRace);

                if (UICommon::ConfirmDialog(
                    "Reset",
                    "%s: clear all values for race?\n\n", curSelName))
                {
                    ResetAllRaceValues(m_currentRace, entry);
                }

                ImGui::Spacing();

                DrawProfileSelector(entry);

                ImGui::Separator();

                DrawSimComponents(m_currentRace, entry->second.second);

                ImGui::PopItemWidth();
            }
        }

        ImGui::End();

        ImGui::PopID();
    }

    void UIRaceEditor::UpdateRaceList()
    {
        bool isFirstUpdate = m_firstUpdate;

        m_firstUpdate = true;

        m_raceList.clear();

        auto& globalConfig = IConfig::GetGlobalConfig();

        for (const auto& e : IData::GetRaceList())
        {
            if (globalConfig.ui.rlPlayableOnly && !e.second.playable)
                continue;

            std::ostringstream ss;
            ss << "[" << std::uppercase << std::setfill('0') <<
                std::setw(8) << std::hex << e.first << "] ";

            if (globalConfig.ui.rlShowEditorIDs)
                ss << e.second.edid;
            else
                ss << e.second.fullname;

            m_raceList.try_emplace(e.first,
                std::move(ss.str()), IConfig::GetRaceConf(e.first));
        }

        if (m_raceList.size() == 0) {
            m_currentRace = 0;
            return;
        }

        if (globalConfig.ui.selectCrosshairActor && !isFirstUpdate) {
            auto crosshairRef = IData::GetCrosshairRef();
            if (crosshairRef)
            {
                auto& actorRaceMap = IData::GetActorRaceMap();
                auto it = actorRaceMap.find(crosshairRef);
                if (it != actorRaceMap.end()) {
                    if (m_raceList.contains(it->second)) {
                        m_currentRace = it->second;
                        return;
                    }
                }
            }
        }

        if (m_currentRace != 0)
            if (!m_raceList.contains(m_currentRace))
                m_currentRace = 0;
    }

    void UIRaceEditor::ResetAllRaceValues(SKSE::FormID a_formid, raceListValue_t* a_data)
    {
        IConfig::EraseRaceConf(a_formid);
        a_data->second.second = IConfig::GetGlobalProfile();
        MarkChanged();
        DCBP::UpdateConfigOnAllActors();
    }

    void UIRaceEditor::ApplyProfile(raceListValue_t* a_data, const SimProfile& a_profile)
    {
        IConfig::CopyComponents(a_profile.Data(), a_data->second.second);
        IConfig::SetRaceConf(a_data->first, a_data->second.second);
        MarkChanged();
        DCBP::UpdateConfigOnAllActors();
    }

    const configComponents_t& UIRaceEditor::GetData(const raceListValue_t* a_data) const
    {
        return a_data->second.second;
    }

    void UIRaceEditor::OnSimSliderChange(
        SKSE::FormID a_formid,
        configComponents_t& a_data,
        configComponentsValue_t& a_pair,
        const componentValueDescMap_t::vec_value_type& a_desc,
        float* a_val)
    {
        auto& globalConfig = IConfig::GetGlobalConfig();

        if (globalConfig.ui.clampValuesMain)
            *a_val = std::clamp(*a_val, a_desc.second.min, a_desc.second.max);

        auto& raceConf = IConfig::GetOrCreateRaceConf(a_formid);
        auto& entry = raceConf.at(a_pair.first);

        auto addr = reinterpret_cast<uintptr_t>(std::addressof(entry)) + a_desc.second.offset;
        *reinterpret_cast<float*>(addr) = *a_val;

        Propagate(a_data, std::addressof(raceConf), a_pair.first, a_desc.first, *a_val);

        if (a_desc.second.counterpart.size() &&
            globalConfig.ui.syncWeightSlidersRace)
        {
            a_pair.second.Set(a_desc.second.counterpart, *a_val);
            entry.Set(a_desc.second.counterpart, *a_val);
            Propagate(a_data, std::addressof(raceConf), a_pair.first, a_desc.second.counterpart, *a_val);
        }

        MarkChanged();
        DCBP::UpdateConfigOnAllActors();
    }

    template<class T, class P>
    void UIProfileSelector<T, P>::DrawProfileSelector(T* a_data)
    {
        auto& pm = GlobalProfileManager::GetSingleton<P>();
        auto& data = pm.Data();

        ImGui::PushID(std::addressof(pm));

        const char* curSelName = nullptr;
        if (state.selected) {
            if (data.contains(*state.selected))
                curSelName = state.selected->c_str();
            else
                state.selected.Clear();
        }

        if (ImGui::BeginCombo("Profile", curSelName, ImGuiComboFlags_HeightLarge))
        {
            for (const auto& e : data)
            {
                ImGui::PushID(reinterpret_cast<const void*>(std::addressof(e.second)));

                bool selected = state.selected &&
                    e.first == *state.selected;

                if (selected)
                    if (ImGui::IsWindowAppearing()) ImGui::SetScrollHereY();

                if (ImGui::Selectable(e.second.Name().c_str(), selected)) {
                    state.selected = e.first;
                }

                ImGui::PopID();
            }
            ImGui::EndCombo();
        }

        ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 30.0f);
        if (ImGui::Button("New")) {
            ImGui::OpenPopup("New profile");
            state.new_input[0] = 0;
        }

        DrawCreateNew();

        if (state.selected)
        {
            auto& profile = data.at(*state.selected);

            ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 77.0f);
            if (ImGui::Button("Apply")) {
                ImGui::OpenPopup("Apply from profile");
            }

            if (UICommon::ConfirmDialog(
                "Apply from profile",
                "Load data from profile '%s'?\n\nCurrent values will be lost.\n\n",
                profile.Name().c_str()))
            {
                ApplyProfile(a_data, profile);
            }

            ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 117.0f);
            if (ImGui::Button("Save")) {
                ImGui::OpenPopup("Save to profile");
            }

            if (UICommon::ConfirmDialog(
                "Save to profile",
                "Save current values to profile '%s'?\n\n",
                profile.Name().c_str()))
            {
                auto& data = GetData(a_data);
                if (!profile.Save(data, true)) {
                    state.lastException = profile.GetLastException();
                    ImGui::OpenPopup("Save to profile error");
                }
            }

            UICommon::MessageDialog("Save to profile error",
                "Error saving to profile '%s'\n\n%s", profile.Name().c_str(),
                state.lastException.what());
        }

        ImGui::PopID();
    }

    template<typename T>
    void UIApplyForce<T>::DrawForceSelector(T* a_data, configForceMap_t& a_forceData)
    {
        auto& globalConfig = IConfig::GetGlobalConfig();

        ImGui::PushID(static_cast<const void*>(std::addressof(m_forceState)));

        static const std::string chKey("Main#Force");

        if (CollapsingHeader(chKey, "Force"))
        {
            auto& data = GetData(a_data);
            auto& globalConfig = IConfig::GetGlobalConfig();

            const char* curSelName = nullptr;
            if (m_forceState.selected) {
                curSelName = (*m_forceState.selected).c_str();
            }
            else {
                if (globalConfig.ui.forceActorSelected.size()) {
                    auto it = data.find(globalConfig.ui.forceActorSelected);
                    if (it != data.end()) {
                        m_forceState.selected = it->first;
                        curSelName = it->first.c_str();
                    }
                }

                if (!m_forceState.selected) {
                    auto it = data.begin();
                    if (it != data.end()) {
                        m_forceState.selected = it->first;
                        curSelName = it->first.c_str();
                    }
                }
            }

            if (ImGui::BeginCombo("Component", curSelName))
            {
                for (const auto& e : data)
                {
                    ImGui::PushID(reinterpret_cast<const void*>(std::addressof(e.second)));

                    bool selected = m_forceState.selected &&
                        e.first == *m_forceState.selected;

                    if (selected)
                        if (ImGui::IsWindowAppearing()) ImGui::SetScrollHereY();

                    if (ImGui::Selectable(e.first.c_str(), selected)) {
                        m_forceState.selected = (
                            globalConfig.ui.forceActorSelected = e.first);
                        DCBP::MarkGlobalsForSave();
                    }

                    ImGui::PopID();
                }
                ImGui::EndCombo();
            }

            ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 43.0f);
            if (ImGui::Button("Apply"))
                for (const auto& e : globalConfig.ui.forceActor)
                    ApplyForce(a_data, e.second.steps, e.first, e.second.force);

            if (m_forceState.selected)
            {
                auto& e = a_forceData[*m_forceState.selected];

                ImGui::Spacing();

                if (ImGui::SliderFloat("X", std::addressof(e.force.x), FORCE_MIN, FORCE_MAX, "%.0f"))
                    DCBP::MarkGlobalsForSave();

                ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 43.0f);
                if (ImGui::Button("Reset")) {
                    e = configForce_t();
                    DCBP::MarkGlobalsForSave();
                }

                if (ImGui::SliderFloat("Y", std::addressof(e.force.y), FORCE_MIN, FORCE_MAX, "%.0f"))
                    DCBP::MarkGlobalsForSave();

                if (ImGui::SliderFloat("Z", std::addressof(e.force.z), FORCE_MIN, FORCE_MAX, "%.0f"))
                    DCBP::MarkGlobalsForSave();

                ImGui::Spacing();

                if (ImGui::SliderInt("Steps", std::addressof(e.steps), 0, 100)) {
                    e.steps = max(e.steps, 0);
                    DCBP::MarkGlobalsForSave();
                }
            }
        }

        ImGui::PopID();
    }

    template <typename T>
    UIActorList<T>::UIActorList() :
        m_currentActor(0),
        m_globLabel("Global"),
        m_lastCacheUpdateId(0),
        m_firstUpdate(false)
    {
        m_strBuf1[0] = 0x0;
    }

    template <typename T>
    void UIActorList<T>::UpdateActorList()
    {
        bool isFirstUpdate = m_firstUpdate;

        m_firstUpdate = true;

        auto& globalConfig = IConfig::GetGlobalConfig();

        m_actorList.clear();

        for (const auto& e : IData::GetActorCache())
        {
            if (!globalConfig.ui.showAllActors && !e.second.active)
                continue;

            m_actorList.try_emplace(e.first, e.second.name, GetData(e.first));
        }

        if (m_actorList.size() == 0) {
            _snprintf_s(m_strBuf1, _TRUNCATE, "No actors");
            SetCurrentActor(0);
            return;
        }

        _snprintf_s(m_strBuf1, _TRUNCATE, "%zu actors", m_actorList.size());

        if (globalConfig.ui.selectCrosshairActor && !isFirstUpdate) {
            auto crosshairRef = IData::GetCrosshairRef();
            if (crosshairRef) {
                if (m_actorList.contains(crosshairRef)) {
                    SetCurrentActor(crosshairRef);
                    return;
                }
            }
        }

        if (m_currentActor != 0) {
            if (!m_actorList.contains(m_currentActor))
                SetCurrentActor(0);
        }
        else {
            if (globalConfig.ui.lastActor &&
                m_actorList.contains(globalConfig.ui.lastActor))
            {
                m_currentActor = globalConfig.ui.lastActor;
            }
        }
    }

    template <typename T>
    void UIActorList<T>::ActorListTick()
    {
        auto cacheUpdateId = IData::GetActorCacheUpdateId();
        if (cacheUpdateId != m_lastCacheUpdateId) {
            m_lastCacheUpdateId = cacheUpdateId;
            UpdateActorList();
        }
    }

    template <typename T>
    void UIActorList<T>::ResetActorList()
    {
        m_firstUpdate = false;
        m_actorList.clear();
        m_lastCacheUpdateId = IData::GetActorCacheUpdateId() - 1;
    }

    template <typename T>
    void UIActorList<T>::SetCurrentActor(SKSE::ObjectHandle a_handle)
    {
        auto& globalConfig = IConfig::GetGlobalConfig();

        m_currentActor = a_handle;

        if (a_handle != 0)
            m_actorList.at(a_handle).second = GetData(a_handle);

        if (a_handle != globalConfig.ui.lastActor) {
            globalConfig.ui.lastActor = a_handle;
            DCBP::MarkGlobalsForSave();
        }
    }

    template <class T>
    auto UIActorList<T>::GetSelectedEntry()
        -> actorListValue_t*
    {
        if (m_currentActor != 0) {
            const auto it = m_actorList.find(m_currentActor);
            return std::addressof(*it);
        }

        return nullptr;
    }

    template <class T>
    void UIActorList<T>::DrawActorList(actorListValue_t*& a_entry, const char*& a_curSelName)
    {
        if (a_entry) {
            a_curSelName = a_entry->second.first.c_str();
        }
        else {
            a_curSelName = m_globLabel.c_str();
        }

        FilterSelected(a_entry, a_curSelName);

        ImGui::PushItemWidth(ImGui::GetFontSize() * -8.0f);

        if (ImGui::BeginCombo(m_strBuf1, a_curSelName, ImGuiComboFlags_HeightLarge))
        {
            if (m_filter.Test(m_globLabel))
            {
                ImGui::PushID(static_cast<const void*>(m_globLabel.c_str()));

                if (ImGui::Selectable(m_globLabel.c_str(), 0 == m_currentActor)) {
                    SetCurrentActor(0);
                    a_entry = nullptr;
                    a_curSelName = m_globLabel.c_str();
                }

                ImGui::PopID();
            }

            for (auto& e : m_actorList)
            {
                if (!m_filter.Test(e.second.first))
                    continue;

                ImGui::PushID(static_cast<const void*>(std::addressof(e.second)));

                bool selected = e.first == m_currentActor;
                if (selected)
                    if (ImGui::IsWindowAppearing()) ImGui::SetScrollHereY();

                if (ImGui::Selectable(e.second.first.c_str(), selected)) {
                    SetCurrentActor(e.first);
                    a_entry = std::addressof(e);
                    a_curSelName = e.second.first.c_str();
                }

                ImGui::PopID();
            }

            ImGui::EndCombo();
        }

        ImGui::SameLine();
        m_filter.DrawButton();
        m_filter.Draw();

        ImGui::PopItemWidth();
    }

    template <class T>
    void UIActorList<T>::FilterSelected(
        actorListValue_t*& a_entry,
        const char*& a_curSelName)
    {
        if (m_filter.Test(a_entry ?
            a_entry->second.first :
            m_globLabel))
        {
            return;
        }

        for (auto& e : m_actorList)
        {
            if (!m_filter.Test(e.second.first))
                continue;

            SetCurrentActor(e.first);
            a_entry = std::addressof(e);
            a_curSelName = e.second.first.c_str();

            return;
        }

        if (m_filter.Test(m_globLabel)) {
            SetCurrentActor(0);
            a_entry = nullptr;
            a_curSelName = m_globLabel.c_str();
        }
    }

    UIGenericFilter::UIGenericFilter() :
        m_searchOpen(false)
    {
        m_filterBuf[0] = 0x0;
    }

    void UIGenericFilter::DrawButton()
    {
        if (ImGui::Button(m_searchOpen ? "<" : ">"))
            m_searchOpen = !m_searchOpen;
    }

    void UIGenericFilter::Draw()
    {
        if (!m_searchOpen)
            return;

        ImGui::PushID(static_cast<const void*>(&m_searchOpen));

        if (ImGui::InputText("Filter", m_filterBuf, sizeof(m_filterBuf))) {
            if (strlen(m_filterBuf))
                m_filter = m_filterBuf;
            else
                m_filter.Clear();
        }

        ImGui::PopID();
    }

    void UIGenericFilter::Toggle()
    {
        m_searchOpen = !m_searchOpen;
    }

    bool UIGenericFilter::Test(const std::string& a_haystack) const
    {
        if (!m_filter)
            return true;

        auto it = std::search(
            a_haystack.begin(), a_haystack.end(),
            m_filter->begin(), m_filter->end(),
            [](char a_lhs, char a_rhs) {
                return std::tolower(a_lhs) ==
                    std::tolower(a_rhs);
            }
        );

        return (it != a_haystack.end());
    }

    void UIGenericFilter::Clear() {
        m_filter.Clear();
        m_filterBuf[0] = 0x0;
    }

    UIContext::UIContext() noexcept :
        m_nextUpdateCurrentActor(false),
        m_activeLoadInstance(0),
        m_tsNoActors(PerfCounter::Query()),
        m_peComponents("Physics Profile Editor"),
        m_peNodes("Node Profile Editor"),
        state({ .windows{false, false, false, false, false, false, false } })
    {
    }

    void UIContext::Reset(uint32_t a_loadInstance)
    {
        ResetActorList();
        m_nextUpdateCurrentActor = false;
        m_activeLoadInstance = a_loadInstance;

        m_raceEditor.Reset();
        m_nodeConfig.Reset();
    }

    void UIContext::Draw(bool* a_active)
    {
        auto& io = ImGui::GetIO();

        ActorListTick();

        if (m_nextUpdateCurrentActor) {
            m_nextUpdateCurrentActor = false;
            UpdateActorValues(m_currentActor);
        }

        if (m_actorList.size() == 0) {
            auto t = PerfCounter::Query();
            if (PerfCounter::delta_us(m_tsNoActors, t) >= 2500000LL) {
                DCBP::QueueActorCacheUpdate();
                m_tsNoActors = t;
            }
        }

        ImGui::SetNextWindowPos(ImVec2(20.0f, 20.0f), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(450.0f, io.DisplaySize.y), ImGuiCond_FirstUseEver);

        ImVec2 sizeMin(min(300.0f, io.DisplaySize.x - 40.0f), min(200.0f, io.DisplaySize.y - 40.0f));
        ImVec2 sizeMax(min(450.0f, io.DisplaySize.x), max(io.DisplaySize.y - 40.0f, sizeMin.y));

        ImGui::SetNextWindowSizeConstraints(sizeMin, sizeMax);

        auto entry = GetSelectedEntry();

        if (ImGui::Begin("CBP Config Editor", a_active, ImGuiWindowFlags_MenuBar))
        {
            ImGui::PushID(static_cast<const void*>(this));
            ImGui::PushItemWidth(ImGui::GetFontSize() * -15.0f);

            bool saveAllFailed = false;

            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("File"))
                {
                    if (ImGui::MenuItem("Save"))
                        if (!DCBP::SaveAll()) {
                            saveAllFailed = true;
                            state.lastException =
                                DCBP::GetLastSerializationException();
                        }

                    ImGui::Separator();
                    if (ImGui::MenuItem("Exit"))
                        *a_active = false;

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Tools"))
                {
                    ImGui::MenuItem("Race config", nullptr, &state.windows.race);
                    ImGui::MenuItem("Node config", nullptr, &state.windows.nodeConf);

                    ImGui::Separator();
                    ImGui::MenuItem("Collision groups", nullptr, &state.windows.collisionGroups);

                    ImGui::Separator();
                    ImGui::MenuItem(m_peComponents.GetName(), nullptr, &state.windows.profileSim);
                    ImGui::MenuItem(m_peNodes.GetName(), nullptr, &state.windows.profileNodes);

                    ImGui::Separator();
                    ImGui::MenuItem("Stats", nullptr, &state.windows.profiling);

                    ImGui::Separator();
                    ImGui::MenuItem("Options", nullptr, &state.windows.options);

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Actions"))
                {
                    if (ImGui::MenuItem("Reset actors"))
                        DCBP::ResetActors();
                    if (ImGui::MenuItem("NiNode update"))
                        DCBP::NiNodeUpdate();

                    ImGui::EndMenu();
                }

                ImGui::EndMenuBar();
            }

            const char* curSelName;

            DrawActorList(entry, curSelName);

            ImGui::Spacing();

            if (m_currentActor) {
                auto confClass = IConfig::GetActorConfigClass(m_currentActor);
                const char* classText;
                switch (confClass)
                {
                case ConfigClass::kConfigActor:
                    classText = "actor";
                    break;
                case ConfigClass::kConfigRace:
                    classText = "race";
                    break;
                default:
                    classText = "global";
                    break;
                }
                ImGui::Text("Config in use: %s", classText);
            }

            auto& globalConfig = IConfig::GetGlobalConfig();

            ImGui::Spacing();
            if (ImGui::Checkbox("Show all actors", &globalConfig.ui.showAllActors)) {
                DCBP::QueueActorCacheUpdate();
                DCBP::MarkGlobalsForSave();
            }
            HelpMarker(MiscHelpText::showAllActors);

            ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 50.0f);
            if (ImGui::Button("Rescan"))
                DCBP::QueueActorCacheUpdate();
            HelpMarker(MiscHelpText::rescanActors);

            ImGui::Spacing();
            if (ImGui::Checkbox("Clamp values", &globalConfig.ui.clampValuesMain))
                DCBP::MarkGlobalsForSave();
            HelpMarker(MiscHelpText::clampValues);

            ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 43.0f);
            if (ImGui::Button("Reset"))
                ImGui::OpenPopup("Reset");
            HelpMarker(MiscHelpText::resetConfOnActor);

            ImGui::Spacing();
            if (ImGui::Checkbox("Sync min/max weight sliders", &globalConfig.ui.syncWeightSlidersMain))
                DCBP::MarkGlobalsForSave();
            HelpMarker(MiscHelpText::syncMinMax);

            if (UICommon::ConfirmDialog(
                "Reset",
                "%s: clear all values for actor?\n\n", curSelName))
            {
                if (m_currentActor) {
                    ResetAllActorValues(m_currentActor);
                    DCBP::DispatchActorTask(m_currentActor, UTTask::kActionUpdateConfig);
                }
                else {
                    IConfig::ClearGlobalProfile();
                    DCBP::UpdateConfigOnAllActors();
                }
            }

            if (saveAllFailed)
                ImGui::OpenPopup("Save failed");

            ImGui::Spacing();

            DrawProfileSelector(entry);

            ImGui::Spacing();

            DrawForceSelector(entry, globalConfig.ui.forceActor);

            ImGui::Separator();

            if (m_currentActor) {
                m_scActor.DrawSimComponents(m_currentActor, entry->second.second);
            }
            else {
                m_scGlobal.DrawSimComponents(0, IConfig::GetGlobalProfile());
            }

            UICommon::MessageDialog("Save failed", "Saving one or more files failed.\nThe last exception was:\n\n%s", state.lastException.what());

            ImGui::PopItemWidth();
            ImGui::PopID();
        }

        ImGui::End();

        if (state.windows.options)
            m_options.Draw(&state.windows.options);

        if (state.windows.profileSim)
            m_peComponents.Draw(&state.windows.profileSim);

        if (state.windows.profileNodes)
            m_peNodes.Draw(&state.windows.profileNodes);

        if (state.windows.race) {
            m_raceEditor.Draw(&state.windows.race);
            if (m_raceEditor.GetChanged())
                UpdateActorValues(entry);
        }

        if (state.windows.collisionGroups)
            m_colGroups.Draw(&state.windows.collisionGroups);

        if (state.windows.nodeConf)
            m_nodeConfig.Draw(&state.windows.nodeConf);

        if (state.windows.profiling)
            m_profiling.Draw(&state.windows.profiling);
    }

    void UIOptions::Draw(bool* a_active)
    {
        auto& io = ImGui::GetIO();
        auto& globalConfig = IConfig::GetGlobalConfig();

        ImVec2 center(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);
        ImGui::SetNextWindowPos(center, ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSizeConstraints(ImVec2(400.0f, 100.0f), ImVec2(800.0f, 800.0f));

        ImGui::PushID(static_cast<const void*>(a_active));

        if (ImGui::Begin("Options", a_active, ImGuiWindowFlags_AlwaysAutoResize))
        {
            if (CollapsingHeader("Options#General", "General"))
            {
                if (ImGui::Checkbox("Select actor in crosshairs on open", &globalConfig.ui.selectCrosshairActor))
                    DCBP::MarkGlobalsForSave();
            }

            if (CollapsingHeader("Options#Controls", "Controls"))
            {
                if (DCBP::GetDriverConfig().force_ini_keys)
                {
                    ImGui::Spacing();
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.66f, 0.13f, 1.0f));
                    ImGui::TextWrapped("ForceINIKeys is enabled, keys configured here will have no effect");
                    ImGui::PopStyleColor();
                }

                ImGui::Spacing();

                DrawKeyOptions("Combo key", comboKeyDesc, globalConfig.ui.comboKey);
                DrawKeyOptions("Key", keyDesc, globalConfig.ui.showKey);

                ImGui::Spacing();

                if (ImGui::Checkbox("Lock game controls while UI active", &globalConfig.ui.lockControls))
                    DCBP::MarkGlobalsForSave();
            }

            if (CollapsingHeader("Options#Simulation", "Simulation"))
            {
                ImGui::Spacing();

                if (ImGui::Checkbox("Female actors only", &globalConfig.general.femaleOnly)) {
                    DCBP::ResetActors();
                    DCBP::MarkGlobalsForSave();
                }

                if (ImGui::Checkbox("Enable collisions", &globalConfig.phys.collisions))
                    DCBP::MarkGlobalsForSave();

                ImGui::Spacing();

                float timeStep = 1.0f / globalConfig.phys.timeStep;
                if (ImGui::SliderFloat("timeStep", &timeStep, 30.0f, 240.0f, "%.0f")) {
                    globalConfig.phys.timeStep = 1.0f / std::clamp(timeStep, 30.0f, 240.0f);
                    DCBP::MarkGlobalsForSave();
                }
                HelpMarker(MiscHelpText::timeStep);

                if (ImGui::SliderFloat("timeScale", &globalConfig.phys.timeScale, 0.05f, 10.0f)) {
                    globalConfig.phys.timeScale = std::clamp(globalConfig.phys.timeScale, 0.05f, 10.0f);
                    DCBP::MarkGlobalsForSave();
                }
                HelpMarker(MiscHelpText::timeScale);

                if (ImGui::SliderFloat("colMaxPenetrationDepth", &globalConfig.phys.colMaxPenetrationDepth, 0.5f, 100.0f)) {
                    globalConfig.phys.colMaxPenetrationDepth = std::clamp(globalConfig.phys.colMaxPenetrationDepth, 0.05f, 500.0f);
                    DCBP::MarkGlobalsForSave();
                }
                HelpMarker(MiscHelpText::colMaxPenetrationDepth);
            }

            if (DCBP::GetDriverConfig().debug_renderer)
            {
                if (CollapsingHeader("Options#DebugRenderer", "Debug Renderer"))
                {
                    ImGui::Spacing();

                    if (ImGui::Checkbox("Enable", &globalConfig.debugRenderer.enabled)) {
                        DCBP::UpdateDebugRendererState();
                        DCBP::MarkGlobalsForSave();
                    }

                    if (ImGui::Checkbox("Wireframe", &globalConfig.debugRenderer.wireframe))
                        DCBP::MarkGlobalsForSave();

                    ImGui::Spacing();

                    if (ImGui::SliderFloat("Contact point sphere radius", &globalConfig.debugRenderer.contactPointSphereRadius, 0.1f, 25.0f, "%.2f")) {
                        globalConfig.debugRenderer.contactPointSphereRadius = std::clamp(globalConfig.debugRenderer.contactPointSphereRadius, 0.1f, 25.0f);
                        DCBP::UpdateDebugRendererSettings();
                        DCBP::MarkGlobalsForSave();
                    }

                    if (ImGui::SliderFloat("Contact normal length", &globalConfig.debugRenderer.contactNormalLength, 0.1f, 50.0f, "%.2f")) {
                        globalConfig.debugRenderer.contactNormalLength = std::clamp(globalConfig.debugRenderer.contactNormalLength, 0.1f, 50.0f);
                        DCBP::UpdateDebugRendererSettings();
                        DCBP::MarkGlobalsForSave();
                    }
                }
            }
        }

        ImGui::End();

        ImGui::PopID();
    }

    void UIOptions::DrawKeyOptions(
        const char* a_desc,
        const keyDesc_t& a_dmap,
        UInt32& a_out)
    {
        std::string tmp;
        const char* curSelName;

        auto it = a_dmap->find(a_out);
        if (it != a_dmap->end())
            curSelName = it->second;
        else {
            std::ostringstream stream;
            stream << "0x"
                << std::uppercase
                << std::setfill('0') << std::setw(2)
                << std::hex << a_out;
            tmp = std::move(stream.str());
            curSelName = tmp.c_str();
        }

        if (ImGui::BeginCombo(a_desc, curSelName, ImGuiComboFlags_HeightLarge))
        {
            for (const auto& e : a_dmap)
            {
                ImGui::PushID(reinterpret_cast<const void*>(std::addressof(e.second)));
                bool selected = e.first == a_out;
                if (selected)
                    if (ImGui::IsWindowAppearing()) ImGui::SetScrollHereY();

                if (ImGui::Selectable(e.second, selected)) {
                    if (a_out != e.first) {
                        a_out = e.first;
                        DCBP::MarkGlobalsForSave();
                    }
                }

                ImGui::PopID();
            }

            ImGui::EndCombo();
        }
    }

    void UICollisionGroups::Draw(bool* a_active)
    {

        auto& io = ImGui::GetIO();
        //auto& globalConfig = IConfig::GetGlobalConfig();

        ImVec2 center(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);
        ImGui::SetNextWindowPos(center, ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));

        ImVec2 sizeMin(min(300.0f, io.DisplaySize.x - 40.0f), min(100.0f, io.DisplaySize.y - 40.0f));
        ImVec2 sizeMax(min(450.0f, io.DisplaySize.x), max(io.DisplaySize.y - 40.0f, sizeMin.y));

        ImGui::SetNextWindowSizeConstraints(sizeMin, sizeMax);
        ImGui::SetNextWindowSize(ImVec2(400.0f, io.DisplaySize.y), ImGuiCond_FirstUseEver);

        ImGui::PushID(static_cast<const void*>(a_active));

        if (ImGui::Begin("Collision groups", a_active))
        {
            ImGui::PushItemWidth(ImGui::GetFontSize() * -14.0f);

            auto& colGroups = IConfig::GetCollisionGroups();
            auto& nodeColGroupMap = IConfig::GetNodeCollisionGroupMap();
            auto& nodeMap = IConfig::GetNodeMap();

            const char* curSelName;
            if (m_selected)
                curSelName = reinterpret_cast<const char*>(std::addressof(*m_selected));
            else
                curSelName = nullptr;

            if (ImGui::BeginCombo("Groups", curSelName))
            {
                for (const auto& e : colGroups)
                {
                    ImGui::PushID(reinterpret_cast<const void*>(std::addressof(e)));

                    bool selected = e == *m_selected;
                    if (selected)
                        if (ImGui::IsWindowAppearing()) ImGui::SetScrollHereY();

                    if (ImGui::Selectable(reinterpret_cast<const char*>(std::addressof(e)), selected)) {
                        m_selected = e;
                    }

                    ImGui::PopID();
                }

                ImGui::EndCombo();
            }

            ImGui::SameLine();
            if (ImGui::Button("New")) {
                ImGui::OpenPopup("New group");
                m_input = 0;
            }

            if (UICommon::TextInputDialog("New group", "Enter group name:",
                reinterpret_cast<char*>(&m_input), sizeof(m_input)))
            {
                if (m_input) {
                    colGroups.emplace(m_input);
                    m_selected = m_input;
                    DCBP::SaveCollisionGroups();
                    DCBP::UpdateGroupInfoOnAllActors();
                }
            }

            ImGui::SameLine();
            if (ImGui::Button("Delete")) {
                if (m_selected)
                    ImGui::OpenPopup("Delete");
            }

            if (UICommon::ConfirmDialog(
                "Delete",
                "Are you sure you want to delete group '%s'?\n\n", curSelName))
            {
                auto it = nodeColGroupMap.begin();
                while (it != nodeColGroupMap.end())
                {
                    if (it->second == *m_selected)
                        it = nodeColGroupMap.erase(it);
                    else
                        ++it;
                }

                colGroups.erase(*m_selected);

                m_selected.Clear();

                DCBP::SaveCollisionGroups();
                DCBP::UpdateGroupInfoOnAllActors();
            }

            ImGui::Separator();

            for (const auto& e : nodeMap)
            {
                uint64_t curSel;

                auto it = nodeColGroupMap.find(e.first);
                if (it != nodeColGroupMap.end()) {
                    curSel = it->second;
                    curSelName = reinterpret_cast<const char*>(&curSel);
                }
                else {
                    curSel = 0;
                    curSelName = nullptr;
                }

                ImGui::PushID(static_cast<const void*>(std::addressof(e)));

                if (ImGui::BeginCombo(e.first.c_str(), curSelName))
                {
                    if (curSel != 0) {
                        if (ImGui::Selectable("")) {
                            nodeColGroupMap.erase(e.first);
                            DCBP::SaveCollisionGroups();
                            DCBP::UpdateGroupInfoOnAllActors();
                        }
                    }

                    for (const auto& j : colGroups)
                    {
                        ImGui::PushID(static_cast<const void*>(std::addressof(j)));

                        bool selected = j == curSel;
                        if (selected)
                            if (ImGui::IsWindowAppearing()) ImGui::SetScrollHereY();

                        if (ImGui::Selectable(reinterpret_cast<const char*>(std::addressof(j)), selected)) {
                            nodeColGroupMap[e.first] = j;
                            DCBP::SaveCollisionGroups();
                            DCBP::UpdateGroupInfoOnAllActors();
                        }

                        ImGui::PopID();
                    }

                    ImGui::EndCombo();
                }

                ImGui::PopID();
            }

            ImGui::PopItemWidth();
        }

        ImGui::End();

        ImGui::PopID();
    }

    void UINodeConfig::Reset()
    {
        ResetActorList();
    }

    void UINodeConfig::Draw(bool* a_active)
    {
        ActorListTick();

        auto& io = ImGui::GetIO();

        ImVec2 center(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);
        ImGui::SetNextWindowPos(center, ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));

        ImVec2 sizeMin(min(300.0f, io.DisplaySize.x - 40.0f), min(100.0f, io.DisplaySize.y - 40.0f));
        ImVec2 sizeMax(min(400.0f, io.DisplaySize.x), max(io.DisplaySize.y - 40.0f, sizeMin.y));

        ImGui::SetNextWindowSizeConstraints(sizeMin, sizeMax);

        ImGui::PushID(static_cast<const void*>(a_active));

        if (ImGui::Begin("Node config", a_active))
        {
            ImGui::PushItemWidth(ImGui::GetFontSize() * -12.0f);

            auto entry = GetSelectedEntry();
            const char* curSelName;

            DrawActorList(entry, curSelName);

            if (entry) 
            {
                if (ImGui::Button("Reset"))
                    ImGui::OpenPopup("Reset Node");

                if (UICommon::ConfirmDialog(
                    "Reset Node",
                    "Reset all values for '%s'?\n\n", curSelName))
                {
                    ResetAllActorValues(entry->first);
                }

                ImGui::SameLine();
            }

            if (ImGui::Button("Rescan"))
                DCBP::QueueActorCacheUpdate();
            HelpMarker(MiscHelpText::rescanActors);

            ImGui::Spacing();

            ImGui::PushItemWidth(ImGui::GetFontSize() * -14.0f);

            DrawProfileSelector(entry);

            ImGui::PopItemWidth();

            ImGui::Spacing();

            ImGui::Separator();

            if (entry) {
                DrawNodes(entry->first, entry->second.second);
            }
            else {
                DrawNodes(0, IConfig::GetGlobalNodeConfig());
            }

            ImGui::PopItemWidth();
        }

        ImGui::End();

        ImGui::PopID();
    }

    auto UINodeConfig::GetData(SKSE::ObjectHandle a_handle) ->
        const actorEntryValue_t&
    {
        return IConfig::GetActorNodeConfig(a_handle);
    }

    const NodeProfile::base_type& UINodeConfig::GetData(const actorListValue_t* a_data) const
    {
        return !a_data ? IConfig::GetGlobalNodeConfig() : a_data->second.second;
    }

    void UINodeConfig::ApplyProfile(actorListValue_t* a_data, const NodeProfile& a_profile)
    {
        auto& profileData = a_profile.Data();

        if (!a_data) {
            IConfig::CopyToGlobalNodeProfile(profileData);
        }
        else {
            IConfig::CopyNodes(profileData, a_data->second.second);
            IConfig::SetActorNodeConfig(a_data->first, a_data->second.second);
        }

        DCBP::ResetActors();
    }

    void UINodeConfig::ResetAllActorValues(SKSE::ObjectHandle a_handle)
    {
        IConfig::EraseActorNodeConfig(a_handle);
        m_actorList.at(a_handle).second = IConfig::GetActorNodeConfig(a_handle);

        DCBP::MarkForSave(Serialization::kGlobalProfile);
        DCBP::ResetActors();
    }

    void UINodeConfig::UpdateNodeData(
        SKSE::ObjectHandle a_handle,
        const std::string& a_node,
        const NodeProfile::base_type::mapped_type& a_data)
    {
        if (a_handle) {
            auto& nodeConfig = IConfig::GetOrCreateActorNodeConfig(a_handle);
            nodeConfig.insert_or_assign(a_node, a_data);
        }
        else {
            DCBP::MarkForSave(Serialization::kGlobalProfile);
        }

        DCBP::ResetActors();
    }

    void UIContext::UISimComponentActor::OnSimSliderChange(
        SKSE::ObjectHandle a_handle,
        configComponents_t& a_data,
        configComponentsValue_t& a_pair,
        const componentValueDescMap_t::vec_value_type& a_desc,
        float* a_val)
    {
        auto& globalConfig = IConfig::GetGlobalConfig();

        if (globalConfig.ui.clampValuesMain)
            *a_val = std::clamp(*a_val, a_desc.second.min, a_desc.second.max);

        auto& actorConf = IConfig::GetOrCreateActorConf(a_handle);
        auto& entry = actorConf.at(a_pair.first);

        auto addr = reinterpret_cast<uintptr_t>(std::addressof(entry)) + a_desc.second.offset;
        *reinterpret_cast<float*>(addr) = *a_val;

        Propagate(a_data, std::addressof(actorConf), a_pair.first, a_desc.first, *a_val);

        if (a_desc.second.counterpart.size() &&
            globalConfig.ui.syncWeightSlidersMain)
        {
            a_pair.second.Set(a_desc.second.counterpart, *a_val);
            entry.Set(a_desc.second.counterpart, *a_val);
            Propagate(a_data, std::addressof(actorConf), a_pair.first, a_desc.second.counterpart, *a_val);
        }

        DCBP::DispatchActorTask(a_handle, UTTask::kActionUpdateConfig);
    }

    bool UIContext::UISimComponentActor::ShouldDrawComponent(
        SKSE::ObjectHandle a_handle,
        const configComponents_t::value_type& a_comp)
    {
        return DCBP::ActorHasConfigGroup(a_handle, a_comp.first);
    }

    void UIContext::UISimComponentGlobal::OnSimSliderChange(
        SKSE::ObjectHandle a_handle,
        configComponents_t& a_data,
        configComponentsValue_t& a_pair,
        const componentValueDescMap_t::vec_value_type& a_desc,
        float* a_val)
    {
        auto& globalConfig = IConfig::GetGlobalConfig();

        if (globalConfig.ui.clampValuesMain)
            *a_val = std::clamp(*a_val, a_desc.second.min, a_desc.second.max);

        Propagate(a_data, nullptr, a_pair.first, a_desc.first, *a_val);

        if (a_desc.second.counterpart.size() && globalConfig.ui.syncWeightSlidersMain) {
            a_pair.second.Set(a_desc.second.counterpart, *a_val);
            Propagate(a_data, nullptr, a_pair.first, a_desc.second.counterpart, *a_val);
        }

        DCBP::UpdateConfigOnAllActors();
    }

    bool UIContext::UISimComponentGlobal::ShouldDrawComponent(
        SKSE::ObjectHandle a_handle,
        const configComponents_t::value_type& a_comp)
    {
        return DCBP::GlobalHasConfigGroup(a_comp.first);
    }

    void UIContext::ApplyProfile(actorListValue_t* a_data, const SimProfile& a_profile)
    {
        auto& profileData = a_profile.Data();

        if (!a_data) {
            IConfig::CopyToGlobalProfile(profileData);
            DCBP::UpdateConfigOnAllActors();
        }
        else {
            IConfig::CopyComponents(profileData, a_data->second.second);
            IConfig::SetActorConf(a_data->first, a_data->second.second);
            DCBP::DispatchActorTask(a_data->first, UTTask::kActionUpdateConfig);
        }
    }

    const SimProfile::base_type& UIContext::GetData(const actorListValue_t* a_data) const
    {
        return !a_data ? IConfig::GetGlobalProfile() : a_data->second.second;
    }

    void UIContext::ResetAllActorValues(SKSE::ObjectHandle a_handle)
    {
        IConfig::EraseActorConf(a_handle);
        m_actorList.at(a_handle).second = IConfig::GetActorConf(a_handle);
    }

    void UIContext::UpdateActorValues(SKSE::ObjectHandle a_handle)
    {
        if (a_handle)
            m_actorList.at(a_handle).second = IConfig::GetActorConf(a_handle);
    }

    void UIContext::UpdateActorValues(actorListValue_t* a_data)
    {
        if (a_data)
            a_data->second.second = IConfig::GetActorConf(a_data->first);
    }

    auto UIContext::GetData(SKSE::ObjectHandle a_handle) ->
        const actorEntryValue_t&
    {
        return IConfig::GetActorConf(a_handle);
    }

    void UIContext::ApplyForce(
        actorListValue_t* a_data,
        uint32_t a_steps,
        const std::string& a_component,
        const NiPoint3& a_force)
    {
        if (a_steps == 0)
            return;

        SKSE::ObjectHandle handle;
        if (a_data != nullptr)
            handle = a_data->first;
        else
            handle = 0;

        DCBP::ApplyForce(handle, a_steps, a_component, a_force);
    }

    template <class T, UIEditorID ID>
    void UISimComponent<T, ID>::Propagate(
        configComponents_t& a_dl,
        configComponents_t* a_dg,
        const std::string& a_comp,
        const std::string& a_key,
        float a_val)
    {
        auto& globalConfig = IConfig::GetGlobalConfig();

        auto itm = globalConfig.ui.mirror.find(ID);
        if (itm == globalConfig.ui.mirror.end())
            return;

        auto it = itm->second.find(a_comp);
        if (it == itm->second.end())
            return;

        for (auto& e : it->second) {
            if (!e.second)
                continue;

            auto it1 = a_dl.find(e.first);
            if (it1 != a_dl.end())
                it1->second.Set(a_key, a_val);

            if (a_dg != nullptr) {
                auto it2 = a_dg->find(e.first);
                if (it2 != a_dg->end())
                    it2->second.Set(a_key, a_val);
            }
        }
    }

    template <class T, UIEditorID ID>
    void UISimComponent<T, ID>::DrawSimComponents(T a_handle, configComponents_t& a_data)
    {
        auto& globalConfig = IConfig::GetGlobalConfig();

        for (auto& p : a_data)
        {
            if (!ShouldDrawComponent(a_handle, p))
                continue;

            auto headerName = p.first;
            if (headerName.size() != 0) {
                headerName[0] = std::toupper(headerName[0]);
            }

            if (CollapsingHeader(GetCSID(p.first), headerName.c_str()))
            {
                ImGui::PushID(static_cast<const void*>(std::addressof(p)));

                if (ImGui::Button("Mirroring >"))
                    ImGui::OpenPopup("mirror_popup");

                if (ImGui::BeginPopup("mirror_popup"))
                {
                    auto& mirrorTo = globalConfig.ui.mirror[ID];

                    auto c = mirrorTo.try_emplace(p.first);
                    auto& d = c.first->second;

                    for (const auto& e : a_data)
                    {
                        if (e.first == p.first)
                            continue;

                        if (!ShouldDrawComponent(a_handle, e))
                            continue;

                        auto headerName = e.first;
                        if (headerName.size() != 0)
                            headerName[0] = std::toupper(headerName[0]);

                        auto i = d.try_emplace(e.first, false);
                        if (ImGui::MenuItem(headerName.c_str(), nullptr, std::addressof(i.first->second)))
                            DCBP::MarkGlobalsForSave();
                    }

                    if (d.size()) {
                        ImGui::Separator();

                        if (ImGui::MenuItem("Clear")) {
                            mirrorTo.erase(p.first);
                            DCBP::MarkGlobalsForSave();
                        }
                    }

                    ImGui::EndPopup();
                }

                DrawSliders(a_handle, a_data, p);

                ImGui::PopID();
            }
        }
    }

    template <class T, UIEditorID ID>
    void UISimComponent<T, ID>::DrawSliders(
        T a_handle,
        configComponents_t& a_data,
        configComponentsValue_t& a_pair
    )
    {
        for (const auto& e : configComponent_t::descMap)
        {
            auto addr = reinterpret_cast<uintptr_t>(std::addressof(a_pair.second)) + e.second.offset;
            float* pValue = reinterpret_cast<float*>(addr);

            if (ImGui::SliderFloat(e.second.descTag, pValue, e.second.min, e.second.max))
                OnSimSliderChange(a_handle, a_data, a_pair, e, pValue);

            ImGui::SameLine(); UICommon::HelpMarker(e.second.helpText);
        }
    }

    template <class T, UIEditorID ID>
    bool UISimComponent<T, ID>::ShouldDrawComponent(
        T m_handle,
        const configComponents_t::value_type& a_comp)
    {
        return true;
    }

    template <class T, UIEditorID ID>
    void UINode<T, ID>::DrawNodes(
        T a_handle,
        configNodes_t& a_data)
    {
        auto& nodeMap = IConfig::GetNodeMap();

        ImGui::PushItemWidth(ImGui::GetFontSize() * -10.0f);

        for (const auto& e : nodeMap)
        {
            ImGui::PushID(static_cast<const void*>(std::addressof(e)));

            if (CollapsingHeader(GetCSID(e.first), e.first.c_str()))
            {
                auto& conf = a_data[e.first];

                bool changed = false;

                ImGui::Columns(2, nullptr, false);

                ImGui::Text("Female");
                ImGui::Spacing();
                changed |= ImGui::Checkbox("Movement", &conf.femaleMovement);
                changed |= ImGui::Checkbox("Collisions", &conf.femaleCollisions);

                ImGui::NextColumn();

                ImGui::Text("Male");
                ImGui::Spacing();
                changed |= ImGui::Checkbox("Movement", &conf.maleMovement);
                changed |= ImGui::Checkbox("Collisions", &conf.maleCollisions);

                ImGui::Columns(1);

                if (changed)
                    UpdateNodeData(a_handle, e.first, conf);
            }

            ImGui::PopID();
        }

        ImGui::PopItemWidth();
    }

    void UIProfiling::Draw(bool* a_active)
    {
        auto& io = ImGui::GetIO();
        auto& globalConfig = IConfig::GetGlobalConfig();

        ImVec2 center(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);
        ImGui::SetNextWindowPos(center, ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSizeConstraints(ImVec2(300.0f, 50.0f), ImVec2(400.0f, 500.0f));

        ImGui::PushID(static_cast<const void*>(a_active));

        if (ImGui::Begin("Stats", a_active, ImGuiWindowFlags_AlwaysAutoResize))
        {
            if (globalConfig.general.enableProfiling)
            {
                auto& stats = DCBP::GetProfiler().Current();

                ImGui::Columns(2, nullptr, false);

                ImGui::Text("Avg. time:");
                ImGui::Text("Avg. actors:");
                ImGui::Text("Avg. rate:");

                ImGui::NextColumn();

                ImGui::Text("%lld us", stats.avgTime);
                ImGui::Text("%u", stats.avgActorCount);
                ImGui::Text("%.1f", stats.avgUpdateRate);

                ImGui::Columns(1);

                if (globalConfig.debugRenderer.enabled)
                {
                    ImGui::Spacing();
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.66f, 0.13f, 1.0f));
                    ImGui::TextWrapped("WARNING: Disable debug renderer for accurate measurement");
                    ImGui::PopStyleColor();
                    ImGui::Spacing();
                }

                ImGui::Separator();
            }

            static const std::string chKey("Stats#Settings");

            if (CollapsingHeader(chKey, "Settings"))
            {
                ImGui::PushItemWidth(ImGui::GetFontSize() * -8.0f);

                if (ImGui::Checkbox("Enabled", &globalConfig.general.enableProfiling))
                {
                    if (globalConfig.general.enableProfiling)
                        DCBP::ResetProfiler();
                    DCBP::MarkGlobalsForSave();
                }

                if (ImGui::SliderInt("Interval (ms)", &globalConfig.general.profilingInterval, 100, 10000, "%d"))
                {
                    globalConfig.general.profilingInterval =
                        std::clamp(globalConfig.general.profilingInterval, 100, 10000);
                    DCBP::SetProfilerInterval(static_cast<long long>(
                        globalConfig.general.profilingInterval) * 1000);
                    DCBP::MarkGlobalsForSave();
                }

                ImGui::PopItemWidth();
            }
        }

        ImGui::End();

        ImGui::PopID();
    }
}