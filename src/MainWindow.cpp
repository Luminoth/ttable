/**
\file MainWindow.cpp
\brief MainWindow class definition.
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

\todo Function minimization.
\todo Min/Maxterm functions.
\todo I don't like how this stores variables...
*/


#include <cstdio>
#include <fstream>

#include "../include/MainWindow.h"
#include "../include/FunctionParser.h"
#include "../include/main.h"


/*
 *  MainWindow class functions
 *
 */


BOOL CALLBACK MainWindow::AboutDlgProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
    case WM_COMMAND:
        switch(LOWORD(wParam))
        {
        case IDOK:
            EndDialog(hWndDlg, wParam);
            return TRUE;
        }
    }
    return FALSE;
}


/*
 *  MainWindow methods
 *
 */


MainWindow::MainWindow(HINSTANCE hInstance)
    : Window(hInstance), m_hWndVariables(NULL), m_hWndFunction(NULL), m_hWndMinterms(NULL), m_hWndMaxterms(NULL),
        m_hWndGenerate(NULL), m_hWndTable(NULL), m_mode(Function)
{
}


/// Sets the current mode
void MainWindow::set_mode(Mode mode)
{
/* FIXME: based on current mode, take values, interpret in new mode */

    switch(mode)
    {
    case Function:
        generate();

        CheckMenuItem(GetMenu(), ID_MODE_FUNCTION, MF_CHECKED);
        CheckMenuItem(GetMenu(), ID_MODE_MINTERM, MF_UNCHECKED);
        CheckMenuItem(GetMenu(), ID_MODE_MAXTERM, MF_UNCHECKED);

        SetWindowText(GetDlgItem(handle(), IDC_MODE_STATIC), "Function");

        //build_function_from(m_mode);

        ::ShowWindow(m_hWndFunction, SW_SHOW);
        ::ShowWindow(m_hWndMinterms, SW_HIDE);
        ::ShowWindow(m_hWndMaxterms, SW_HIDE);
        break;
    case Minterm:
        generate();

        CheckMenuItem(GetMenu(), ID_MODE_FUNCTION, MF_UNCHECKED);
        CheckMenuItem(GetMenu(), ID_MODE_MINTERM, MF_CHECKED);
        CheckMenuItem(GetMenu(), ID_MODE_MAXTERM, MF_UNCHECKED);

        SetWindowText(GetDlgItem(handle(), IDC_MODE_STATIC), "Minterm");

        build_minterm_from(m_mode);

        ::ShowWindow(m_hWndFunction, SW_HIDE);
        ::ShowWindow(m_hWndMinterms, SW_SHOW);
        ::ShowWindow(m_hWndMaxterms, SW_HIDE);
        break;
    case Maxterm:
        generate();

        CheckMenuItem(GetMenu(), ID_MODE_FUNCTION, MF_UNCHECKED);
        CheckMenuItem(GetMenu(), ID_MODE_MINTERM, MF_UNCHECKED);
        CheckMenuItem(GetMenu(), ID_MODE_MAXTERM, MF_CHECKED);

        SetWindowText(GetDlgItem(handle(), IDC_MODE_STATIC), "Maxterm");

        //build_maxterm_from(m_mode);

        ::ShowWindow(m_hWndFunction, SW_HIDE);
        ::ShowWindow(m_hWndMinterms, SW_HIDE);
        ::ShowWindow(m_hWndMaxterms, SW_SHOW);
        break;
    }
    m_mode = mode;
}


/// Clears all control values
void MainWindow::clear_controls()
{
    SetWindowText(m_hWndVariables, "");
    SetWindowText(m_hWndFunction, "");
    SetWindowText(m_hWndMinterms, "");
    SetWindowText(m_hWndMaxterms, "");

    delete_columns();
}


/// Deletes the list view columns
void MainWindow::delete_columns()
{
    ListView_DeleteAllItems(m_hWndTable);

    // column 0 can't be deleted, so we gotta fake around it
    const int count = Header_GetItemCount(ListView_GetHeader(m_hWndTable));

    LVCOLUMN lvc;
    lvc.mask = LVCF_WIDTH;
    lvc.cx = 0;
    ListView_InsertColumn(m_hWndTable, 0, &lvc);

    for(int i=0; i<count; ++i)
        ListView_DeleteColumn(m_hWndTable, 1);
}


/// Builds the minterm stuff from the given mode.
void MainWindow::build_minterm_from(Mode mode)
{
    const int size = 1 << m_variables.size();

    int i=0;
    bool f = false;

    char scratch2[32] = {0};
    switch(mode)
    {
    case Function:
        char scratch1[8];
        for(i=0; i<size; ++i) {
            ListView_GetItemText(m_hWndTable, i, static_cast<int>(m_variables.size()), scratch1, 8);
            if(!atoi(scratch1))
                continue;

            if(!f) {
                _snprintf(scratch2, 32, "%d", i);
                f = true;
                continue;
            }
            _snprintf(scratch2, 32, "%s, %d", scratch2, i);
        }
        Edit_SetText(m_hWndMinterms, scratch2);
        break;
    case Maxterm:
        break;
    }
}


/// Inserts the function value for a given row.
void MainWindow::insert_function(unsigned int value, unsigned int row)
{
    if(value < 0)
        return;

    char scratch[8];
    _snprintf(scratch, 8, "%d", value);

    LVITEM lvi;
    lvi.mask = LVIF_TEXT;
    lvi.iItem = row;
    lvi.iSubItem = static_cast<int>(m_variables.size());
    lvi.pszText = scratch;
    ListView_InsertItem(m_hWndTable, &lvi);
    ListView_SetItemText(m_hWndTable, row, static_cast<int>(m_variables.size()), scratch);
}


/// Inserts a value in the truth table.
void MainWindow::insert_value(unsigned int value, unsigned int row, unsigned int column)
{
    if(value < 0)
        return;

    char scratch[8];
    _snprintf(scratch, 8, "%d", value);

    LVITEM lvi;
    lvi.mask = LVIF_TEXT;
    lvi.iItem = row;
    lvi.iSubItem = column;
    lvi.pszText = scratch;
    ListView_InsertItem(m_hWndTable, &lvi);
    ListView_SetItemText(m_hWndTable, row, column, scratch);
}


/// Inserts a row in the truth table.
bool MainWindow::insert_row(unsigned int row, FunctionParser* const parser)
{
    for(int i = parser->size()-1; i>=0; --i)
        insert_value(getbit(i, row), row, (parser->size()-1) - i);

    try {
        insert_function(parser->eval(row), row);
    } catch(FunctionParser::FunctionParserException& e) {
        MessageBox(e.what(), "Function Evaluation Error", MB_OK | MB_ICONERROR);
        return false;
    }
    return true;
}


/// Builds the truth table columns.
void MainWindow::insert_columns()
{
    LVCOLUMN lvc;
    lvc.mask = LVCF_FMT | LVCF_TEXT;
    lvc.fmt = LVCFMT_LEFT;

    int i=0;
    for(std::list<char>::const_iterator it = m_variables.begin(); it != m_variables.end(); ++it) {
        char v[2] = { *it, '\0' };

        lvc.pszText = v;
        ListView_InsertColumn(m_hWndTable, i, &lvc);
        ListView_SetColumnWidth(m_hWndTable, i++, 20);
    }

    if(i > 0) {
        lvc.pszText = "f";
        ListView_InsertColumn(m_hWndTable, i, &lvc);
        ListView_SetColumnWidth(m_hWndTable, i, 20);
    }
}


/// Builds the truth table.
void MainWindow::build_table(FunctionParser* const parser)
{
    if(!parser || !parser->size())
        return;

    insert_columns();

    const unsigned int size = 1 << parser->size();
    for(unsigned int row = 0; row < size; ++row)
        if(!insert_row(row, parser))
            return;
}


/// Gets the variables for the function, etc.
void MainWindow::get_variables()
{
    m_variables.clear();

    char vars[128];
    Edit_GetText(m_hWndVariables, vars, 128);

    char* tok = strtok(vars, ", \t\r\f\n");
    while(tok) {
        if(!isalpha(tok[0])) {
            MessageBox("Variable must be a letter", "Variable List Parse Error", MB_OK | MB_ICONERROR);
            return;
        }

        m_variables.push_back(tok[0]);
        tok = strtok(NULL, ", \t\r\f\n");
    }
}


/// Generates the truth table.
void MainWindow::generate()
{
    delete_columns();

    get_variables();

    FunctionParser p;
    switch(m_mode)
    {
    case Function:
        char func[128];
        Edit_GetText(m_hWndFunction, func, 128);

        try {
            p = FunctionParser(m_variables, func);
        } catch(FunctionParser::FunctionParserException& e) {
            MessageBox(e.what(), "Function Parser Error", MB_OK | MB_ICONERROR);
            return;
        }

        build_table(&p);
        break;
    case Minterm:
        break;
    case Maxterm:
        break;
    }
}


/// Saves the truth table to an excel parsable text file.
void MainWindow::save()
{
/* FIXME: what we write is mode dependant */
    char outfile[MAX_PATH];
    strncpy(outfile, m_last_save_file.c_str(), MAX_PATH);

    OPENFILENAME of;
    memset(&of, 0, sizeof(of));
    of.lStructSize = sizeof(OPENFILENAME);
    of.hwndOwner = handle();
    of.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
    of.lpstrDefExt = "txt";
    of.lpstrFile = outfile;
    of.nMaxFile = MAX_PATH;
    of.Flags = OFN_EXPLORER | OFN_CREATEPROMPT | OFN_HIDEREADONLY | OFN_NOREADONLYRETURN | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
    if(!GetSaveFileName(&of))
        return;
    m_last_save_file = outfile;

    std::ofstream f(outfile);
    if(!f) {
        MessageBox("Could not open output file", "File Error", MB_OK | MB_ICONERROR);
        return;
    }

    char function[128];
    Edit_GetText(m_hWndFunction, function, 128);
    f << "f = " << function << std::endl << std::endl;

    for(std::list<char>::const_iterator it = m_variables.begin(); it != m_variables.end(); ++it)
        f << *it << "\t";
    f << "f" << std::endl;

    int items = ListView_GetItemCount(m_hWndTable);

    char t[8];
    memset(t, 0, 8);

    LVITEM lvi;
    lvi.mask = LVIF_TEXT;
    lvi.pszText = t;
    lvi.cchTextMax = 8;
    for(int i=0; i<items; ++i) {
        lvi.iItem = i;
        for(size_t j=0; j<m_variables.size(); ++j) {
            lvi.iSubItem = static_cast<int>(j);
            ListView_GetItem(m_hWndTable, &lvi);
            f << t << "\t";
        }
        lvi.iSubItem = static_cast<int>(m_variables.size());
        ListView_GetItem(m_hWndTable, &lvi);
        f << t << std::endl;
    }
    f.close();
}


bool MainWindow::OnCreate(LPCREATESTRUCT const lpCreateStruct)
{
    SetMenu(LoadMenu(MAKEINTRESOURCE(IDR_MAIN_MENU)));

    HWND hWndControl = CreateWindow(
                "STATIC",
                "Variables",
                WS_CHILD | WS_VISIBLE | WS_GROUP,
                0, 0, 0, 0,
                handle(),
                reinterpret_cast<HMENU>(IDC_VARIABLES_STATIC),
                instance(), NULL);
    if(!hWndControl)
        return false;
    SetWindowFont(hWndControl, GetStockObject(DEFAULT_GUI_FONT), TRUE);

    m_hWndVariables = CreateWindowEx(
                WS_EX_CLIENTEDGE,
                "EDIT",
                "",
                WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL,
                0, 0, 0, 0,
                handle(),
                reinterpret_cast<HMENU>(IDC_VARIABLES),
                instance(), NULL);
    if(!m_hWndVariables)
        return false;
    SetWindowFont(m_hWndVariables, GetStockObject(DEFAULT_GUI_FONT), TRUE);

    hWndControl = CreateWindow(
                "STATIC",
                "Function",
                WS_CHILD | WS_VISIBLE | WS_GROUP,
                0, 0, 0, 0,
                handle(),
                reinterpret_cast<HMENU>(IDC_MODE_STATIC),
                instance(), NULL);
    if(!hWndControl)
        return false;
    SetWindowFont(hWndControl, GetStockObject(DEFAULT_GUI_FONT), TRUE);

    m_hWndFunction = CreateWindowEx(
                WS_EX_CLIENTEDGE,
                "EDIT",
                "",
                WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL,
                0, 0, 0, 0,
                handle(),
                reinterpret_cast<HMENU>(IDC_FUNCTION),
                instance(), NULL);
    if(!m_hWndFunction)
        return false;
    SetWindowFont(m_hWndFunction, GetStockObject(DEFAULT_GUI_FONT), TRUE);

    m_hWndMinterms = CreateWindowEx(
                WS_EX_CLIENTEDGE,
                "EDIT",
                "",
                WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL,
                0, 0, 0, 0,
                handle(),
                reinterpret_cast<HMENU>(IDC_MINTERMS),
                instance(), NULL);
    if(!m_hWndMinterms)
        return false;
    SetWindowFont(m_hWndMinterms, GetStockObject(DEFAULT_GUI_FONT), TRUE);

    m_hWndMaxterms = CreateWindowEx(
                WS_EX_CLIENTEDGE,
                "EDIT",
                "",
                WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL,
                0, 0, 0, 0,
                handle(),
                reinterpret_cast<HMENU>(IDC_MAXTERMS),
                instance(), NULL);
    if(!m_hWndMaxterms)
        return false;
    SetWindowFont(m_hWndMaxterms, GetStockObject(DEFAULT_GUI_FONT), TRUE);

    m_hWndGenerate = CreateWindow(
                "BUTTON",
                "Generate",
                WS_CHILD | WS_VISIBLE | WS_GROUP | WS_TABSTOP | BS_DEFPUSHBUTTON,
                0, 0, 0, 0,
                handle(),
                reinterpret_cast<HMENU>(ID_GENERATE_GENERATE),
                instance(), NULL);
    if(!m_hWndGenerate)
        return false;
    SetWindowFont(m_hWndGenerate, GetStockObject(DEFAULT_GUI_FONT), TRUE);

   m_hWndTable = CreateWindowEx(
                WS_EX_CLIENTEDGE,
                WC_LISTVIEW,
                NULL,
                WS_CHILD | WS_VISIBLE | WS_GROUP | WS_TABSTOP | LVS_REPORT | LVS_SINGLESEL,
                0, 0, 0, 0,
                handle(),
                reinterpret_cast<HMENU>(IDC_TABLE),
                instance(), NULL);
    if(!m_hWndTable)
        return false;
    ListView_SetExtendedListViewStyle(m_hWndTable, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES/*| LVS_EX_HEADERDRAGDROP*/);
    SetWindowFont(m_hWndTable, GetStockObject(DEFAULT_GUI_FONT), TRUE);

    LVCOLUMN lvc;
    lvc.mask = LVCF_WIDTH;
    lvc.cx = 0;
    ListView_InsertColumn(m_hWndTable, 0, &lvc);

    ::SetFocus(m_hWndVariables);

    return true;
}


void MainWindow::OnSize(UINT state, int cx, int cy)
{
    HWND hWndControl = GetDlgItem(handle(), IDC_VARIABLES_STATIC);
    ::MoveWindow(hWndControl, 10, 10, 96, 20, TRUE);

    ::MoveWindow(m_hWndVariables, 10, 30, 96, 20, TRUE);

    hWndControl = GetDlgItem(handle(), IDC_MODE_STATIC);
    ::MoveWindow(hWndControl, 10, 55, 96, 20, TRUE);

    ::MoveWindow(m_hWndFunction, 10, 75, 96, 20, TRUE);
    ::MoveWindow(m_hWndMinterms, 10, 75, 96, 20, TRUE);
    ::MoveWindow(m_hWndMaxterms, 10, 75, 96, 20, TRUE);

    ::MoveWindow(m_hWndGenerate, 10, 105, 64, 32, TRUE);

    ::MoveWindow(m_hWndTable, 128, 10, cx - 128 - 10, cy - 10 - 10, TRUE);
    ListView_SetColumnWidth(m_hWndTable, Header_GetItemCount(ListView_GetHeader(m_hWndTable)) - 1, LVSCW_AUTOSIZE_USEHEADER);
}


bool MainWindow::OnCommand(int id, HWND hWndCtl, UINT codeNotify)
{
    int i=0;

    switch(id)
    {
    case ID_FILE_SAV:
        save();
        break;
    case ID_FILE_EXIT:
        SendMessage(WM_CLOSE, 0, 0);
        break;
/* FIXME: all of these are mode based */
    case ID_GENERATE_NOT:
        Edit_SetText(m_hWndVariables, "x");
        Edit_SetText(m_hWndFunction, "x'");
        break;
    case ID_GENERATE_AND:
        Edit_SetText(m_hWndVariables, "x,y");
        Edit_SetText(m_hWndFunction, "x & y");
        break;
    case ID_GENERATE_OR:
        Edit_SetText(m_hWndVariables, "x,y");
        Edit_SetText(m_hWndFunction, "x | y");
        break;
    case ID_GENERATE_XOR1:
        Edit_SetText(m_hWndVariables, "x,y");
        Edit_SetText(m_hWndFunction, "x ^ y");
        break;
    case ID_GENERATE_XOR2:
        Edit_SetText(m_hWndVariables, "x,y");
        Edit_SetText(m_hWndFunction, "(x' & y) | (x & y')");
        break;
    case ID_GENERATE_NAND:
        Edit_SetText(m_hWndVariables, "x,y");
        Edit_SetText(m_hWndFunction, "(x & y)'");
        break;
    case ID_GENERATE_NOR:
        Edit_SetText(m_hWndVariables, "x,y");
        Edit_SetText(m_hWndFunction, "(x | y)'");
        break;
    case ID_GENERATE_XNOR1:
        Edit_SetText(m_hWndVariables, "x,y");
        Edit_SetText(m_hWndFunction, "(x ^ y)'");
        break;
    case ID_GENERATE_XNOR2:
        Edit_SetText(m_hWndVariables, "x,y");
        Edit_SetText(m_hWndFunction, "(x | y') & (x' | y)");
        break;
    case ID_GENERATE_XNOR3:
        Edit_SetText(m_hWndVariables, "x,y");
        Edit_SetText(m_hWndFunction, "(x' & y') | (x & y)");
        break;
    case ID_GENERATE_GENERATE:
        generate();

        // add a sizeable column... this isn't exactly what I want, but it'll do
        i = Header_GetItemCount(ListView_GetHeader(m_hWndTable));
        LVCOLUMN lvc;
        lvc.mask = LVCF_WIDTH;
        lvc.cx = 0;
        ListView_InsertColumn(m_hWndTable, i, &lvc);
        ListView_SetColumnWidth(m_hWndTable, i, LVSCW_AUTOSIZE_USEHEADER);
        break;
    case ID_GENERATE_CLEAR:
        clear_controls();
        break;
    case ID_MODE_FUNCTION:
        set_mode(Function);
        break;
    case ID_MODE_MINTERM:
        set_mode(Minterm);
        break;
    case ID_MODE_MAXTERM:
        set_mode(Maxterm);
        break;
    case ID_HELP_ABOUT:
        DialogBox(instance(), MAKEINTRESOURCE(IDD_ABOUT), handle(), AboutDlgProc);
        break;
    default:
        return false;
    }
    return true;
}


void MainWindow::OnClose()
{
    DestroyWindow();
}


void MainWindow::OnDestroy()
{
    DestroyMenu(GetMenu());
    PostQuitMessage(0);
}
