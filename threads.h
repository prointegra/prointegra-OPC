/*
 *  Prointegra OPC
 *
 *  Copyright 2016 by it's authors. 
 *
 *  Some rights reserved. See COPYING, AUTHORS.
 *  This file may be used under the terms of the GNU General Public
 *  License version 3.0 as published by the Free Software Foundation
 *  and appearing in the file COPYING included in the packaging of
 *  this file.
 *
 *  This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 *  WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */
/**
@file threads.h
*/
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>

//THREADS

#ifndef THREADS_
#define THREADS_

#ifdef __FUCKIN_WINDOWS

#include <windows.h>
extern PROCESS_INFORMATION wpidmodbusTCP;
extern STARTUPINFO si;

#else

#include <unistd.h>

#endif


/*! function to check if a process is running*/
int piproc_find(const char* name);

/*! it executes the modbus TCP IP daemon with no parameters, could be improved the path to daemon personalized, a .ini file choosable, etc.*/
int launchDaemonMBTCP();



#endif
