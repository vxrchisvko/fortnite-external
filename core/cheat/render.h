#pragma once

#include <dependencies/framework/imgui_internal.h>
#include <dependencies/framework/imgui_impl_dx11.h>
#include <D3D11.h>
#include <dependencies/framework/imgui_impl_win32.h>
#include <dependencies/encryption/xor.hpp>
#include <dwmapi.h>
#include "menu/menu.h"
#include <core/cheat/loops/actors.h>

ID3D11Device* d3d_device = NULL;
ID3D11DeviceContext* d3d_device_ctx = NULL;
IDXGISwapChain* swap_chain = NULL;
ID3D11RenderTargetView* target_view = NULL;


class globals_t {
public:
    HWND window;
}; globals_t globals;
ImVec4 clear_color = ImVec4( 0.45f, 0.55f, 0.60f, 0.f );

HWND game_window;
RECT game_rec;
int gamex, gamey, gamewidth, gameheight, overlaywidth, overlayheight;

class overlay_t
{
public:

    bool initialize_imgui( )
    {
        DXGI_SWAP_CHAIN_DESC swap_chain_description;
        ZeroMemory(&swap_chain_description, sizeof( swap_chain_description ) );
        swap_chain_description.BufferCount = 2;
        swap_chain_description.BufferDesc.Width = 0;
        swap_chain_description.BufferDesc.Height = 0;
        swap_chain_description.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swap_chain_description.BufferDesc.RefreshRate.Numerator = 60;
        swap_chain_description.BufferDesc.RefreshRate.Denominator = 1;
        swap_chain_description.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
        swap_chain_description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swap_chain_description.OutputWindow = globals.window;
        swap_chain_description.SampleDesc.Count = 1;
        swap_chain_description.SampleDesc.Quality = 0;
        swap_chain_description.Windowed = 1;
        swap_chain_description.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

        D3D_FEATURE_LEVEL d3d_feature_lvl;

        const D3D_FEATURE_LEVEL d3d_feature_array[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };

        D3D11CreateDeviceAndSwapChain( NULL,
            D3D_DRIVER_TYPE_HARDWARE,
            NULL,
            0,
            d3d_feature_array,
            2,
            D3D11_SDK_VERSION,
            &swap_chain_description,
            &swap_chain, &d3d_device,
            &d3d_feature_lvl,
            &d3d_device_ctx
        );

        ID3D11Texture2D* pBackBuffer;
        swap_chain->GetBuffer( 0, IID_PPV_ARGS( &pBackBuffer ) );
        d3d_device->CreateRenderTargetView( pBackBuffer, NULL, &target_view );
        pBackBuffer->Release( );

        IMGUI_CHECKVERSION( );
        ImGui::CreateContext( );
        ImGuiIO& io = ImGui::GetIO( ); ( void )io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

        ImGui::StyleColorsDark( );

        ImGui_ImplWin32_Init( globals.window );
        ImGui_ImplDX11_Init( d3d_device, d3d_device_ctx );

        ImFontConfig font_config;
        font_config.OversampleH = 1;
        font_config.OversampleV = 1;
        font_config.PixelSnapH = 1;

        static const ImWchar ranges[] = {
            0x0020, 0x00FF, // Basic Latin + Latin Supplement
            0x0400, 0x052F, // Cyrillic + Cyrillic Supplement
            0x2DE0, 0x2DFF, // Cyrillic Extended-A
            0xA640, 0xA69F, // Cyrillic Extended-B
            0xE000, 0xE226, // icons
            0,
        };
        font_config.GlyphRanges = ranges;

        d3d_device->Release();
        return true;
    }

    void cleanup_render_target( ) {
        if ( target_view ) {
            target_view->Release( );
            target_view = NULL;
        }
    }

    void create_render_target( ) {
        ID3D11Texture2D* back_buffer;
        swap_chain->GetBuffer( 0, IID_PPV_ARGS( &back_buffer ) );
        d3d_device->CreateRenderTargetView( back_buffer, NULL, &target_view );
        back_buffer->Release( );
    }

    void shutdown()
    {
        ImGui_ImplDX11_Shutdown( );
        ImGui_ImplWin32_Shutdown( );
        ImGui::DestroyContext( );
        PostQuitMessage( 0 );
        exit( 0x100) ;
    }

    static LRESULT CALLBACK static_wnd_proc_hook( HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param ) {
        overlay_t* overlay = reinterpret_cast<overlay_t*>( GetWindowLongPtr( hwnd, GWLP_USERDATA ) );
        if ( overlay ) {
            return overlay->wnd_proc_hook( hwnd, message, w_param, l_param );
        }
        return DefWindowProc( hwnd, message, w_param, l_param );
    }

    LRESULT CALLBACK wnd_proc_hook( HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param ) {
        switch ( message ) {
        case WM_DESTROY:
            shutdown( );
            break;
        case WM_SIZE:
            if ( d3d_device != NULL && w_param != SIZE_MINIMIZED ) {
                cleanup_render_target( );
                swap_chain->ResizeBuffers(0, ( UINT )LOWORD( l_param ), (UINT)HIWORD( l_param ), DXGI_FORMAT_UNKNOWN, 0 );
                create_render_target( );
            }
            break;
        }
        return DefWindowProc( hwnd, message, w_param, l_param );
    }

    bool initialize_overlay( ) {
        const char* ClassName = ("Fortnite");

        WNDCLASSEX wc;
        ZeroMemory(&wc, sizeof(wc));
        wc.cbSize = sizeof(wc);
        wc.lpfnWndProc = static_wnd_proc_hook;
        wc.lpszClassName = ClassName;
        (RegisterClassExA)(&wc);

        globals.window = (CreateWindowExA)(
            (DWORD)NULL,
            ClassName,
            ClassName,
            WS_POPUP | WS_VISIBLE,
            0, 0, cfg_t::screen_width, cfg_t::screen_height,
            (HWND)0, (HMENU)0, (HINSTANCE)0, (LPVOID)0);

        if (!globals.window)
            return FALSE;

        (SetWindowLongA)(globals.window, GWL_EXSTYLE, WS_EX_TOOLWINDOW | WS_EX_LAYERED | WS_EX_TRANSPARENT);

        MARGINS Margin = { -1 };
        DwmExtendFrameIntoClientArea(globals.window, &Margin);
        (ShowWindow)(globals.window, SW_SHOW);
        (UpdateWindow)(globals.window);

        if (!globals.window)
            return FALSE;

        return TRUE;
    }

    void draw_instance( )
    {
        ImGui_ImplDX11_NewFrame( );
        ImGui_ImplWin32_NewFrame( );
        ImGui::NewFrame( );
        
        actors_t::instance( ).render_elements( );
        actors_t::instance( ).execute_loop( );

        if (GetAsyncKeyState( cfg_t::menu_key ) & 1 ) {
            cfg_t::t_show_menu = !cfg_t::t_show_menu;
        }

        if ( cfg_t::t_show_menu ) {  menu_t::instance( ).render_menu( ); }

        ImGui::Render( );

        const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
        d3d_device_ctx->ClearRenderTargetView( target_view, clear_color_with_alpha );
        d3d_device_ctx->OMSetRenderTargets( 1, &target_view, NULL );
        ImGui_ImplDX11_RenderDrawData( ImGui::GetDrawData( ) );
        swap_chain->Present( 0, 0 );
    }

    bool is_fortnite_active( ) {
        HWND hwnd = GetForegroundWindow( );
        if ( hwnd ) {
            char class_name[256];
            GetClassNameA(hwnd, class_name, sizeof( class_name ) );
            std::string current_class_name = class_name;
            return current_class_name == "UnrealWindow";
        }
        return false;
    }

    DWORD WINAPI render_loop( ) {
        RECT old_rc = { 0, 0, 0, 0 };
        MSG Message = { NULL };
        ZeroMemory(&Message, sizeof(MSG));

        while (Message.message != WM_QUIT) {
            if (PeekMessage(&Message, globals.window, 0, 0, PM_REMOVE)) {
                TranslateMessage(&Message);
                DispatchMessage(&Message);
            }

            game_window = FindWindowA("UnrealWindow", "Fortnite  ");
            if (GetWindow(game_window, 0) == NULL) {
                SendMessage(globals.window, WM_DESTROY, 0, 0);
                break;
            }

            HWND hwnd_active = GetForegroundWindow();
            if (hwnd_active == game_window || hwnd_active == globals.window) {
                POINT xy;
                ZeroMemory(&game_rec, sizeof(RECT));
                ZeroMemory(&xy, sizeof(POINT));

                GetClientRect(game_window, &game_rec);
                ClientToScreen(game_window, &xy);
                game_rec.left = xy.x;
                game_rec.top = xy.y;

                if (game_rec.left != old_rc.left || game_rec.right != old_rc.right ||
                    game_rec.top != old_rc.top || game_rec.bottom != old_rc.bottom) {
                    old_rc = game_rec;
                    gamex = game_rec.left;
                    gamey = game_rec.top;
                    gamewidth = game_rec.right;
                    gameheight = game_rec.bottom;

                    overlaywidth = game_rec.right;
                    overlayheight = game_rec.bottom;

                    SetWindowPos(globals.window, 0, xy.x, xy.y, overlaywidth, overlayheight, SWP_NOREDRAW);
                }

                ImGui::GetIO().MouseDrawCursor = cfg_t::t_show_menu;
                draw_instance();

                HWND hwnd = GetWindow(hwnd_active, GW_HWNDPREV);
                if (hwnd_active == globals.window) {
                    hwnd = GetWindow(game_window, GW_HWNDPREV);
                }
                SetWindowPos(globals.window, hwnd, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
            }
            else {
                float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
                d3d_device_ctx->ClearRenderTargetView(target_view, clearColor);
                d3d_device_ctx->OMSetRenderTargets(1, &target_view, NULL);
                swap_chain->Present(0, 0);
            }
        }

        return 0;
    }

};
overlay_t overlay;
