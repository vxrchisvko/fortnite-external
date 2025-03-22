#pragma once

#include <core/cheat/aimbot/keybind.h>

class aimbot_t
{
public:
	inline void move( fvector2d head_2d )
	{
		const float aim_speed = cfg_t::aimbot::settings::smoothnes;
		const fvector2d screen_center = { static_cast<double>( cfg_t::screen_width ) / 2, static_cast<double>( cfg_t::screen_height ) / 2 };
		fvector2d target = { 0, 0 };

		if ( head_2d.x != 0 ) {
			target.x = ( head_2d.x > screen_center.x) ? -(screen_center.x - head_2d.x) : (head_2d.x - screen_center.x );
			target.x /= aim_speed;
			target.x = ( target.x + screen_center.x > screen_center.x * 2 || target.x + screen_center.x < 0 ) ? 0 : target.x;
		}
		if (head_2d.y != 0) {
			target.y = ( head_2d.y > screen_center.y) ? -(screen_center.y - head_2d.y ) : ( head_2d.y - screen_center.y );
			target.y /= aim_speed;
			target.y = ( target.y + screen_center.y > screen_center.y * 2 || target.y + screen_center.y < 0 ) ? 0 : target.y;
		}

		driver.move_mouse( target.x, target.y );
	}
}; aimbot_t aimbot;