#pragma once

namespace cfg_t
{
	inline bool t_show_menu = false;
	inline int menu_key = VK_INSERT;

	int screen_width = GetSystemMetrics(SM_CXSCREEN);;
	int screen_height = GetSystemMetrics(SM_CYSCREEN);;

	namespace aimbot
	{

		inline bool aimbot_t = true;
		inline bool triggerbot_t = false;
		inline bool visible_check = true;
		inline bool show_radius = true;
		inline bool nearest_bone = false;

		namespace settings
		{
			inline float radius_value = 100.f;
			inline float smoothnes = 5.f;
			inline float distance = FLT_MAX;
			inline int hitbox = 0;
			inline const char* bone_names[]{ ("head"), ("neck"), ("torso"), ("left arm"), ("right arm"), ("left leg"), ("right leg") };
			int aimbot_key = VK_RBUTTON;
			int triggerbot_key = VK_RBUTTON;
		}
	}

	namespace visuals {
		inline bool skeleton = true;
		inline bool box_2d = true;
		inline bool corner_box = true;
		inline bool snap_lines = true;
		inline bool fov_arrows = true;
		inline bool username = true;
		inline bool platform = true;
		inline bool distance = true;
		inline bool radar = true;
		inline bool outline = false;

		namespace settings {
			float visible_color[4] = { 0.f / 255.f, 255.f / 255.f, 0.f / 255.f, 1.f };
			float invisible_color[4] = { 1.f / 255, 0.f / 255, 0.f / 255, 1.f };

			float skeleton_color[4] = { 0.f / 255.f, 255.f / 255.f, 0.f / 255.f, 1.f };
			float box_color[4] = { 0.f / 255.f, 255.f / 255.f, 0.f / 255.f, 1.f };

			int skeleton_thickness = 1.0;

			int box_style = 1;
		}

	}

	inline float render_distance = 200.0f;
}