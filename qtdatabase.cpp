/***************************************************************************
                          qtdatabase.cpp  -  description
                             -------------------
    begin                : Fri May 28 2010
    http://pvbrowser.org
	improved 			: January 2017
 ***************************************************************************/

#include "qtdatabase.h"

qtDatabase::qtDatabase()
{
  //db = new QSqlDatabase;
  //std::cout << db->drivers().isEmpty() <<std::endl;
  //delete db;

  db = NULL;
  result = new QSqlQuery();
  error  = new QSqlError();
}

qtDatabase::~qtDatabase()
{
  if(db != NULL)
  {
    db->close();
  }
  delete db;
  delete result;
  delete error;
}

int qtDatabase::open(const char *dbtype, const char *hostname, const char *dbname, const char *user, const char *pass)
{ 
  if(db != NULL) return -1;
  db = new QSqlDatabase;

  *db = QSqlDatabase::addDatabase(dbtype);
  db->setHostName(hostname);
  db->setDatabaseName(dbname);
  db->setUserName(user);
  db->setPassword(pass);
  if(db->open())
  {
    return 0;
  }
  else
  {
    delete db;
    db = NULL;
    return -1;
  }
}

int qtDatabase::open(const char *dbtype, const char *hostname, const char *dbname, const char *user, const char *pass, const char * connectionName)
{ 
  if(db != NULL) return -1;
  db = new QSqlDatabase;

  *db = QSqlDatabase::addDatabase(dbtype,connectionName);
  db->setHostName(hostname);
  db->setDatabaseName(dbname);
  db->setUserName(user);
  db->setPassword(pass);
  
  if(db->open())
  {
    return 0;
  }
  else
  {
    delete db;
    db = NULL;
    return -1;
  }
}

int qtDatabase::close()
{
  if(db == NULL) 
  {
    return -1;
  }  
  db->close();
  delete db;
  db = NULL;
  return 0;
}

int qtDatabase::close(const char* connName)
{
  if(db == NULL) 
  {
    return -1;
  }  
  db->close();
  delete db;
  db = NULL;
  
  QSqlDatabase::removeDatabase(connName);
  
  return 0;
}

int qtDatabase::query(PARAM *p, const char *sqlcommand)
{
  int failed = -1;
  //std::cout << "DEBUG: inside qtDatabase::query " << sqlcommand << std::endl;
  if(db != NULL) failed = 0;
  if(!failed)
    {
      //std::cout << "DEBUG: inside qtDatabase::query converting to QSTRING"<< std::endl;
      QString qsqlcommand = QString::fromUtf8(sqlcommand,strlen(sqlcommand));
      //std::cout << "DEBUG: inside qtDatabase::query QSTRING sql:"<< qsqlcommand.toUtf8().constData()<< std::endl;
      //std::cout << "DEBUG: inside qtDatabase::query creating"<< std::endl;
      //std::cout << "DEBUG: inside qtDatabase::query exec: "<< qsqlcommand.toUtf8().constData() << std::endl;       
      *result = db->exec(qsqlcommand);
      //std::cout << "DEBUG: inside qtDatabase::query capturing error"<< std::endl;
      *error = db->lastError();
      //std::cout << "DEBUG: inside qtDatabase::query error?"<< std::endl;
      if(error->isValid())
	{
	  QString e = error->databaseText();
	  std::cout << "ERROR: qtDatabase::query " << e.toUtf8().constData() << std::endl;
	  std::cout << "ERROR: qtDatabase::query query:" << qsqlcommand.toUtf8().constData() << std::endl;
	  //printf("qtDatabase::query ERROR: %s\n", (const char *) e.toUtf8());
	  //pvStatusMessage(p,255,0,0,"ERROR: qtDatabase::query(%s) %s", sqlcommand, (const char *) e.toUtf8());
	  failed = -1;
	}
  }
  //std::cout << "DEBUG: inside qtDatabase::query, exiting ok! " << std::endl;
  return failed;
}

int qtDatabase::populateTable(PARAM *p, int id)
{
  int x,y,xmax,ymax;
  
  if(db == NULL)
  {
    pvStatusMessage(p,255,0,0,"ERROR: qtDatabase::populateTable() db==NULL");
    return -1;
  }  

  // set table dimension
  xmax = result->record().count();
  //
  // Using SQLITE a user from our forum found an issue
  // getting ymax.
  // With SQLITE numRowsAffected() does not return the correct value.
  // Other database systems do.
  //
  if(db->driverName() == "QSQLITE")
  {
    result->last();
    ymax = result->at()+1;
    result->first();
    //printf("SQLITE ymax = %d \n",ymax);
  }
  else
  {
    ymax = result->numRowsAffected();
    //printf("no SQLITE, ymax = %d \n",ymax);
  }
  pvSetNumRows(p,id,ymax);
  pvSetNumCols(p,id,xmax);

  // populate table
  QSqlRecord record = result->record();
  if(record.isEmpty())
  {
    pvStatusMessage(p,255,0,0,"ERROR: qtDatabase::populateTable() record is empty");
    return -1;
  }

  for(x=0; x<xmax; x++)
  { // write name of fields
    pvSetTableText(p, id, x, -1, (const char *) record.fieldName(x).toUtf8());
  }
  result->next();
  for(y=0; y<ymax; y++)
  { // write fields
    QSqlRecord record = result->record();
    for(x=0; x<xmax; x++)
    {
      QSqlField f = record.field(x);
      if(f.isValid())
      {
        QVariant v = f.value();
        pvSetTableText(p, id, x, y, (const char *) v.toString().toUtf8());
      }
      else
      {
        pvSetTableText(p, id, x, y, "ERROR:");
      }
    }
    result->next();
  }

  return 0;
}

int qtDatabase::retData(PARAM *p, char **** table, int **columns, int **rows)
{
  //std::cout << "DEBUG: (inside qtDatabase::retData)" << std::endl;
  int x,y,xmax,ymax, failed = -1;
  char ***retTable;
  int * pointX;
  int * pointY;

  pointX = new int();
  pointY = new int();
  
  if(db != NULL)
  {
    // set table dimension
    xmax = result->record().count();
    //
    // Using SQLITE a user from our forum found an issue
    // getting ymax.
    // With SQLITE numRowsAffected() does not return the correct value.
    // Other database systems do.
    //
    if(db->driverName() == "QSQLITE")
      {
	result->last();
	ymax = result->at()+1;
	result->first();
	//printf("SQLITE ymax = %d \n",ymax);
      }
    else
      {
	ymax = result->numRowsAffected();
	//printf("no SQLITE, ymax = %d \n",ymax);
      }
    //std::cout << "DEBUG: (inside qtDatabase::retData) return of columns:" << xmax <<"  and rows:" << ymax << std::endl;
    retTable = new char**[ymax];
    for(int i =0; i < ymax; i++)
      retTable[i] = new char*[xmax];
    *pointX = xmax;
    *pointY = ymax;
    // populate table
    QSqlRecord record = result->record();
    if(!record.isEmpty())
      {
	result->next();
	for(y=0; y<ymax; y++)
	  { // write fields
	    QSqlRecord record = result->record();
	    for(x=0; x<xmax; x++)
	      {
		QSqlField f = record.field(x);
		if(f.isValid())
		  {
		    QVariant v = f.value();
		    //retTable[y][x] = new char[strlen(v.toString().toUtf8())];
		    //sprintf(retTable[y][x],"%s",v.toString().toUtf8());
		    retTable[y][x] = new char[strlen(v.toByteArray().data())];
		    sprintf(retTable[y][x],"%s",v.toByteArray().data());
		  }
		else
		  {
		    retTable[y][x] = new char[strlen("ERROR")];
		    sprintf(retTable[y][x],"ERROR");
		  }
	      }
	    result->next();
	  }
	failed = 0;
      }
  }
  //std::cout << "DEBUG: (inside qtDatabase::retData) returning" << std::endl; 
  *table = retTable;
  *columns = pointX;
  *rows = pointY;
  return failed;
}
/*! C++ style ret data fucntion from sql query*/
int qtDatabase::retData(PARAM *p, std::vector< std::vector < std::string>> & sqlResult)
{
  //std::cout << "DEBUG: (inside qtDatabase::retData)" << std::endl;
  int x,y,xmax,ymax, failed = -1;
  std::vector <std::string> rowResult;
  std::string strResult;

  if(db != NULL)
  {
    // set table dimension
    xmax = result->record().count();
    //
    // Using SQLITE a user from our forum found an issue
    // getting ymax.
    // With SQLITE numRowsAffected() does not return the correct value.
    // Other database systems do.
    //
    if(db->driverName() == "QSQLITE")
      {
	result->last();
	ymax = result->at()+1;
	result->first();
	//printf("SQLITE ymax = %d \n",ymax);
      }
    else
      {
	ymax = result->numRowsAffected();
	//printf("no SQLITE, ymax = %d \n",ymax);
      }
    //std::cout << "DEBUG: (inside qtDatabase::retData) return of columns:" << xmax <<"  and rows:" << ymax << std::endl;
    // populate table
    QSqlRecord record = result->record();
    if(!record.isEmpty())
      {
	result->next();
	for(y=0; y<ymax; y++)
	  { // write fields
	    QSqlRecord record = result->record();
	    rowResult.clear();
	    for(x=0; x<xmax; x++)
	      {
		QSqlField f = record.field(x);
		if(f.isValid())
		  {
		    QVariant v = f.value();
		    strResult.clear();
		    //strResult = v.toChar();
		    rowResult.push_back(v.toString().toUtf8().constData());
		  }
		else
		  {
		    rowResult.push_back("ERROR");
		  }
	      }
	    sqlResult.push_back(rowResult);
	    result->next();
	  }
	failed = 0;
      }
  }
  //std::cout << "DEBUG: (inside qtDatabase::retData) returning" << std::endl; 
  return failed;
}
const char *qtDatabase::recordFieldValue(PARAM *p, int x)
{
  QSqlRecord record = result->record();
  if(record.isEmpty())
  {
    pvStatusMessage(p,255,0,0,"ERROR: qtDatabase::recordFieldValue(%d) record is empty", x);
    return "ERROR:";
  }
  QSqlField f = record.field(x);
  if(f.isValid())
  {
    QVariant v = f.value();
    //return v.toString().toUtf8();
    //printf("%s\n", v.toString());
	//modified by Joa
    return v.toByteArray().data();
  }
  else
  {
    pvStatusMessage(p,255,0,0,"ERROR: qtDatabase::recordFieldValue(%d) field is invalid", x);
    return "ERROR:";
  }
}

int qtDatabase::nextRecord()
{
  result->next();
  QSqlRecord record = result->record();
  if(record.isEmpty()) return -1;
  return 0;
}

