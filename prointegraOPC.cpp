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
  //cout<<"DEBUG: checking and creating!" << endl;
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
  int failed = 0;
   for(int i=0; i < nSlaves; i++)
    {
      //std::cout << "DEBUG: slave "<< i << " status: " << commDaemonManager->checkDaemon(i)<< std::endl;
      failed = failed + commDaemonManager->checkDaemon(i);
    }
  return failed;   
}
/*!checking communication processes if running or not
TODO: it's in fact still not be used*/
int ProintegraOPC::stopComm()
{
  delete commDaemonManager;
  return 0;   
}
/*data capturing process
it should takes data from communications mailboxes and save it to it's slave structure*/
int ProintegraOPC::dataCapture()
{
  int failed = -1;
  for(int i=0; i < nSlaves; i++)
    failed= failed & hSlaves[i]->readData();
  return failed;   
}
/*data from DB to communication process (WRITTING)
it takes data from database table and write it to it's slave*/
int ProintegraOPC::dataToComm()
{
  std::cout << "DEBUG: (inside ProintegraOPC::dataToComm)" << std::endl;
  int failed = -1;
  field ***tagsToWrite = NULL;
  int *nTables = NULL;
  int **nFields = NULL;

  for(int i=0; i< nDBs ; i++)
    {
      if(*nTriggers[i] > 0)
	{
	  failed = hDatabase[i]->retDataToWrite(stTriggers[i],nTriggers[i],&tagsToWrite,&nTables,&nFields);
	  std::cout << "DEBUG: (inside ProintegraOPC::dataToComm) nº of tables"<<*nTables << std::endl;
	  for(int j=0;j<*nTables;j++)
	    {
	      for(int k = 0; k < *nFields[j]; k++)
		{
		  std::cout << "DEBUG: (inside ProintegraOPC::dataToComm) table nº:"<<j<<"  field nº:" << k << "  tag:" << tagsToWrite[j][k]->tag << std::endl;
		}
	    }
	  std::cout << "DEBUG: (inside ProintegraOPC::dataToComm) deleting data!" << std::endl;
	  for(int j=*nTables-1;j>=0;j--)
	    {
	      std::cout << "DEBUG: (inside ProintegraOPC::dataToComm) deleting temporal data in table:" << j << std::endl;
	      for(int k = *nFields[j]-1; k >= 0; k--)
		{
		  std::cout << "DEBUG: (inside ProintegraOPC::dataToComm) deleting tagsToWrite[" << j <<"][" << k <<"]"<< std::endl;
		  delete tagsToWrite[j][k];
		}
	      std::cout << "DEBUG: (inside ProintegraOPC::dataToComm) deleting dimension j:" << j << std::endl;
	      delete nFields[j];
	      delete tagsToWrite[j];
	    }
	  std::cout << "DEBUG: (inside ProintegraOPC::dataToComm) deleting pointers" << std::endl;
	  delete nFields;
	  delete tagsToWrite;
	  delete nTables;
	}
      else
	std::cout << "DEBUG: (inside ProintegraOPC::dataToComm) no triggers found for database" << std::endl;
    }
  return failed;
}


/*data from communication to database process (READING)
it takes data from slave structures and save it to our tables structures
TODO: it has to be cut in smaller functions!*/
int ProintegraOPC::dataToDB()
{
  int* link = NULL;
  //databases
  for(int i=0; i< nDBs ; i++)
    {
      //tables
      for(int j=0; j < hDatabase[i]->retNumTables();j++)
	{
	  //fields
	  for(int k=0; k < hDatabase[i]->retNumFields(j);k++)
	    {
	      //std::cout << "**DEBUG: field:"<< k << " , from table:" << j << " , database:" << i << std::endl;
	      //std::cout << "**DEBUG: setting it invalid!"<< std::endl;
	      hDatabase[i]->setFieldValid(j,k,0);
	      //std::cout << "**DEBUG: checking linking!"<< std::endl;
	      if(hDatabase[i]->fieldLinked(j,k))
		{
		  link = hDatabase[i]->retFieldLink(j,k);
		  //std::cout << "**DEBUG: linked! is valid?"<< hSlaves[link[0]]->retTagValid(link[1]) <<  std::endl;		  
		  hDatabase[i]->setFieldValid(j,k,hSlaves[link[0]]->retTagValid(link[1]));
		  hDatabase[i]->setFieldValue(j,k,hSlaves[link[0]]->retTagValue(link[1]));
		}
	      else
		{
		  //std::cout << "**DEBUG: not linked!"<< std::endl;
		  for(int slave = 0; slave < nSlaves;slave++)
		    {
		      for(int tag=0; tag < hSlaves[slave]->retNumTags(); tag++)
			{
			  if(!strcmp(hSlaves[slave]->retTagName(tag),hDatabase[i]->retFieldTag(j,k)))
			    {
			      //std::cout << "**DEBUG: linking: slave:" << slave << " tag:"<< tag <<" value:"<< hSlaves[slave]->retTagValue(tag) << " isValid?:" << hSlaves[slave]->retTagValid(tag) <<  std::endl;
			      hDatabase[i]->setFieldValid(j,k,hSlaves[slave]->retTagValid(tag));
			      hDatabase[i]->setFieldValue(j,k,hSlaves[slave]->retTagValue(tag));
			      hDatabase[i]->fieldLink(j,k,slave,tag);
			    }
			}

		    }

		}
	    }
	}
    }
  return 0;   
}
/*data to database process
it takes data from slave structures and save it to our tables structures
TODO: it has to be cut in smaller functions!*/
int ProintegraOPC::storeDB()
{
  int ret = -1;
  //databases
  for(int i=0; i< nDBs ; i++)
    {
      hDatabase[i]->storeData();
    }
  return ret;
}
/*start communications
*/
int ProintegraOPC::startCommunications()
{
  commDaemonManager = new CommDaemon(nSlaves);

  for(int i=0; i < nSlaves; i++)
    commDaemonManager->launchDaemon(i,confParser->retSlaveParams(i).commId,confParser->retSlaveParams(i).commType);
  return 0;   
}

/*capturing proccess
TODO: we don't manage daemons!*/
int ProintegraOPC::loop()
{
  //exit handling
  struct sigaction sigIntHandler;
  int failed = -1;
  field *** stTriggers = NULL;
  int **numFields = NULL;
  
  sigIntHandler.sa_handler = ProintegraOPC::exitHandler;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;
  sigaction(SIGINT, &sigIntHandler, NULL);
  
  while(1)
    {
      if(!lExit)
	{
	  failed = 0;
	  std::cout << "INFO: checking communications ..." << std::endl;     
	  checkComm();
	  std::cout << "INFO: taking data from communications ..." << std::endl;  
	  if(dataCapture())
	    {
	      failed = -1;
	      std::cout << "ERROR: NO DATA CAPTURED! ..." << std::endl;   
	    }
	  if(!failed)
	    {
	      getTriggers();
	      dataToComm();
		  //dataToDB();
		  //storeDB();
		  //cleaning
	      //  if(*numFields >0)
	      //    {
	      //      for(int i = *numFields-1; i >=0;i--)
	      //	delete stTriggers[i];
	      //      delete stTriggers;
	      //    }
		  //
	      //}
	      delTriggers();
	    }
	  //std::cout << "DEBUG: showing what we have stored ..." << std::endl;
	  //showDBData();	  
	}
      else
	{
	  std::cout << "INFO: EXITING! ..." << std::endl;
	  stopComm();
	  return 0;
	}
    }
  return 0;   
}

/*!function for getting a complete list of trigered triggers*/
int ProintegraOPC::getTriggers()
{
  //std::cout << "DEBUG:(inside ProintegraOPC::getTriggers)" << std::endl; 
  int failed = -1;

  //std::cout << "DEBUG:(inside ProintegraOPC::getTriggers) creating structures for:" << nDBs << "  databases" << std::endl; 
  stTriggers = new field**[nDBs];
  nTriggers = new int*[nDBs];
  
  failed = 0;
  for(int i=0; i < nDBs; i++)
    {
      //std::cout << "INFO: database:" << i+1 <<"  is Triggered? ... ";

      failed = failed +  hDatabase[i]->retTriggers(stTriggers[i],nTriggers[i]);
      std::cout << "DEBUG: (inside ProintegraOPC::getTriggers) database:" << i <<" definitive  triggers:" << *nTriggers[i] << std::endl;     
      //if(*nTriggers[i] > 0 && *nTriggers[i] != NULL)
      //std::cout << "yes! " << std::endl;
      //else
      //std::cout << "no! " << std::endl;
      //for(int j = 0; j < *nTriggers[i]; j++)
      //std::cout << "DEBUG: (inside ProintegraOPC::getTriggers) checking trigger n:" << j+1 <<"  name:" << stTriggers[i][j]->name << std::endl;
    }
  
 
  return failed;
}

/*!function for deleting the complete list of trigered triggers*/
int ProintegraOPC::delTriggers()
{
  //std::cout << "DEBUG:(inside ProintegraOPC::delTriggers)" << std::endl; 
  int failed = -1;

  for(int i=nDBs-1; i>=0;i--)
    {
      //std::cout << "DEBUG:(inside ProintegraOPC::delTriggers) database:" << i << std::endl; 
      if(*nTriggers[i]>0)
	{
	  //std::cout << "DEBUG:(inside ProintegraOPC::delTriggers) there are triggers here! *nTriggers[i]:" << *nTriggers[i]<< std::endl; 
	  for(int j=*nTriggers[i]-1;j>=0;j--)
	    {
	      //std::cout << "DEBUG:(inside ProintegraOPC::delTriggers) inside loop, deleting trigger:" << j<< std::endl;
	      delete stTriggers[i][j];
	    }
	  //std::cout << "DEBUG:(inside ProintegraOPC::delTriggers) deletting dimension i:" << i << std::endl;
	  if(i>0)
	    {
	      delete nTriggers[i];
	      delete stTriggers[i];
	    }
	}
    }
  //std::cout << "DEBUG:(inside ProintegraOPC::delTriggers) delete pointers" << std::endl; 
  delete nTriggers;
  delete stTriggers;
 
  return failed;
}

//DEBUG FUNCTIONS
/*show data in memory structures, data to be written in databases*/
int ProintegraOPC::showDBData()
{
  int slave = 0;
  int field = 0;
  //databases
  for(int i=0; i< nDBs ; i++)
    {
      std::cout << "**************************************************" << std::endl;
      std::cout << "*DEBUG: showing data in memory, to save in databases" << std::endl;
      std::cout << std::endl;
      std::cout << "*DATA IN DATABASE: " << i+1 << std::endl;
      //tables
      for(int j=0; j < hDatabase[i]->retNumTables();j++)
	{
	  std::cout << std::endl;
	  std::cout << "*--->TABLE: " << j+1 << std::endl;
	  //fields
	  for(int k=0; k < hDatabase[i]->retNumFields(j);k++)
	    {
	      std::cout << "*------->FIELD: " << k+1 << std::endl;
	      std::cout << "*--------->NAME: " << hDatabase[i]->retFieldTag(j,k)  << std::endl;
	      std::cout << "*--------->VALUE: " << hDatabase[i]->retFieldValue(j,k)  << std::endl;
	      std::cout << "*--------->VALID: " << hDatabase[i]->retFieldValid(j,k)  << std::endl;
	    }
	}
    }

}


