# AndroidImGui-UE

An Android shared library that renders an **ImGui overlay on top of Unreal Engine 4 games** at runtime. It creates an independent system-level surface using the Android `SurfaceControl` API and renders via OpenGL ES 3, without modifying the game's own render pipeline.

> Requires **Android 11+ (API 30+)**
> Targets **arm64-v8a**

---

## How It Works

1. The library is injected into the target process. A `__attribute__((constructor))` entry point spawns a background thread.
2. The thread polls until `libUE4.so` is loaded, then reads the `android_app*` from a known offset.
3. Once valid, it hooks `android_app::onInputEvent` to forward all touch/key events to ImGui.
4. An `ANativeWindow` overlay is created via `ANwCreator` (JNI `SurfaceControl.Builder`) at the maximum z-layer (`0x7FFFFFFE`), so it renders on top of everything.
5. An EGL context is set up on that window, and Dear ImGui renders at ~60 FPS using OpenGL ES 3 (`#version 300 es`).
6. Display rotation changes are detected automatically — the EGL environment re-initialises seamlessly.

---

## Requirements

- [Android NDK](https://developer.android.com/ndk) (r23+)
- CMake 3.22.1+
- Ninja
- `NDK_PATH` environment variable set to your NDK root

---

## Build

```bat
set NDK_PATH=C:\path\to\your\ndk
build.bat
```

Output: `build\libs\arm64-v8a\libProject.so`

To push to device:
```bat
push.bat
```

---

## Project Structure

```
AndroidImGui-UE/
├── CMakeLists.txt              # Build definition (shared library)
├── build.bat                   # Windows build script
├── push.bat                    # ADB push script
├── Main/
│   └── Entry.cpp               # .so entry point + render loop + input hook
├── Header/
│   ├── Header.hpp              # Common includes & Android log macros
│   ├── Structs.hpp             # GameData (libUE4 base address)
│   ├── Memory.hpp              # dl_iterate_phdr module finder
│   └── ANwCreator.hpp          # JNI-based ANativeWindow overlay creator
└── Render/
    ├── AImGui.hpp / AImGui.cpp # EGL + Dear ImGui lifecycle wrapper
    └── ImGui/                  # Dear ImGui (imgui_impl_android + opengl3)
```

---

## Porting to a Different Game / UE4 Version

The only game-specific value is the offset to the `android_app*` inside `libUE4.so` in `Main/Entry.cpp`:

```cpp
g_App = *(android_app **)(Data.libUE4 + 0x19DEFED0); // 0x19DEFED0 is the offset of GNativeAndroidApp in libUE4.so (this offset is for Garena Delta Force version 1.203.37110.3901)
```

Find the correct offset for your target build and update this line.

---

## Credits

Inspired by **[AndroidSurfaceImgui](https://github.com/Bzi-Han/AndroidSurfaceImgui)** by [Bzi-Han](https://github.com/Bzi-Han).

This project is a fork of **[AndroidImGui-UE](https://github.com/TinyTime-Joy/AndroidImGui-UE)**.
