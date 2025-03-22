#ifndef entrypoint_cpp
#define entrypoint_cpp

//header
#include <dependencies/kernel/driver.hpp>
#include <core/cheat/render.h>
#include <core/cheat/loops/cache.h>

auto execute_main( ) -> void {
	if ( !driver.setup_driver( ) ) { std::cout << "[fortnite] -> failed #1" << std::endl; }
	if ( !driver.get_pid( "FortniteClient-Win64-Shipping.exe" ) ) { std::cout << "[fortnite] -> failed #2 (where it shouldn't)" << std::endl; }
	if ( driver.module_base = driver.get_module( ) ) { std::cout << "[fortnite] -> base address sucessfully retrieved" << std::endl; }
	driver.fix_dtb();

	overlay.initialize_overlay( );
	overlay.initialize_imgui( );

	std::thread(cache_thread).detach();
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)overlay.render_loop(), NULL, 0, NULL);

}

int main( ) {
	execute_main( );

	return 0;
}

#endif