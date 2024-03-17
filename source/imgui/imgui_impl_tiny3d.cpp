
#include "imgui.h"
#include "imgui_impl_tiny3d.h"
#include <tiny3d.h>
#include <io/pad.h>
#include <sys/systime.h>
#include <dbglogger.h>

#define BUTTON_LEFT 0x00000010
#define BUTTON_RIGHT 0x00000020
#define BUTTON_UP 0x00000040
#define BUTTON_DOWN 0x00000080

static u32 previous_down = 0;
static int repeat_count = 0;
static int repeat_delay = 10000000;
static u64 previous_time = 0;

struct ImGui_ImplTiny3D_Data
{
	ImTextureID FontTexture = nullptr;
	u32 *texture_mem;
	u32 *texture_pointer;

	ImGui_ImplTiny3D_Data() { memset(this, 0, sizeof(*this)); }
};

// Backend data stored in io.BackendRendererUserData to allow support for multiple Dear ImGui contexts
// It is STRONGLY preferred that you use docking branch with multi-viewports (== single Dear ImGui context + multiple windows) instead of multiple Dear ImGui contexts.
static ImGui_ImplTiny3D_Data *ImGui_ImplTiny3D_GetBackendData()
{
	return ImGui::GetCurrentContext() ? (ImGui_ImplTiny3D_Data *)ImGui::GetIO().BackendRendererUserData : NULL;
}

bool ImGui_ImplTiny3D_Init()
{
	ImGuiIO &io = ImGui::GetIO();
	IM_ASSERT(io.BackendRendererUserData == NULL && "Already initialized a renderer backend!");

	// setup display
	if (io.DisplaySize.x >= 0.0f && io.DisplaySize.y >= 0.0f)
	{
		tiny3d_UserViewport(1, 0, 0, io.DisplaySize.x, io.DisplaySize.y, 0, 0);
	}
	else
	{
		io.DisplaySize.x = 848;
		io.DisplaySize.y = 512;
	}

	// Setup backend capabilities flags
	ImGui_ImplTiny3D_Data *bd = IM_NEW(ImGui_ImplTiny3D_Data)();
	io.BackendRendererUserData = (void *)bd;
	io.BackendPlatformName = "PS3";
	io.BackendRendererName = "imgui_impl_tiny3d";
	io.BackendFlags |= ImGuiBackendFlags_HasGamepad;

	bd->texture_mem = (u32 *)tiny3d_AllocTexture(64 * 1024 * 1024);
	if (!bd->texture_mem)
		return false;
	bd->texture_pointer = bd->texture_mem;

	return true;
}

void ImGui_ImplTiny3D_Shutdown()
{
	ImGui_ImplTiny3D_Data *bd = ImGui_ImplTiny3D_GetBackendData();
	IM_ASSERT(bd != NULL && "No renderer backend to shutdown, or already shutdown?");
	ImGuiIO &io = ImGui::GetIO();

	ImGui_ImplTiny3D_DestroyDeviceObjects();
	io.BackendRendererName = NULL;
	io.BackendRendererUserData = NULL;
	IM_DELETE(bd);
}

void ImGui_ImplTiny3D_UpdateGamepads()
{
	ImGuiIO &io = ImGui::GetIO();

	padInfo padinfo;
	padData paddata;
	// Check the pads.
	ioPadGetInfo(&padinfo);

	#define MAP_BUTTON(KEY_NO, BUTTON_NO) {io.AddKeyEvent(KEY_NO, BUTTON_NO);}
	#define IM_SATURATE(V)                      (V < 0.0f ? 0.0f : V > 1.0f ? 1.0f : V)
	#define MAP_ANALOG(KEY_NO, AXIS_NO, V0, V1) { float vn = (float)(AXIS_NO - V0) / (float)(V1 - V0); vn = IM_SATURATE(vn); io.AddKeyAnalogEvent(KEY_NO, vn > 0.1f, vn); }
	const int thumb_dead_zone = 40;

	for (int i = 0; i < MAX_PADS; i++)
	{
		if (padinfo.status[i])
		{
			ioPadGetData(i, &paddata);
			u64 sec, usec;
			sysGetCurrentTime(&sec, &usec);
			u64 current_time = ((sec - 1710000000) * 1000000000) + usec;

			if (paddata.len >= 8)
			{
				u32 down = 0;
				if (paddata.ANA_L_H < 127-80)
					down |= BUTTON_LEFT;
				else if (paddata.ANA_L_H  > 127+80)
					down |= BUTTON_RIGHT;
				else if (paddata.ANA_L_V  > 127+80)
					down |= BUTTON_DOWN;
				else if (paddata.ANA_L_V < 127-80)
					down |= BUTTON_UP;

				u32 pressed = down & ~previous_down;
				if (previous_down == down)
				{
					u64 delay = 200000000;
					if (repeat_count > 0)
						delay = repeat_delay;
					if (current_time - previous_time > delay)
					{
						pressed = down;
						previous_time = current_time;
						repeat_count++;
					}
				}
				else
				{
					repeat_count = 0;
				}

				if (pressed & BUTTON_LEFT)
					io.AddKeyEvent(ImGuiKey_GamepadDpadLeft, true);
				if (pressed & BUTTON_RIGHT)
					io.AddKeyEvent(ImGuiKey_GamepadDpadRight, true);
				if (pressed & BUTTON_UP)
					io.AddKeyEvent(ImGuiKey_GamepadDpadUp, true);
				if (pressed & BUTTON_DOWN)
					io.AddKeyEvent(ImGuiKey_GamepadDpadDown, true);
				previous_down = down;

				MAP_BUTTON(ImGuiKey_GamepadStart, paddata.BTN_START);
				MAP_BUTTON(ImGuiKey_GamepadBack, paddata.BTN_SELECT);
				MAP_BUTTON(ImGuiKey_Keypad0, paddata.BTN_SQUARE);
				MAP_BUTTON(ImGuiKey_GamepadFaceRight, paddata.BTN_CIRCLE);
				MAP_BUTTON(ImGuiKey_GamepadFaceUp, paddata.BTN_TRIANGLE);
				MAP_BUTTON(ImGuiKey_GamepadFaceDown, paddata.BTN_CROSS);
				MAP_BUTTON(ImGuiKey_GamepadDpadLeft, paddata.BTN_LEFT);
				MAP_BUTTON(ImGuiKey_GamepadDpadRight, paddata.BTN_RIGHT);
				MAP_BUTTON(ImGuiKey_GamepadDpadUp, paddata.BTN_UP);
				MAP_BUTTON(ImGuiKey_GamepadDpadDown, paddata.BTN_DOWN);
				MAP_BUTTON(ImGuiKey_GamepadL1, paddata.BTN_L1);
				MAP_BUTTON(ImGuiKey_GamepadR1, paddata.BTN_R1);
				MAP_BUTTON(ImGuiKey_GamepadL2, paddata.BTN_L2);
				MAP_BUTTON(ImGuiKey_GamepadR2, paddata.BTN_R2);
				MAP_BUTTON(ImGuiKey_GamepadL3, paddata.BTN_L3);
				MAP_BUTTON(ImGuiKey_GamepadR3, paddata.BTN_R3);
				/*
				MAP_ANALOG(ImGuiKey_GamepadLStickLeft,  paddata.ANA_R_H-128, -thumb_dead_zone, -127);
				MAP_ANALOG(ImGuiKey_GamepadLStickRight, paddata.ANA_R_H-128, +thumb_dead_zone, +127);
				MAP_ANALOG(ImGuiKey_GamepadLStickUp,    paddata.ANA_R_V-128, -thumb_dead_zone, -127);
				MAP_ANALOG(ImGuiKey_GamepadLStickDown,  paddata.ANA_R_V-128, +thumb_dead_zone, +127);
				*/
			}
		}
	}
    #undef MAP_BUTTON
    #undef MAP_ANALOG
}

void ImGui_ImplTiny3D_NewFrame()
{
	ImGui_ImplTiny3D_Data *bd = ImGui_ImplTiny3D_GetBackendData();
	IM_ASSERT(bd != NULL && "Did you call ImGui_ImplTiny3D_Init()?");

	if (!bd->FontTexture)
		ImGui_ImplTiny3D_CreateDeviceObjects();

	ImGui_ImplTiny3D_UpdateGamepads();
}

static void ImGui_ImplTiny3D_SetupRenderState(ImDrawData *draw_data, int fb_width, int fb_height)
{
}

void ImGui_ImplTiny3D_RenderDrawData(ImDrawData *draw_data)
{
	// Render command lists
	for (int n = 0; n < draw_data->CmdListsCount; n++)
	{
		const ImDrawList *cmd_list = draw_data->CmdLists[n];
		const ImDrawVert *vtx_buffer = cmd_list->VtxBuffer.Data;
		const ImDrawIdx *idx_buffer = cmd_list->IdxBuffer.Data;

		for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
		{
			const ImDrawCmd *pcmd = &cmd_list->CmdBuffer[cmd_i];
			if (pcmd->UserCallback)
			{
				pcmd->UserCallback(cmd_list, pcmd);
			}
			else
			{
				Tiny3DTexture *texture = pcmd->TextureId;
				tiny3d_SetTexture(0, texture->offset, texture->width, texture->height, texture->pitch, TINY3D_TEX_FORMAT_A8R8G8B8, TEXTURE_LINEAR);

				tiny3d_SetPolygon(TINY3D_TRIANGLES);
				u8 *indices = (u8 *)idx_buffer;

				for (int idx = 0; idx < pcmd->ElemCount; idx++)
				{
					u16 index = *((u16 *)(indices + sizeof(ImDrawIdx) * idx));
					float *vertices = (float *)((const char *)vtx_buffer + IM_OFFSETOF(ImDrawVert, pos) + sizeof(ImDrawVert) * index);
					float *texcoords = (float *)((const char *)vtx_buffer + IM_OFFSETOF(ImDrawVert, uv) + sizeof(ImDrawVert) * index);
					u32 *colors = (u32 *)((const char *)vtx_buffer + IM_OFFSETOF(ImDrawVert, col) + sizeof(ImDrawVert) * index);

					u32 rgba = ((*colors & 0xFF000000) >> 24) |
									((*colors & 0x00FF0000) >> 8) |
									((*colors & 0x0000FF00) << 8) |
									((*colors & 0x000000FF) << 24);
					tiny3d_VertexPos(vertices[0], vertices[1], 1.0f);
					tiny3d_VertexTexture(texcoords[0], texcoords[1]);
					tiny3d_VertexColor(rgba);
				}
				tiny3d_End();
			}
			idx_buffer += pcmd->ElemCount;
		}
	}
}

bool ImGui_ImplTiny3D_CreateFontsTexture()
{
	// Build texture atlas
	ImGuiIO &io = ImGui::GetIO();
	ImGui_ImplTiny3D_Data *bd = ImGui_ImplTiny3D_GetBackendData();
	unsigned char *pixels;
	int width, height, bpp;
	io.Fonts->AddFontDefault();
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height, &bpp); // Load as RGBA 32-bit (75% of the memory is wasted, but default font is so small) because it is more likely to be compatible with user's existing shaders. If your ImTextureId represent a higher-level concept than just a GL texture id, consider calling GetTexDataAsAlpha8() instead to save on GPU memory.

	bd->FontTexture = (Tiny3DTexture *)malloc(sizeof(Tiny3DTexture));
	memset(bd->FontTexture, 0, sizeof(Tiny3DTexture));
	RSX_MEMCPY(bd->texture_pointer, pixels, width * bpp * height);

	bd->FontTexture->offset = tiny3d_TextureOffset(bd->texture_pointer);
	bd->FontTexture->width = width;
	bd->FontTexture->height = height;
	bd->FontTexture->pitch = width * bpp;

	bd->texture_pointer += width * bpp * height;

	// Store our identifier
	io.Fonts->SetTexID((ImTextureID)(intptr_t)bd->FontTexture);

	return true;
}

void ImGui_ImplOpenTiny3D_DestroyFontsTexture()
{
	ImGuiIO &io = ImGui::GetIO();
	ImGui_ImplTiny3D_Data *bd = ImGui_ImplTiny3D_GetBackendData();
	if (bd->FontTexture)
	{
		free(bd->FontTexture);
		io.Fonts->SetTexID(0);
		bd->FontTexture = 0;
	}
}

bool ImGui_ImplTiny3D_CreateDeviceObjects()
{
	return ImGui_ImplTiny3D_CreateFontsTexture();
}

void ImGui_ImplTiny3D_DestroyDeviceObjects()
{
	ImGui_ImplOpenTiny3D_DestroyFontsTexture();
}

void ImGui_ImplTiny3D_InvalidateDeviceObjects()
{
}