#pragma once
#include <vector>
#include <mutex>
#include <dependencies/kernel/driver.hpp>
#include <core/sdk/offsets.h>

class structures_t
{
public:

	uintptr_t
		gworld,
		game_instance,
		game_state,
		local_player,
		local_pawn,
		player_state,
		player_controller,
		root_component,
		mesh,
		player_array,
		local_weapon;

	int
		team_index,
		player_array_size;

}; structures_t pointers;
class entities {
public:
	uintptr_t
		entity,
		skeletal_mesh,
		root_component,
		player_state;
};
std::vector<entities> entity_list;
std::vector<entities> temporary_entity_list;
std::mutex base;

class cache_t
{
public:
	void initialize( );
};


auto cache_thread() -> void {
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

		bool debug = false;

        temporary_entity_list.clear();
        {
            std::lock_guard<std::mutex> lock(base);

            pointers.gworld = driver.read<uintptr_t>(driver.module_base + offsets.gworld);
            pointers.game_instance = driver.read<uintptr_t>(pointers.gworld + offsets.game_instance);
            pointers.game_state = driver.read<uintptr_t>(pointers.gworld + offsets.game_state);
            pointers.local_player = driver.read<uintptr_t>(driver.read<uintptr_t>(pointers.game_instance + offsets.local_player));
            pointers.player_controller = driver.read<uintptr_t>(pointers.local_player + offsets.player_controller);
            pointers.local_pawn = driver.read<uintptr_t>(pointers.player_controller + offsets.local_pawn);
            pointers.mesh = driver.read<uintptr_t>(pointers.local_pawn + offsets.mesh);
            pointers.player_state = driver.read<uintptr_t>(pointers.local_pawn + offsets.player_state);
            pointers.root_component = driver.read<uintptr_t>(pointers.local_pawn + offsets.root_component);
            pointers.player_array = driver.read<uintptr_t>(pointers.game_state + offsets.player_array);
            pointers.player_array_size = driver.read<int>(pointers.game_state + (offsets.player_array + sizeof(uintptr_t)));
            pointers.local_weapon = driver.read<uintptr_t>(pointers.local_pawn + offsets.current_weapon);
            pointers.team_index = driver.read<uintptr_t>(pointers.player_state + offsets.team_index);

			if (debug)
			{
				std::cout << "gworld : " << pointers.gworld << std::endl;
				std::cout << "game_instance : " << pointers.game_instance << std::endl;
				std::cout << "game_state : " << pointers.game_state << std::endl;
				std::cout << "local_player : " << pointers.local_player << std::endl;
				std::cout << "player_controller : " << pointers.player_controller << std::endl;
				std::cout << "local_pawn : " << pointers.local_pawn << std::endl;
				std::cout << "mesh : " << pointers.mesh << std::endl;
				std::cout << "player_state : " << pointers.player_state << std::endl;
				std::cout << "root_component : " << pointers.root_component << std::endl;
				std::cout << "player_array : " << pointers.player_array << std::endl;
				std::cout << "team_index : " << pointers.team_index << std::endl;

			}
        }
    }
}
