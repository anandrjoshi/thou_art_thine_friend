// dllmain.h : Declaration of module class.

class CMyFirstBHOModule : public ATL::CAtlDllModuleT< CMyFirstBHOModule >
{
public :
	DECLARE_LIBID(LIBID_MyFirstBHOLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_MYFIRSTBHO, "{1B16EFD4-6D53-423B-91BA-F235E93D5898}")
};

extern class CMyFirstBHOModule _AtlModule;
