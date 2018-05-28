/*
 *  Prointegra OPC
 *
 *  Copyright 2016-2018 by it's authors. 
 *
 *  Some rights reserved. See COPYING, AUTHORS.
 *  This file may be used under the terms of the GNU General Public
 *  License version 3.0 (or any later version of GPL) as published by the Free Software Foundation
 *  and appearing in the file COPYING included in the packaging of
 *  this file.
 *
 *  This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 *  WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

/*!
@file prointegraOPC.cpp
*/

#include "prointegraOPC.h"


/*! Constructor*/
ProintegraOPC::ProintegraOPC()
{
  int i = 0;
  std::vector <mbSlaves> slavesParams;
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
  hSlaves = new SlaveInterface*[nSlaves];
  comms = new CommDaemon*[nSlaves];
  for(i=0;i<nSlaves;i++)
    {
      comms[i] = new CommDaemon(confParser->retSlaveParams(i));
      hSlaves[i] = new SlaveInterface();
      hSlaves[i]->setup(comms[i]->retParams());
      slavesParams.push_back(comms[i]->retParams());
      std::cout << "INFO: (inside ProintegraOPC::ProintegraOPC) new commDaemon instance" << std::endl;
      //comms[i]->launchDaemon();
    }
  /*******/
  //link database tags with slaves
  linkTags();
  //FOR DEBUGGING PURPOSES
  //showDBDataLinkage();

  return;   
}
/*! destructor*/
/*!TODO: it's in fact not be called, when we close with crtl+c the application doesn't take the sigterm signal*/
ProintegraOPC::~ProintegraOPC()
{
  delete confParser;
  for(int i=nDBs-1;i== 0 ; i--)
    delete hDatabase[i];
  delete hDatabase;
  for(int i=nSlaves-1;i== 0 ; i--)
    delete hSlaves[i];
  delete hSlaves;
  //delete commDaemonManager;
  return;
}
/*! function for linking between slaves and databases tags*/ 
int ProintegraOPC::linkTags()
{
  //std::cout << "DEBUG:(inside ProointegraOPC::linkTags)"<< std::endl;
  //databases
  for(int i=0; i< nDBs ; i++)
    {
      //tables
      for(int j=0; j < hDatabase[i]->retNumTables();j++)
	{
	  //fields
	  for(int k=0; k < hDatabase[i]->retNumFields(j);k++)
	    {
	      for(int slave = 0; slave < nSlaves;slave++)
		{
		  for(int tag=0; tag < hSlaves[slave]->retNumTags(); tag++)
		    {
		      if(!strcmp(hSlaves[slave]->retTagName(tag),hDatabase[i]->retFieldTag(j,k)))
			{
			  hDatabase[i]->fieldLink(j,k,slave,tag);
			}
		    }
		}
	    }
	}
    }
  return 0;
}

/*!checking database devices and creating it's tables if don't exists*/
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
  //for(int i=0; i < nSlaves; i++)
     //{
      //std::cout << "DEBUG: slave "<< i << " status: " << commDaemonManager->checkDaemon(i)<< std::endl;
      //failed = failed + commDaemonManager->checkDaemon(i);
      //}
  return failed;   
}
/*!checking communication processes if running or not
TODO: it's in fact still not be used*/
int ProintegraOPC::stopComm()
{
  //delete comms;
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
  //std::cout << "DEBUG: (inside ProintegraOPC::dataToComm)" << std::endl;
  int failed = -1;
  int tableFail = 0;
  std::vector<field> tags;
  std::vector <std::vector <int>> tagLink;
  std::vector<int> tablesList;

  //all databases
  for(int i=0; i< nDBs ; i++)
    {
      //taking tables wo be written
      failed = hDatabase[i]->retWTabsList(tablesList);
      for(int j=0; j < tablesList.size() ; j++)
	{
	  tableFail = hDatabase[i]->retrieveData(tablesList.at(j));
	  tableFail = tableFail + hDatabase[i]->retDataFrTable(tags,tablesList.at(j));
	  for (int k = 0; k < tags.size(); k++)
	    {
	      for (int l = 0; l < tags[k].fromTags.size(); l++)
		{
		  for (int m = 0; m < tags[k].fromTags[l].size(); m++)
		    {
		      if(k >50 || l > 50 || m > 50)
			std::cout << "tags:" << k << " .fromTags:"<< l << " .at:" << m << std::endl;
		      tableFail = tableFail + hSlaves[l]->writeTag(tags[k].fromTags[l].at(m),l+1,tags[k].iValue);
		    }
		}
	    }
	  std::cout << "DEBUG: (inside ProintegraOPC::dataToComm) tableFail:"<< tableFail << std::endl;
	  //resetting trigger
	  if(!tableFail)
	    {
	      hDatabase[i]->unlockTable(tablesList.at(j));
	      hDatabase[i]->wTriggerDoneAt(tablesList.at(j));
	    }
	  failed = failed +tableFail;
	  tableFail = 0;
	    
	}
    }
  return failed;
}


/*data from communication to database process (READING)
it takes data from slave structures and save it to our tables structures
TODO: it has to be cut in smaller functions!*/
int ProintegraOPC::dataToDB()
{
  //std::cout << "DEBUG: (inside ProintegraOPC::dataToDB)" << std::endl;
  int failed = -1;
  std::vector<int> tablesList;
  std::vector<field> tags;
  int tableFail = 0;
  
  for(int i=0; i< nDBs ; i++)
    {
      //taking tables wo be written
      failed = hDatabase[i]->retRTabsList(tablesList);
      
      for(int j=0; j < tablesList.size() ; j++)
	{
	  if(!hDatabase[i]->isTableLocked(tablesList.at(j)))
	    {
	      failed = failed & hDatabase[i]->retDataFrTable(tags, tablesList.at(j));
	      for (int k = 0; k < tags.size(); k++)
		{
		  for (int l = 0; l < tags[k].fromTags.size(); l++)
		    {
		      for (int m = 0; m < tags[k].fromTags[l].size(); m++)
			{		 
			  tableFail = tableFail + hSlaves[l]->readTag(tags[k].fromTags[l].at(m));
			  tags[k].iValue = hSlaves[l]->retTagValue(tags[k].fromTags[l].at(m));
			  tags[k].isValid = hSlaves[l]->retTagValid(tags[k].fromTags[l].at(m));
			}
		    }
		}
	      hDatabase[i]->rTriggerDoneAt(j);
	      hDatabase[i]->storeData(tablesList.at(j),tags);
	      failed = failed +tableFail;
	    }
	}
    }
  
  return failed;
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
  //commDaemon->launchDaemons(); TODO launch one daemon for every slave, not one for all
  return 0;   
}

/*capturing proccess
TODO: we don't manage daemons!*/
int ProintegraOPC::loop()
{
  //exit handling
  struct sigaction sigIntHandler;
  int failed = -1;
  
  sigIntHandler.sa_handler = ProintegraOPC::exitHandler;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;
  sigaction(SIGINT, &sigIntHandler, NULL);
  signal(SIGSEGV, sigsevHandler);

  //startCommunications();
  while(1)
    {
      if(!lExit)
	{
	  failed = 0;
	  std::cout << "*----------------------------------------------*" << std::endl;
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
	      std::cout << "INFO: are there some tables locked ..."; 
	      lockTables();
	      std::cout << "INFO: are there some triggers triggered? ..." << std::endl;	      
	      getTriggers();
	      //showTriggers();
	      std::cout << "INFO: dataBase to Slaves ..." << std::endl;		      
	      dataToComm();
	      std::cout << "INFO: slaves to DataBase ..." << std::endl;		      
	      dataToDB();
	      std::cout << "INFO: reset Triggers ..." << std::endl;	      
	      delTriggers();
	      //Sleep(500);
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

/*!function for locking any modified table in database, waiting later to be written to Comm*/
int ProintegraOPC::lockTables()
{
  //std::cout << "DEBUG:(inside ProintegraOPC::lockTables)" << std::endl; 
  int failed = -1;
 
  failed = 0;
  for(int i=0; i < nDBs; i++)
    {
      failed = failed +  hDatabase[i]->lockTables(); 
    }
  
  return failed;
}
/*!function for getting a complete list of trigered triggers*/
int ProintegraOPC::getTriggers()
{
  //std::cout << "DEBUG:(inside ProintegraOPC::getTriggers)" << std::endl; 
  int failed = -1;
 
  failed = 0;
  for(int i=0; i < nDBs; i++)
    {
      failed = failed +  hDatabase[i]->takeTriggers(); 
    }
  
  return failed;
}

/*!function for deleting the complete list of trigered triggers*/
int ProintegraOPC::delTriggers()
{
  //std::cout << "DEBUG:(inside ProintegraOPC::delTriggers)" << std::endl; 
  int failed = -1;
  
  for(int i=0; i < nDBs; i++)
    {
      failed = failed +  hDatabase[i]->resetTriggers(); 
    }
  //std::cout << "DEBUG:(inside ProintegraOPC::delTriggers) returning" << std::endl; 
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
/*show data links with communication tags*/
int ProintegraOPC::showDBDataLinkage()
{
  std::cout <<"DEBUG:(inside ProintegraOPC::showDBDataLinkage)" << std::endl;
  std::vector < std::vector <int>> temp;
   //databases
  for(int i=0; i< nDBs ; i++)
    {
      std::cout << "**************************************************" << std::endl;
      std::cout << "*DEBUG: showing data links" << std::endl;
      std::cout << std::endl;
      std::cout << "*DATABASE: " << i+1 << std::endl;
      //tables
      for(int j=0; j < hDatabase[i]->retNumTables();j++)
	{
	  std::cout << std::endl;
	  std::cout << "*--->TABLE: " << j+1 << std::endl;
	  //fields
	  for(int k=0; k < hDatabase[i]->retNumFields(j);k++)
	    {
	      std::cout << "*----->FIELD: " << k+1 << std::endl;
	      temp = hDatabase[i]->retFieldLink(j,k);
	      for(int l=0 ; l < temp.size() ; l++)
		for(int m=0 ; m < temp[l].size() ; m++)
		  std::cout << "*------->LINKED WITH SLAVE:" << l <<"  TAG:" << temp[l].at(m) << std::endl;
	    }
	}
    }
}

/*show triggers ON*/
int ProintegraOPC::showTriggers()
{
  std::cout <<"DEBUG:(inside ProintegraOPC::showTriggers)" << std::endl;
   //databases
  for(int i=0; i< nDBs ; i++)
    {
      std::cout << "**************************************************" << std::endl;
      std::cout << "*DEBUG: showing triggers in.." << std::endl;
      std::cout << std::endl;
      std::cout << "*DATABASE: " << i+1 << std::endl;
      //tables
      hDatabase[i]->showTriggers();
    }
}
void ProintegraOPC::sigsevHandler(int s)
{
  void *array[10];
  size_t size;

  // get void*'s for all entries on the stack
  size = backtrace(array, 10);

  // print out all the frames to stderr
  fprintf(stderr, "Error: signal %d:\n", s);
  backtrace_symbols_fd(array, size, STDERR_FILENO);
  exit(1);
}
