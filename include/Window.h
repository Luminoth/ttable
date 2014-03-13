/*
====================
File: Window.h
Author: Shane Lillie
Description: Window class declaration.

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

#if !defined WINDOW_H
#define WINDOW_H


#pragma warning(disable : 4290)


#include <stdexcept>
#include <string>

#if !defined WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <commdlg.h>
#include <richedit.h>
#include <shellapi.h>

#include "controls.h"
//#include "messages.h"

#include "../rsc/resource.h"


/*
class Window

Subclass this to get more specificly detailed windows.
Include this file instead of including windows.h and other Win32 headers.
*/
class Window
{
public:
    class WindowException : public std::exception
    {
    public:
        explicit WindowException(const std::string& what) : _what(what) {}
        virtual ~WindowException() throw() {}
        virtual const char* what() const throw() { return _what.c_str(); }
    private:
        std::string _what;
    };

public:
    // registers a new window class
    // parameters come from WNDCLASSEX structure documentation
    // throws WindowException containing the error that occured
    static void RegisterClass(HINSTANCE hInstance, DWORD dwStyle, const std::string& name) throw(WindowException);

private:
	static Window* const getWindow(HWND hWnd);
    static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
    // constructs a Window object, but not the window itself
    explicit Window(HINSTANCE hInstance=NULL);

    // destroyes the window if it's valid
    virtual ~Window() throw();

public:
    // creates the window itself with CreateWindowEx()
    // parameters are from CreateWindowEx() documentation
    // returns true if the window was created successfully
    bool Create(DWORD dwExStyle, const std::string& class_name, const std::string& title, DWORD dwStyle, HWND hWndParent=NULL);

    // calls ShowWindow(m_hWnd, nCmdShow)
    BOOL ShowWindow(int nCmdShow);

    // calls UpdateWindow(m_hWnd)
    BOOL UpdateWindow();

    // calls SetFocus(m_hWnd)
    HWND SetFocus();

    // calls SetWindowFont(m_hWnd, GetStockObject(DEFAULT_GUI_FONT), bRedraw)
    void SetGuiFont(BOOL bRedraw=FALSE);

    // calls SetWindowText(m_hWnd, text.c_str())
    void SetText(const std::string& text);

    // calls MoveWindow(m_hWnd, x, y, width, height, bRepaint)
    // if any position/size parameter is < 0, the current value is used
    BOOL MoveWindow(int x, int y, int width, int height, BOOL bRepaint);

    // calls GetClientRect(m_hWnd, rect)
    BOOL GetClientRect(LPRECT rect);

    // calls GetWindowRect(m_hWnd, rect)
    BOOL GetWindowRect(LPRECT rect);

    // calls CloseWindow(m_hWnd)
    // NOTE: this minimizes the window, it doesn't close it
    BOOL CloseWindow();

    // calls DestroyWindow(m_hWnd)
    BOOL DestroyWindow() throw();

    // calls LoadMenu(m_hInstance, lpMenuName)
    HMENU LoadMenu(LPCTSTR lpMenuName);
    HMENU LoadMenu(int iMenuRes);   // uses MAKEINTRESOURCE(iMenuRes) to get the menu name

    // calls GetMenu(m_hWnd)
    HMENU GetMenu();

    // calls SetMenu(m_hWnd, hMenu)
    BOOL SetMenu(HMENU hMenu);

    // calls SendMessage(WM_SETICON, type, hIcon)
    HICON SetIcon(int type, HICON hIcon);

    // calls GetDC(m_hWnd)
    HDC GetDC();

    // calls ReleaseDC(m_hWnd, hDC)
    int ReleaseDC(HDC hDC);

    // calls BeginPaint(m_hWnd, lpPaint)
    HDC BeginPaint(LPPAINTSTRUCT lpPaint);

    // calls EndPaint(m_hWnd, lpPaint)
    BOOL EndPaint(LPPAINTSTRUCT lpPaint);

    // calls ScreenToClient(m_hWnd, lpPoint)
    BOOL ScreenToClient(LPPOINT lpPoint);

    // calls ClientToScreen(m_hWnd, lpPoint)
    BOOL ClientToScreen(LPPOINT lpPoint);

    // calls InvalidateRect(m_hWnd, lpRect, bErase)
    BOOL InvalidateRect(CONST RECT* lpRect, BOOL bErase);

    // calls ValidateRect(m_hWnd, lpRect)
    BOOL ValidateRect(CONST RECT* lpRect);

    // calls LoadIcon(m_hInstance, lpIconName)
    HICON LoadIcon(LPCTSTR lpIconName);
    HICON LoadIcon(int iIconRes);   // uses MAKEINTRESOURCE(iIconRes) to get the icon name

    // calls LoadImage(m_hInstance, lpszName, uType, cxDesired, cyDesired, fuLoad)
    HANDLE LoadImage(LPCTSTR lpszName, UINT uType, int cxDesired, int cyDesired, UINT fuLoad);
    HANDLE LoadImage(int iResource, UINT uType, int cxDesired, int cyDesired, UINT fuLoad);   // uses MAKEINTRESOURCE(iResource) to get the resource name

    // calls SendMessage(m_hWnd, uMsg, wParam, lParam)
    LRESULT SendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

    // calls DialogBox(m_hInstance, lpTemplate, m_hWnd, lpDialogFunc)
    INT_PTR Dialog(LPCTSTR lpTemplate, DLGPROC lpDialogFunc);
    INT_PTR Dialog(int iTemplateRes, DLGPROC lpDialogFunc);   // uses MAKEINTRESOURCE(iTemplateRes) to get the dialog name

    // calls MessageBox(m_hWnd, text.c_str(), caption.c_str(), uType)
    int MessageBox(const std::string& text, const std::string& caption, UINT uType);

public:
    // returns the application instance
    HINSTANCE instance() const
    {
        return m_hInstance;
    }

    // returns a handle to the window
    HWND handle() const
    {
        return m_hWnd;
    }

    // returns a handle to the parent window (NULL if there isn't one)
    HWND parent() const
    {
        return m_hWndParent;
    }

    // returns true if the window is valid
    operator BOOL() const
    {
        return IsWindow(m_hWnd);
    }

protected:
    // WM_CREATE handler
    // lpCreateStruct is a pointer to a CREATESTRUCT structure that contains information about the window being created
    // returns true if the creation succeeded, false otherwise (and the window will be destroyed)
    virtual bool OnCreate(LPCREATESTRUCT const lpCreateStruct) { return true; }

    // WM_PAINT handler.
    virtual void OnPaint();

    // WM_SIZE handler
    // state specifies the type of resizing request
    // cx/cy are the new width/height of the client area
    virtual void OnSize(UINT state, int cx, int cy) {}

    // WM_SHOWWINDOW handler
    // fShow is true if the window is being shown, false if the window is being hidden
    // status specifies the status of the window being shown (see http://msdn.microsoft.com/library/ for more details)
    virtual void OnShowWindow(BOOL fShow, UINT status) {}

    // WM_SETFOCUS handler
    // hWndOldFocus is a handle to the window that had focus before.
    virtual void OnSetFocus(HWND hWndOldFocus) {}

    // WM_CONTEXTMENU handler
    // hWndCtl is a handle to the window in which the user right clicked the mouse
    // x/y are the horizontal and vertical position of the cursor, in screen coordinates, at the time of the mouse click
    virtual void OnContextMenu(HWND hWndCtl, int x, int y) {}

    // WM_COMMAND handler
    // id is the identifier of the menu item, control, or accelerator
    // hWndCtl is a handle to the control sending the message if the message is from a control
    // codeNotify is the notification code if the message is from a control (1 from an accelerator, 0 from a menu)
    // returns true if the message was handled, false if it wasn't
    virtual bool OnCommand(int id, HWND hWndCtl, UINT codeNotify) { return false; }

    // WM_NOTIFY handler
    // idCtrl is the identifier of the common control sending the message
    // (not guaranteed to be unique, an application should use the hwndFrom or idFrom member of the NMHDR structure to identify the control)
    // pnmh is a pointer to an NMHDR structure that contains the notification code and additional information
    // (For some notification messages, this parameter points to a larger structure that has the NMHDR structure as its first member)
    // returns true if the message was handled, false if it wasn't
    virtual bool OnNotify(int idCtrl, LPNMHDR const pnmh) { return false; }

    // WM_TIMER handler
    // id is the timer ID
    virtual void OnTimer(UINT id) {}

    // WM_CLOSE handler.
    virtual void OnClose() {}

    // WM_DESTROY handler.
    virtual void OnDestroy() {}

    // default message handler
    // returns true if the message was handler, false to pass on to DefWindowProc()
    virtual bool OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) { return false; }

private:
    HINSTANCE m_hInstance;
    HWND m_hWnd, m_hWndParent;
};


#endif