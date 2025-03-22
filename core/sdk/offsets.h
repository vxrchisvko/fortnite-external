#pragma once
#include <cstdint>

class offsets_t
{
public :
    std::uint64_t  gworld = 0x17719838;
    std::uint64_t  owning_world = 0xc0;
    std::uint64_t  local_pawn = 0x350;
    std::uint64_t  mesh = 0x328;
    std::uint64_t  root_component = 0x1B0;

    std::uint64_t  game_instance = 0x210;
    std::uint64_t  game_state = 0x198;
    std::uint64_t  bone_array = 0x5D8;

    std::uint64_t  persistent_level = 0x40;
    std::uint64_t  component_to_world = 0x1E0;

    std::uint64_t  platform = 0x430;
    std::uint64_t  team_index = 0x1259;

    std::uint64_t  local_player = 0x38;
    std::uint64_t  player_array = 0x2c0;
    std::uint64_t  player_state = 0x2b0;
    std::uint64_t  player_controller = 0x30; //uplayer

    std::uint64_t current_weapon = 0xa80;

    std::uint64_t pawn_private = 0x320;

}; inline offsets_t offsets;