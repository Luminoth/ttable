/**
\file main.cpp
\brief The main application functions and constructs.
\author Shane Lillie

\verbatim
Copyright 2003 Energon Software

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation,
Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
\endverbatim
*/

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "comdlg32.lib")

/**
\mainpage

@todo Detailed documentation needs written.
*/


#include <string>

#include "../include/main.h"
#include "../include/MainWindow.h"
#include "../include/error.h"


/*
 *  constants
 *
 */


/**
\defgroup wscs Window Size Constants

@{
*/
const int MAIN_WIDTH = 300;
const int MAIN_HEIGHT = 300;
/** @} */


/*
 *  globals
 *
 */


MainWindow g_main_window;


/*
 *  functions
 *
 */


/**
\brief Creates all of the window classes for the application.

@retval true All classes were created successfully.
@retval false A class could not be created.
*/
bool init_window_classes(HINSTANCE hInstance)
{
    try {
        Window::RegisterClass(hInstance, CS_DBLCLKS | CS_PARENTDC, "ttable");
    } catch(Window::WindowException& e) {
        MessageBox(NULL, e.what(), "Window Class Error", MB_OK | MB_ICONERROR);
        return false;
    }
    return true;
}


/**
\brief Fully creates the application windows.

@retval true All windows were created successfully.
@retval false A window could not be created.
*/
bool create_windows(HINSTANCE hInstance)
{
    g_main_window = MainWindow(hInstance);
    if(!g_main_window.Create(WS_EX_CONTROLPARENT, "ttable", "Truth Table Generator",
        WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX))
    {
        MessageBox(NULL, last_error().c_str(), "Main Window Error", MB_OK | MB_ICONERROR);
        return false;
    }
    g_main_window.SetGuiFont();
    g_main_window.MoveWindow(-1, -1, MAIN_WIDTH, MAIN_HEIGHT, FALSE);

    return true;
}


/**
\brief Application entry point.
*/
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(icex);
    icex.dwICC = ICC_WIN95_CLASSES;
    if(!InitCommonControlsEx(&icex)) {
        std::string err("Could not initialize common controls: " + last_error());
        MessageBox(NULL, err.c_str(), "Control Error", MB_OK | MB_ICONERROR);
        return 0;
    }

    if(!init_window_classes(hInstance))
        return 0;
    if(!create_windows(hInstance))
        return 0;

    g_main_window.ShowWindow(nCmdShow);
    g_main_window.UpdateWindow();
    g_main_window.SetFocus();

    MSG msg;
    int ret = -1;
    while((ret = GetMessage(&msg, NULL, 0, 0)) != 0) {
        if(-1 == ret) {
            MessageBox(NULL, last_error().c_str(), "Message Error", MB_OK | MB_ICONERROR);
            break;
        }

        if((!IsWindow(g_main_window.handle()) ||
            !IsDialogMessage(g_main_window.handle(), &msg)))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return static_cast<int>(msg.wParam);
}