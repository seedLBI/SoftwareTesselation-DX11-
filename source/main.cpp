#include <windows.h>
#include "Application/Application.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int nCmdShow) {

    Application app(hInstance, nCmdShow);
   
    bool shouldExit = false;
    while (!shouldExit) {

        app.Update();
        app.Render();


        MSG msg;
        while (!shouldExit && PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {

            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT)
                shouldExit = true;
        }
    }

    return 0;
}