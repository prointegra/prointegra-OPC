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
/*!
@File main.cpp
'''program starts here'''
*/

#include "pvapp.h"
#include "prointegraOPC.h"


int pvMain(PARAM *p)
{
  return 0;
}

/* main function,  it could be integrate in other programs, or pvbrwoser*/
int main(int ac, char **av)
{
  ProintegraOPC* server;
#ifdef __FUCKIN_WINDOWS
  QCoreApplication a(ac, av);
  a.addLibraryPath ( "C:/Qt/Qt5.6.0/5.6/mingw49_32/plugins/sqldrivers");
#endif

  int ret;
  std::cout << "INFO: Prointegra OPC version: " << sVERSION << std::endl;
  server = new ProintegraOPC();
  server->startCommunications();
  std::cout << "INFO: start capturing..." << std::endl;
  ret = server->loop();

  return ret;
}

