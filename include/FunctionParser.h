/**
\file FunctionParser.h
\brief FunctionParser class declaration.
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

#if !defined FUNCTIONPARSER_H
#define FUNCTIONPARSER_h


#pragma warning(disable : 4290)


#include <stdexcept>
#include <string>
#include <list>
#include <map>


/**
\brief Parses a function based on the funciton grammar.
\todo add something like <term> ::= <term> (<expression>)* <variable>? for implicit anding.
*/
class FunctionParser
{
public:
    /// Generic parser exception.
    class FunctionParserException : public std::exception
    {
    public:
        explicit FunctionParserException(const std::string& what) : _what(what) {}
        virtual ~FunctionParserException() throw() {}
        virtual const char* what() const throw() { return _what.c_str(); }
    private:
        std::string _what;
    };

public:
    FunctionParser();
    FunctionParser(const std::list<char>& variables, const std::string& function) throw(FunctionParserException);

public:
    /// Evaluates the function for the given row.
    unsigned int eval(unsigned int row) throw(FunctionParserException);

public:
    unsigned int size() const
    {
        return m_var_count;
    }

private:
    void advance();
    char current() const;
    void skip_whitespace();
    void match_cparen() throw(FunctionParserException);
    void match_oparen() throw(FunctionParserException);
    char check_variable();
    bool check_oparen();
    bool primary_operator_pending();
    unsigned int primary(unsigned int row) throw(FunctionParserException);
    bool term_operator_pending();
    unsigned int term(unsigned int row);
    bool expression_operator_pending();
    unsigned int expression(unsigned int row);

private:
    unsigned int m_var_count;
    unsigned int m_index;

    std::string m_function;
    std::map<char, int> m_variables;
};


#endif