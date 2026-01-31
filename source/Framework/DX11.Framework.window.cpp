#include "DX11.Framework.h"


LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_INPUT: {
			framework::core::VARS::input::mouse::HandleMouseInput(lParam);
			break;
		}
		case WM_CLOSE: {
			PostQuitMessage(0);
			break;
		}
		case WM_KEYDOWN: {
			if (wParam == VK_ESCAPE)
				PostQuitMessage(0);
			break;
		}
		case WM_SIZE: {

			printf("current size: [%u]x[%u]\n", LOWORD(lParam), HIWORD(lParam));

			using namespace framework::core::VARS::window;
			using namespace framework::core::VARS::directx;


			mWindowWidth_i = LOWORD(lParam);
			mWindowHeight_i = HIWORD(lParam);

			m_aspectRatio = (float)LOWORD(lParam) / (float)HIWORD(lParam);

			OnResize(LOWORD(lParam), HIWORD(lParam));


			break;
		}
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void framework::core::VARS::InitWindow() {
	using namespace framework::core::VARS::window;

	ZeroMemory(&mWindow, sizeof(mWindow));
	mWindow.cbSize = sizeof(WNDCLASSEX);
	mWindow.style = CS_CLASSDC;
	mWindow.lpfnWndProc = WindowProc;
	mWindow.hInstance = mInstance;
	mWindow.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	mWindow.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
	mWindow.hCursor = LoadCursor(nullptr, IDC_ARROW);
	mWindow.hbrBackground = GetSysColorBrush(COLOR_BTNFACE);
	mWindow.lpszClassName = mWindowClass.c_str();

	RECT rect_ = { 0, 0, mScreenWidth, mScreenHeight};

	AdjustWindowRect(&rect_, WS_OVERLAPPEDWINDOW, FALSE);
	RegisterClassEx(&mWindow);


	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	POINT center;
	center.x = (screenWidth - mScreenWidth) / 2;
	center.y = (screenHeight - mScreenHeight) / 2;


	mWindowHandle = CreateWindow(
		mWindowClass.c_str(),
		mWindowTitle.c_str(), 
		WS_OVERLAPPEDWINDOW, 
		center.x, center.y,
		rect_.right - rect_.left, 
		rect_.bottom - rect_.top,
		nullptr, 
		nullptr, 
		mInstance, 
		nullptr);

	ShowWindow(mWindowHandle, mShowCommand);
	UpdateWindow(mWindowHandle);

	if (mConsoleDraw) {
		FILE* conin = stdin;
		FILE* conout = stdout;
		FILE* conerr = stderr;
		AllocConsole();
		AttachConsole(GetCurrentProcessId());
		freopen_s(&conin, "CONIN$", "r", stdin);
		freopen_s(&conout, "CONOUT$", "w", stdout);
		freopen_s(&conerr, "CONOUT$", "w", stderr);
		SetConsoleTitle(L"Лютая консоль для мощной отладки");
	}

}

void framework::core::VARS::TerminateWindow() {

	using namespace framework::core::VARS::window;

	DestroyWindow(mWindowHandle);
	UnregisterClass(mWindowClass.c_str(), mWindow.hInstance);
}


namespace framework {
	namespace core {
		namespace VARS {
			namespace window {
				HINSTANCE mInstance;
				std::wstring mWindowClass = L"WindowClass1";
				std::wstring mWindowTitle;
				int mShowCommand;
				HWND mWindowHandle;
				WNDCLASSEX mWindow;
				UINT mScreenWidth;
				UINT mScreenHeight;
				bool mConsoleDraw = true;
				bool mCursorVisible = true;
				bool mClipCursor = false;

				int mWindowWidth_i;
				int mWindowHeight_i;

				float m_aspectRatio;

				bool isFullscreen;
				int windowedWidth;
				int windowedHeight;
				int windowedPosX;
				int windowedPosY;
			}

		}
	}

	namespace window {

		using namespace framework::core::VARS::window;

		HWND& GetHandle() {
			return mWindowHandle;
		}

		void SetTitle(const std::wstring& title) {
			SetWindowTextW(GetHandle(), title.c_str());
		}
		std::wstring GetTitle() {
			int length = GetWindowTextLengthW(GetHandle());

			if (length == 0)
				return L"";

			std::vector<wchar_t> buffer(length + 1);
			GetWindowTextW(GetHandle(), buffer.data(), length + 1);

			return std::wstring(buffer.data());

		}

		void SetPosition(const int& x, const int& y) {
			SetWindowPos(GetHandle(), NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		}
		void SetPosition(const DirectX::XMINT2& pos) {
			SetWindowPos(GetHandle(), NULL, pos.x, pos.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		}
		DirectX::XMINT2 GetPosition() {
			RECT rect;
			if (GetWindowRect(GetHandle(), &rect)) {
				int x = rect.left;
				int y = rect.top;
				int width = rect.right - rect.left;
				int height = rect.bottom - rect.top;

				return { x,y };
			}
			return { -100000,-100000 };
		}

		float GetAspectRatio() {
			return m_aspectRatio;
		}

		DirectX::XMINT2 GetSize() {
			return { mWindowWidth_i, mWindowHeight_i };
		}

		void SetSize(const int& width, const int& height) {
			SetWindowPos(GetHandle(), NULL, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER);
		}
		void SetSize(const DirectX::XMINT2& size) {
			SetWindowPos(GetHandle(), NULL, 0,0, size.x, size.y, SWP_NOMOVE | SWP_NOZORDER);
		}

		void SetFocus() {
			SetForegroundWindow(GetHandle());
		}
		bool IsFocused() {
			HWND focused_window = GetFocus();
			return focused_window == GetHandle();
		}
		bool IsWindowVisible() {
			return ::IsWindowVisible(GetHandle());
		}

		void HideCursor() {
			while (::ShowCursor(FALSE) >= 0);
			mCursorVisible = false;
		}
		void ShowCursor() {
			while (::ShowCursor(TRUE) < 0);
			mCursorVisible = true;
		}
		bool IsCursorVisible() {
			return mCursorVisible;
		}

		void EnableClipCursor() {
			mClipCursor = true;
			HideCursor();
		}
		void DisableClipCursor() {
			mClipCursor = false;
			ShowCursor();
		}
		void ToggleClipCursor() {
			if (mClipCursor)
				DisableClipCursor();
			else
				EnableClipCursor();
		}
		bool IsCursorClipped() {
			return mClipCursor;
		}

		void Maximize() {
			ShowWindow(GetHandle(), SW_MAXIMIZE);
		}
		void Minimize() {
			ShowWindow(GetHandle(), SW_MINIMIZE);
		}
		void Hide() {
			ShowWindow(GetHandle(), SW_HIDE);
		}
		void Restore() {
			ShowWindow(GetHandle(), SW_RESTORE);
		}


		void ToggleFullscreen() {
			using namespace framework::core::VARS::directx;
			using namespace framework::core::VARS::window;

			if (m_swapChain == nullptr) return;

			BOOL isCurrentlyFullscreen;
			IDXGIOutput* pOutput = nullptr;
			m_swapChain->GetFullscreenState(&isCurrentlyFullscreen, &pOutput);

			if (pOutput) pOutput->Release();

			SetFullscreen(!isCurrentlyFullscreen);
		}
		void SetFullscreen(bool fullscreen) {
			using namespace framework::core::VARS::directx;
			using namespace framework::core::VARS::window;

			if (m_swapChain == nullptr) return;

			BOOL isCurrentlyFullscreen;
			IDXGIOutput* pOutput = nullptr;
			m_swapChain->GetFullscreenState(&isCurrentlyFullscreen, &pOutput);
			if (pOutput) pOutput->Release();

			if ((fullscreen && isCurrentlyFullscreen) || (!fullscreen && !isCurrentlyFullscreen)) {
				return;
			}

			if (m_renderTargetView) {
				m_renderTargetView->Release();
				m_renderTargetView = nullptr;
			}

			if (m_depthStencilView) {
				m_depthStencilView->Release();
				m_depthStencilView = nullptr;
			}

			if (m_depthStencilBuffer) {
				m_depthStencilBuffer->Release();
				m_depthStencilBuffer = nullptr;
			}

			HWND hwnd = framework::window::GetHandle();

			if (fullscreen) {
				// Сохраняем текущие параметры окна
				RECT windowRect;
				GetWindowRect(hwnd, &windowRect);
				windowedWidth = windowRect.right - windowRect.left;
				windowedHeight = windowRect.bottom - windowRect.top;
				windowedPosX = windowRect.left;
				windowedPosY = windowRect.top;

				// Получаем информацию о мониторе, на котором находится окно
				HMONITOR hMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
				MONITORINFO monitorInfo = {  };
				GetMonitorInfo(hMonitor, &monitorInfo);

				// Переключаемся в полноэкранный режим
				HRESULT hr = m_swapChain->SetFullscreenState(TRUE, nullptr);
				if (SUCCEEDED(hr)) {
					isFullscreen = true;

					// Изменяем размер окна под размер монитора
					int screenWidth = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
					int screenHeight = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;

					// Изменяем размер буферов swap chain
					hr = m_swapChain->ResizeBuffers(
						0, // Сохраняем текущее количество буферов
						screenWidth,
						screenHeight,
						DXGI_FORMAT_UNKNOWN, // Сохраняем текущий формат
						0  // Флаги
					);

					if (SUCCEEDED(hr)) {
						// Воссоздаем ресурсы с новыми размерами
						CreateRenderTarget();
						CreateDepthStencil(screenWidth, screenHeight);

						// Обновляем вьюпорт
						m_viewport.Width = static_cast<float>(screenWidth);
						m_viewport.Height = static_cast<float>(screenHeight);
						m_deviceContext->RSSetViewports(1, &m_viewport);
					}
				}
			}
			else {
				// Переключаемся в оконный режим
				HRESULT hr = m_swapChain->SetFullscreenState(FALSE, nullptr);
				if (SUCCEEDED(hr)) {
					isFullscreen = false;

					// Восстанавливаем стиль окна (убираем WS_POPUP если был)
					LONG style = GetWindowLong(hwnd, GWL_STYLE);
					style &= ~WS_POPUP;
					style |= WS_OVERLAPPEDWINDOW;
					SetWindowLong(hwnd, GWL_STYLE, style);

					// Восстанавливаем положение и размер окна
					SetWindowPos(hwnd, HWND_TOP,
						windowedPosX, windowedPosY,
						windowedWidth, windowedHeight,
						SWP_FRAMECHANGED | SWP_SHOWWINDOW);

					// Изменяем размер буферов swap chain
					hr = m_swapChain->ResizeBuffers(
						0, // Сохраняем текущее количество буферов
						windowedWidth,
						windowedHeight,
						DXGI_FORMAT_UNKNOWN, // Сохраняем текущий формат
						0  // Флаги
					);

					if (SUCCEEDED(hr)) {
						// Воссоздаем ресурсы с новыми размерами
						CreateRenderTarget();
						CreateDepthStencil(windowedWidth, windowedHeight);

						// Обновляем вьюпорт
						m_viewport.Width = static_cast<float>(windowedWidth);
						m_viewport.Height = static_cast<float>(windowedHeight);
						m_deviceContext->RSSetViewports(1, &m_viewport);
					}
				}
			}
		}
		bool IsFullscreen() {
			using namespace framework::core::VARS::directx;

			if (m_swapChain == nullptr) return false;

			BOOL isFullscreen;
			IDXGIOutput* pOutput = nullptr;
			m_swapChain->GetFullscreenState(&isFullscreen, &pOutput);

			if (pOutput) pOutput->Release();

			return isFullscreen != FALSE;
		}

	}
}