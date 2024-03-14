#include <stdio.h>
#include <tiny3d.h>

#include "imgui.h"
#include "windows.h"
#include "gui.h"
#include "imgui_impl_tiny3d.h"
#include <dbglogger.h>

bool done = false;
int gui_mode = GUI_MODE_BROWSER;

namespace GUI
{
	int RenderLoop()
	{
		Windows::Init();
		while (!done)
		{
			if (gui_mode == GUI_MODE_BROWSER)
			{
				tiny3d_Clear(0xff000000, TINY3D_CLEAR_ALL);

				// Enable alpha Test
				tiny3d_AlphaTest(1, 0x10, TINY3D_ALPHA_FUNC_GEQUAL);

				// Enable alpha blending.
				tiny3d_BlendFunc(1, (blend_src_func) (TINY3D_BLEND_FUNC_SRC_RGB_SRC_ALPHA | TINY3D_BLEND_FUNC_SRC_ALPHA_SRC_ALPHA),
								(blend_dst_func) (TINY3D_BLEND_FUNC_DST_RGB_ONE_MINUS_SRC_ALPHA | TINY3D_BLEND_FUNC_DST_ALPHA_ZERO),
								(blend_func) (TINY3D_BLEND_RGB_FUNC_ADD | TINY3D_BLEND_ALPHA_FUNC_ADD));

				ImGui_ImplTiny3D_NewFrame();
				ImGui::NewFrame();

				Windows::HandleWindowInput();
				Windows::MainWindow();
				Windows::ExecuteActions();

				ImGui::Render();
				ImGui_ImplTiny3D_RenderDrawData(ImGui::GetDrawData());

				tiny3d_Flip();
			}
			else if (gui_mode == GUI_MODE_IME)
			{
				Windows::HandleImeInput();
			}
		}
		return 0;
	}
}
