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
@file capture.c
*/

#include "prointegraOPC.h"


/*! Constructor*/
ProintegraOPC::ProintegraOPC()
{
  int i = 0;
  //load xml config file
  confParser = new ConfigParser("config/database.xml","config/slaves.xml");
  //retrieve database info from config file
  confParser->retrieveDBParams();
  //creating database interfaces
  nDBs = confParser->retnDBs();
  hDatabase = new DBInterface*[nDBs];
  for(i=0;i<nDBs;i++)
    {
      hDatabase[i] = new DBInterface();
      hDatabase[i]->setup(confParser->retDBParams(i),confParser->retDBTables(i));
    }
  //retrieve slaves info from config file
  confParser->retrieveCommParams();
  nSlaves = confParser->retnSlaves();
  hSlaves = new CommInterface*[nSlaves];
  for(i=0;i<nSlaves;i++)
    {
      hSlaves[i] = new CommInterface();
      hSlaves[i]->setup(confParser->retSlaveParams(i));
    }  

  return;   
}
/*! destructor*/
/*!TODO: it's in fact not be called, when we close with crtl+c the application doesn't take the sigterm signal*/
ProintegraOPC::~ProintegraOPC()
{
  delete confParser;
  for(int i=nDBs-1;i== 0 ; i--)
    delete hDatabase[nDBs];
  delete hDatabase;
  return;
}

/*checking database devices and creating it's tables if don't exists*/
int ProintegraOPC::checkDB()
{
  cout<<"DEBUG: checking and creating!" << endl;
  //check database
  //  for(int i = 0;i < nDBs ;i++)
  //  hDatabase[i]->checkAndCreate();
  //check comunications
  return 0;   
}

/*!checking communication processes if running or not
TODO: it's in fact still not be used*/
int ProintegraOPC::checkComm()
{
  //check comunications

      if(piproc_find("../comm/modbus_client/modbus_client") <= 0)
	    {
	      //comms not working
	      std::cout << "ERR:modbus not running!" << std::endl;
	      return(-1);
	    }

  return 0;   
}
/*data capturing! process
WORKAROUND: It works only with 5 Casas*/
int ProintegraOPC::dataCapture()
{
  Sleep(2000);
  std::cout << "ERROR: capture not implemented yet" << std::endl;
  return 0;   
}

/*start communications
*/
int ProintegraOPC::startCommunications()
{

  return 0;   
}

/*capturing proccess
TODO: we don't manage daemons!*/
int ProintegraOPC::startCapture()
{

  //launchDaemonMBTCP();

  std::cout << "capturing!" << std::endl;
  while(1)
    {
      dataCapture();

    }
  return 0;   
}


