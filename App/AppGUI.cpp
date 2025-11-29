#include "App.hpp"


void App::render_GUI(FPSMeter& fps_meter, Color& triangle_color, Color& background_color)
{
    ImGui::Begin("FPS Meter", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    {
        // Display current FPS value
        ImGui::Text("FPS: %.1f", fps_meter.get());

        // Show if the value was just updated
        if (fps_meter.is_updated()) {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0, 1, 0, 1), " (Updated)");
        }

        // FPS history graph
        static std::vector<float> fps_history;
        static const int history_size = 100;

        // FIX 1: Use fps_meter (the parameter) not fps_meter_main
        fps_history.push_back(static_cast<float>(fps_meter.get()));
        if (fps_history.size() > history_size) {
            fps_history.erase(fps_history.begin());
        }

        // Plot the FPS history
        ImGui::PlotLines("FPS History", fps_history.data(),
            static_cast<int>(fps_history.size()), 0,
            nullptr, 0.0f, 200.0f,
            ImVec2(200, 50));

        // Controls
        if (ImGui::Button("Reset FPS Counter")) {
            fps_meter.reset();
            fps_history.clear();
        }

        // Interval adjustment
        static float interval_seconds = 1.0f;
        // FIX 2: Use fps_meter (the parameter) not fps_meter_main
        if (ImGui::SliderFloat("Update Interval (s)", &interval_seconds, 0.1f, 5.0f)) {
            fps_meter.set_interval(std::chrono::duration<double>(interval_seconds));
        }
    }
    ImGui::End();

    // Controls Window
    ImGui::Begin("Render Controls");
    {
        // Display info
        ImGui::Text("FOV: %.1f", FOV);

        ImGui::Text("Camera coors: %.1f/%.1f/%.1f", camera.position.x, camera.position.y, camera.position.z);

        ImGui::Separator();

        float triangle_color_arr[3] = { triangle_color.r, triangle_color.g, triangle_color.b };
        if (ImGui::ColorEdit3("Teapot color", triangle_color_arr)) {
            triangle_color.r = triangle_color_arr[0];
            triangle_color.g = triangle_color_arr[1];
            triangle_color.b = triangle_color_arr[2];
        }

        float background_color_arr[3] = { background_color.r, background_color.g, background_color.b };
        if (ImGui::ColorEdit3("Sky color", background_color_arr)) {
            background_color.r = background_color_arr[0];
            background_color.g = background_color_arr[1];
            background_color.b = background_color_arr[2];
            glClearColor(background_color_arr[0], background_color_arr[1], background_color_arr[2], 1.0f);
        }

        if (ImGui::Button("Reset colors")) {
            triangle_color = { 1.0f, 0.6f, 1.0f, 1.0f };
            background_color = { 0.549f, 0.823f, 0.858f };
            glClearColor(background_color.r, background_color.g, background_color.b, 1.0f);
        }

        ImGui::SameLine();

        if (ImGui::Button("Apply teapot color")) {
            texture_library.at(key_tex_singlecolor)->replace_color(
                glm::vec3(triangle_color.r * 255, triangle_color.g * 255, triangle_color.b * 255)
            );
        }

        ImGui::Separator();

        if (ImGui::Button("VSYNC on/off [V]")) {
            enable_or_disable_vsync();
        }
        ImGui::SameLine();
        ImGui::BeginDisabled();
        ImGui::Checkbox("##readonly_checkbox_vsync", &is_vsync_on);
        ImGui::EndDisabled();

        if (ImGui::Button("Mouselook on/off [RMB]")) {
            enable_or_disable_mouselook();
        }
        ImGui::SameLine();
        ImGui::BeginDisabled();
        ImGui::Checkbox("##readonly_checkbox_mouselook", &is_mouselook_on);
        ImGui::EndDisabled();

        if (ImGui::Button("Spectator mode on/off [C]")) {
            is_camera_freeform = !is_camera_freeform;
        }
        ImGui::SameLine();
        ImGui::BeginDisabled();
        ImGui::Checkbox("##readonly_checkbox_freeform", &is_camera_freeform);
        ImGui::EndDisabled();
    }
    ImGui::End();
}
