cmake_minimum_required(VERSION 3.22.1)

set(pName Project)

project(${pName})

set(CMAKE_CXX_STANDARD 20)
add_compile_options(-fno-rtti -fno-exceptions -fvisibility=hidden)
add_link_options(-s)

# android_native_app_glue (equivalent of: $(call import-module,android/native_app_glue))
add_library(android_native_app_glue STATIC
    ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)
target_include_directories(android_native_app_glue PUBLIC
    ${ANDROID_NDK}/sources/android/native_app_glue)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} Header Render/ImGui Render/ImGui/backends
    ${ANDROID_NDK}/sources/android/native_app_glue)

add_library(${pName} SHARED 
    Main/Entry.cpp
    Render/AImGui.cpp
    Render/ImGui/imgui.cpp
    Render/ImGui/imgui_demo.cpp
    Render/ImGui/imgui_draw.cpp
    Render/ImGui/imgui_widgets.cpp
    Render/ImGui/imgui_tables.cpp
    Render/ImGui/backends/imgui_impl_android.cpp
    Render/ImGui/backends/imgui_impl_opengl3.cpp
)

target_link_libraries(${pName} android_native_app_glue log android EGL GLESv3 dl)

set_target_properties(${pName} PROPERTIES LIBRARY_OUTPUT_DIRECTORY "libs/${ANDROID_ABI}")
