#pragma once

#include <core/sdk/math.h>"
#include <string>
#include <sstream>
#include <cstdint>


struct camera_t {
	fvector location{};
	fvector rotation{};
	float field_of_view{};
};
inline camera_t camera_position{};

class monitor_t {
public:
	int width;
	int height;
}; monitor_t monitor;

class game_functions {
public:
	static auto get_bone_location( uintptr_t skeletal_mesh, int bone_index ) -> fvector
	{
		uintptr_t bone_array = driver.read<uintptr_t>( skeletal_mesh + 0x5C8 );

		if ( bone_array == NULL ) bone_array = driver.read<uintptr_t>( skeletal_mesh + 0x5C8 + 0x10 );

		ftransform bone = driver.read<ftransform>(bone_array + ( bone_index * 0x60 ) );

		ftransform component_to_world = driver.read<ftransform>( skeletal_mesh + offsets.component_to_world );

		D3DMATRIX matrix = matrix_multiplication( bone.to_matrix_with_scale( ), component_to_world.to_matrix_with_scale( ) );

		return fvector( matrix._41, matrix._42, matrix._43 );
	}

	auto get_view_state( ) -> uintptr_t
	{
		tarray<uintptr_t> view_state = driver.read<tarray<uintptr_t>>( pointers.local_player + 0xD0 );
		return view_state.Get( 1 );
	}

	inline double radians_to_degress( double d_radians )
	{
		return d_radians * ( 180.0 / m_pi);
	}

	auto initialize_camera( ) -> camera_t {
		camera_t camera;
		auto m_projection = driver.read<f_matrix>( get_view_state( ) + 0x940 );
		camera.rotation.x = radians_to_degress( std::asin( m_projection.zplane.w ) );
		camera.rotation.y = radians_to_degress( std::atan2( m_projection.yplane.w, m_projection.xplane.w ) );
		camera.rotation.z = 0.0;

		camera.location.x = m_projection.m[3][0];
		camera.location.y = m_projection.m[3][1];
		camera.location.z = m_projection.m[3][2];
		float field_of_view = atanf(1 / driver.read<double>( get_view_state( ) + 0x740 ) ) * 2;
		camera.field_of_view = ( field_of_view ) * ( 180.f / m_pi); return camera;
	}
	
	inline fvector2d world_to_screen(fvector world_location)
	{
		camera_position = initialize_camera( );
		if (world_location.x == 0)
			return fvector2d(0, 0);

		_matrix tempMatrix = matrix(camera_position.rotation);
		fvector vAxisX = fvector(tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2]);
		fvector vAxisY = fvector(tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2]);
		fvector vAxisZ = fvector(tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2]);

		fvector vDelta = world_location - camera_position.location;
		fvector vTransformed = fvector(vDelta.dot(vAxisY), vDelta.dot(vAxisZ), vDelta.dot(vAxisX));

		if (vTransformed.z < 1.f)
			vTransformed.z = 1.f;

		return fvector2d(
			(cfg_t::screen_width / 2.0f) + vTransformed.x * (((cfg_t::screen_width / 2.0f) / tanf(camera_position.field_of_view * (float)M_PI / 360.f))) / vTransformed.z, (cfg_t::screen_height / 2.0f) - vTransformed.y * (((cfg_t::screen_width / 2.0f) / tanf(camera_position.field_of_view * (float)M_PI / 360.f))) / vTransformed.z);
	}

	bool is_entity_visible( uintptr_t mesh, float tolerance = 0.06f ) {
		double seconds = driver.read<double>( pointers.gworld + 0x160 );
		float last_render_time = driver.read<float>( mesh + 0x32C );
		return seconds - last_render_time <= tolerance;
	}
	
}; game_functions game;

class decryption_t {
public:
	
	auto decrypt_platform( uintptr_t platforms ) {
		uintptr_t platform = driver.read<uintptr_t>( platforms + 0x430 );
		wchar_t platform_bit[64] = { 0 };

		driver.read_memory( PVOID( platform ), reinterpret_cast<uint8_t*>( platform_bit ), sizeof( platform_bit ) );
		std::wstring platform_wstr( platform_bit );
		std::string platform_str( platform_wstr.begin( ), platform_wstr.end( ) );

		return platform_str;
	}

	static std::string decrypt_username( uintptr_t player_state ) {
		int name_length;
		char16_t* name_buffer;
		uintptr_t name_structure = driver.read<uintptr_t>( player_state + 0xb08 );

		if ( driver.is_valid( name_structure ) ) {
			name_length = driver.read<int>(name_structure + 0x10);
			if (name_length <= 0)
				return "BOT/Ai";

			name_buffer = new char16_t[name_length];
			uintptr_t encrypted_buffer = driver.read<uintptr_t>( name_structure + 0x8 );
			if ( driver.is_valid( encrypted_buffer ) ) {
				if ( driver.read_array( encrypted_buffer, name_buffer, name_length ) ) {
					int v25 = name_length - 1;
					int v26 = 0;
					WORD* buffer_pointer = reinterpret_cast<WORD*>(name_buffer);

					for ( int i = ( v25 ) & 3;; *buffer_pointer++ += i & 7 ) {
						int v29 = name_length - 1;
						if ( !( DWORD )name_length )
							v29 = 0;

						if (v26 >= v29)
							break;

						i += 3;
						++v26;
					}

					std::u16string temp_wstring( name_buffer );
					delete[] name_buffer;
					return std::string( temp_wstring.begin( ), temp_wstring.end( ) );
				}
				else {
					delete[] name_buffer;
					return "BOT/AI";
				}
			}
		}
		return "BOT";
	}

}; decryption_t decryption;

void draw_text(const std::string& text, const ImVec2& pos, float size, uint32_t color, bool center) {
	constexpr float stroke_val = 1.0f;
	uint32_t edge_color = 0xFF000000;

	float edge_a = (edge_color >> 24) & 0xff;
	float edge_r = (edge_color >> 16) & 0xff;
	float edge_g = (edge_color >> 8) & 0xff;
	float edge_b = (edge_color) & 0xff;

	std::stringstream text_stream(text);
	std::string line;
	int line_index = 0;

	while ( std::getline( text_stream, line ) ) {
		ImVec2 text_size = ImGui::GetFont( )->CalcTextSizeA( size, FLT_MAX, 0.0f, line.c_str( ) );
		float line_pos_x = pos.x - ( center ? text_size.x / 2.0f : 0.0f );
		float line_pos_y = pos.y + text_size.y * line_index;

		for ( int offset = -1; offset <= 1; ++offset ) {
			if ( offset != 0 ) {
				ImGui::GetBackgroundDrawList( )->AddText(
					ImGui::GetFont( ), size, ImVec2( line_pos_x + offset * stroke_val, line_pos_y ),
					ImGui::GetColorU32(ImVec4( edge_r / 255.0f, edge_g / 255.0f, edge_b / 255.0f, edge_a / 255.0f ) ),
					line.c_str( )
				);

				ImGui::GetBackgroundDrawList ()->AddText(
					ImGui::GetFont( ), size, ImVec2( line_pos_x, line_pos_y + offset * stroke_val ),
					ImGui::GetColorU32( ImVec4(edge_r / 255.0f, edge_g / 255.0f, edge_b / 255.0f, edge_a / 255.0f ) ),
					line.c_str( )
				);
			}
		}

		ImGui::GetBackgroundDrawList( )->AddText( ImGui::GetFont( ), size, ImVec2( line_pos_x, line_pos_y ), color, line.c_str( ) );

		++line_index;
	}
}

void curved_line_3_points(ImDrawList* drawList, ImVec2 p0, ImVec2 p1, ImVec2 p2, int segments, ImU32 color, float thickness)
{
	drawList->PathClear();
	drawList->PathLineTo(p0);

	float t_step = 1.0f / segments;
	for (int i = 1; i <= segments; ++i)
	{
		float t = static_cast<float>(i) * t_step;
		float u = 1.0f - t;

		float w0 = u * u;
		float w1 = 2 * u * t;
		float w2 = t * t;

		ImVec2 pt = ImVec2(w0 * p0.x + w1 * p1.x + w2 * p2.x,
			w0 * p0.y + w1 * p1.y + w2 * p2.y);

		drawList->PathLineTo(pt);
	}

	drawList->PathStroke(color, false, thickness);
}
void render_curved_skeleton(ImDrawList* drawList, fvector2d* bonePositions, const std::pair<int, int>* bonePairs, int bonePairCount, ImU32 color, float thickness, bool drawOutline)
{
	drawList->PushClipRectFullScreen();

	for (int i = 0; i < bonePairCount; ++i)
	{
		const auto& pair = bonePairs[i];
		ImVec2 start(bonePositions[pair.first].x, bonePositions[pair.first].y);
		ImVec2 end(bonePositions[pair.second].x, bonePositions[pair.second].y);

		ImVec2 midPoint = ImVec2((start.x + end.x) / 2, (start.y + end.y) / 2);

		float curvatureFactor = 0.0f;
		if ((pair.first == 1 && pair.second == 2) ||
			(pair.first == 3 && pair.second == 2) ||
			(pair.first == 2 && pair.second == 4))
		{
			curvatureFactor = 5.0f;
		}
		else if ((pair.first == 5 && pair.second == 7) ||
			(pair.first == 6 && pair.second == 8) ||
			(pair.first == 5 && pair.second == 3) ||
			(pair.first == 6 && pair.second == 4))
		{
			curvatureFactor = 20.0f;
		}
		else
		{
			curvatureFactor = 10.0f;
		}

		float distance = sqrtf(powf(end.x - start.x, 2) + powf(end.y - start.y, 2));
		float adjustedCurvature = distance * (curvatureFactor / 100.0f);
		ImVec2 controlPoint = ImVec2(midPoint.x, midPoint.y - adjustedCurvature);

		if (drawOutline)
		{
			curved_line_3_points(drawList, start, controlPoint, end, 20, ImColor(0, 0, 0, 255), thickness + 2.f);
		}

		curved_line_3_points(drawList, start, controlPoint, end, 20, color, thickness);
	}

	drawList->PopClipRect();
}