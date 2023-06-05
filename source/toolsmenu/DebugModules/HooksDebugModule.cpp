#include "StdInc.h"

#include <reversiblehooks/RootHookCategory.h>
#include "HooksDebugModule.h"
#include "Utility.h"
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>

#include <imgui.h>
#include <imgui_stdlib.h>
#include <TristateCheckbox.h>
#include <string>
#include <ranges>
#include <optional>
#include <format>

namespace RH = ReversibleHooks;
namespace rng = std::ranges;
using namespace ImGui;

// Namespace for the hook filter
// It is intentionally not a class/struct (As it's a singleton and static classes are stupid)
namespace HookFilter {
    // If either of these separators is changed make sure to alter the regex as well.
    // Also all the comments :D
    static constexpr std::string_view NAMESPACE_SEP{ "/" };
    static constexpr std::string_view HOOKNAME_SEP{ "::" };

    // Maybe one day.. For now it's fine
    // Regex used for checking if the input is valid, see `ProcessImGui` in this class.
    //static inline const std::regex INPUT_VALID_REGEX{
    //    R"r(\/?(?:\w+\/)*\w+(?:\:{2}\w*|\:))r", // We're using in r-string here to avoid the need of escaping all `\`
    //    std::regex::optimize | std::regex::icase
    //};

    // Used as char buffer for the ImGUI input
    std::string m_input{};

    // Are the string checks case sensitive?
    // It is used, but there's no GUI to change it for now. It is case-insensitive by defult (false).
    bool m_caseSensitive{};

    // Contains all the tokens on the left side split by `NAMESPACE_SEP` of the input split by `HOOKNAME_SEP`
    // Eg `m_input` => content:
    // - `Name/Space/` => `Name`, `Space`, `` (<= empty string)
    // - `Name/Space/::` => -||- (Same as the above example)
    // - `/` - `` (empty string) - Indicates root namespace (See `IsRelativeToRootNamespace`)
    // - `///` - 4 empty strings
    std::vector<std::string_view> m_namespaceTokens{};

    // Filter of hook name
    // If `nullopt` means there was no `::` (HOOKNAME_SEP) in the user input
    // othewise if there was, it contains whatever was after it (Which might be nothing - So it's empty)
    std::optional<std::string_view> m_hookFilter{};

    // Clears both filters
    // Making all items visible again is done by `DoFilter`
    void ClearFilters() {
        m_namespaceTokens.clear(); // Clear only, so allocated memory is kept
        m_hookFilter = {};
    }

    // Are we filtering namespaces
    bool IsNamespaceFilterActive() {
        return !m_namespaceTokens.empty();
    }

    // If empty it won't filter anything
    bool IsHookFilterEmpty() {
        return m_hookFilter->empty();
    }

    // Check if hook filter is present.
    // even in case it's present it might be empty
    // in which case it wouldn't filter out anything.
    // Usually you want to use `IsHookFilterActive` which checks both.
    bool IsHookFilterPresent() {
        return m_hookFilter.has_value();
    }

    bool IsHookFilterActive() {
        return IsHookFilterPresent() && !IsHookFilterEmpty();
    }

    // Are either filters active
    bool EitherFiltersActive() {
        return IsNamespaceFilterActive() || IsHookFilterActive();
    }

    // Should the current filtered namespace be relative to the root namespace.
    // This is the case when the user prepends the namespace tokens with a `/` (NAMESPACE_SEP).
    // Eg.: `/Entity` should only show the `Entity` namespace under `Root` (But not, for example, `Audio/AEVehicleAudioEntity`)
    bool IsRelativeToRootNamespace() {
        return m_namespaceTokens.size() >= 1 && m_namespaceTokens.front().empty();
    }

    // Make all categories and their items possibly visible and/or open
    void MakeAllVisibleAndOpen(RH::HookCategory& cat, bool visible, bool open) {
        cat.Visible(true);
        cat.Open(open);

        cat.m_anyItemsVisible = true;
        for (auto& i : cat.Items()) {
            i->m_isVisible = true;
        }

        for (auto& sc : cat.SubCategories()) {
            MakeAllVisibleAndOpen(sc, visible, open);
        }
    }

    // Returns `pair<visible, open>` of this category
    auto DoFilter_Internal(RH::HookCategory& cat, size_t depth = 0) -> std::pair<bool, bool> {
        // Will be set to the appropriate values on return
        cat.Visible(false);
        cat.Open(false);

        const auto hasSubCategories = !cat.SubCategories().empty();

        // Process all sub-categories, and return if any category is pair<visible, open>
        const auto ProcessSubCategories = [&] {
            bool anyVisible{}, anyOpen{};
            for (auto& sc : cat.SubCategories()) {
                const auto [visible, open] = DoFilter_Internal(sc, depth + 1);
                anyVisible |= visible;
                anyOpen |= open;
            }
            return std::make_pair(anyVisible, anyOpen);
        };

        // If `doFilter` argument is `false` all items are set visible,
        // and either true (if we have hooks) or false (if `cat.Items().empty()`) is returned.
        // Otherwise items are filtered and true if returned if at least 1 item is visible.
        const auto ProcessItems = [&](bool allowFilter) {
            if (allowFilter && IsHookFilterActive()) {
                cat.m_anyItemsVisible = false;
                for (auto& i : cat.Items()) {
                    i->m_isVisible = StringContainsString(i->Name(), *m_hookFilter, m_caseSensitive);
                    cat.m_anyItemsVisible |= i->m_isVisible;
                }
            } else { // Otherwise make sure all items are visible (if any)
                if (cat.Items().empty()) { // No items, make sure flag is set correctly.
                    cat.m_anyItemsVisible = false;
                } else {
                    cat.m_anyItemsVisible = true;
                    for (auto& i : cat.Items()) {
                        i->m_isVisible = true;
                    }
                }
            }
            return cat.m_anyItemsVisible;
        };

        if (IsNamespaceFilterActive()) {
            if (IsRelativeToRootNamespace()) { // Eg.: (Notice the trailing `/`) /Entity/Ped/Player/ (Should show `Root/Entity/Ped/CPlayerPed`)
                // Using root namespace.
                // In this case all tokens must match up to root

                const auto ProcessFilter = [&]() -> bool {
                    if (depth == 0) { // Special case - Root namespace depth, no need to check any tokens
                        assert(m_namespaceTokens.front().empty()); // This codepath should be unreachable unless first token is empty
                        return true;
                    } else if (depth < m_namespaceTokens.size()) {
                        if (   hasSubCategories                      // Unless we're last category to match we must have more sub-categories so all tokens can match
                            || depth == m_namespaceTokens.size() - 1 // Last token to match, so there will be no more, thus it's fine if there are no more subcategories.
                            || m_namespaceTokens.back().empty()      // Or last token is empty (Empty strings always match everything) - This way a trailing `/` opens the category (like `::` does)
                        ) {
                            return StringContainsString(cat.Name(), m_namespaceTokens[depth], m_caseSensitive);
                        } else {
                            return false; // Not enough children to statify all tokens
                        }
                    } else { // Our parents fully matched the tokens, they just want to make us visible, but not open
                        return true;
                    }
                };

                const auto byFilterVisible = ProcessFilter();
                if (!byFilterVisible) {
                    return {};
                }

                // If botom level, and hook filtering is present: make us open, and dont show any more sub-categories
                if (depth == m_namespaceTokens.size() - 1 /*bottom level*/ && IsHookFilterPresent()) {
                    MakeAllVisibleAndOpen(cat, false, false);

                    (void)ProcessItems(true);

                    cat.Visible(true);
                    cat.Open(true);

                    return { true, true };
                }

                const auto [anySCVisible, anySCOpen] = ProcessSubCategories();

                const bool open    = anySCOpen || depth + 1 < m_namespaceTokens.size() /*All categories before the bottom level should be open*/;
                const bool visible = anySCVisible || byFilterVisible;

                cat.Visible(visible);
                cat.Open(open);

                return { visible, open };
            } else { // Eg.: Ped/Player/ (Should show `Root/Entity/Ped/CPlayerPed`)
                // Not in the root namespace
                // All tokens should match in reverse order starting at us, eg.:
                // Entity::Ped::Ped
                // So, in order to be visible* `cat`s name should contain `Ped`
                // it's parent's name should contain `Ped` and it's parent's name should contain `Entity`
                // *We may be visible if there are subcategories visible even if this function returns false

                // Example:
                // Parents: Root-Entity-Ped-CPed
                // Depth:   0     1       2    3   <= Also number of tokens
                // Tokens:        Entity::Ped::Ped

                const auto ProcessFilter = [&]() -> bool {
                    if (depth < m_namespaceTokens.size()) { // Optimization: Not enough categories to possibly staisfy all tokens
                        return false;
                    }

                    for (auto iterCat{ &cat }; auto&& token : m_namespaceTokens | rng::views::reverse) {
                        if (!StringContainsString(iterCat->Name(), token, m_caseSensitive)) { // Couldn't statify all tokens - Remember: `contains` always returns true if `token.empty()`
                            return false;
                        }
                        iterCat = iterCat->Parent();
                    }

                    return true;
                };

                const auto byFilterVisible           = ProcessFilter();
                const auto itemsVisible              = ProcessItems(true); // Filter items
                const auto [anySCVisible, anySCOpen] = ProcessSubCategories();

                const bool open    = anySCOpen || anySCVisible || (hasSubCategories && byFilterVisible) || (IsHookFilterPresent() && itemsVisible);
                const bool visible = (byFilterVisible && itemsVisible) || anySCVisible;

                cat.Visible(visible);
                cat.Open(open);

                return { visible, open };
            }
;       } else {
            // Filter by hook names
            // Category is visible if it:
            // - It has visible hooks (After filtering)
            // - Or it has visible sub-categories

            const auto itemsVisible = ProcessItems(true); // Filter items
            const auto [anySubCatVisible, anySubCatOpen] = ProcessSubCategories();

            const auto open    = itemsVisible || anySubCatOpen;
            const bool visible = open || anySubCatVisible;

            cat.Visible(visible);
            cat.Open(open);

            return { visible, open };
        }
    }

    void DoFilter(RH::HookCategory& cat) {
        if (EitherFiltersActive()) {
            DoFilter_Internal(cat);
        } else {
            MakeAllVisibleAndOpen(cat, true, false); // Make all visible, but closed
        }
    }

    void OnInputUpdate() {
        const std::string_view inputsv{ m_input };

        ClearFilters();

        if (inputsv.empty()) {
            return; // No filters
        }

        {
            const auto sepPos = inputsv.rfind(HOOKNAME_SEP);

            // First half contains the namespace filter tokens
            for (auto t : SplitStringView(inputsv.substr(0, sepPos), NAMESPACE_SEP)) {
                m_namespaceTokens.emplace_back(t);
            }

            // Second half (if any) contains the hook/function name filter
            if (sepPos != std::string_view::npos) {
                m_hookFilter = inputsv.substr(sepPos + HOOKNAME_SEP.size());
            }
        }

        // In case user passes in a string with multiple `/` with nothing inbetween we will have quite a few empty tokens.
        //// We have have to remove all the leading empty tokens up until the last empty one.
        while (m_namespaceTokens.size() >= 2 && m_namespaceTokens[0].empty() && m_namespaceTokens[1].empty()) {
            m_namespaceTokens.erase(m_namespaceTokens.begin());
        }

        // Don't delete this please //
        //
        /*std::cout << "update input: ";
        for (auto&& t : m_namespaceTokens) {
            std::cout << '`' << t << "`,";
        }
        std::cout << ";" << m_hookFilter.value_or("None") << "\n";*/

        // If we're using the root namespace only but there's no hook filter we paractically dont't filter anything
        // Example user inputs: `/`, `/::`, `::`
        if (IsRelativeToRootNamespace() && m_namespaceTokens.size() == 1 && IsHookFilterActive()) {
            ClearFilters();
        }
    }

    void Render() {
        PushItemWidth(GetWindowContentRegionMax().x - 10.f);
        if (InputText(" ", &m_input)) {
            OnInputUpdate();
            DoFilter(RH::GetRootCategory());
        }
        if (IsItemHovered()) {
            SetTooltip(
                "`::function`         - Filters only functions \n"
                "`cpy`                - Filter namespace - Will only show namespace with name containing \"cphy\"\n"
                "`ped/player`         - Should only show Ped/CPlayerPed\n"
                "`ped/player::busted` - Should only show `Ped/CPlayerPed` with the `busted` function visible only\n"
                "`/entity`            - Should only show the top level `Entity` namespace in Root\n"
                "For more tips see gta-reversed-modern/discussions/190\n"
            );
        }
        PopItemWidth();
    }
};

template<typename T> 
struct IDScope_Helper {
    IDScope_Helper(T id) { PushID(id); }
    ~IDScope_Helper() { PopID(); }
};

struct DisabledScope_Helper {
    DisabledScope_Helper(bool disabled) { BeginDisabled(disabled); }
    ~DisabledScope_Helper() { EndDisabled(); }
};

// https://stackoverflow.com/a/17624752
#define CONCAT(a, b) CONCAT_INNER(a, b)
#define CONCAT_INNER(a, b) a ## b
#define UNIQUE_VAR_NAME(base) CONCAT(base, __COUNTER__)
#define IDScope(id) IDScope_Helper UNIQUE_VAR_NAME(__helper){id}
#define DisabledScope(disabled) DisabledScope_Helper UNIQUE_VAR_NAME(__helper){disabled}

bool HooksDebugModule::HandleSlideSetterForItem(bool& inOutState) {
    if (m_SlideSetter.Mode == SlideSetterMode::NONE || !IsItemHovered()) {
        return false;
    }
    if (m_SlideSetter.Mode == SlideSetterMode::SETTER) {
        m_SlideSetter.Mode = inOutState
            ? SlideSetterMode::TURN_ON
            : SlideSetterMode::TURN_OFF;
    }
    inOutState = [&]{
        switch (m_SlideSetter.Mode) {
        case SlideSetterMode::TURN_ON:  return true;
        case SlideSetterMode::TURN_OFF: return false;
        case SlideSetterMode::TOGGLE:   return !inOutState;
        default: NOTSA_UNREACHABLE();
        }
    }();
    return true;
}

void HooksDebugModule::RenderCategoryItems(RH::HookCategory& cat) {
    for (auto& item : cat.Items()) {
        if (!item->m_isVisible) {
            continue;
        }

        // Use hook's address as unique ID - Hooks aren't dynamically created, so this should really be unique
        IDScope(item.get());

        // Draw hook symbol (`S` or `V`)
        {
            PushStyleVar(ImGuiStyleVar_Alpha, GetStyle().Alpha * 0.5f);
            AlignTextToFramePadding();
            Text(item->Symbol());
            PopStyleVar();
        }

        // State checkbox
        {
            IDScope("state");
            DisabledScope(item->Locked());

            bool hooked{ item->Hooked() };

            if (SameLine(); Checkbox(item->Name().c_str(), &hooked) && !item->Locked()) {
                cat.SetItemEnabled(item, hooked);
            }
            if (!item->Locked() && HandleSlideSetterForItem(hooked)) {
                cat.SetItemEnabled(item, hooked);
            }
        }

        if (!IsItemHovered()) {
            continue;
        }

        const auto ProcessToolTip = [](const auto& i) {
            const auto DrawToolTip = [](void* gta, void* our, bool locked) {
                const auto AddrToClipboard = [](void* addr) {
                    SetClipboardText(std::format("{}", addr).c_str());
                };

                std::string tooltipText = std::format("SA: {} / Our: {}", gta, our);
                if (locked) {
                    tooltipText += "\n(locked)";
                }
                SetTooltip(tooltipText.c_str());

                if (IsItemClicked(ImGuiMouseButton_Right)) {
                    AddrToClipboard(gta);
                } else if (IsItemClicked(ImGuiMouseButton_Middle)) {
                    AddrToClipboard(our);
                }
            };
            DrawToolTip(i->GetHookGTAAddress(), i->GetHookOurAddress(), i->Locked());
        };

        switch (item->Type()) {
        case RH::ReversibleHook::Base::HookType::Simple: {
            ProcessToolTip(std::static_pointer_cast<RH::ReversibleHook::Simple>(item));
            break;
        }
        case RH::ReversibleHook::Base::HookType::Virtual: {
            ProcessToolTip(std::static_pointer_cast<RH::ReversibleHook::Virtual>(item));
            break;
        }
        }
    }
}

void HooksDebugModule::RenderCategory(RH::HookCategory& cat) {
    if (!cat.Visible()) {
        return;
    }

    // We copy ImGui style here to alter it for indication.
    const auto styleRestore = ImGui::GetStyle();
    auto& style = ImGui::GetStyle();

    // NOTE: Won't work properly if `cat::m_items`s type is changed from `std::list`
    // Using this instead of the name as it's faster than encoding a string (And we care about performance, since this is mostly run in debug mode)
    IDScope(&cat);

    const auto& name = cat.Name();

    // TODO: Replace with `TreeNodeWithTriStateCheckBox`
    //! returns tuple<bool open, bool cbStateChanged, bool cbState>
    const auto TreeNodeWithCheckbox = [](auto label, ImTristate triState, bool disabled) {
        // TODO/NOTE: The Tree's label is a workaround for when the label is shorter than the visual checkbox (otherwise the checkbox can't be clic
        const auto open = TreeNodeEx("##         ", ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanFullWidth);
        
        SameLine();

        // Tree is never disabled, otherwise it couldn't be opened
        DisabledScope(disabled);

        // Checkbox + it's label will be the tree name
        bool cbState{};
        const auto stateChanged = CheckboxTristate(label, triState, cbState);

        return std::make_tuple(open, stateChanged, cbState);
    };

    // Disable all hooks in category at once
    {
        SetNextItemOpen(cat.Open());

        const auto [open, stateChanged, cbState] = TreeNodeWithCheckbox(cat.Name().c_str(), cat.OverallState(), cat.Disabled());
        if (stateChanged) {
            cat.ToggleAllItemsState();
        }
        if (bool state = cbState; HandleSlideSetterForItem(state)) {
            cat.SetAllItemsEnabled(state);
        }
        cat.Open(open);
    }

    if (!cat.Open()) {
        return;
    }

    //
    // Draw hooks, and subcategories
    //

    // Draw hooks (items) (if any)
    if (!cat.Items().empty() && cat.m_anyItemsVisible) {
        if (cat.SubCategories().empty()) { // If there are no subcategories we can draw all items directly
            RenderCategoryItems(cat);
        } else { // Otherwise use a tree node + checkbox
            const auto [open, stateChanged, cbState] = TreeNodeWithCheckbox("Hooks", cat.ItemsState(), cat.ItemsDisabled());

            if (stateChanged) {
                //cat.SetOurItemsState(cbState);
                cat.ToggleAllItemsState();
            }

            if (open) {
                RenderCategoryItems(cat);
                TreePop();
            }
        }
    }

    // Draw subcategories
    for (auto& v : cat.SubCategories()) {
        RenderCategory(v);
    }

    TreePop();
}

void HooksDebugModule::RenderWindow() {
    const notsa::ui::ScopedWindow window{ "ReversibleHooks (TM) (R)", {500.f, 700.f}, m_IsOpen };
    if (!m_IsOpen) {
        return;
    }
    m_SlideSetter.Mode = IsMouseDown(ImGuiMouseButton_Middle)
        ? SlideSetterMode::TOGGLE
        : IsMouseDown(ImGuiMouseButton_Right)
            ? m_SlideSetter.Mode == SlideSetterMode::TURN_OFF || m_SlideSetter.Mode == SlideSetterMode::TURN_ON
                ? m_SlideSetter.Mode // Technically in setter mode already
                : SlideSetterMode::SETTER
            : SlideSetterMode::NONE;
    HookFilter::Render();
    RenderCategory(RH::GetRootCategory());
}

void HooksDebugModule::RenderMenuEntry() {
    notsa::ui::DoNestedMenuIL({ "Settings" }, [&] {
        ImGui::MenuItem("Hooks", nullptr, &m_IsOpen);
    });
}
