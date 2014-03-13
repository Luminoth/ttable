/**
\file error.cpp
\brief Error function definitions.
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


#include <cstring>
#include <string>

#if defined WIN32
    #if !defined WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>
#endif

#include <errno.h>

#include "../include/error.h"


/*
 *  external globals
 *
 */


extern int errno;


/*
 *  functions
 *
 */


#if defined WIN32
std::string last_error()
{
    char buffer[512];
    memset(buffer, 0, 512);

    FormatMessage(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, GetLastError(),
        0, buffer, 512, NULL);
    return std::string(buffer);
}
#endif


std::string last_std_error()
{
    return strerror(errno);
}