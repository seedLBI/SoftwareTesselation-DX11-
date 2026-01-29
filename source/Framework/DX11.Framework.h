#ifndef FRAMEWORK_H
#define FRAMEWORK_H

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include "Time/FPS.Limiter.h"
#include "Time/Timer.HighResolution.h"

#include "Enums/Keyboard.Enums.h"
#include "Enums/Mouse.Enums.h"

#include "GPUInfo/GPUInfo.h"

#include <windows.h>
#include <string>
#include <vector>
#include <array>


#include <d3d11.h>
#include <DirectXMath.h>

namespace framework {

	namespace core {

		namespace VARS {

			namespace time {
				extern FPSLimiter* fps_limiter;
				extern TimerHighResolution timer_program;
			}
			void InitTime();
			void TerminateTime();


			namespace window {
				extern HINSTANCE mInstance;
				extern std::wstring mWindowClass;
				extern std::wstring mWindowTitle;
				extern int mShowCommand;
				extern HWND mWindowHandle;
				extern WNDCLASSEX mWindow;
				extern UINT mScreenWidth;
				extern UINT mScreenHeight;
				extern bool mConsoleDraw;
				extern bool mCursorVisible;
				extern bool mClipCursor;
			}
			void InitWindow();
			void TerminateWindow();


			namespace directx {
				extern int m_videoCardMemory;
				extern char m_videoCardDescription[128];
				extern IDXGISwapChain* m_swapChain;
				extern ID3D11Device* m_device;
				extern ID3D11DeviceContext* m_deviceContext;
				extern ID3D11RenderTargetView* m_renderTargetView;
				extern ID3D11Texture2D* m_depthStencilBuffer;
				extern ID3D11DepthStencilState* m_depthStencilState;
				extern ID3D11DepthStencilView* m_depthStencilView;
				extern ID3D11RasterizerState* m_rasterState;
				extern D3D11_VIEWPORT m_viewport;

				extern bool isVSyncEnabled;
			}
			void InitDirectX();
			void TerminateDirectX();


			namespace input {

				namespace mouse {
					extern int mouseX;
					extern int mouseY;
					extern int mouseX_screen;
					extern int mouseY_screen;
					extern int deltaX;
					extern int deltaY;
					extern int wheelDelta;

					extern std::array<bool, 5> buttons_prev;
					extern std::array<bool, 5> buttons_down;
					extern std::array<bool, 5> buttons_pressed;
					extern std::array<bool, 5> buttons_released;

					void HandleMouseInput(LPARAM lParam);
				}

				namespace keyboard {
					extern std::array<bool, 256> keys_prev;
					extern std::array<bool, 256> keys_down;
					extern std::array<bool, 256> keys_pressed;
					extern std::array<bool, 256> keys_released;
				}

			}
			void UpdateInput();
			void InitInput();
			void TerminateInput();

		}
		
		void Init(
			const HINSTANCE& hInstance,
			const int& nCmdShow,
			const bool& showConsole = true,
			const std::wstring& title = L"DirectX app",
			const int& width = 1920,
			const int& height = 1000
			);

		void Terminate();

		void Update();
	}

	namespace time {

		void  SetTimeProgram();
		void  SetFPS(const unsigned int&);

		float GetTimeProgram();
		float GetDeltaTime();
		float GetFPS();

	}

	namespace window {

		HWND&			GetHandle();

		void			SetTitle(const std::wstring&);
		std::wstring	GetTitle();

		DirectX::XMINT2 GetPosition();
		void			SetPosition(const int& x, const int& y);
		void			SetPosition(const DirectX::XMINT2& pos);

		DirectX::XMINT2 GetSize();
		void			SetSize(const int& width, const int& height);
		void			SetSize(const DirectX::XMINT2& size);

		void			SetFocus();
		bool			IsFocused();

		bool			IsWindowVisible();

		void			HideCursor();
		void			ShowCursor();
		bool			IsCursorVisible();

		void			EnableClipCursor();
		void			DisableClipCursor();
		void			ToggleClipCursor();
		bool			IsCursorClipped();

		void			Maximize();
		void			Minimize();
		void			Hide();
		void			Restore();

	}

	namespace directx {

		ID3D11Device*			GetDevice();
		ID3D11DeviceContext*	GetContext();

		void BeginScene(const float& red, const float& green, const float& blue, const float& alpha);
		void BeginScene(const DirectX::XMFLOAT4& color);
		void EndScene();

		void SetBackBufferRenderTarget();
		void ResetViewport();


		void EnableVSync();
		void DisableVSync();
		void ToggleVSync();

	}

	namespace input {

		bool IsKeyUp	  (const KeyboardButton& Key);
		bool IsKeyDown	  (const KeyboardButton& Key);
		bool IsKeyPressed (const KeyboardButton& Key);
		bool IsKeyReleased(const KeyboardButton& Key);


		bool IsMouseUp		(const MouseButton& button);
		bool IsMouseDown	(const MouseButton& button);
		bool IsMousePressed (const MouseButton& button);
		bool IsMouseReleased(const MouseButton& button);


		DirectX::XMINT2 GetPositionClient();
		DirectX::XMINT2 GetPositionScreen();
		DirectX::XMINT2 GetDelta();
		int				GetWheelDelta();


	}

}




#endif