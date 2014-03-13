/*
====================
File: Window.cpp
Author: Shane Lillie
Description: Window class definition.

(c) 2002-2003 Energon Software

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
====================
*/

#pragma warning(disable : 4312)
#pragma warning(disable : 4311)

#include <cstring>
#include <string>

#include "../include/Window.h"


/*
 *  Window class functions
 *
 */


void Window::RegisterClass(HINSTANCE hInstance, DWORD dwStyle, const std::string& name) throw(WindowException)
{
    WNDCLASSEX wndcls;
    wndcls.cbSize        = sizeof(WNDCLASSEX);
    wndcls.style         = dwStyle;
    wndcls.lpfnWndProc   = StaticWndProc;
    wndcls.cbClsExtra    = 0;
    wndcls.cbWndExtra    = 0;
    wndcls.hInstance     = hInstance;
    wndcls.hIcon         = ::LoadIcon(NULL, IDI_APPLICATION);
    wndcls.hCursor       = ::LoadCursor(NULL, IDC_ARROW);
    wndcls.hbrBackground = (HBRUSH)COLOR_APPWORKSPACE+1;
    wndcls.lpszMenuName  = NULL;
    wndcls.lpszClassName = name.c_str();
    wndcls.hIconSm       = ::LoadIcon(NULL, IDI_APPLICATION);

    if(!RegisterClassEx(&wndcls)) {
        char buffer[512];
        std::memset(buffer, 0, 512);

        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                    NULL, GetLastError(),
                    0, buffer, 512, NULL);

        throw WindowException("Could not register window class: " + std::string(buffer));
    }
}


Window* const Window::getWindow(HWND hWnd)
{
    return reinterpret_cast<Window*>(GetWindowLong(hWnd, GWL_USERDATA));
}


LRESULT CALLBACK Window::StaticWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if(uMsg == WM_NCCREATE)
        SetWindowLong(hWnd, GWL_USERDATA, reinterpret_cast<LONG>(reinterpret_cast<LPCREATESTRUCT>(lParam)->lpCreateParams));

    Window* window = getWindow(hWnd);
    if(window) {
        if(!window->m_hWnd)
            window->m_hWnd = hWnd;

        switch(uMsg)
        {
        case WM_CREATE:
            if(!window->OnCreate(reinterpret_cast<LPCREATESTRUCT>(lParam)))
                return -1;
            break;
        case WM_PAINT:
            window->OnPaint();
            break;
        case WM_SIZE:
            window->OnSize(static_cast<UINT>(wParam), LOWORD(lParam), HIWORD(lParam));
            break;
        case WM_SHOWWINDOW:
            window->OnShowWindow(static_cast<BOOL>(wParam), static_cast<UINT>(lParam));
            break;
        case WM_SETFOCUS:
            window->OnSetFocus(reinterpret_cast<HWND>(wParam));
            break;
        case WM_CONTEXTMENU:
            window->OnContextMenu(reinterpret_cast<HWND>(wParam), GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            break;
        case WM_COMMAND:
            if(!window->OnCommand(LOWORD(wParam), reinterpret_cast<HWND>(lParam), HIWORD(wParam)))
                return DefWindowProc(hWnd, uMsg, wParam, lParam);
            break;
        case WM_NOTIFY:
            if(!window->OnNotify(static_cast<int>(wParam), reinterpret_cast<LPNMHDR>(lParam)))
                return DefWindowProc(hWnd, uMsg, wParam, lParam);
            break;
        case WM_TIMER:
            window->OnTimer(static_cast<UINT>(wParam));
            break;
        case WM_CLOSE:
            window->OnClose();
            break;
        case WM_DESTROY:
            window->OnDestroy();
            break;
        default:
            if(!window->OnMessage(uMsg, wParam, lParam))
                return DefWindowProc(hWnd, uMsg, wParam, lParam);
            break;
        }
    } else return DefWindowProc(hWnd, uMsg, wParam, lParam);
    return 0;
}


/*
 *  Window methods
 *
 */


Window::Window(HINSTANCE hInstance)
    : m_hInstance(hInstance),
        m_hWnd(NULL), m_hWndParent(NULL)
{
}


Window::~Window()
{
    if(IsWindow(m_hWnd))
        DestroyWindow();
}


bool Window::Create(DWORD dwExStyle, const std::string& class_name, const std::string& title, DWORD dwStyle, HWND hWndParent)
{
    m_hWnd = CreateWindowEx(
            dwExStyle,
            class_name.c_str(),
            title.c_str(),
            dwStyle,
            CW_USEDEFAULT, CW_USEDEFAULT,
            CW_USEDEFAULT, CW_USEDEFAULT,
            hWndParent,
            NULL,
            instance(),
            this);

    m_hWndParent = hWndParent;
    return (m_hWnd != NULL);
}


BOOL Window::ShowWindow(int nCmdShow)
{
    return ::ShowWindow(m_hWnd, nCmdShow);
}


BOOL Window::UpdateWindow()
{
    return ::UpdateWindow(m_hWnd);
}


HWND Window::SetFocus()
{
    return ::SetFocus(m_hWnd);
}


void Window::SetGuiFont(BOOL bRedraw)
{
    SetWindowFont(m_hWnd, GetStockObject(DEFAULT_GUI_FONT), bRedraw);
}


void Window::SetText(const std::string& text)
{
    ::SetWindowText(m_hWnd, text.c_str());
}


BOOL Window::MoveWindow(int x, int y, int width, int height, BOOL bRepaint)
{
    RECT rect;
    GetWindowRect(&rect);

    return ::MoveWindow(m_hWnd,
        (x < 0) ? rect.left : x,
        (y < 0) ? rect.top : y,
        (width < 0) ? rect.right : width,
        (height < 0) ? rect.bottom : height,
        bRepaint);
}


BOOL Window::GetClientRect(LPRECT rect)
{
    return ::GetClientRect(m_hWnd, rect);
}


BOOL Window::GetWindowRect(LPRECT rect)
{
    return ::GetWindowRect(m_hWnd, rect);
}


BOOL Window::CloseWindow()
{
    return ::CloseWindow(m_hWnd);
}


BOOL Window::DestroyWindow() throw()
{
    BOOL r = ::DestroyWindow(m_hWnd);
    m_hWnd = NULL;
    return r;
}


HMENU Window::LoadMenu(LPCTSTR lpMenuName)
{
    return ::LoadMenu(m_hInstance, lpMenuName);
}


HMENU Window::LoadMenu(int iMenuRes)
{
    return ::LoadMenu(m_hInstance, MAKEINTRESOURCE(iMenuRes));
}


HMENU Window::GetMenu()
{
    return ::GetMenu(m_hWnd);
}


BOOL Window::SetMenu(HMENU hMenu)
{
    return ::SetMenu(m_hWnd, hMenu);
}


HICON Window::SetIcon(int type, HICON hIcon)
{
    return reinterpret_cast<HICON>(SendMessage(WM_SETICON, static_cast<WPARAM>(type), reinterpret_cast<LPARAM>(hIcon)));
}


HDC Window::GetDC()
{
    return ::GetDC(m_hWnd);
}


int Window::ReleaseDC(HDC hDC)
{
    return ::ReleaseDC(m_hWnd, hDC);
}


HDC Window::BeginPaint(LPPAINTSTRUCT lpPaint)
{
    return ::BeginPaint(m_hWnd, lpPaint);
}


BOOL Window::EndPaint(LPPAINTSTRUCT lpPaint)
{
    return ::EndPaint(m_hWnd, lpPaint);
}


BOOL Window::ScreenToClient(LPPOINT lpPoint)
{
    return ::ScreenToClient(m_hWnd, lpPoint);
}


BOOL Window::ClientToScreen(LPPOINT lpPoint)
{
    return ::ClientToScreen(m_hWnd, lpPoint);
}


BOOL Window::InvalidateRect(CONST RECT* lpRect, BOOL bErase)
{
    return ::InvalidateRect(m_hWnd, lpRect, bErase);
}


BOOL Window::ValidateRect(CONST RECT* lpRect)
{
    return ::ValidateRect(m_hWnd, lpRect);
}


HICON Window::LoadIcon(LPCTSTR lpIconName)
{
    return ::LoadIcon(m_hInstance, lpIconName);
}


HICON Window::LoadIcon(int iIconRes)
{
    return ::LoadIcon(m_hInstance, MAKEINTRESOURCE(iIconRes));
}


HANDLE Window::LoadImage(LPCTSTR lpszName, UINT uType, int cxDesired, int cyDesired, UINT fuLoad)
{
    return ::LoadImage(m_hInstance, lpszName, uType, cxDesired, cyDesired, fuLoad);
}


HANDLE Window::LoadImage(int iResource, UINT uType, int cxDesired, int cyDesired, UINT fuLoad)
{
    return ::LoadImage(m_hInstance, MAKEINTRESOURCE(iResource), uType, cxDesired, cyDesired, fuLoad);
}


LRESULT Window::SendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return ::SendMessage(m_hWnd, uMsg, wParam, lParam);
}


INT_PTR Window::Dialog(LPCTSTR lpTemplate, DLGPROC lpDialogFunc)
{
    return ::DialogBox(m_hInstance, lpTemplate, m_hWnd, lpDialogFunc);
}


INT_PTR Window::Dialog(int iTemplateRes, DLGPROC lpDialogFunc)
{
    return ::DialogBox(m_hInstance, MAKEINTRESOURCE(iTemplateRes), m_hWnd, lpDialogFunc);
}


int Window::MessageBox(const std::string& text, const std::string& caption, UINT uType)
{
    return ::MessageBox(m_hWnd, text.c_str(), caption.c_str(), uType);
}


void Window::OnPaint()
{
    PAINTSTRUCT ps;
    BeginPaint(&ps);
    EndPaint(&ps);
}