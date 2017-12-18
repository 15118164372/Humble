/* Copyright (c) 2009, 2017, Oracle and/or its affiliates. All rights reserved.
 
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; version 2 of the License.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA */

#ifndef MY_CONFIG_H
#define MY_CONFIG_H

#if defined _WIN32
	#ifdef _WIN64
        #include "my_config_winx64.h"
    #else
        #include "my_config_win32.h"
    #endif
#else
    #if __x86_64__ || __ppc64__ || __x86_64 || __amd64__  || __amd64
        #include "my_config_linuxx64.h"
    #else
        #include "my_config_linuxx86.h"
    #endif
#endif

#endif
