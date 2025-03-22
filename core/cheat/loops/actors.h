#pragma once
#include <core/cheat/loops/cache.h>
#include <core/sdk/functions.h>
#include <dependencies/framework/imgui.h>
#include <core/cheat/config/config.h>
#include <dependencies/encryption/xor.hpp>
#include <core/cheat/aimbot/aimbot.h>

class actors_t {
public:
    void execute_loop( );
    void render_elements( );

    static actors_t& instance( ) {
        static actors_t instance;
        return instance;
    }

private:
    actors_t( ) = default;
    actors_t( const actors_t& ) = delete;
    actors_t& operator=( const actors_t& ) = delete;
};

auto actors_t::render_elements( ) -> void {
    if ( cfg_t::aimbot::show_radius )
    {
        ImDrawList* drawList = ImGui::GetBackgroundDrawList( );
        ImVec2 center = ImVec2( cfg_t::screen_width / 2, cfg_t::screen_height / 2 );

        drawList->AddCircle( center, cfg_t::aimbot::settings::radius_value + ( 1 / 2 ), ImColor( 255, 255, 255, 255 ), 30, 2.f );
    }

    ImGuiIO& io = ImGui::GetIO( );
    float fps = io.Framerate;
    char fps_text[64];

    snprintf( fps_text, sizeof( fps_text ), "FPS: %.1f", fps);
    ImVec2 text_position(10.f, 10.f);
    ImGui::GetForegroundDrawList( )->AddText( text_position, ImColor( 255, 255, 255, 255 ), fps_text );
}

uintptr_t target_entity = NULL;
float target_distance = FLT_MAX;

auto is_in_screen( fvector2d screen_position ) -> bool {
    if ( screen_position.x > 0 && screen_position.x < cfg_t::screen_width && screen_position.y > 0 && screen_position.y < cfg_t::screen_height )
        return true;
    else
        return false;
}

auto actors_t::execute_loop( ) -> void {
    target_distance = FLT_MAX;
    target_entity = NULL;
    for ( int i = 0; i < pointers.player_array_size; ++i ) {
        uintptr_t player_state = driver.read<uintptr_t>( pointers.player_array + ( i * sizeof( uintptr_t ) ));
        uintptr_t current_actor = driver.read<uintptr_t>( player_state + offsets.pawn_private );
        uintptr_t actors_mesh = driver.read<uintptr_t>( current_actor + offsets.mesh );
        uintptr_t otherplaystate = driver.read<uintptr_t>( current_actor + offsets.player_state );

        if ( current_actor == pointers.local_pawn ) continue;
        if ( !actors_mesh ) continue;

        fvector check_bone( game.get_bone_location( actors_mesh, 110 ) );

        if ( !is_in_screen( game.world_to_screen( check_bone ) ) ) continue;

       /* auto team_index = driver.read<int>( otherplaystate + offsets.team_index );
        if ( pointers.team_index == team_index ) continue;*/

        ImDrawList* drawlist = ImGui::GetBackgroundDrawList( );

        fvector root_bone = game.get_bone_location( actors_mesh, 0 );
        fvector2d root_box = game.world_to_screen( fvector( root_bone.x, root_bone.y, root_bone.z - 15 ) );
        fvector head_bone = game.get_bone_location( actors_mesh, 110 );
        fvector2d head_box = game.world_to_screen( fvector( head_bone.x, head_bone.y, head_bone.z + 15 ) );
        fvector2d head = game.world_to_screen( head_bone );
        fvector2d root = game.world_to_screen( root_bone );

        float distance = camera_position.location.distance( root_bone ) / 100;
        float dynamic_font_size = 15.0f - ( distance / 50.0f );
        if ( dynamic_font_size < 10.0f ) {
            dynamic_font_size = 10.0f;
        }

        ImColor box_color, skeleton_color;
        if ( game.is_entity_visible( actors_mesh ) ) {
            box_color = ImGui::GetColorU32( { cfg_t::visuals::settings::visible_color[0],
                cfg_t::visuals::settings::visible_color[1],
                cfg_t::visuals::settings::visible_color[2],
                1.0f }
            );

            skeleton_color = ImGui::GetColorU32( { cfg_t::visuals::settings::visible_color[0],
                cfg_t::visuals::settings::visible_color[1],
                cfg_t::visuals::settings::visible_color[2],
                1.0f }
            );
        }
        else
        {
            box_color = ImGui::GetColorU32( { cfg_t::visuals::settings::invisible_color[0],
                cfg_t::visuals::settings::invisible_color[1],
                cfg_t::visuals::settings::invisible_color[2],
                1.0f }
            );

            skeleton_color = ImGui::GetColorU32( { cfg_t::visuals::settings::invisible_color[0],
                cfg_t::visuals::settings::invisible_color[1],
                cfg_t::visuals::settings::invisible_color[2],
                1.0f }
            );
        }

        if ( cfg_t::aimbot::aimbot_t ) {
            double dx = head.x - ( cfg_t::screen_width / 2 );

            double dy = head.y - ( cfg_t::screen_height / 2 );

            float dist = sqrtf(dx * dx + dy * dy);

            if ( ( cfg_t::aimbot::visible_check &&
                game.is_entity_visible( actors_mesh ) || !cfg_t::aimbot::visible_check ) && dist < cfg_t::aimbot::settings::radius_value && dist < target_distance )
            {
                target_distance = dist;
                target_entity = current_actor;
            }
        }

        if (cfg_t::visuals::box_2d) {
            float box_height = abs(head.y - root_box.y);
            float box_width = box_height * 0.30f;
            const float half_width = box_width / 1.7f;

            const ImVec2 top_left(head_box.x - half_width, head_box.y);
            const ImVec2 bottom_right(root_box.x + half_width, root_box.y);

            ImGui::GetBackgroundDrawList()->AddRect(top_left, bottom_right, box_color, 0.0f, 0, 1.0f);
        }

        if ( cfg_t::visuals::snap_lines ) {
            ImVec2 position = ImVec2( cfg_t::screen_width / 2, 0 );
            ImGui::GetBackgroundDrawList( )->AddLine(ImVec2( head.x, head.y ), position, box_color, 1.0 );
        }

        if ( cfg_t::visuals::username ) {
            ImVec2 text_position( head_box.x, head_box.y - 30 );
            std::string player_name = decryption.decrypt_username( player_state );
            draw_text( player_name, text_position, 12, ImColor( box_color ), true );
        }

        if ( cfg_t::visuals::platform ) {
            ImVec2 text_position( head_box.x, head_box.y - 50 );
            std::string platform_name = decryption.decrypt_platform( player_state );

            if ( strstr( platform_name.c_str( ), _xor_( "WIN" ).c_str( ) ) ) {
                draw_text("PC", text_position, 12, box_color, true);
            } else if ( strstr( platform_name.c_str( ), _xor_( "PSN" ).c_str( ) ) || strstr(platform_name.c_str( ), _xor_( "PS5" ).c_str( ) ) ) {
                draw_text("PSN", text_position, 12, box_color, true);
            } else if ( strstr( platform_name.c_str( ), _xor_( "XBL" ).c_str( ) ) || strstr(platform_name.c_str( ), _xor_( "XSX" ).c_str( ) ) ) {
                draw_text("XBL", text_position, 12, box_color, true);
            } else if ( strstr( platform_name.c_str( ), _xor_("SWT").c_str())) {
                draw_text("SW", text_position, 12, box_color, true);
            } else {
                draw_text( platform_name.c_str( ), text_position, 12, box_color, true );
            }
        }

        if ( cfg_t::visuals::distance ) {
            int bottom_t = 0;
            bottom_t += 10;
            ImVec2 text_position( root.x, root.y + bottom_t );
            std::string distance_str = std::to_string( static_cast<int>( distance ) ) + "m";
            char buffer[128];
            sprintf_s(buffer, _xor_( "%s" ).c_str( ), distance_str.c_str( ) );

            ImDrawList* draw_list = ImGui::GetBackgroundDrawList( );
            draw_text( buffer, text_position, 12, ImColor( box_color ), true );
        }

        if (cfg_t::visuals::skeleton) {
            fvector2d bonePositions[16];
            const int boneIndices[] = { 110, 3, 66, 9, 38, 10, 39, 11, 40, 78, 71, 79, 72, 75, 82, 67 };

            for (int i = 0; i < 16; ++i) {
                bonePositions[i] = game.world_to_screen(game.get_bone_location(actors_mesh, boneIndices[i]));
            }

            std::pair<int, int> bonePairs[] = {
                {1, 2},  // spine
                {3, 2},
                {2, 4},
                {5, 3},  // left upper arm
                {6, 4},  // right upper arm
                {5, 7},  // left forearm
                {6, 8},  // right forearm
                {10, 1},
                {9, 1},
                {12, 10},
                {11, 9},
                {13, 12},
                {14, 11},
                {2, 15},
                // Feet connections
                {71, 72}, // left lower leg to left ankle
                {72, 83}, // left ankle to left foot
                {74, 75}, // right lower leg to right ankle
                {75, 76}, // right ankle to right foot
            };

            int bonePairCount = sizeof(bonePairs) / sizeof(bonePairs[0]);
            float thickness = cfg_t::visuals::settings::skeleton_thickness;

            for (int i = 0; i < bonePairCount; ++i) {
                int bone1 = bonePairs[i].first;
                int bone2 = bonePairs[i].second;

                ImVec2 start = ImVec2(bonePositions[bone1].x, bonePositions[bone1].y);
                ImVec2 end = ImVec2(bonePositions[bone2].x, bonePositions[bone2].y);

                ImGui::GetBackgroundDrawList()->AddLine(start, end, box_color, thickness);
            }
        }


        if ( target_entity && cfg_t::aimbot::aimbot_t ) {
            auto closest_entity = driver.read<uint64_t>( target_entity + offsets.mesh );
            game.initialize_camera( );

            fvector hitbox;
            std::vector<int> bones = { 110, 66, 8, 5, 7, 2, 1, 6 };
            fvector2d hitbox_screen;

            if ( cfg_t::aimbot::nearest_bone ) {
                float closest_distance = FLT_MAX;
                fvector closest_bone_screen;

                for ( int bone : bones ) {
                    fvector bone_position = game.get_bone_location( closest_entity, bone );
                    fvector2d screen_position = game.world_to_screen( bone_position );
                    float distance = get_cross_distance( screen_position.x, screen_position.y, cfg_t::screen_width / 2, cfg_t::screen_height / 2 );

                    if ( distance < closest_distance ) {
                        closest_distance = distance;
                        closest_bone_screen = bone_position;
                    }
                }
                hitbox = closest_bone_screen;
            }
            else {
                switch ( cfg_t::aimbot::settings::hitbox ) {
                case 0: hitbox = game.get_bone_location( closest_entity, 110 ); break; // Head
                case 1: hitbox = game.get_bone_location( closest_entity, 66 ); break; // Neck
                case 2: hitbox = game.get_bone_location( closest_entity, 7 ); break;  // Chest
                case 3: hitbox = game.get_bone_location( closest_entity, 2 ); break;
                case 4: {
                    static std::mt19937 rng( static_cast<unsigned int>( std::time( nullptr ) ) );
                    static std::uniform_int_distribution<int> distribution(0, 2);
                    int random_hitbox = distribution( rng );

                    int random_bone = ( random_hitbox == 0) ? 110 : ( random_hitbox == 1 ) ? 66 : 7;
                    hitbox = game.get_bone_location( closest_entity, random_bone );
                    break;
                }
                }
            }

            hitbox_screen = game.world_to_screen( hitbox );
            float cross_distance = get_cross_distance( hitbox_screen.x, hitbox_screen.y, cfg_t::screen_width / 2, cfg_t::screen_height / 2 );

            if ( ( hitbox_screen.x != 0 || hitbox_screen.y != 0 ) && cross_distance <= cfg_t::aimbot::settings::radius_value ) {
                if ( cfg_t::aimbot::visible_check ? game.is_entity_visible( closest_entity ) : true) {
                    if ( GetAsyncKeyState( cfg_t::aimbot::settings::aimbot_key ) ) {
                        aimbot.move( hitbox_screen );
                    }
                }
            }
        }

        else
        {
            target_distance = FLT_MAX;
            target_entity = NULL;
        }

        entity_list.clear( );
        entity_list = temporary_entity_list;
    }
}
