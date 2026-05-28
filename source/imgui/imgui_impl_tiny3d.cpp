
#include "imgui.h"
#include "imgui_impl_tiny3d.h"
#include <tiny3d.h>
#include <io/pad.h>
#include <sys/systime.h>

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

// ---------------------------------------------------------------------------
// Software clipping helpers
// ---------------------------------------------------------------------------

struct ClipVertex
{
	float x, y;
	float u, v;
	u32   col; // already byte-swapped to RGBA for tiny3d
};

// Clip a convex polygon against one half-plane defined by:
//   axis=0 -> clip x >= limit  (left edge)
//   axis=0 -> clip x <= limit  (right edge, negate)
//   axis=1 -> clip y >= limit  (top edge)
//   axis=1 -> clip y <= limit  (bottom edge, negate)
// We use the Sutherland-Hodgman single-plane clip.
// inside(v) == true means the vertex is on the kept side.
static int ClipPolygonAgainstPlane(
	const ClipVertex *in, int in_count,
	ClipVertex *out,
	int axis, float limit, bool keep_greater)
{
	if (in_count == 0)
		return 0;

	int out_count = 0;

	auto inside = [&](const ClipVertex &v) -> bool {
		float val = (axis == 0) ? v.x : v.y;
		return keep_greater ? (val >= limit) : (val <= limit);
	};

	auto lerp_vert = [](const ClipVertex &a, const ClipVertex &b, float t) -> ClipVertex {
		ClipVertex r;
		r.x   = a.x   + t * (b.x   - a.x);
		r.y   = a.y   + t * (b.y   - a.y);
		r.u   = a.u   + t * (b.u   - a.u);
		r.v   = a.v   + t * (b.v   - a.v);
		// Interpolate each colour channel independently
		u8 a_r = (a.col >> 24) & 0xFF, b_r = (b.col >> 24) & 0xFF;
		u8 a_g = (a.col >> 16) & 0xFF, b_g = (b.col >> 16) & 0xFF;
		u8 a_b = (a.col >>  8) & 0xFF, b_b = (b.col >>  8) & 0xFF;
		u8 a_a = (a.col      ) & 0xFF, b_a = (b.col      ) & 0xFF;
		u8 r_r = (u8)(a_r + t * (b_r - a_r));
		u8 r_g = (u8)(a_g + t * (b_g - a_g));
		u8 r_b = (u8)(a_b + t * (b_b - a_b));
		u8 r_a = (u8)(a_a + t * (b_a - a_a));
		r.col = ((u32)r_r << 24) | ((u32)r_g << 16) | ((u32)r_b << 8) | r_a;
		return r;
	};

	for (int i = 0; i < in_count; i++)
	{
		const ClipVertex &cur  = in[i];
		const ClipVertex &next = in[(i + 1) % in_count];

		bool cur_in  = inside(cur);
		bool next_in = inside(next);

		if (cur_in)
		{
			out[out_count++] = cur;
			if (!next_in)
			{
				// Compute intersection
				float cur_val  = (axis == 0) ? cur.x  : cur.y;
				float next_val = (axis == 0) ? next.x : next.y;
				float t = (limit - cur_val) / (next_val - cur_val);
				out[out_count++] = lerp_vert(cur, next, t);
			}
		}
		else if (next_in)
		{
			// Entering: emit intersection
			float cur_val  = (axis == 0) ? cur.x  : cur.y;
			float next_val = (axis == 0) ? next.x : next.y;
			float t = (limit - cur_val) / (next_val - cur_val);
			out[out_count++] = lerp_vert(cur, next, t);
		}
	}
	return out_count;
}

// Clip a triangle (3 vertices) against all 4 edges of clip_rect.
// Returns the resulting polygon in 'out' (up to 7 vertices for a triangle
// clipped against 4 planes). Returns vertex count (0 if fully clipped).
static int ClipTriangle(
	const ClipVertex tri[3],
	ClipVertex *out,
	float clip_x0, float clip_y0, float clip_x1, float clip_y1)
{
	// Temporary buffers: a triangle clipped against 4 planes yields at most 7 verts
	ClipVertex buf0[8], buf1[8];

	buf0[0] = tri[0];
	buf0[1] = tri[1];
	buf0[2] = tri[2];
	int count = 3;

	count = ClipPolygonAgainstPlane(buf0, count, buf1, 0, clip_x0, true);  // x >= x0
	if (count == 0) return 0;
	count = ClipPolygonAgainstPlane(buf1, count, buf0, 0, clip_x1, false); // x <= x1
	if (count == 0) return 0;
	count = ClipPolygonAgainstPlane(buf0, count, buf1, 1, clip_y0, true);  // y >= y0
	if (count == 0) return 0;
	count = ClipPolygonAgainstPlane(buf1, count, out,  1, clip_y1, false); // y <= y1
	return count;
}

// ---------------------------------------------------------------------------

void ImGui_ImplTiny3D_RenderDrawData(ImDrawData *draw_data)
{
	// Render command lists
	for (int n = 0; n < draw_data->CmdListsCount; n++)
	{
		const ImDrawList *cmd_list = draw_data->CmdLists[n];
		const ImDrawVert *vtx_buffer = cmd_list->VtxBuffer.Data;
		const ImDrawIdx  *idx_buffer = cmd_list->IdxBuffer.Data;

		for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
		{
			const ImDrawCmd *pcmd = &cmd_list->CmdBuffer[cmd_i];
			if (pcmd->UserCallback)
			{
				pcmd->UserCallback(cmd_list, pcmd);
			}
			else
			{
				Tiny3DTexture *texture = (Tiny3DTexture *)pcmd->TextureId;
				tiny3d_SetTexture(0, texture->offset, texture->width, texture->height, texture->pitch, TINY3D_TEX_FORMAT_A8R8G8B8, TEXTURE_LINEAR);

				// Clip rect for this draw command (in screen-space pixels)
				const float clip_x0 = pcmd->ClipRect.x;
				const float clip_y0 = pcmd->ClipRect.y;
				const float clip_x1 = pcmd->ClipRect.z;
				const float clip_y1 = pcmd->ClipRect.w;

				const u8 *indices = (const u8 *)idx_buffer;
				bool polygon_open = false;

				// Process one triangle (3 indices) at a time
				for (int idx = 0; idx + 2 < (int)pcmd->ElemCount; idx += 3)
				{
					ClipVertex tri[3];

					for (int v = 0; v < 3; v++)
					{
						u16 index = *((const u16 *)(indices + sizeof(ImDrawIdx) * (idx + v)));
						const ImDrawVert &vert = vtx_buffer[index];

						u32 col = vert.col;
						// ImGui stores RGBA; tiny3d expects RGBA with bytes in big-endian order
						u32 rgba = ((col & 0xFF000000) >> 24) |
						           ((col & 0x00FF0000) >>  8) |
						           ((col & 0x0000FF00) <<  8) |
						           ((col & 0x000000FF) << 24);

						tri[v].x   = vert.pos.x;
						tri[v].y   = vert.pos.y;
						tri[v].u   = vert.uv.x;
						tri[v].v   = vert.uv.y;
						tri[v].col = rgba;
					}

					// Software-clip the triangle against the command's clip rect
					ClipVertex clipped[8];
					int count = ClipTriangle(tri, clipped, clip_x0, clip_y0, clip_x1, clip_y1);
					if (count == 0)
						continue;

					// Open the polygon block lazily on the first visible triangle
					if (!polygon_open)
					{
						tiny3d_SetPolygon(TINY3D_TRIANGLES);
						polygon_open = true;
					}

					// Fan-triangulate the resulting polygon and emit vertices
					for (int t = 1; t + 1 < count; t++)
					{
						const ClipVertex *fan[3] = { &clipped[0], &clipped[t], &clipped[t + 1] };
						for (int v = 0; v < 3; v++)
						{
							tiny3d_VertexPos(fan[v]->x, fan[v]->y, 1.0f);
							tiny3d_VertexTexture(fan[v]->u, fan[v]->v);
							tiny3d_VertexColor(fan[v]->col);
						}
					}
				}
				if (polygon_open)
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