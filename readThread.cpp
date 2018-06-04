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
@File readThread.cpp
'''thread, for a multi-thread reading from database'''
*/

#include "readThread.h"

int readThread::launch(DBInterface*& myDbClass, int tableId, SlaveInterface** hSlaves)
{
  field currentTag;
  int indexTag = 0;
  int fail = 0;
  
  while(!myDbClass->retFieldFrTable(indexTag,currentTag,tableId))
    {
      for (int l = 0; l < currentTag.fromTags.size(); l++)
	{
	  for (int m = 0; m < currentTag.fromTags[l].size(); m++)
	    {		 
	      fail = fail + hSlaves[l]->readTag(currentTag.fromTags[l].at(m));
	      myDbClass->setFieldValue(tableId,indexTag,hSlaves[l]->retTagValue(currentTag.fromTags[l].at(m)));
	      myDbClass->setFieldValid(tableId,indexTag,hSlaves[l]->retTagValid(currentTag.fromTags[l].at(m)));
	    }
	}
      indexTag++;
    }
  myDbClass->threaded_rTriggerDoneAt(tableId);
  myDbClass->threaded_storeData(tableId);

  return fail;
}
