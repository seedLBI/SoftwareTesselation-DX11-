#include "DX11.Framework.h"


namespace framework {
	namespace core {
		namespace VARS {
			namespace input {


				namespace mouse{
					int mouseX = 0;
					int mouseY = 0;
					int mouseX_screen;
					int mouseY_screen;
					int deltaX = 0;
					int deltaY = 0;
					int wheelDelta = 0;

					std::array<bool, 5> buttons_prev = { false };
					std::array<bool, 5> buttons_down = { false };
					std::array<bool, 5> buttons_pressed = { false };
					std::array<bool, 5> buttons_released = { false };

					void HandleMouseInput(LPARAM lParam) {
						UINT size = 0;
						GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, nullptr, &size, sizeof(RAWINPUTHEADER));
						if (size == 0) 
							return;

						std::vector<BYTE> rawBuffer(size);
						if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, rawBuffer.data(), &size, sizeof(RAWINPUTHEADER)) == UINT(-1)) {
							return;  // Error
						}

						RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(rawBuffer.data());

						if (raw->header.dwType == RIM_TYPEMOUSE) {
							framework::core::VARS::input::mouse::deltaX += raw->data.mouse.lLastX;
							framework::core::VARS::input::mouse::deltaY += raw->data.mouse.lLastY;

							if (raw->data.mouse.usButtonFlags & RI_MOUSE_WHEEL) {
								framework::core::VARS::input::mouse::wheelDelta += static_cast<short>(raw->data.mouse.usButtonData);
							}


							if (raw->data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN)   framework::core::VARS::input::mouse::buttons_down[0] = true;
							if (raw->data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP)     framework::core::VARS::input::mouse::buttons_down[0] = false;
							if (raw->data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN)  framework::core::VARS::input::mouse::buttons_down[1] = true;
							if (raw->data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP)    framework::core::VARS::input::mouse::buttons_down[1] = false;
							if (raw->data.mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN) framework::core::VARS::input::mouse::buttons_down[2] = true;
							if (raw->data.mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_UP)   framework::core::VARS::input::mouse::buttons_down[2] = false;
							if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_4_DOWN)      framework::core::VARS::input::mouse::buttons_down[3] = true;
							if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_4_UP)        framework::core::VARS::input::mouse::buttons_down[3] = false;
							if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_5_DOWN)      framework::core::VARS::input::mouse::buttons_down[4] = true;
							if (raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_5_UP)        framework::core::VARS::input::mouse::buttons_down[4] = false;

						}

					}
				}


				namespace keyboard {
					std::array<bool, 256> keys_prev = { false };
					std::array<bool, 256> keys_down = { false };
					std::array<bool, 256> keys_pressed = { false };
					std::array<bool, 256> keys_released = { false };
				}

			}
			void InitInput() {
				using namespace core::VARS::input::keyboard;

				std::fill(keys_prev.begin(), keys_prev.end(), false);
				std::fill(keys_down.begin(), keys_down.end(), false);
				std::fill(keys_pressed.begin(), keys_pressed.end(), false);
				std::fill(keys_released.begin(), keys_released.end(), false);




				using namespace core::VARS::input::mouse;

				std::fill(buttons_prev.begin(), buttons_prev.end(), false);
				std::fill(buttons_down.begin(), buttons_down.end(), false);
				std::fill(buttons_pressed.begin(), buttons_pressed.end(), false);
				std::fill(buttons_released.begin(), buttons_released.end(), false);


				RAWINPUTDEVICE rid[1];
				rid[0].usUsagePage = 0x01;
				rid[0].usUsage = 0x02;
				rid[0].dwFlags = 0x0;
				rid[0].hwndTarget = framework::window::GetHandle();
				if (!RegisterRawInputDevices(rid, 1, sizeof(rid[0]))) {
					MessageBox(nullptr, L"Failed to register raw input devices!", L"Error", MB_OK | MB_ICONERROR);
				}

				POINT cursorPos;
				GetCursorPos(&cursorPos);
				ScreenToClient(VARS::window::mWindowHandle, &cursorPos);
				mouseX = cursorPos.x;
				mouseY = cursorPos.y;


			}
			void TerminateInput() {
				RAWINPUTDEVICE rid[1];
				rid[0].usUsagePage = 0x01;
				rid[0].usUsage = 0x02;
				rid[0].dwFlags = RIDEV_REMOVE;
				rid[0].hwndTarget = NULL;
				RegisterRawInputDevices(rid, 1, sizeof(rid[0]));


			}
			void UpdateInput() {
				using namespace core::VARS::input::keyboard;

				BYTE kb[256];
				if (GetKeyboardState(kb)) {
					for (int key = 0; key < 256; ++key) {
						uint8_t down = (kb[key] & 0x80) != 0;
						keys_pressed[key] = down && !keys_prev[key];
						keys_released[key] = !down && keys_prev[key];
						keys_prev[key] = down;
						keys_down[key] = down;
					}
				}

				using namespace core::VARS::input::mouse;

				for (int i = 0; i < 5; ++i) {
					buttons_pressed[i] = !buttons_prev[i] && buttons_down[i];
					buttons_released[i] = buttons_prev[i] && !buttons_down[i];
					buttons_prev[i] = buttons_down[i];
				}

				POINT cursorPos;
				GetCursorPos(&cursorPos);
				mouseX_screen = cursorPos.x;
				mouseY_screen = cursorPos.y;

				ScreenToClient(framework::window::GetHandle(), &cursorPos);
				mouseX = cursorPos.x;
				mouseY = cursorPos.y;


				if (framework::window::IsCursorClipped() && framework::window::IsFocused()) {
					RECT rect;
					GetWindowRect(framework::window::GetHandle(), &rect);
					ClipCursor(&rect);
				}
				else {
					ClipCursor(NULL);
				}

			}
		}



	}



	namespace input {

		using namespace core::VARS::input::keyboard;

		bool IsKeyUp(const KeyboardButton& Key) {
			return keys_down[Key] == false;
		}

		bool IsKeyDown(const KeyboardButton& Key) {
			return keys_down[Key];
		}

		bool IsKeyPressed(const KeyboardButton& Key) {
			bool isPressed = keys_pressed[Key];
			keys_pressed[Key] = false;
			return isPressed;
		}
		
		bool IsKeyReleased(const KeyboardButton& Key) {
			bool isReleased = keys_released[Key];
			keys_released[Key] = false;
			return isReleased;
		}


		using namespace core::VARS::input::mouse;

		bool IsMouseUp(const MouseButton& button) {
			return !buttons_down[button];
		}
		bool IsMouseDown(const MouseButton& button) {
			return buttons_down[button];
		}
		bool IsMousePressed(const MouseButton& button) {
			bool isPressed = buttons_pressed[button];
			buttons_pressed[button] = false;
			return isPressed;
		}
		bool IsMouseReleased(const MouseButton& button) {
			bool isReleased = buttons_released[button];
			buttons_released[button] = false;
			return isReleased;
		}

		DirectX::XMINT2 GetPositionClient() {
			return { mouseX, mouseY };
		}
		DirectX::XMINT2 GetPositionScreen() {
			return { mouseX_screen, mouseY_screen };
		}
		DirectX::XMINT2 GetDelta() {
			DirectX::XMINT2 delta = { deltaX, deltaY };
			deltaX = 0;
			deltaY = 0;
			return delta;
		}
		int GetWheelDelta() {
			int delta = wheelDelta;
			wheelDelta = 0;
			return delta/120;
		}


	}
}