/**
\file MainWindow.h
\brief MainWindow class declaration.
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

#if !defined MAINWINDOW_H
#define MAINWINDOW_H


#include <string>
#include <list>

#include "Window.h"


class FunctionParser;


/// Main window.
class MainWindow : public Window
{
private:
    enum Mode
    {
        Function,
        Minterm,
        Maxterm
    };

private:
    static BOOL CALLBACK AboutDlgProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
    /// Constructs a new MainWindow object.
    explicit MainWindow(HINSTANCE hInstance=NULL);

private:
    void set_mode(Mode mode);

    void clear_controls();
    void delete_columns();

    void build_minterm_from(Mode mode);

    void insert_function(unsigned int value, unsigned int row);
    void insert_value(unsigned int value, unsigned int row, unsigned int column);
    bool insert_row(unsigned int row, FunctionParser* const parser);
    void insert_columns();
    void build_table(FunctionParser* const parser);

    void get_variables();

    void generate();
    void save();

private:
    virtual bool OnCreate(LPCREATESTRUCT const lpCreateStruct);
    virtual void OnSize(UINT state, int cx, int cy);
    virtual bool OnCommand(int id, HWND hWndCtl, UINT codeNotify);
    virtual void OnClose();
    virtual void OnDestroy();

private:
    HWND m_hWndVariables, m_hWndFunction, m_hWndMinterms, m_hWndMaxterms;
    HWND m_hWndGenerate;
    HWND m_hWndTable;

    Mode m_mode;

    std::string m_last_save_file;
    std::list<char> m_variables;
};


#endif