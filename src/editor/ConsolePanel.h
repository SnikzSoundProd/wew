// --- CREATE NEW FILE: src/editor/ConsolePanel.h ---
#pragma once
#include "imgui.h"
#include "../core/Log.h"
#include <glm/glm.hpp> // Для ImVec4

class ConsolePanel {
public:
    void render() {
        ImGui::Begin("Console");

        // Кнопка "Clear"
        if (ImGui::Button("Clear")) {
            Log::GetInstance().Clear();
        }
        ImGui::Separator();

        // Область с прокруткой для сообщений
        ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

        const auto& messages = Log::GetInstance().GetMessages();
        for (const auto& msg : messages) {
            ImVec4 color;
            bool has_color = true;
            switch (msg.level) {
                case LogLevel::Error:   color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f); break;
                case LogLevel::Warning: color = ImVec4(1.0f, 0.8f, 0.4f, 1.0f); break;
                default:                has_color = false; break;
            }

            if (has_color) {
                ImGui::PushStyleColor(ImGuiCol_Text, color);
            }
            
            ImGui::TextUnformatted(msg.text.c_str());

            if (has_color) {
                ImGui::PopStyleColor();
            }
        }
        
        // Авто-прокрутка вниз, если мы уже внизу
        if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
            ImGui::SetScrollHereY(1.0f);
        }

        ImGui::EndChild();
        ImGui::End();
    }
};