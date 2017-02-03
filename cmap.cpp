#include "cmap.h"
#include <memory.h>
#include <string.h>
#include <stdio.h>


///////////////////////////////////////////////////////////////////
////////////////     CMapNumToString Methods     //////////////////
///////////////////////////////////////////////////////////////////
void CMapNumToString::CheckAndGrow()
{
	  if( nMap == 0 || pMap  == NULL ) { Init(); }
    int n = index + 2;  
    if( n >= nMap ) //we will over run the Array
    {
        n = nMap+50;
        MapNumToString* tmp = new MapNumToString[n+2];
        memcpy(tmp,pMap,nMap * sizeof(MapNumToString));
		    delete []pMap;
        pMap = tmp;
		    nMap = n;
    }  
}
                             
int CMapNumToString::AddItem(int id,const char* name)
{
	CheckAndGrow();
	for(int i=1; i < index ; i++ )
	{
    if( pMap[i].ID == id )
    {
      memset(pMap[i].Name,0,MAX_MapNumToString);
      strncpy(pMap[i].Name,name,MAX_MapNumToString-4);
      return i;
    }
  }
  pMap[index].ID = id;
  memset(pMap[index].Name,0,MAX_MapNumToString);
  strncpy(pMap[index].Name,name,MAX_MapNumToString-4);
	int old = index;
	index++;
	return old;
}

const char* CMapNumToString::AddRaw(int id)
{
   int n = AddItem(id,"UnKnown");
   sprintf(pMap[n].Name,"%d",id);
   return pMap[n].Name;  
}
const char* CMapNumToString::AddRaw_Prefix(int id,const char* prefix)
{
   int n = AddItem(id,"UnKnown");
   sprintf(pMap[n].Name,"%s%d_",prefix,id);
   return pMap[n].Name;  
}



const char* CMapNumToString::GetItem(int idx)
{
   if( idx != 0 && idx < index ) return pMap[idx].Name;
   return NULL; 
}

const char* CMapNumToString::Find(int id)
{
	for(int i=1; i < index ; i++ )
	{
    if( pMap[i].ID == id ) return pMap[i].Name;  
  }
	return NULL;
}
///////////////////////////////////////////////////////////////////
///////////    C Wrapper(s)  and Utility Objects //////////////////
///////////////////////////////////////////////////////////////////
extern "C"
{
  const char* GetXyzNameFromID(int id)
  {
    static CMapNumToString XyzName;
    if( XyzName.IsEmpty() == 1 ) //Lets Feed the Data
    {
      XyzName.AddItem(0,"Asif");    
      XyzName.AddItem(1,"Khan");    
      XyzName.AddItem(2,"Suri");    
    }
    //
    const char* tmp = XyzName.Find(id);
    if( tmp != NULL ) return tmp;
    return XyzName.AddRaw_Prefix(id,"myprefix_");
  }
  const char* GetSubscriptionIDTypeName(int id)
  {
    static CMapNumToString SubscriptionIDs;
    if( SubscriptionIDs.IsEmpty() == 1 ) //Lets Feed the Data
    {
      SubscriptionIDs.AddItem(0,"endUserE164");    
      SubscriptionIDs.AddItem(1,"endUserIMSI");    
      SubscriptionIDs.AddItem(2,"endUserSIPURI");    
      SubscriptionIDs.AddItem(3,"endUserNAI");    
      SubscriptionIDs.AddItem(4,"endUserPrivate");    
    }
    //
    const char* tmp = SubscriptionIDs.Find(id);
    if( tmp != NULL ) return tmp;
    return SubscriptionIDs.AddRaw(id);
  }
}


