#include <cstdint>
#include <thread>
#include <chrono>

#include "../Header/Header.hpp"
#include "../Header/ANwCreator.hpp"
#include "../Render/AImGui.hpp"

#include <android_native_app_glue.h>
android_app *g_App = nullptr;

int32_t (*orig_onInputEvent)(struct android_app *app, AInputEvent *inputEvent);
int32_t onInputEvent(struct android_app *app, AInputEvent *inputEvent)
{
    ImGui_ImplAndroid_HandleInputEvent(inputEvent);
    if (orig_onInputEvent)
        return orig_onInputEvent(app, inputEvent);
    return 0;
}

__attribute__((constructor))
static void MyStart()
{
    std::thread([]
    {
        while (Data.libUE4 <= 0)
        {
            Data.libUE4 = Memory::FindModuleBase("libUE4.so");
            if (Data.libUE4 > 0) break;
            sleep(1);
        }

        g_App = *(android_app **) (Data.libUE4 + 0x19DEFED0);
        while (!g_App)
        {
            LogInfo("g_App is null");
            g_App = *(android_app **) (Data.libUE4 + 0x19DEFED0);
            sleep(1);
        }

        // Hook input event handler
        if (g_App->onInputEvent != onInputEvent)
        {
            orig_onInputEvent = decltype(orig_onInputEvent)(g_App->onInputEvent);
            g_App->onInputEvent = onInputEvent;
        }

        android::AImGui imgui({g_App->activity, false});

        bool state = true, showDemoWindow = false, showAnotherWindow = false;
        while (state)
        {
            imgui.BeginFrame();

            if (showDemoWindow)
                ImGui::ShowDemoWindow(&showDemoWindow);

            {
                static float f = 0.0f;
                static int counter = 0;

                ImGui::Begin("Hello, world!", &state);

                ImGui::Text("This is some useful text.");
                ImGui::Checkbox("Demo Window", &showDemoWindow);
                ImGui::Checkbox("Another Window", &showAnotherWindow);

                ImGui::SliderFloat("float", &f, 0.0f, 1.0f);

                if (ImGui::Button("Button"))
                    counter++;
                ImGui::SameLine();
                ImGui::Text("counter = %d", counter);

                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
                ImGui::End();
            }

            if (showAnotherWindow)
            {
                ImGui::Begin("Another Window", &showAnotherWindow);
                ImGui::Text("Hello from another window!");
                if (ImGui::Button("Close Me"))
                    showAnotherWindow = false;
                ImGui::End();
            }

            imgui.EndFrame();
            std::this_thread::sleep_for(std::chrono::milliseconds(16)); // 60fps
        }
        imgui.Destroy();

    }).detach();
}
