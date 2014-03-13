/**
\file FunctionParser.cpp
\brief FunctionParser class definition.
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

\todo Work in short circuiting.
\todo This could be better/faster by using a tree and such.
*/


#include <cctype>
#include <list>

#include "../include/FunctionParser.h"
#include "../include/main.h"


/*
 *  FunctionParser methods
 *
 */


FunctionParser::FunctionParser()
    : m_var_count(0), m_index(0)
{
}


FunctionParser::FunctionParser(const std::list<char>& variables, const std::string& function) throw(FunctionParserException)
    : m_var_count(0), m_index(0), m_function(function)
{
    for(std::list<char>::const_iterator it = variables.begin(); it != variables.end(); ++it)
        m_variables[*it] = m_var_count++;
}


unsigned int FunctionParser::eval(unsigned int row) throw(FunctionParserException)
{
    m_index = 0;
    int t = expression(row);

    if(m_index != m_function.size())
        throw FunctionParserException("Parsing finished early at: " + m_function.substr(m_index));
    return t;
}


/// Advances the function index.
void FunctionParser::advance()
{
    m_index++;
}


/// Returns the current character (-1 if we're at the end).
char FunctionParser::current() const
{
    if(m_index >= m_function.length())
        return -1;
    return m_function[m_index];
}


/// Skips whitespace in the function.
void FunctionParser::skip_whitespace()
{
    while(isspace(current()))
        advance();
}


/// Matches a close paren.
void FunctionParser::match_cparen() throw(FunctionParserException)
{
    skip_whitespace();

    if(current() != ')') {
        char err[64];
        _snprintf(err, 64, "Expected close parenthesis, got %c", current());
        throw FunctionParserException(err);
    }
    advance();
}


/// Matches an open paren.
void  FunctionParser::match_oparen() throw(FunctionParserException)
{
    skip_whitespace();

    if(current() != '(') {
        char err[64];
        _snprintf(err, 64, "Expected open parenthesis, got %c", current());
        throw FunctionParserException(err);
    }
    advance();
}


/// Returns -1 if a variable is not next, or the variable character if it is (doesn't advance past the variable).
char FunctionParser::check_variable()
{
    skip_whitespace();

    if(isalpha(current())) {
        char ch = current();
        return (m_variables.find(ch) != m_variables.end()) ? ch : -1;
    }
    return -1;
}


/// Returns true if an open paren is next.
bool FunctionParser::check_oparen()
{
    skip_whitespace();
    return (current() == '(');
}


/// Returns true if a primary operator is next.
bool FunctionParser::primary_operator_pending()
{
    skip_whitespace();
    return (current() == '\'');
}


/// Parses a primary.
unsigned int FunctionParser::primary(unsigned int row) throw(FunctionParserException)
{
/* FIXME: should this be a while loop somehow? */

    skip_whitespace();

    char ch = check_variable();
    if(ch >= 0) {
        advance();
        int r = getbit(m_variables[ch], row);

        while(primary_operator_pending()) {
            advance();
            r = getbit(0, ~r);
        }
        return r;
    } else if(check_oparen()) {
        match_oparen();
        int r = expression(row);
        match_cparen();

        while(primary_operator_pending()) {
            advance();
            r = getbit(0, ~r);
        }
        return r;
    }

    char err[64];
    _snprintf(err, 64, "Expected variable or open parenthesis, got %c", current());
    throw FunctionParserException(err);
}


/// Returns true if a term operator is next.
bool FunctionParser::term_operator_pending()
{
    skip_whitespace();
    return (current() == '&');
}


/// Parses a term.
unsigned int FunctionParser::term(unsigned int row)
{
    skip_whitespace();

    int root = primary(row);

    // the while makes things left-associative instead of right
    while(term_operator_pending()) {
        advance();
        int t = term(row);
        root &= t;
    }
    return root;
}


/// Returns true if an expression operator is next.
bool FunctionParser::expression_operator_pending()
{
    skip_whitespace();
    return (current() == '|' || current() == '^');
}


/// Parses an expression.
unsigned int FunctionParser::expression(unsigned int row)
{
    skip_whitespace();

    int root = term(row);

    // the while makes things left-associative instead of right
    while(expression_operator_pending()) {
        if(current() == '|') {
            advance();
            int t = expression(row);
            root |= t;
        } else if(current() == '^') {
            advance();
            int t = expression(row);
            root ^= t;
        }
    }
    return root;
}