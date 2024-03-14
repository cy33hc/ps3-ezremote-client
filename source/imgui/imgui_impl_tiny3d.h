
#include "imgui.h"      // IMGUI_IMPL_API

IMGUI_API bool        ImGui_ImplTiny3D_Init();
IMGUI_API void        ImGui_ImplTiny3D_Shutdown();
IMGUI_API void        ImGui_ImplTiny3D_NewFrame();
IMGUI_API void        ImGui_ImplTiny3D_RenderDrawData(ImDrawData* draw_data);
//IMGUI_API bool        ImGui_ImplPSGL_ProcessEvent(SceCtrlData* pad);

// Use if you want to reset your rendering device without losing ImGui state.
IMGUI_API void        ImGui_ImplTiny3D_DestroyDeviceObjects();
IMGUI_API void        ImGui_ImplTiny3D_InvalidateDeviceObjects();
IMGUI_API bool        ImGui_ImplTiny3D_CreateDeviceObjects();