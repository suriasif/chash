
#if !defined(NULL)
 #define NULL 0L
#endif

#define MAX_MapNumToString 64
typedef struct MapNumToString_t
{
  int  ID;
  char Name[MAX_MapNumToString];
} MapNumToString;

class CMapNumToString
{
public:
  CMapNumToString()  { Init();	 }
  ~CMapNumToString() { Release(); }
	
  void  Empty()   { index = 1; }//0 index is sentenal value
	int   IsEmpty() { if (index == 1) return 1; else return 0;  }
	int   AddItem(int id,const char* name);
	const char* AddRaw(int id);
	const char* AddRaw_Prefix(int id,const char* prefix);
  const char* Find(int id);
  const char* GetItem(int idx);

private:
	void  Init()
	{
		nMap = 50;
		pMap = new MapNumToString[nMap+2];
		index  = 1;	//0 index is sentenal value
		pMap[0].ID = 0;
		pMap[0].Name[0] =0;
	}
	void Release()
	{
		if( pMap != NULL ) delete []pMap; 
		pMap = NULL;
	}
	void  CheckAndGrow();
  
  MapNumToString* pMap;
  int             nMap;
  int             index;
};

