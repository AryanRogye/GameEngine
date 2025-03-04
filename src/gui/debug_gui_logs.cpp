#include "comfy_lib.h"
#include "debug_gui.h"
#include "imgui.h"


void DebugGUI::addDebugLog(std::string log, std::vector<ErrorCode> codes)
{
    for (auto code : codes)
    addDebugLog(log, code);
}

void DebugGUI::addDebugLog(std::string log, ErrorCode code)
{
    const std::string timestamp = getTimeStamp();
    
    size_t start = 0;
    size_t end = log.find("\n");

    while (end != std::string::npos)
    {
        // Extract and store each log line separately
        guiValues.debugLogs.emplace_back(timestamp + " | " + log.substr(start, end - start), code);

        // Move to the next part of the string
        start = end + 1;
        end = log.find("\n", start);
    }

    // Add the last part (or the whole log if there were no newlines)
    if (start < log.size()) {
        guiValues.debugLogs.emplace_back(timestamp + " | " + log.substr(start), code);
    }
}

static std::vector<int> errorCodes(allErrorCodes.size(), 0);
void DebugGUI::renderLogs()
{
    // =====================================================================================================================
    // Error Codes Table of Checkboxes
    // =====================================================================================================================
    ImGui::Columns(3, nullptr, false); // 3 columns
    bool anyCheckBoxChecked = false;    // used to check if any checkbox is checked and then filter the logs
    for (int i = 0; i < allErrorCodes.size(); i++)
    {
        bool checked = errorCodes[i];  // Convert int to bool
        if (ImGui::Checkbox(errorCodeToString(allErrorCodes[i]).c_str(), &checked))
        {
            errorCodes[i] = checked ? 1 : 0; // Update the actual vector value
        }
        if (errorCodes[i])
        {
            anyCheckBoxChecked = true;
        }
        ImGui::NextColumn();
    }

    ImGui::Columns(1); // Reset

    // =====================================================================================================================
    // Search Bar
    // =====================================================================================================================
    static char searchBuffer[256] = "";
    ImGui::InputText("Search", searchBuffer, IM_ARRAYSIZE(searchBuffer));
    ImGui::SameLine();
    if (ImGui::Button("Clear"))
    {
        memset(searchBuffer, 0, sizeof(searchBuffer));
    }
    ImGui::Separator();

    // Start scrolling region
    ImGui::BeginChild("LogScroll", ImVec2(0, 300), true, ImGuiWindowFlags_HorizontalScrollbar);

    for (size_t i = 0; i < guiValues.debugLogs.size(); i++)
    {
        bool passesFilter = true; // Default to showing everything

        // Apply error code filtering ONLY if a checkbox is checked
        if (anyCheckBoxChecked)
        {
            if (errorCodes[static_cast<int>(guiValues.debugLogs[i].second)] == 0)
            {
                passesFilter = false;
            }
        }

        // Apply search filtering ONLY if the user typed something
        if (strlen(searchBuffer) > 0)
        {
            if (guiValues.debugLogs[i].first.find(searchBuffer) == std::string::npos)
            {
                passesFilter = false;
            }
        }

        // Show log if it passes the filter
        if (passesFilter)
        {
            ImGui::Text("%s", guiValues.debugLogs[i].first.c_str());
        }
    }

    // End scrolling region
    ImGui::EndChild();



    // =====================================================================================================================
    // Refactoring Logs
    // =====================================================================================================================
    /**
    for (size_t i = 0; i < guiValues.debugLogs.size(); ++i)
    {
        const auto &[logText, copy] = guiValues.debugLogs[i];
        // If the log should be copied, show the button
        ImGui::TextWrapped("%s", logText.c_str());
        if (copy)
        {
            std::string buttonText = "Copy##" + std::to_string(i);
            if (ImGui::Button(buttonText.c_str()))
            {
                // Use SDL clipboard for other platforms
                if (SDL_SetClipboardText(logText.c_str()) == 0)
                {
                    std::cout << "✅ Successfully copied!" << std::endl;
                }
                else
            {
                    std::cerr << "❌ SDL Clipboard Error: " << SDL_GetError() << std::endl;
                }
                addDebugLog("Copied to clipboard", false);
            }
        }         
    }
    **/
}
