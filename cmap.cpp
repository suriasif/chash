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
  const char* GetTariffChangeUsageName(int id)
  {
    static CMapNumToString TariffChangeUsages;
    if( TariffChangeUsages.IsEmpty() == 1 ) //Lets Feed the Data
    {
      TariffChangeUsages.AddItem(0,"sdpccr_usedServiceUnits_before_");    
      TariffChangeUsages.AddItem(1,"sdpccr_usedServiceUnits_after_");    
      TariffChangeUsages.AddItem(2,"sdpccr_usedServiceUnits_indeterminate_");    
    }
    //
    const char* tmp = TariffChangeUsages.Find(id);
    if( tmp != NULL ) return tmp;
    return TariffChangeUsages.AddRaw_Prefix(id,"sdpccr_usedServiceUnits_");
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
  const char* GetContextParameterName(int id) 
  {
    static CMapNumToString CtxParamIds;
    if( CtxParamIds.IsEmpty() == 1 ) //Lets Feed the Data
    {
      CtxParamIds.AddItem(16778217, "sdpccr_Subscription_Id_Location_"  );
      CtxParamIds.AddItem(16778218, "sdpccr_ServiceProvider_Id_"        );
      CtxParamIds.AddItem(16778219, "sdpccr_Other_Party_Id_Address_"    );
      CtxParamIds.AddItem(16778220, "sdpccr_Other_Party_Id_MNP_RN_"     );
      CtxParamIds.AddItem(16778221, "sdpccr_Other_Party_Id_Location_"   );
      CtxParamIds.AddItem(16778222, "sdpccr_Other_Party_Id_MNP_Result_" );
      CtxParamIds.AddItem(16778223, "sdpccr_Other_Party_Id_MNP_IMSI_"   );
      CtxParamIds.AddItem(16778224, "sdpccr_Subscription_Type_"         );
      CtxParamIds.AddItem(16778225, "sdpccr_SMSC_Address_"              );
      CtxParamIds.AddItem(16777216, "sdpccr_RatingGroup_"               );
      CtxParamIds.AddItem(16777217, "sdpccr_MNC_"                       );
      CtxParamIds.AddItem(16777218, "sdpccr_MCC_"                       );
      CtxParamIds.AddItem(16777219, "sdpccr_Volume_Event_"              );
      CtxParamIds.AddItem(16777220, "sdpccr_Unknown_16777220_"          );
      CtxParamIds.AddItem(16777221, "sdpccr_Unknown_16777221_"          );
      CtxParamIds.AddItem(16777222, "sdpccr_TrafficCase_"               );
      CtxParamIds.AddItem(16777223, "sdpccr_Unknown_16777223_"          );
      CtxParamIds.AddItem(16777420, "sdpccr_APNName_"                   );
      //Gy V2.0 (merging)
      CtxParamIds.AddItem(16778235,	"sdpccr_APNName_"                   ); //Called-Station-Id
      CtxParamIds.AddItem(16778239,	"sdpccr_MCC_"                       ); //3GPP-SGSN-MCC-MNC
      CtxParamIds.AddItem(16778240,	"sdpccr_Unknown_16777221_"          ); //3GPP-RAT-Type
      CtxParamIds.AddItem(16778254,	"sdpccr_RatingGroup_"               ); //Rating-Group
      //Gy V2.0
      CtxParamIds.AddItem(16778226,	"sdpccr_3GPP_Charging_Id_"  );
      CtxParamIds.AddItem(16778227,	"sdpccr_3GPP_PDP_Type_"  );
      CtxParamIds.AddItem(16778228,	"sdpccr_PDP_Address_"  );
      CtxParamIds.AddItem(16778229,	"sdpccr_SGSN_Address_"  );
      CtxParamIds.AddItem(16778230,	"sdpccr_GGSN_Address_"  );
      CtxParamIds.AddItem(16778231,	"sdpccr_CG_Address_"  );
      CtxParamIds.AddItem(16778232,	"sdpccr_3GPP_IMSI_MCC_MNC_"  );
      CtxParamIds.AddItem(16778233,	"sdpccr_3GPP_GGSN_MCC_MNC_"  );
      CtxParamIds.AddItem(16778234,	"sdpccr_3GPP_NSAPI_"  );
      CtxParamIds.AddItem(16778236,	"sdpccr_3GPP_Session_Stop_Indicator_"  );
      CtxParamIds.AddItem(16778237,	"sdpccr_3GPP_Selection_Mode_"  );
      CtxParamIds.AddItem(16778238,	"sdpccr_3GPP_Charging_Characteristics_"  );
      CtxParamIds.AddItem(16778241,	"sdpccr_PDP_Context_Type_"  );
      CtxParamIds.AddItem(16778242,	"sdpccr_NAS_Port_"  );
      CtxParamIds.AddItem(16778243,	"sdpccr_User_Equipment_Info_IMEISV_"  );
      CtxParamIds.AddItem(16778244,	"sdpccr_Traffic_Class_"  );
      CtxParamIds.AddItem(16778245,	"sdpccr_Guaranteed_Bit_Rate_For_Uplink_"  );
      CtxParamIds.AddItem(16778246,	"sdpccr_Guaranteed_Bit_Rate_For_Uplink_Subscribed_"  );
      CtxParamIds.AddItem(16778247,	"sdpccr_Guaranteed_Bit_Rate_For_Downlink_"  );
      CtxParamIds.AddItem(16778248,	"sdpccr_Guaranteed_Bit_Rate_For_Downlink_Subscribed_"  );
      CtxParamIds.AddItem(16778249,	"sdpccr_Ericsson_Location_MCC_MNC_"  );
      CtxParamIds.AddItem(16778250,	"sdpccr_Ericsson_Location_Area_Code_"  );
      CtxParamIds.AddItem(16778251,	"sdpccr_Ericsson_Geo_Location_Cell_Identity_"  );
      CtxParamIds.AddItem(16778252,	"sdpccr_Ericsson_Geo_Location_Service_Area_Code_"  );
      CtxParamIds.AddItem(16778253,	"sdpccr_Ericsson_Geo_Location_Routing_Area_Code_"  );
    }
    //
    const char* tmp = CtxParamIds.Find(id);
    if( tmp != NULL ) return tmp;
    return CtxParamIds.AddRaw(id);
  }
}


