#include "DX11.Framework.h"

namespace framework {
	namespace core {

		void Init(
			const HINSTANCE& hInstance, 
			const int& nCmdShow,
			const bool& showConsole,
			const std::wstring& title, 
			const int& width, 
			const int& height) {

			using namespace framework::core::VARS;

			window::mInstance = hInstance;
			window::mShowCommand = nCmdShow;
			window::mConsoleDraw = showConsole;

			window::mScreenWidth = width;
			window::mScreenHeight = height;
			window::mWindowTitle = title;

			InitWindow();
			InitDirectX();
			InitInput();
			InitTime();

		}
		void Terminate() {
			using namespace framework::core::VARS;

			TerminateWindow();
			TerminateDirectX();
			TerminateTime();
			TerminateInput();
		}
		void Update() {
			core::VARS::time::fps_limiter->Process();
			core::VARS::UpdateInput();
		}
		
	}
}