/*
 *  Prointegra OPC
 *
 *  Copyright 2016-2018 by it's authors. 
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
@File readThread.h
*/

#ifndef _READ_THREAD_
#define _READ_THREAD_

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <time.h>
#include <QThread>

#include "qtdatabase.h"
#include "piDatabase.h"
#include "config.h"
#include "piTriggersTable.h"
#include "piDataTable.h"
#include "piSlave.h"

/*! Database interface class, derived from pvbrowser addons examples */
class readThread : public QThread
{
 public:
  readThread(){};
  ~readThread(){};

  int launch(DBInterface*& , int, SlaveInterface**);
 private:

};




#endif
