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

#include "threads.h"


#ifdef __FUCKIN_WINDOWS
PROCESS_INFORMATION wpidmodbusTCP;
STARTUPINFO si;

//FUCKIN WINDOWS
/*! function to check if a process is running*/

int piproc_find(const char* name) 
{
  int ret = GetExitCodeProcess(&wpidmodbusTCP , NULL);
  if (ret == 256)
    return 0;
  else
    return -1;
}

/*! it executes the modbus TCP IP daemon with no parameters, could be improved the path to daemon personalized, a .ini file choosable, etc.*/
int launchDaemonMBTCP()//path configurable? .ini to use, etc.
{

  ZeroMemory( &si, sizeof(si) );
  si.cb = sizeof(si);
  ZeroMemory( &wpidmodbusTCP, sizeof(wpidmodbusTCP) );

  if( !CreateProcess(NULL,   // No module name (use command line)
        L"..\\comm\\modbus_client\\modbus_client.exe ..\\comm\\modbus_client\\config.ini",        // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &si,            // Pointer to STARTUPINFO structure
        &wpidmodbusTCP)           // Pointer to PROCESS_INFORMATION structure
    ) 
    {
      std::cout<< "ERR:CreateProcess failed "<< GetLastError() << std::endl;
      return -1;
    }
    return 0;   

}

#else

/*! function to check if a process is running
 only  under Gnu/Linux*/
int piproc_find(const char* name) 
{
    DIR* dir;
    struct dirent* ent;
    char* endptr;
    char buf[512];

    if (!(dir = opendir("/proc"))) {
        perror("can't open /proc");
        return -1;
    }

    while((ent = readdir(dir)) != NULL) {
        /* if endptr is not a null character, the directory is not
         * entirely numeric, so ignore it */
        long lpid = strtol(ent->d_name, &endptr, 10);
        if (*endptr != '\0') {
            continue;
        }

        /* try to open the cmdline file */
        snprintf(buf, sizeof(buf), "/proc/%ld/cmdline", lpid);
        FILE* fp = fopen(buf, "r");

        if (fp) {
            if (fgets(buf, sizeof(buf), fp) != NULL) {
                /* check the first token in the file, the program name */
                char* first = strtok(buf, " ");
                if (!strcmp(first, name)) {
                    fclose(fp);
                    closedir(dir);
                    return (pid_t)lpid;
                }
            }
            fclose(fp);
        }

    }

    closedir(dir);
    return -1;
}

/*! it executes the modbus TCP IP daemon with no parameters, could be improved the path to daemon personalized, a .ini file choosable, etc.*/
int launchDaemonMBTCP()//path configurable? .ini to use, etc.
{

  char line[128]; //should be IMPROVED
  FILE *file;
  FILE *fout; // out file

  rename("MBdaemon.log", "MBdaemon_old.log");
  fout = fopen("MBdaemon.log","w");
  
  //execute pipe
  file = popen("../comm/modbus_client/modbus_client ../comm/modbus_client/config.ini", "r");
  if (!file) 
    { 
      fprintf(stderr,"ERROR: bad command to execute modbus tcp ip daemon\n");
      perror(".daemon_log"); 
      return -1;        
  }
  //show output
  while (fgets(line, 128, file) != NULL) //buffer can be improved IMPROVE
    {
      fprintf(fout,"%s", line);

    }
  pclose(file);
  pclose(fout); 
  return 0;    

}

#endif
