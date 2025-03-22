#pragma once
#include <dependencies/framework/imgui.h>
#include <dependencies/encryption/hash.hpp>
#include <core/cheat/config/config.h>
#include <core/cheat/aimbot/keybind.h>


class menu_t {
public :
	void render_menu( );

    static menu_t& instance( ) {
        static menu_t instance;
        return instance;
    }

private:
    menu_t( ) = default;
    menu_t( const menu_t& ) = delete;
    menu_t& operator=( const menu_t& ) = delete;
};

auto menu_t::render_menu( ) -> void {
    const char* hitboxes[] = { "head", "neck", "chest", "pelvis", "random" };

    ImGui::SetNextWindowSize( ImVec2( 740, 530 ) );
    ImGui::Begin( hash_string( "fortnite" ), NULL );

    ImGui::Checkbox( hash_string( "enable aimbot" ), &cfg_t::aimbot::aimbot_t );
    ImGui::Checkbox( hash_string( "visible check" ), &cfg_t::aimbot::visible_check );
    ImGui::Checkbox( hash_string( "show radius" ), &cfg_t::aimbot::show_radius );
    ImGui::Spacing( );
    ImGui::SliderFloat( hash_string( "##radius_value" ), &cfg_t::aimbot::settings::radius_value, 30, 150, "%.1f" );
    ImGui::SliderFloat( hash_string( "##smoothness" ), &cfg_t::aimbot::settings::smoothnes, 1, 20, "%.1f" );
    ImGui::Combo( hash_string( "##hitbox" ), &cfg_t::aimbot::settings::hitbox, hitboxes, IM_ARRAYSIZE( hitboxes ) );

    ImGui::Checkbox(hash_string("box"), &cfg_t::visuals::box_2d);
    ImGui::Checkbox(hash_string("corner"), &cfg_t::visuals::corner_box);
    ImGui::Checkbox(hash_string("skeleton"), &cfg_t::visuals::skeleton);
    HotkeyButton(cfg_t::aimbot::settings::aimbot_key, ChangeKey, keystatus);

    ImGui::End( );
}