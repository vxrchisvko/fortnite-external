#pragma once

inline static int keystatus = 0;
inline static int triggerbot_keystatus = 0;
inline static int menu_keystatus = 0;

inline static int realkey = 0;

inline bool GetKey(int key)
{
	realkey = key;
	return true;
}

inline void ChangeKey(void* blank)
{
	keystatus = 1;
	while (true)
	{
		for (int i = 0; i < 0x87; i++)
		{
			if (GetAsyncKeyState(i) & 0x8000)
			{
				cfg_t::aimbot::settings::aimbot_key = i;
				keystatus = 0;
				return;
			}
		}
	}
}

inline void ChangeKeyTriggerbot(void* blank)
{
	triggerbot_keystatus = 1;
	while (true)
	{
		for (int i = 0; i < 0x87; i++)
		{
			if (GetAsyncKeyState(i) & 0x8000)
			{
				cfg_t::aimbot::settings::triggerbot_key = i;
				triggerbot_keystatus = 0;
				return;
			}
		}
	}
}

inline void ChangeMenuKey(void* blank)
{
	menu_keystatus = 1;
	while (true)
	{
		for (int i = 0; i < 0x87; i++)
		{
			if (GetAsyncKeyState(i) & 0x8000)
			{
				cfg_t::menu_key = i;
				menu_keystatus = 0;
				return;
			}
		}
	}
}

//static const char* keyNames[] =
//{
//	"Keybind",
//	"Left Mouse",
//	"Right Mouse",
//	"Cancel",
//	"Middle Mouse",
//	"Mouse 5",
//	"Mouse 4",
//	"",
//	"Backspace",
//	"Tab",
//	"",
//	"",
//	"Clear",
//	"Enter",
//	"",
//	"",
//	"Shift",
//	"Control",
//	"Alt",
//	"Pause",
//	"Caps",
//	"",
//	"",
//	"",
//	"",
//	"",
//	"",
//	"Escape",
//	"",
//	"",
//	"",
//	"",
//	"Space",
//	"Page Up",
//	"Page Down",
//	"End",
//	"Home",
//	"Left",
//	"Up",
//	"Right",
//	"Down",
//	"",
//	"",
//	"",
//	"Print",
//	"Insert",
//	"Delete",
//	"",
//	"0",
//	"1",
//	"2",
//	"3",
//	"4",
//	"5",
//	"6",
//	"7",
//	"8",
//	"9",
//	"",
//	"",
//	"",
//	"",
//	"",
//	"",
//	"",
//	"A",
//	"B",
//	"C",
//	"D",
//	"E",
//	"F",
//	"G",
//	"H",
//	"I",
//	"J",
//	"K",
//	"L",
//	"M",
//	"N",
//	"O",
//	"P",
//	"Q",
//	"R",
//	"S",
//	"T",
//	"U",
//	"V",
//	"W",
//	"X",
//	"Y",
//	"Z",
//	"",
//	"",
//	"",
//	"",
//	"",
//	"Numpad 0",
//	"Numpad 1",
//	"Numpad 2",
//	"Numpad 3",
//	"Numpad 4",
//	"Numpad 5",
//	"Numpad 6",
//	"Numpad 7",
//	"Numpad 8",
//	"Numpad 9",
//	"Multiply",
//	"Add",
//	"",
//	"Subtract",
//	"Decimal",
//	"Divide",
//	"F1",
//	"F2",
//	"F3",
//	"F4",
//	"F5",
//	"F6",
//	"F7",
//	"F8",
//	"F9",
//	"F10",
//	"F11",
//	"F12",
//};

static const char* key_names[] =
{
	"keybind",
	"lmouse",
	"rmouse",
	"cancel",
	"middle_mouse",
	"mouse_5",
	"mouse_4",
	"",
	"backspace",
	"{tab}",
	"",
	"",
	"clear",
	"enter",
	"",
	"",
	"shift",
	"control",
	"alt",
	"pause",
	"caps",
	"",
	"",
	"",
	"",
	"",
	"",
	"escape",
	"",
	"",
	"",
	"",
	"space",
	"page_up",
	"page_down",
	"end",
	"home",
	"left",
	"up",
	"right",
	"down",
	"",
	"",
	"",
	"print",
	"insert",
	"delete",
	"",
	"0",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"a",
	"b",
	"c",
	"d",
	"e",
	"f",
	"g",
	"h",
	"i",
	"j",
	"k",
	"l",
	"m",
	"n",
	"o",
	"p",
	"q",
	"r",
	"s",
	"t",
	"u",
	"v",
	"w",
	"x",
	"y",
	"z",
	"",
	"",
	"",
	"",
	"",
	"numpad_0",
	"numpad_1",
	"numpad_2",
	"numpad_3",
	"numpad_4",
	"numpad_5",
	"numpad_6",
	"numpad_7",
	"numpad_8",
	"numpad_9",
	"multiply",
	"add",
	"",
	"subtract",
	"decimal",
	"divide",
	"f1",
	"f2",
	"f3",
	"f4",
	"f5",
	"f6",
	"f7",
	"f8",
	"f9",
	"f10",
	"f11",
	"f12",
};

inline static bool Items_ArrayGetter(void* data, int idx, const char** out_text)
{
	const char* const* items = (const char* const*)data;
	if (out_text)
		*out_text = items[idx];
	return true;
}
static void HotkeyButton(int aimkey, void* changekey, int status)
{
	const char* preview_value = hash_string("Key");
	if (aimkey >= 0 && aimkey < IM_ARRAYSIZE(key_names))
		Items_ArrayGetter(key_names, aimkey, &preview_value);

	std::string aimkeys;
	if (preview_value[0] == '\0')
		aimkeys = hash_string("your key");
	else
		aimkeys = preview_value;

	if (status == 1)
	{
		aimkeys = hash_string("press key");
	}
	if (ImGui::Button(aimkeys.c_str(), ImVec2(60, 20)))
	{
		if (status == 0)
		{
			CreateThread(0, 0, (LPTHREAD_START_ROUTINE)changekey, nullptr, 0, nullptr);
		}
	}
}