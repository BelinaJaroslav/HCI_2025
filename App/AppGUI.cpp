#include "App.hpp"


void App::render_GUI()
{
    ImVec2 viewport_size = ImGui::GetMainViewport()->Size;

    // = WINDOW 1 :: FPS and GL info =
    ImGui::SetNextWindowPos(ImVec2(viewport_size.x, 0.0f), 0, ImVec2(1.0f, 0.0f));
    ImGui::Begin("FPS Meter and GL info", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    {
        // Display current FPS value
        ImGui::Text("FPS: %.1f", fps_meter_main.get());

        // Show if the value was just updated
        if (fps_meter_main.is_updated()) {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0, 1, 0, 1), " (Updated)");
        }

        // FPS history graph
        static std::vector<float> fps_history;
        static const int history_size = 100;
        fps_history.push_back(static_cast<float>(fps_meter_main.get()));
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
            fps_meter_main.reset();
            fps_history.clear();
        }

        // Interval adjustment
        static float interval_seconds = 1.0f;
        if (ImGui::SliderFloat("Update Interval [s]", &interval_seconds, 0.1f, 5.0f)) {
            fps_meter_main.set_interval(std::chrono::duration<double>(interval_seconds));
        }

        // GL info        
        ImGui::Separator();
        if (ImGui::CollapsingHeader("GL info")) {
            if (ImGui::BeginTable("table", 2)) {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Vendor");
                    ImGui::TableNextColumn();
                    ImGui::Text("%s", gl_info_vendor);

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Renderer");
                    ImGui::TableNextColumn();
                    ImGui::Text("%s", gl_info_renderer);

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Version");
                    ImGui::TableNextColumn();
                    ImGui::Text("%s", gl_info_version);

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Shading version");
                    ImGui::TableNextColumn();
                    ImGui::Text("%s", gl_info_shading_version);

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Texture units");
                    ImGui::TableNextColumn();
                    ImGui::Text("%d", gl_info_n_texture_units);

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Profile");
                    ImGui::TableNextColumn();
                    ImGui::Text("%s", gl_info_profile);
                ImGui::EndTable();
            }
        }
        // Development help
        if (ImGui::CollapsingHeader("Dev tools")) {
            ImGui::SeparatorText("Number of detected faces");
            ImGui::RadioButton("Get from webcam", &n_faces_found_debug_override, -1);
            ImGui::SameLine();
            ImGui::RadioButton("Force 0", &n_faces_found_debug_override, 0);
            ImGui::SameLine();
            ImGui::RadioButton("Force 1", &n_faces_found_debug_override, 1);
            ImGui::SameLine();
            ImGui::RadioButton("Force 2", &n_faces_found_debug_override, 2);
        }
    }
    ImGui::End();

    // = WINDOW 2 :: Controls =
    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), 0, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    {
        // Display info
        ImGui::Text("[Scrollwheel] FOV: %.1f", FOV);

        ImGui::Text("[W/A/S/D/(Q)/(E)] Camera coors: %.1f/%.1f/%.1f", camera.position.x, camera.position.y, camera.position.z);     

        ImGui::Checkbox("[LMB] :: Flashlight on/off", &is_flashlight_on);

        if (ImGui::Checkbox("[RMB] :: Mouselook on/off", &is_mouselook_on)) {
            enable_or_disable_mouselook(false);
        }

        if (ImGui::Checkbox("[F] :: Fullscreen on/off", &is_fullscreen_on)) {
            enable_or_disable_fullscreen(false);
        }

        ImGui::Checkbox("[C] :: Spectator mode on/off", &is_camera_freeform);

        if (ImGui::Checkbox("[V] :: VSYNC on/off", &is_vsync_on)) {
            enable_or_disable_vsync(false);
        }

        if (ImGui::Checkbox("[B] :: Anti-aliasing on/off", &is_antialiasing_on)) {
            enable_or_disable_antialiasing(false);
        }

        if (ImGui::Button("[1] Take a screenshot")) {
            save_screenshot();
        }
    }
    ImGui::End();

    // = WINDOW 3 :: webcam =
    ImGui::SetNextWindowPos(viewport_size, 0, ImVec2(1.0f, 1.0f));
    ImGui::Begin("Live reaction", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    const float scale = 0.75f;

    ImGui::Image(
        (ImTextureID)(intptr_t)texture_library.at(key_tex_webcam)->get_name(),
        ImVec2(texture_library.at(key_tex_webcam)->get_width()* scale, texture_library.at(key_tex_webcam)->get_height()* scale)
    );
    
    ImGui::Text("No. of detected faces: %d", n_faces_found);
    ImGui::End();
}
