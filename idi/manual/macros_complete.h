//////////////////////////////////////////////////////////////////////////////////////////
//
// Borland Macros

#define BEGIN_MESSAGE_MAP   virtual void __fastcall Dispatch(void *Message) \
        {                                           \
          switch  (((PMessage)Message)->Msg)        \
          {
#define END_MESSAGE_MAP(base)           default:    \
                        base::Dispatch(Message);    \
                        break;                      \
          }                                         \
        }
#define VCL_MESSAGE_HANDLER(msg,type,meth)          \
          case    msg:                              \
            meth(*((type *)Message));               \
            break;

//////////////////////////////////////////////////////////////////////////////////////////
//
// Borland Macros (SYSCLASS.H)


  #define USEDATAMODULE(FileName, DataModuleName)  \
    class DELPHICLASS T##DataModuleName;       \
    extern PACKAGE T##DataModuleName *DataModuleName;

  #define USEDATAMODULENS(FileName, UnitName, DataModuleName)  \
    namespace UnitName {         \
    class DELPHICLASS T##DataModuleName;       \
    extern PACKAGE T##DataModuleName *DataModuleName;              \
    };               \
    using namespace UnitName

  #define USEFORM(FileName, FormName) \
    class DELPHICLASS T##FormName;       \
    extern PACKAGE T##FormName *FormName;

#define USEFORMRES(FileName, FormName, AncestorName) \
   extern PACKAGE DummyThatIsNeverReferenced

  #define USEFORMNS(FileName, UnitName, FormName) \
    namespace UnitName {         \
    class DELPHICLASS T##FormName;       \
    extern PACKAGE T##FormName *FormName;          \
    };               \
    using namespace UnitName

#define USEUNIT(ModName) \
   extern PACKAGE DummyThatIsNeverReferenced

#define USEOBJ(FileName) \
   extern PACKAGE DummyThatIsNeverReferenced

#define USERC(FileName) \
   extern PACKAGE DummyThatIsNeverReferenced

#define USEASM(FileName) \
   extern PACKAGE DummyThatIsNeverReferenced

#define USEDEF(FileName) \
   extern PACKAGE DummyThatIsNeverReferenced

#define USERES(FileName) \
   extern PACKAGE DummyThatIsNeverReferenced

#define USETLB(FileName) \
   extern PACKAGE DummyThatIsNeverReferenced

#define USELIB(FileName) \
   extern PACKAGE DummyThatIsNeverReferenced

#define USEFILE(FileName) \
   extern PACKAGE DummyThatIsNeverReferenced

#define USEPACKAGE(FileName) \
   extern PACKAGE DummyThatIsNeverReferenced

#define USEIDL(FileName) \
   extern PACKAGE DummyThatIsNeverReferenced


#define DECLARE_DINTERFACE_TYPE(iface) \
        interface iface;               \
        typedef System::DelphiInterface< iface >  _di_##iface;

//////////////////////////////////////////////////////////////////////////////////////////
//
// ATL macros (ATLWIN.H)

#define BEGIN_MSG_MAP(theClass) \
public: \
	BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0) \
	{ \
		BOOL bHandled = TRUE; \
		hWnd; \
		switch(dwMsgMapID) \
		{ \
		case 0:

#define ALT_MSG_MAP(msgMapID) \
		break; \
		case msgMapID:

#define MESSAGE_HANDLER(msg, func) \
	if(uMsg == msg) \
	{ \
		bHandled = TRUE; \
		lResult = func(uMsg, wParam, lParam, bHandled); \
		if(bHandled) \
			return TRUE; \
	}

#define MESSAGE_RANGE_HANDLER(msgFirst, msgLast, func) \
	if(uMsg >= msgFirst && uMsg <= msgLast) \
	{ \
		bHandled = TRUE; \
		lResult = func(uMsg, wParam, lParam, bHandled); \
		if(bHandled) \
			return TRUE; \
	}

#define COMMAND_HANDLER(id, code, func) \
	if(uMsg == WM_COMMAND && id == LOWORD(wParam) && code == HIWORD(wParam)) \
	{ \
		bHandled = TRUE; \
		lResult = func(HIWORD(wParam), LOWORD(wParam), (HWND)lParam, bHandled); \
		if(bHandled) \
			return TRUE; \
	}

#define COMMAND_ID_HANDLER(id, func) \
	if(uMsg == WM_COMMAND && id == LOWORD(wParam)) \
	{ \
		bHandled = TRUE; \
		lResult = func(HIWORD(wParam), LOWORD(wParam), (HWND)lParam, bHandled); \
		if(bHandled) \
			return TRUE; \
	}

#define COMMAND_CODE_HANDLER(code, func) \
	if(uMsg == WM_COMMAND && code == HIWORD(wParam)) \
	{ \
		bHandled = TRUE; \
		lResult = func(HIWORD(wParam), LOWORD(wParam), (HWND)lParam, bHandled); \
		if(bHandled) \
			return TRUE; \
	}

#define COMMAND_RANGE_HANDLER(idFirst, idLast, func) \
	if(uMsg == WM_COMMAND && LOWORD(wParam) >= idFirst  && LOWORD(wParam) <= idLast) \
	{ \
		bHandled = TRUE; \
		lResult = func(HIWORD(wParam), LOWORD(wParam), (HWND)lParam, bHandled); \
		if(bHandled) \
			return TRUE; \
	}

#define NOTIFY_HANDLER(id, cd, func) \
	if(uMsg == WM_NOTIFY && id == ((LPNMHDR)lParam)->idFrom && cd == ((LPNMHDR)lParam)->code) \
	{ \
		bHandled = TRUE; \
		lResult = func((int)wParam, (LPNMHDR)lParam, bHandled); \
		if(bHandled) \
			return TRUE; \
	}

#define NOTIFY_ID_HANDLER(id, func) \
	if(uMsg == WM_NOTIFY && id == ((LPNMHDR)lParam)->idFrom) \
	{ \
		bHandled = TRUE; \
		lResult = func((int)wParam, (LPNMHDR)lParam, bHandled); \
		if(bHandled) \
			return TRUE; \
	}

#define NOTIFY_CODE_HANDLER(cd, func) \
	if(uMsg == WM_NOTIFY && cd == ((LPNMHDR)lParam)->code) \
	{ \
		bHandled = TRUE; \
		lResult = func((int)wParam, (LPNMHDR)lParam, bHandled); \
		if(bHandled) \
			return TRUE; \
	}

#define NOTIFY_RANGE_HANDLER(idFirst, idLast, func) \
	if(uMsg == WM_NOTIFY && ((LPNMHDR)lParam)->idFrom >= idFirst && ((LPNMHDR)lParam)->idFrom <= idLast) \
	{ \
		bHandled = TRUE; \
		lResult = func((int)wParam, (LPNMHDR)lParam, bHandled); \
		if(bHandled) \
			return TRUE; \
	}

#define CHAIN_MSG_MAP(theChainClass) \
	{ \
		if(theChainClass::ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult)) \
			return TRUE; \
	}

#define CHAIN_MSG_MAP_MEMBER(theChainMember) \
	{ \
		if(theChainMember.ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult)) \
			return TRUE; \
	}

#define CHAIN_MSG_MAP_ALT(theChainClass, msgMapID) \
	{ \
		if(theChainClass::ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult, msgMapID)) \
			return TRUE; \
	}

#define CHAIN_MSG_MAP_ALT_MEMBER(theChainMember, msgMapID) \
	{ \
		if(theChainMember.ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult, msgMapID)) \
			return TRUE; \
	}

#define CHAIN_MSG_MAP_DYNAMIC(dynaChainID) \
	{ \
		if(CDynamicChain::CallChain(dynaChainID, hWnd, uMsg, wParam, lParam, lResult)) \
			return TRUE; \
	}

#define CHAIN_MSG_MAP_ALT_DYNAMIC(dynaChainID, msgMapID) \
	{ \
		if(CDynamicChain::CallChain(dynaChainID, hWnd, uMsg, wParam, lParam, lResult, msgMapID)) \
			return TRUE; \
	}

#define END_MSG_MAP() \
			break; \
		default: \
			ATLTRACE(_T("Invalid message map ID (%i)\n"), dwMsgMapID); \
			_ASSERTE(FALSE); \
			break; \
		} \
		return FALSE; \
	}

#define DECLARE_WND_CLASS(WndClassName) \
static CWndClassInfo& GetWndClassInfo() \
{ \
	static CWndClassInfo wc = \
	{ \
		{ sizeof(WNDCLASSEX), CS_HREDRAW|CS_VREDRAW, StartWindowProc, \
		  0, 0, 0, 0, 0, (HBRUSH)(COLOR_WINDOW+1), 0, WndClassName, 0 }, \
		NULL, NULL, IDC_ARROW, TRUE, 0, _T("") \
	}; \
	return wc; \
}

#define DECLARE_WND_SUPERCLASS(WndClassName, OrigWndClassName) \
static CWndClassInfo& GetWndClassInfo() \
{ \
	static CWndClassInfo wc = \
	{ \
		{ sizeof(WNDCLASSEX), NULL, StartWindowProc, \
		  0, 0, 0, 0, 0, NULL, 0, WndClassName, 0 }, \
		OrigWndClassName, NULL, NULL, TRUE, 0, _T("") \
	}; \
	return wc; \
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// MFC macros (AFXWIN.H)


#define DECLARE_MESSAGE_MAP() \
private: \
	static const AFX_MSGMAP_ENTRY _messageEntries[]; \
protected: \
	static AFX_DATA const AFX_MSGMAP messageMap; \
	virtual const AFX_MSGMAP* GetMessageMap() const; \

#define BEGIN_MESSAGE_MAP(theClass, baseClass) \
	const AFX_MSGMAP* theClass::GetMessageMap() const \
		{ return &theClass::messageMap; } \
	AFX_DATADEF const AFX_MSGMAP theClass::messageMap = \
	{ &baseClass::messageMap, &theClass::_messageEntries[0] }; \
	const AFX_MSGMAP_ENTRY theClass::_messageEntries[] = \
	{ \

#define END_MESSAGE_MAP() \
		{0, 0, 0, 0, AfxSig_end, (AFX_PMSG)0 } \
	}; \

#define DECLARE_INTERFACE_MAP() \
private: \
	static const AFX_INTERFACEMAP_ENTRY _interfaceEntries[]; \
protected: \
	static AFX_DATA const AFX_INTERFACEMAP interfaceMap; \
	virtual const AFX_INTERFACEMAP* GetInterfaceMap() const; \

#define DECLARE_DISPATCH_MAP() \
private: \
	static const AFX_DISPMAP_ENTRY _dispatchEntries[]; \
	static UINT _dispatchEntryCount; \
	static DWORD _dwStockPropMask; \
protected: \
	static AFX_DATA const AFX_DISPMAP dispatchMap; \
	virtual const AFX_DISPMAP* GetDispatchMap() const; \

#define DECLARE_OLECMD_MAP() \
private: \
	static const AFX_OLECMDMAP_ENTRY _commandEntries[]; \
protected: \
	static AFX_DATA const AFX_OLECMDMAP commandMap; \
	virtual const AFX_OLECMDMAP* GetCommandMap() const; \

#define BEGIN_OLECMD_MAP(theClass, baseClass) \
	const AFX_OLECMDMAP* theClass::GetCommandMap() const \
		{ return &theClass::commandMap; } \
	AFX_DATADEF const AFX_OLECMDMAP theClass::commandMap = \
	{ &baseClass::commandMap, &theClass::_commandEntries[0] }; \
	const AFX_OLECMDMAP_ENTRY theClass::_commandEntries[] = \
	{ \

#define END_OLECMD_MAP() \
		{NULL, 0, 0} \
	}; \

#define DECLARE_EVENTSINK_MAP() \
private: \
	static const AFX_EVENTSINKMAP_ENTRY _eventsinkEntries[]; \
	static UINT _eventsinkEntryCount; \
protected: \
	static AFX_DATA const AFX_EVENTSINKMAP eventsinkMap; \
	virtual const AFX_EVENTSINKMAP* GetEventSinkMap() const; \


#define DECLARE_CONNECTION_MAP() \
private: \
	static const AFX_CONNECTIONMAP_ENTRY _connectionEntries[]; \
protected: \
	static AFX_DATA const AFX_CONNECTIONMAP connectionMap; \
	virtual const AFX_CONNECTIONMAP* GetConnectionMap() const; \


//////////////////////////////////////////////////////////////////////////////////////////
//
// MFC macros (AFX.H)


#define DECLARE_DYNAMIC(class_name) \
public: \
	static const AFX_DATA CRuntimeClass class##class_name; \
	virtual CRuntimeClass* GetRuntimeClass() const; \

#define _DECLARE_DYNAMIC(class_name) \
public: \
	static AFX_DATA CRuntimeClass class##class_name; \
	virtual CRuntimeClass* GetRuntimeClass() const; \

#define RUNTIME_CLASS(class_name) ((CRuntimeClass*)(&class_name::class##class_name))
#define ASSERT_KINDOF(class_name, object) \
	ASSERT((object)->IsKindOf(RUNTIME_CLASS(class_name)))

#define DECLARE_DYNCREATE(class_name) \
	DECLARE_DYNAMIC(class_name) \
	static CObject* PASCAL CreateObject();

#define _DECLARE_DYNCREATE(class_name) \
	_DECLARE_DYNAMIC(class_name) \
	static CObject* PASCAL CreateObject();

#define DECLARE_SERIAL(class_name) \
	_DECLARE_DYNCREATE(class_name) \
	friend CArchive& AFXAPI operator>>(CArchive& ar, class_name* &pOb);

#define IMPLEMENT_RUNTIMECLASS(class_name, base_class_name, wSchema, pfnNew) \
	const AFX_DATADEF CRuntimeClass class_name::class##class_name = { \
		#class_name, sizeof(class class_name), wSchema, pfnNew, \
			RUNTIME_CLASS(base_class_name), NULL }; \
	CRuntimeClass* class_name::GetRuntimeClass() const \
		{ return RUNTIME_CLASS(class_name); } \

#define _IMPLEMENT_RUNTIMECLASS(class_name, base_class_name, wSchema, pfnNew) \
	AFX_DATADEF CRuntimeClass class_name::class##class_name = { \
		#class_name, sizeof(class class_name), wSchema, pfnNew, \
			RUNTIME_CLASS(base_class_name), NULL }; \
	CRuntimeClass* class_name::GetRuntimeClass() const \
		{ return RUNTIME_CLASS(class_name); } \


#define IMPLEMENT_DYNAMIC(class_name, base_class_name) \
	IMPLEMENT_RUNTIMECLASS(class_name, base_class_name, 0xFFFF, NULL)

#define IMPLEMENT_DYNCREATE(class_name, base_class_name) \
	CObject* PASCAL class_name::CreateObject() \
		{ return new class_name; } \
	IMPLEMENT_RUNTIMECLASS(class_name, base_class_name, 0xFFFF, \
		class_name::CreateObject)

#define IMPLEMENT_SERIAL(class_name, base_class_name, wSchema) \
	CObject* PASCAL class_name::CreateObject() \
		{ return new class_name; } \
	_IMPLEMENT_RUNTIMECLASS(class_name, base_class_name, wSchema, \
		class_name::CreateObject) \
	static const AFX_CLASSINIT _init_##class_name(RUNTIME_CLASS(class_name)); \
	CArchive& AFXAPI operator>>(CArchive& ar, class_name* &pOb) \
		{ pOb = (class_name*) ar.ReadObject(RUNTIME_CLASS(class_name)); \
			return ar; } \



//////////////////////////////////////////////////////////////////////////////////////////
//
// COM macros (ATLCOM.H)

#define _ATLDUMPIID(iid, name, hr) hr

#define _ATL_DEBUG_ADDREF_RELEASE_IMPL(className)\
	virtual ULONG STDMETHODCALLTYPE AddRef(void) = 0;\
	virtual ULONG STDMETHODCALLTYPE Release(void) = 0;

#define DECLARE_PROTECT_FINAL_CONSTRUCT()\
	void InternalFinalConstructAddRef() {InternalAddRef();}\
	void InternalFinalConstructRelease() {InternalRelease();}

#define DECLARE_NOT_AGGREGATABLE(x) public:\
	typedef CComCreator2< CComCreator< CComObject< x > >, CComFailCreator<CLASS_E_NOAGGREGATION> > _CreatorClass;
#define DECLARE_AGGREGATABLE(x) public:\
	typedef CComCreator2< CComCreator< CComObject< x > >, CComCreator< CComAggObject< x > > > _CreatorClass;
#define DECLARE_ONLY_AGGREGATABLE(x) public:\
	typedef CComCreator2< CComFailCreator<E_FAIL>, CComCreator< CComAggObject< x > > > _CreatorClass;
#define DECLARE_POLY_AGGREGATABLE(x) public:\
	typedef CComCreator< CComPolyObject< x > > _CreatorClass;

#define DEBUG_QI_ENTRY(x)

#define BEGIN_COM_MAP(x) public: \
	typedef x _ComMapClass; \
	static HRESULT WINAPI _Cache(void* pv, REFIID iid, void** ppvObject, DWORD dw)\
	{\
		_ComMapClass* p = (_ComMapClass*)pv;\
		p->Lock();\
		HRESULT hRes = CComObjectRootBase::_Cache(pv, iid, ppvObject, dw);\
		p->Unlock();\
		return hRes;\
	}\
	IUnknown* GetUnknown() \
	{ _ASSERTE(_GetEntries()[0].pFunc == _ATL_SIMPLEMAPENTRY); return (IUnknown*)((int)this+_GetEntries()->dw); } \
	HRESULT _InternalQueryInterface(REFIID iid, void** ppvObject) \
	{ return InternalQueryInterface(this, _GetEntries(), iid, ppvObject); } \
	const static _ATL_INTMAP_ENTRY* WINAPI _GetEntries() { \
	static const _ATL_INTMAP_ENTRY _entries[] = { DEBUG_QI_ENTRY(x)

#define DECLARE_GET_CONTROLLING_UNKNOWN() public:\
	virtual IUnknown* GetControllingUnknown() {return GetUnknown();}

#define COM_INTERFACE_ENTRY_BREAK(x)\
	{&IID_##x, \
	NULL, \
	_Break},

#define COM_INTERFACE_ENTRY_NOINTERFACE(x)\
	{&IID_##x, \
	NULL, \
	_NoInterface},

#define COM_INTERFACE_ENTRY(x)\
	{&IID_##x, \
	offsetofclass(x, _ComMapClass), \
	_ATL_SIMPLEMAPENTRY},

#define COM_INTERFACE_ENTRY_IID(iid, x)\
	{&iid,\
	offsetofclass(x, _ComMapClass),\
	_ATL_SIMPLEMAPENTRY},

#define COM_INTERFACE_ENTRY_IMPL(x)\
	COM_INTERFACE_ENTRY_IID(IID_##x, x##Impl<_ComMapClass>)

#define COM_INTERFACE_ENTRY_IMPL_IID(iid, x)\
	COM_INTERFACE_ENTRY_IID(iid, x##Impl<_ComMapClass>)

#define COM_INTERFACE_ENTRY2(x, x2)\
	{&IID_##x,\
	(DWORD)((x*)(x2*)((_ComMapClass*)8))-8,\
	_ATL_SIMPLEMAPENTRY},

#define COM_INTERFACE_ENTRY2_IID(iid, x, x2)\
	{&iid,\
	(DWORD)((x*)(x2*)((_ComMapClass*)8))-8,\
	_ATL_SIMPLEMAPENTRY},

#define COM_INTERFACE_ENTRY_FUNC(iid, dw, func)\
	{&iid, \
	dw, \
	func},

#define COM_INTERFACE_ENTRY_FUNC_BLIND(dw, func)\
	{NULL, \
	dw, \
	func},

#define COM_INTERFACE_ENTRY_TEAR_OFF(iid, x)\
	{&iid,\
	(DWORD)&_CComCreatorData<\
		CComInternalCreator< CComTearOffObject< x > >\
		>::data,\
	_Creator},

#define COM_INTERFACE_ENTRY_CACHED_TEAR_OFF(iid, x, punk)\
	{&iid,\
	(DWORD)&_CComCacheData<\
		CComCreator< CComCachedTearOffObject< x > >,\
		(DWORD)offsetof(_ComMapClass, punk)\
		>::data,\
	_Cache},

#define COM_INTERFACE_ENTRY_AGGREGATE(iid, punk)\
	{&iid,\
	(DWORD)offsetof(_ComMapClass, punk),\
	_Delegate},

#define COM_INTERFACE_ENTRY_AGGREGATE_BLIND(punk)\
	{NULL,\
	(DWORD)offsetof(_ComMapClass, punk),\
	_Delegate},

#define COM_INTERFACE_ENTRY_AUTOAGGREGATE(iid, punk, clsid)\
	{&iid,\
	(DWORD)&_CComCacheData<\
		CComAggregateCreator<_ComMapClass, &clsid>,\
		(DWORD)offsetof(_ComMapClass, punk)\
		>::data,\
	_Cache},

#define COM_INTERFACE_ENTRY_AUTOAGGREGATE_BLIND(punk, clsid)\
	{NULL,\
	(DWORD)&_CComCacheData<\
		CComAggregateCreator<_ComMapClass, &clsid>,\
		(DWORD)offsetof(_ComMapClass, punk)\
		>::data,\
	_Cache},

#define COM_INTERFACE_ENTRY_CHAIN(classname)\
	{NULL,\
	(DWORD)&_CComChainData<classname, _ComMapClass>::data,\
	_Chain},

#define END_COM_MAP()   {NULL, 0, 0}};\
	return _entries;}

#define BEGIN_OBJECT_MAP(x) static _ATL_OBJMAP_ENTRY x[] = {
#define END_OBJECT_MAP()   {NULL, NULL, NULL, NULL}};
#define OBJECT_ENTRY(clsid, class) {&clsid, &class::UpdateRegistry, &class::_ClassFactoryCreatorClass::CreateInstance, &class::_CreatorClass::CreateInstance, NULL, 0, &class::GetObjectDescription },

#define DECLARE_CLASSFACTORY_EX(cf) typedef CComCreator< CComObjectNoLock< cf > > _ClassFactoryCreatorClass;
#define DECLARE_CLASSFACTORY() DECLARE_CLASSFACTORY_EX(CComClassFactory)
#define DECLARE_CLASSFACTORY2(lic) DECLARE_CLASSFACTORY_EX(CComClassFactory2<lic>)
#define DECLARE_CLASSFACTORY_AUTO_THREAD() DECLARE_CLASSFACTORY_EX(CComClassFactoryAutoThread)
#define DECLARE_CLASSFACTORY_SINGLETON(obj) DECLARE_CLASSFACTORY_EX(CComClassFactorySingleton<obj>)

#define DECLARE_OBJECT_DESCRIPTION(x)\
	static LPCTSTR WINAPI GetObjectDescription()\
	{\
		return _T(x);\
	}

#define DECLARE_NO_REGISTRY()\
	static HRESULT WINAPI UpdateRegistry(BOOL /*bRegister*/)\
	{return S_OK;}

#define DECLARE_REGISTRY(class, pid, vpid, nid, flags)\
	static HRESULT WINAPI UpdateRegistry(BOOL bRegister)\
	{\
		return _Module.UpdateRegistryClass(GetObjectCLSID(), pid, vpid, nid,\
			flags, bRegister);\
	}

#define DECLARE_REGISTRY_RESOURCE(x)\
	static HRESULT WINAPI UpdateRegistry(BOOL bRegister)\
	{\
	return _Module.UpdateRegistryFromResource(_T(#x), bRegister);\
	}

#define DECLARE_REGISTRY_RESOURCEID(x)\
	static HRESULT WINAPI UpdateRegistry(BOOL bRegister)\
	{\
	return _Module.UpdateRegistryFromResource(x, bRegister);\
	}

#define BEGIN_CONNECTION_POINT_MAP(x)\
	typedef x _atl_conn_classtype;\
	static const _ATL_CONNMAP_ENTRY* GetConnMap(int* pnEntries) {\
	static const _ATL_CONNMAP_ENTRY _entries[] = {
// CONNECTION_POINT_ENTRY computes the offset of the connection point to the
// IConnectionPointContainer interface
#define CONNECTION_POINT_ENTRY(iid){offsetofclass(_ICPLocator<&iid>, _atl_conn_classtype)-\
	offsetofclass(IConnectionPointContainerImpl<_atl_conn_classtype>, _atl_conn_classtype)},
#define END_CONNECTION_POINT_MAP() {(DWORD)-1} }; \
	if (pnEntries) *pnEntries = sizeof(_entries)/sizeof(_ATL_CONNMAP_ENTRY) - 1; \
	return _entries;}



//////////////////////////////////////////////////////////////////////////////////////////
//
// MFC macros (AFXCTL.H)

#define DECLARE_EVENT_MAP() \
private: \
	static const AFX_DATA AFX_EVENTMAP_ENTRY _eventEntries[]; \
	static DWORD _dwStockEventMask; \
protected: \
	static const AFX_DATA AFX_EVENTMAP eventMap; \
	virtual const AFX_EVENTMAP* GetEventMap() const;

#define BEGIN_EVENT_MAP(theClass, baseClass) \
	const AFX_EVENTMAP* theClass::GetEventMap() const \
		{ return &eventMap; } \
	const AFX_DATADEF AFX_EVENTMAP theClass::eventMap = \
		{ &(baseClass::eventMap), theClass::_eventEntries, \
			&theClass::_dwStockEventMask }; \
	DWORD theClass::_dwStockEventMask = (DWORD)-1; \
	const AFX_DATADEF AFX_EVENTMAP_ENTRY theClass::_eventEntries[] = \
	{

#define END_EVENT_MAP() \
		{ afxEventCustom, DISPID_UNKNOWN, NULL, NULL }, \
	};

#define EVENT_CUSTOM(pszName, pfnFire, vtsParams) \
	{ afxEventCustom, DISPID_UNKNOWN, _T(pszName), vtsParams },

#define EVENT_CUSTOM_ID(pszName, dispid, pfnFire, vtsParams) \
	{ afxEventCustom, dispid, _T(pszName), vtsParams },

#define EVENT_PARAM(vtsParams) (BYTE*)(vtsParams)

/////////////////////////////////////////////////////////////////////////////
// Stock events

#define EVENT_STOCK_CLICK() \
	{ afxEventStock, DISPID_CLICK, _T("Click"), VTS_NONE },

#define EVENT_STOCK_DBLCLICK() \
	{ afxEventStock, DISPID_DBLCLICK, _T("DblClick"), VTS_NONE },

#define EVENT_STOCK_KEYDOWN() \
	{ afxEventStock, DISPID_KEYDOWN, _T("KeyDown"), VTS_PI2 VTS_I2 },

#define EVENT_STOCK_KEYPRESS() \
	{ afxEventStock, DISPID_KEYPRESS, _T("KeyPress"), VTS_PI2 },

#define EVENT_STOCK_KEYUP() \
	{ afxEventStock, DISPID_KEYUP, _T("KeyUp"), VTS_PI2 VTS_I2 },

#define EVENT_STOCK_MOUSEDOWN() \
	{ afxEventStock, DISPID_MOUSEDOWN, _T("MouseDown"), \
	  VTS_I2 VTS_I2 VTS_XPOS_PIXELS VTS_YPOS_PIXELS },

#define EVENT_STOCK_MOUSEMOVE() \
	{ afxEventStock, DISPID_MOUSEMOVE, _T("MouseMove"), \
	  VTS_I2 VTS_I2 VTS_XPOS_PIXELS VTS_YPOS_PIXELS },

#define EVENT_STOCK_MOUSEUP() \
	{ afxEventStock, DISPID_MOUSEUP, _T("MouseUp"), \
	  VTS_I2 VTS_I2 VTS_XPOS_PIXELS VTS_YPOS_PIXELS },

#define EVENT_STOCK_ERROREVENT() \
	{ afxEventStock, DISPID_ERROREVENT, _T("Error"), \
	  VTS_I2 VTS_PBSTR VTS_SCODE VTS_BSTR VTS_BSTR VTS_I4 VTS_PBOOL },

#define EVENT_STOCK_READYSTATECHANGE() \
	{ afxEventStock, DISPID_READYSTATECHANGE, _T("ReadyStateChange"), \
	  VTS_I4 },

#define DISP_PROPERTY_STOCK(theClass, szExternalName, dispid, pfnGet, pfnSet, vtPropType) \
	{ _T(szExternalName), dispid, NULL, vtPropType, \
		(AFX_PMSG)(void (theClass::*)(void))&pfnGet, \
		(AFX_PMSG)(void (theClass::*)(void))&pfnSet, 0, afxDispStock }, \

#define DISP_STOCKPROP_APPEARANCE() \
	DISP_PROPERTY_STOCK(COleControl, "Appearance", DISPID_APPEARANCE, \
		COleControl::GetAppearance, COleControl::SetAppearance, VT_I2)

#define DISP_STOCKPROP_BACKCOLOR() \
	DISP_PROPERTY_STOCK(COleControl, "BackColor", DISPID_BACKCOLOR, \
		COleControl::GetBackColor, COleControl::SetBackColor, VT_COLOR)

#define DISP_STOCKPROP_BORDERSTYLE() \
	DISP_PROPERTY_STOCK(COleControl, "BorderStyle", DISPID_BORDERSTYLE, \
		COleControl::GetBorderStyle, COleControl::SetBorderStyle, VT_I2)

#define DISP_STOCKPROP_CAPTION() \
	DISP_PROPERTY_STOCK(COleControl, "Caption", DISPID_CAPTION, \
		COleControl::GetText, COleControl::SetText, VT_BSTR)

#define DISP_STOCKPROP_ENABLED() \
	DISP_PROPERTY_STOCK(COleControl, "Enabled", DISPID_ENABLED, \
		COleControl::GetEnabled, COleControl::SetEnabled, VT_BOOL)

#define DISP_STOCKPROP_FONT() \
	DISP_PROPERTY_STOCK(COleControl, "Font", DISPID_FONT, \
		COleControl::GetFont, COleControl::SetFont, VT_FONT)

#define DISP_STOCKPROP_FORECOLOR() \
	DISP_PROPERTY_STOCK(COleControl, "ForeColor", DISPID_FORECOLOR, \
		COleControl::GetForeColor, COleControl::SetForeColor, VT_COLOR)

#define DISP_STOCKPROP_HWND() \
	DISP_PROPERTY_STOCK(COleControl, "hWnd", DISPID_HWND, \
		COleControl::GetHwnd, SetNotSupported, VT_HANDLE)

#define DISP_STOCKPROP_TEXT() \
	DISP_PROPERTY_STOCK(COleControl, "Text", DISPID_TEXT, \
		COleControl::GetText, COleControl::SetText, VT_BSTR)

#define DISP_STOCKPROP_READYSTATE() \
	DISP_PROPERTY_STOCK(COleControl, "ReadyState", DISPID_READYSTATE, \
		COleControl::GetReadyState, SetNotSupported, VT_I4)

/////////////////////////////////////////////////////////////////////////////
// Stock methods

#define DISP_FUNCTION_STOCK(theClass, szExternalName, dispid, pfnMember, vtRetVal, vtsParams) \
	{ _T(szExternalName), dispid, vtsParams, vtRetVal, \
		(AFX_PMSG)(void (theClass::*)(void))&pfnMember, (AFX_PMSG)0, 0, \
		afxDispStock }, \

#define DISP_STOCKFUNC_REFRESH() \
	DISP_FUNCTION_STOCK(COleControl, "Refresh", DISPID_REFRESH, \
			COleControl::Refresh, VT_EMPTY, VTS_NONE)

#define DISP_STOCKFUNC_DOCLICK() \
	DISP_FUNCTION_STOCK(COleControl, "DoClick", DISPID_DOCLICK, \
			COleControl::DoClick, VT_EMPTY, VTS_NONE)

/////////////////////////////////////////////////////////////////////////////
// Macros for object factory and class ID

#define BEGIN_OLEFACTORY(class_name) \
protected: \
	class class_name##Factory : public COleObjectFactoryEx \
	{ \
	public: \
		class_name##Factory(REFCLSID clsid, CRuntimeClass* pRuntimeClass, \
			BOOL bMultiInstance, LPCTSTR lpszProgID) : \
				COleObjectFactoryEx(clsid, pRuntimeClass, bMultiInstance, \
				lpszProgID) {} \
		virtual BOOL UpdateRegistry(BOOL);

#define END_OLEFACTORY(class_name) \
	}; \
	friend class class_name##Factory; \
	static AFX_DATA class_name##Factory factory; \
public: \
	static AFX_DATA const GUID guid; \
	virtual HRESULT GetClassID(LPCLSID pclsid);

#define DECLARE_OLECREATE_EX(class_name) \
	BEGIN_OLEFACTORY(class_name) \
	END_OLEFACTORY(class_name)

#define IMPLEMENT_OLECREATE_EX(class_name, external_name, \
			l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
	static const TCHAR _szProgID_##class_name[] = _T(external_name); \
	AFX_DATADEF class_name::class_name##Factory class_name::factory( \
		class_name::guid, RUNTIME_CLASS(class_name), FALSE, \
		_szProgID_##class_name); \
	const AFX_DATADEF GUID class_name::guid = \
		{ l, w1, w2, { b1, b2, b3, b4, b5, b6, b7, b8 } }; \
	HRESULT class_name::GetClassID(LPCLSID pclsid) \
		{ *pclsid = guid; return NOERROR; }

/////////////////////////////////////////////////////////////////////////////
// Macros for type name and misc status

#define DECLARE_OLECTLTYPE(class_name) \
	virtual UINT GetUserTypeNameID(); \
	virtual DWORD GetMiscStatus();

#define IMPLEMENT_OLECTLTYPE(class_name, idsUserTypeName, dwOleMisc) \
	UINT class_name::GetUserTypeNameID() { return idsUserTypeName; } \
	DWORD class_name::GetMiscStatus() { return dwOleMisc; }

/////////////////////////////////////////////////////////////////////////////
// Macros for property page IDs

#define DECLARE_PROPPAGEIDS(class_name) \
	protected: \
		virtual LPCLSID GetPropPageIDs(ULONG& cPropPages);

#define BEGIN_PROPPAGEIDS(class_name, count) \
	static CLSID _rgPropPageIDs_##class_name[count]; \
	static ULONG _cPropPages_##class_name = (ULONG)-1; \
	LPCLSID class_name::GetPropPageIDs(ULONG& cPropPages) { \
		if (_cPropPages_##class_name == (ULONG)-1) { \
			_cPropPages_##class_name = count; \
			LPCLSID pIDs = _rgPropPageIDs_##class_name; \
			ULONG iPageMax = count; \
			ULONG iPage = 0;

#define PROPPAGEID(clsid) \
			ASSERT(iPage < iPageMax); \
			if (iPage < iPageMax) \
				pIDs[iPage++] = clsid;

#define END_PROPPAGEIDS(class_name) \
			ASSERT(iPage == iPageMax); \
		} \
		cPropPages = _cPropPages_##class_name; \
		return _rgPropPageIDs_##class_name; }



//////////////////////////////////////////////////////////////////////////////////////////
//
// OLE COM macros (AFXDISP.H)

#define METHOD_PROLOGUE(theClass, localClass) \
	theClass* pThis = \
		((theClass*)((BYTE*)this - offsetof(theClass, m_x##localClass))); \
	AFX_MANAGE_STATE(pThis->m_pModuleState) \

#define METHOD_PROLOGUE_(theClass, localClass) \
	theClass* pThis = \
		((theClass*)((BYTE*)this - offsetof(theClass, m_x##localClass))); \

#define METHOD_PROLOGUE_EX(theClass, localClass) \
	METHOD_PROLOGUE(theClass, localClass) \

#define METHOD_PROLOGUE_EX_(theClass, localClass) \
	METHOD_PROLOGUE_(theClass, localClass) \

#define BEGIN_INTERFACE_PART(localClass, baseClass) \
	class X##localClass : public baseClass \
	{ \
	public: \
		STDMETHOD_(ULONG, AddRef)(); \
		STDMETHOD_(ULONG, Release)(); \
		STDMETHOD(QueryInterface)(REFIID iid, LPVOID* ppvObj); \

#define BEGIN_INTERFACE_PART_DERIVE(localClass, baseClass) \
	BEGIN_INTERFACE_PART(localClass, baseClass) \

#define INIT_INTERFACE_PART(theClass, localClass)
#define INIT_INTERFACE_PART_DERIVE(theClass, localClass)

#define END_INTERFACE_PART(localClass) \
	} m_x##localClass; \
	friend class X##localClass; \

#define BEGIN_INTERFACE_MAP(theClass, theBase) \
	const AFX_INTERFACEMAP* theClass::GetInterfaceMap() const \
		{ return &theClass::interfaceMap; } \
	const AFX_DATADEF AFX_INTERFACEMAP theClass::interfaceMap = \
		{ &theBase::interfaceMap, &theClass::_interfaceEntries[0], }; \
	const AFX_DATADEF AFX_INTERFACEMAP_ENTRY theClass::_interfaceEntries[] = \
	{ \

#define INTERFACE_PART(theClass, iid, localClass) \
		{ &iid, offsetof(theClass, m_x##localClass) }, \

#define INTERFACE_AGGREGATE(theClass, theAggr) \
		{ NULL, offsetof(theClass, theAggr) }, \

#define END_INTERFACE_MAP() \
		{ NULL, (size_t)-1 } \
	}; \

#define BEGIN_DISPATCH_MAP(theClass, baseClass) \
	const AFX_DISPMAP* theClass::GetDispatchMap() const \
		{ return &theClass::dispatchMap; } \
	const AFX_DISPMAP theClass::dispatchMap = \
		{ &baseClass::dispatchMap, &theClass::_dispatchEntries[0], \
			&theClass::_dispatchEntryCount, &theClass::_dwStockPropMask }; \
	UINT theClass::_dispatchEntryCount = (UINT)-1; \
	DWORD theClass::_dwStockPropMask = (DWORD)-1; \
	const AFX_DISPMAP_ENTRY theClass::_dispatchEntries[] = \
	{ \

#define END_DISPATCH_MAP() \
	{ VTS_NONE, DISPID_UNKNOWN, VTS_NONE, VT_VOID, \
		(AFX_PMSG)NULL, (AFX_PMSG)NULL, (size_t)-1, afxDispCustom } }; \

#define DISP_FUNCTION(theClass, szExternalName, pfnMember, vtRetVal, vtsParams) \
	{ _T(szExternalName), DISPID_UNKNOWN, vtsParams, vtRetVal, \
		(AFX_PMSG)(void (theClass::*)(void))&pfnMember, (AFX_PMSG)0, 0, \
		afxDispCustom }, \

#define DISP_PROPERTY(theClass, szExternalName, memberName, vtPropType) \
	{ _T(szExternalName), DISPID_UNKNOWN, NULL, vtPropType, (AFX_PMSG)0, (AFX_PMSG)0, \
		offsetof(theClass, memberName), afxDispCustom }, \

#define DISP_PROPERTY_NOTIFY(theClass, szExternalName, memberName, pfnAfterSet, vtPropType) \
	{ _T(szExternalName), DISPID_UNKNOWN, NULL, vtPropType, (AFX_PMSG)0, \
		(AFX_PMSG)(void (theClass::*)(void))&pfnAfterSet, \
		offsetof(theClass, memberName), afxDispCustom }, \

#define DISP_PROPERTY_EX(theClass, szExternalName, pfnGet, pfnSet, vtPropType) \
	{ _T(szExternalName), DISPID_UNKNOWN, NULL, vtPropType, \
		(AFX_PMSG)(void (theClass::*)(void))&pfnGet, \
		(AFX_PMSG)(void (theClass::*)(void))&pfnSet, 0, afxDispCustom }, \

#define DISP_PROPERTY_PARAM(theClass, szExternalName, pfnGet, pfnSet, vtPropType, vtsParams) \
	{ _T(szExternalName), DISPID_UNKNOWN, vtsParams, vtPropType, \
		(AFX_PMSG)(void (theClass::*)(void))&pfnGet, \
		(AFX_PMSG)(void (theClass::*)(void))&pfnSet, 0, afxDispCustom }, \

// these DISP_ macros allow the app to determine the DISPID
#define DISP_FUNCTION_ID(theClass, szExternalName, dispid, pfnMember, vtRetVal, vtsParams) \
	{ _T(szExternalName), dispid, vtsParams, vtRetVal, \
		(AFX_PMSG)(void (theClass::*)(void))&pfnMember, (AFX_PMSG)0, 0, \
		afxDispCustom }, \

#define DISP_PROPERTY_ID(theClass, szExternalName, dispid, memberName, vtPropType) \
	{ _T(szExternalName), dispid, NULL, vtPropType, (AFX_PMSG)0, (AFX_PMSG)0, \
		offsetof(theClass, memberName), afxDispCustom }, \

#define DISP_PROPERTY_NOTIFY_ID(theClass, szExternalName, dispid, memberName, pfnAfterSet, vtPropType) \
	{ _T(szExternalName), dispid, NULL, vtPropType, (AFX_PMSG)0, \
		(AFX_PMSG)(void (theClass::*)(void))&pfnAfterSet, \
		offsetof(theClass, memberName), afxDispCustom }, \

#define DISP_PROPERTY_EX_ID(theClass, szExternalName, dispid, pfnGet, pfnSet, vtPropType) \
	{ _T(szExternalName), dispid, NULL, vtPropType, \
		(AFX_PMSG)(void (theClass::*)(void))&pfnGet, \
		(AFX_PMSG)(void (theClass::*)(void))&pfnSet, 0, afxDispCustom }, \

#define DISP_PROPERTY_PARAM_ID(theClass, szExternalName, dispid, pfnGet, pfnSet, vtPropType, vtsParams) \
	{ _T(szExternalName), dispid, vtsParams, vtPropType, \
		(AFX_PMSG)(void (theClass::*)(void))&pfnGet, \
		(AFX_PMSG)(void (theClass::*)(void))&pfnSet, 0, afxDispCustom }, \

// the DISP_DEFVALUE is a special case macro that creates an alias for DISPID_VALUE
#define DISP_DEFVALUE(theClass, szExternalName) \
	{ _T(szExternalName), DISPID_UNKNOWN, NULL, VT_MFCVALUE, \
		(AFX_PMSG)0, (AFX_PMSG)0, 0, afxDispCustom }, \

#define DISP_DEFVALUE_ID(theClass, dispid) \
	{ NULL, dispid, NULL, VT_MFCVALUE, (AFX_PMSG)0, (AFX_PMSG)0, 0, \
		afxDispCustom }, \

/////////////////////////////////////////////////////////////////////////////
// Macros for creating "creatable" automation classes.

#define DECLARE_OLECREATE(class_name) \
public: \
	static AFX_DATA COleObjectFactory factory; \
	static AFX_DATA const GUID guid; \

#define IMPLEMENT_OLECREATE(class_name, external_name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
	AFX_DATADEF COleObjectFactory class_name::factory(class_name::guid, \
		RUNTIME_CLASS(class_name), FALSE, _T(external_name)); \
	const AFX_DATADEF GUID class_name::guid = \
		{ l, w1, w2, { b1, b2, b3, b4, b5, b6, b7, b8 } }; \

/////////////////////////////////////////////////////////////////////////////
// Connection maps

#define BEGIN_CONNECTION_PART(theClass, localClass) \
	class X##localClass : public CConnectionPoint \
	{ \
	public: \
		X##localClass() \
			{ m_nOffset = offsetof(theClass, m_x##localClass); }

#define CONNECTION_IID(iid) \
		REFIID GetIID() { return iid; }

#define END_CONNECTION_PART(localClass) \
	} m_x##localClass; \
	friend class X##localClass;

#define BEGIN_CONNECTION_MAP(theClass, theBase) \
	const AFX_CONNECTIONMAP* theClass::GetConnectionMap() const \
		{ return &theClass::connectionMap; } \
	const AFX_DATADEF AFX_CONNECTIONMAP theClass::connectionMap = \
		{ &(theBase::connectionMap), &theClass::_connectionEntries[0], }; \
	const AFX_DATADEF AFX_CONNECTIONMAP_ENTRY theClass::_connectionEntries[] = \
	{ \

#define CONNECTION_PART(theClass, iid, localClass) \
		{ &iid, offsetof(theClass, m_x##localClass) }, \

#define END_CONNECTION_MAP() \
		{ NULL, (size_t)-1 } \
	}; \

#define BEGIN_EVENTSINK_MAP(theClass, baseClass) \
	const AFX_EVENTSINKMAP* theClass::GetEventSinkMap() const \
		{ return &theClass::eventsinkMap; } \
	const AFX_EVENTSINKMAP theClass::eventsinkMap = \
		{ &baseClass::eventsinkMap, &theClass::_eventsinkEntries[0], \
			&theClass::_eventsinkEntryCount }; \
	UINT theClass::_eventsinkEntryCount = (UINT)-1; \
	const AFX_EVENTSINKMAP_ENTRY theClass::_eventsinkEntries[] = \
	{ \

#define END_EVENTSINK_MAP() \
	{ VTS_NONE, DISPID_UNKNOWN, VTS_NONE, VT_VOID, \
		(AFX_PMSG)NULL, (AFX_PMSG)NULL, (size_t)-1, afxDispCustom, \
		(UINT)-1, 0 } }; \

#define ON_EVENT(theClass, id, dispid, pfnHandler, vtsParams) \
	{ _T(""), dispid, vtsParams, VT_BOOL, \
		(AFX_PMSG)(void (theClass::*)(void))&pfnHandler, (AFX_PMSG)0, 0, \
		afxDispCustom, id, (UINT)-1 }, \

#define ON_EVENT_RANGE(theClass, idFirst, idLast, dispid, pfnHandler, vtsParams) \
	{ _T(""), dispid, vtsParams, VT_BOOL, \
		(AFX_PMSG)(void (theClass::*)(void))&pfnHandler, (AFX_PMSG)0, 0, \
		afxDispCustom, idFirst, idLast }, \

#define ON_PROPNOTIFY(theClass, id, dispid, pfnRequest, pfnChanged) \
	{ _T(""), dispid, VTS_NONE, VT_VOID, \
		(AFX_PMSG)(BOOL (CCmdTarget::*)(BOOL*))&pfnRequest, \
		(AFX_PMSG)(BOOL (CCmdTarget::*)(void))&pfnChanged, \
		1, afxDispCustom, id, (UINT)-1 }, \

#define ON_PROPNOTIFY_RANGE(theClass, idFirst, idLast, dispid, pfnRequest, pfnChanged) \
	{ _T(""), dispid, VTS_NONE, VT_VOID, \
		(AFX_PMSG)(BOOL (CCmdTarget::*)(UINT, BOOL*))&pfnRequest, \
		(AFX_PMSG)(BOOL (CCmdTarget::*)(UINT))&pfnChanged, \
		1, afxDispCustom, idFirst, idLast }, \

#define ON_DSCNOTIFY(theClass, id, pfnNotify) \
	{ _T(""), DISPID_UNKNOWN, VTS_NONE, VT_VOID, \
		(AFX_PMSG)(BOOL (CCmdTarget::*)(DSCSTATE, DSCREASON, BOOL*))&pfnNotify, (AFX_PMSG)0, \
		1, afxDispCustom, id, (UINT)-1 }, \

#define ON_DSCNOTIFY_RANGE(theClass, idFirst, idLast, pfnNotify) \
	{ _T(""), DISPID_UNKNOWN, VTS_NONE, VT_VOID, \
		(AFX_PMSG)(BOOL (CCmdTarget::*)(UINT, DSCSTATE, DSCREASON, BOOL*))&pfnNotify, (AFX_PMSG)0, \
		1, afxDispCustom, idFirst, idLast }, \

#define ON_EVENT_REFLECT(theClass, dispid, pfnHandler, vtsParams) \
	{ _T(""), dispid, vtsParams, VT_BOOL, \
		(AFX_PMSG)(void (theClass::*)(void))&pfnHandler, (AFX_PMSG)0, 0, \
		afxDispCustom, (UINT)-1, (UINT)-1 }, \

#define ON_PROPNOTIFY_REFLECT(theClass, dispid, pfnRequest, pfnChanged) \
	{ _T(""), dispid, VTS_NONE, VT_VOID, \
		(AFX_PMSG)(BOOL (CCmdTarget::*)(BOOL*))&pfnRequest, \
		(AFX_PMSG)(BOOL (CCmdTarget::*)(void))&pfnChanged, \
		1, afxDispCustom, (UINT)-1, (UINT)-1 }, \

/////////////////////////////////////////////////////////////////////////////
// Macros for type library information

#define DECLARE_OLETYPELIB(class_name) \
	protected: \
		virtual UINT GetTypeInfoCount(); \
		virtual HRESULT GetTypeLib(LCID, LPTYPELIB*); \
		virtual CTypeLibCache* GetTypeLibCache(); \

#define IMPLEMENT_OLETYPELIB(class_name, tlid, wVerMajor, wVerMinor) \
	UINT class_name::GetTypeInfoCount() \
		{ return 1; } \
	HRESULT class_name::GetTypeLib(LCID lcid, LPTYPELIB* ppTypeLib) \
		{ return ::LoadRegTypeLib(tlid, wVerMajor, wVerMinor, lcid, ppTypeLib); } \
	CTypeLibCache* class_name::GetTypeLibCache() \
		{ AFX_MANAGE_STATE(m_pModuleState); return AfxGetTypeLibCache(&tlid); } \


//////////////////////////////////////////////////////////////////////////////////////////
//
// MFC macros (AFXISAPI.H)



#define DECLARE_PARSE_MAP() \
private: \
	static AFX_PARSEMAP_ENTRY _parseEntries[]; \
public: \
	static const AFX_PARSEMAP parseMap; \
	static UINT PASCAL GetNumMapEntries(); \
	virtual const AFX_PARSEMAP* GetParseMap() const; \

#define BEGIN_PARSE_MAP(theClass, baseClass) \
	typedef void (AFX_PARSE_CALL theClass::*theClass##CALL)(CHttpServerContext*); \
	const AFX_PARSEMAP* theClass::GetParseMap() const \
		{ return &theClass::parseMap; } \
	AFXIS_DATADEF const AFX_PARSEMAP theClass::parseMap = \
		{ &theClass::GetNumMapEntries, &baseClass::parseMap, &theClass::_parseEntries[0] }; \
	AFX_PARSEMAP_ENTRY theClass::_parseEntries[] = \
	{ \

#define ON_PARSE_COMMAND(FnName, mapClass, Args) \
	{ _T(#FnName), (AFX_PISAPICMD) (mapClass##CALL)mapClass::FnName,\
		Args, NULL },

#define ON_PARSE_COMMAND_PARAMS(Params) \
	{ NULL, (AFX_PISAPICMD) NULL, Params, NULL },

#define DEFAULT_PARSE_COMMAND(FnName, mapClass) \
	{ _T(#FnName), (AFX_PISAPICMD) (mapClass##CALL)mapClass::FnName,\
		NULL, NULL },

#define END_PARSE_MAP(theClass) \
	}; \
	UINT PASCAL theClass::GetNumMapEntries() { \
		return sizeof(theClass::_parseEntries) /\
		sizeof(AFX_PARSEMAP_ENTRY); } \


//////////////////////////////////////////////////////////////////////////////////////////
//
// MFC macros (BASETYPS.H)

#define EXTERN_C    extern "C"

#define STDMETHODCALLTYPE       __stdcall
#define STDMETHODVCALLTYPE      __cdecl

#define STDAPICALLTYPE          __stdcall
#define STDAPIVCALLTYPE         __cdecl

#define STDAPI                  EXTERN_C HRESULT STDAPICALLTYPE
#define STDAPI_(type)           EXTERN_C type STDAPICALLTYPE

#define STDMETHODIMP            HRESULT STDMETHODCALLTYPE
#define STDMETHODIMP_(type)     type STDMETHODCALLTYPE

#define STDAPIV                 EXTERN_C HRESULT STDAPIVCALLTYPE
#define STDAPIV_(type)          EXTERN_C type STDAPIVCALLTYPE

#define STDMETHODIMPV           HRESULT STDMETHODVCALLTYPE
#define STDMETHODIMPV_(type)    type STDMETHODVCALLTYPE

#define interface struct
#define STDMETHOD(method)       virtual HRESULT STDMETHODCALLTYPE method
#define STDMETHOD_(type,method) virtual type STDMETHODCALLTYPE method
#define PURE                    = 0
#define THIS_
#define THIS                    void
#define DECLARE_INTERFACE(iface)    interface iface
#define DECLARE_INTERFACE_(iface, baseiface)    interface iface : public baseiface

#define DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    EXTERN_C const GUID FAR name

#define DEFINE_OLEGUID(name, l, w1, w2) \
    DEFINE_GUID(name, l, w1, w2, 0xC0,0,0,0,0,0,0,0x46)

#define BEGIN_INTERFACE
#define END_INTERFACE


//////////////////////////////////////////////////////////////////////////////////////////
//
// OWL macros (OCBOCOLE.H)

#define DEFINE_BOLEGUID(name, l) EXTERN_C const GUID CDECL FAR name


//////////////////////////////////////////////////////////////////////////////////////////
//
// MFC macros (AFXTLS_.H)

#define THREAD_LOCAL(class_name, ident_name) \
	AFX_DATADEF CThreadLocal<class_name> ident_name;
#define EXTERN_THREAD_LOCAL(class_name, ident_name) \
	extern AFX_DATA THREAD_LOCAL(class_name, ident_name)

#define PROCESS_LOCAL(class_name, ident_name) \
	AFX_DATADEF CProcessLocal<class_name> ident_name;
#define EXTERN_PROCESS_LOCAL(class_name, ident_name) \
	extern AFX_DATA PROCESS_LOCAL(class_name, ident_name)


//////////////////////////////////////////////////////////////////////////////////////////
//
// MFC macros (AFXMOD.H)

#define DECLARE_OLEMISC_FLAGS(flags)  static DWORD   _GetObjectMiscStatus(){ return flags; }
#define DECLARE_PROGID(progid)        static LPCTSTR GetProgID()           { return progid;}
#define DECLARE_DESCRIPTION(desc)     static LPCTSTR GetDescription()      { return desc;  }
#define DECLARE_THREADING_MODEL(thrd) static ObjectThreadingModel GetThreadModel() { return thrd; }

#define BEGIN_VERB_MAP()            \
static const OLEVERB* _GetVerbs()   \
{                                   \
  static const OLEVERB  _verbs[]=   \
  {

#define VERB_ENTRY_EX(verbId, verbString, mfFlags, verbAttrib)  \
      {verbId, verbString, mfFlags, verbAttrib },

#define VERB_ENTRY(verbId, verbString)  \
         VERB_ENTRY_EX(verbId, verbString, 0 /* No MF_xxxx Flags */, OLEVERBATTRIB_ONCONTAINERMENU)

#define END_VERB_MAP()  \
      { 0, 0, 0, 0 }    \
  };                    \
  return _verbs;        \
};


// Automation Object Macros
//
#define  AUTOOBJECT_COM_INTERFACE_ENTRIES(intf) \
      COM_INTERFACE_ENTRY(intf)                 \
      COM_INTERFACE_ENTRY2(IDispatch, intf)

// Entry of the default dispatch interface of a COCLASS
//
#define   COM_INTERFACE_ENTRY_OF_DEF_DISPINTF(intf) \
      COM_INTERFACE_ENTRY(intf)                     \
      COM_INTERFACE_ENTRY2(IDispatch, intf)

// Macro defining base class of an Automation Object implementation class.
// NOTE: This macro uses the ambient Threading model setting
//
#define AUTOOBJECT_IMPL(cppClass, CoClass, intf)  \
  public CComObjectRootEx<CComObjectThreadModel>, \
  public CComCoClass<cppClass, &CLSID_##CoClass>, \
  public IDispatchImpl<intf, &IID_##intf, LIBID_OF_##CoClass >


// The DUALINTERFACE macros are inserted by the CodeManager if you insert additional interfaces
// to your CoClass.
//
#define DUALINTERFACE_IMPL(coclass, intf) \
   public IDispatchImpl<intf, &IID_##intf, LIBID_OF_##coclass>

#define DUALINTERFACE_ENTRY(i) \
   COM_INTERFACE_ENTRY(i)


#define UPDATE_REGISTRY_METHOD(code) \
static HRESULT WINAPI UpdateRegistry(BOOL bRegister)  \
{ \
  HRESULT hres; \
  try \
  { \
   code \
  } \
  catch (Exception& e) \
  { \
   hres = Error(e.Message.c_str()); \
  } \
  return hres; \
}

#define DECLARE_COMSERVER_REGISTRY(progid, desc)                \
   UPDATE_REGISTRY_METHOD(                                      \
      TComServerRegistrar CSR(GetObjectCLSID(), progid, desc);  \
      hres = CSR.UpdateRegistry(bRegister);)

#define DECLARE_TYPED_COMSERVER_REGISTRY(progid)                \
   UPDATE_REGISTRY_METHOD(                                      \
    TTypedComServerRegistrar TCSR(GetObjectCLSID(), progid);    \
    hres = TCSR.UpdateRegistry(bRegister);)


//////////////////////////////////////////////////////////////////////////////////////////
//
// ATL/VCL macros (ATLVCL.H)

// VCL_CONTROL_COM_INTERFACE_ENTRIES
//
//  This macro defines the entries of the required Interfaces for exposing a VCL
//  component as an ActiveX Control
//
#define  VCL_CONTROL_COM_INTERFACE_ENTRIES(intf)  \
  COM_INTERFACE_ENTRY_IMPL(IViewObjectEx) \
  COM_INTERFACE_ENTRY_IMPL_IID(IID_IViewObject2, IViewObjectEx) \
  COM_INTERFACE_ENTRY_IMPL_IID(IID_IViewObject, IViewObjectEx) \
  COM_INTERFACE_ENTRY_IMPL_IID(IID_IOleInPlaceObject, IOleInPlaceObjectWindowless) \
  COM_INTERFACE_ENTRY_IMPL_IID(IID_IOleWindow, IOleInPlaceObjectWindowless) \
  COM_INTERFACE_ENTRY_IMPL(IOleInPlaceActiveObject) \
  COM_INTERFACE_ENTRY_IMPL(IOleControl) \
  COM_INTERFACE_ENTRY_IMPL(IOleObject) \
  COM_INTERFACE_ENTRY_IMPL(IQuickActivate) \
  COM_INTERFACE_ENTRY_IMPL(IPersistStorage) \
  COM_INTERFACE_ENTRY_IMPL(IPersistStreamInit) \
  COM_INTERFACE_ENTRY_IMPL(ISpecifyPropertyPages) \
  COM_INTERFACE_ENTRY_IMPL(IDataObject) \
  COM_INTERFACE_ENTRY_IMPL(ISimpleFrameSite) \
  COM_INTERFACE_ENTRY(IProvideClassInfo) \
  COM_INTERFACE_ENTRY(IProvideClassInfo2) \
  COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer) \
  COM_INTERFACE_ENTRY(ISupportErrorInfo) \
  COM_INTERFACE_ENTRY(intf) \
  COM_INTERFACE_ENTRY2(IDispatch, intf)

// VCLCONTROL_IMPL
// This macro is used to encapsulate the various base classes an ActiveX VCL Control derives from.
//
#define VCLCONTROL_IMPL(cppClass, CoClass, VclClass, intf, EventID) \
   public TVclControlImpl<cppClass, VclClass, &CLSID_##CoClass, &IID_##intf, &EventID, LIBID_OF_##CoClass>,\
   public IDispatchImpl<intf, &IID_##intf, LIBID_OF_##CoClass>, \
   public TEvents_##CoClass<cppClass>

// COM_MAP entry for VCL-based Property Page
//
#define  PROPERTYPAGE_COM_INTERFACE_ENTRIES                         \
     COM_INTERFACE_ENTRY(IUnknown)                                  \
     COM_INTERFACE_ENTRY_AGGREGATE(IID_IPropertyPage, m_InnerUnk)   \
     COM_INTERFACE_ENTRY_AGGREGATE(IID_IPropertyPage2, m_InnerUnk)

// Base classes of RemoteDataModule
//
#define REMOTEDATAMODULE_IMPL(cppClass, CoClass, VclClass, intf) \
  public CComObjectRootEx<CComObjectThreadModel>,                \
  public CComCoClass<cppClass, &CLSID_##CoClass>,                \
  public IDataBrokerImpl<VclClass, cppClass, intf, &IID_##intf, LIBID_OF_##CoClass>

// Base class of VCL-based Property Page 
//
#define PROPERTYPAGE_IMPL(cppClass, CoClass, VclClass) \
   public TVCLPropertyPage<cppClass, &CLSID_##CoClass, VclClass>

#define DECLARE_REMOTEDATAMODULE_REGISTRY(progid)                        \
  UPDATE_REGISTRY_METHOD(                                                \
     TRemoteDataModuleRegistrar RDMRegistrar(GetObjectCLSID(), progid);  \
     hres = RDMRegistrar.UpdateRegistry(bRegister);)

#define DECLARE_ACTIVEXCONTROL_REGISTRY(progid, idbmp) \
  UPDATE_REGISTRY_METHOD( \
     TAxControlRegistrar AXCR(GetObjectCLSID(), progid, idbmp, _GetObjectMiscStatus(), _GetVerbs()); \
     hres = AXCR.UpdateRegistry(bRegister);)


//////////////////////////////////////////////////////////////////////////////////////////
//
// ATL macros (ATLCTL.H)

#define BEGIN_PROPERTY_MAP(theClass) \
  typedef _ATL_PROP_NOTIFY_EVENT_CLASS __ATL_PROP_NOTIFY_EVENT_CLASS; \
  static ATL_PROPMAP_ENTRY* GetPropertyMap()\
  {\
    static ATL_PROPMAP_ENTRY pPropMap[] = \
    {

#define PROP_ENTRY(szDesc, dispid, clsid) \
    {OLESTR(szDesc), dispid, &clsid, &IID_IDispatch},

#define PROP_ENTRY_EX(szDesc, dispid, clsid, iidDispatch) \
    {OLESTR(szDesc), dispid, &clsid, &iidDispatch},

#define PROP_PAGE(clsid) \
    {NULL, NULL, &clsid, &IID_NULL},

#define END_PROPERTY_MAP() \
      {NULL, 0, NULL, &IID_NULL} \
    }; \
    return pPropMap; \
  }

#define IMPLEMENT_STOCKPROP(type, fname, pname, dispid) \
  HRESULT STDMETHODCALLTYPE put_##fname(type pname) \
  { \
    T* pT = (T*) this; \
    if (pT->FireOnRequestEdit(dispid) == S_FALSE) \
      return S_FALSE; \
    pT->m_##pname = pname; \
    pT->m_bRequiresSave = TRUE; \
    pT->FireOnChanged(dispid); \
    pT->FireViewChange(); \
    return S_OK; \
  } \
  HRESULT STDMETHODCALLTYPE get_##fname(type* p##pname) \
  { \
    T* pT = (T*) this; \
    *p##pname = pT->m_##pname; \
    return S_OK; \
  }

#define IMPLEMENT_BOOL_STOCKPROP(fname, pname, dispid) \
  HRESULT STDMETHODCALLTYPE put_##fname(VARIANT_BOOL pname) \
  { \
    T* pT = (T*) this; \
    if (pT->FireOnRequestEdit(dispid) == S_FALSE) \
      return S_FALSE; \
    pT->m_##pname = pname; \
    pT->m_bRequiresSave = TRUE; \
    pT->FireOnChanged(dispid); \
    pT->FireViewChange(); \
    return S_OK; \
  } \
  HRESULT STDMETHODCALLTYPE get_##fname(VARIANT_BOOL* p##pname) \
  { \
    T* pT = (T*) this; \
    *p##pname = pT->m_##pname ? VARIANT_TRUE : VARIANT_FALSE; \
    return S_OK; \
  }

#define IMPLEMENT_BSTR_STOCKPROP(fname, pname, dispid) \
  HRESULT STDMETHODCALLTYPE put_##fname(BSTR pname) \
  { \
    T* pT = (T*) this; \
    if (pT->FireOnRequestEdit(dispid) == S_FALSE) \
      return S_FALSE; \
    *(&(pT->m_##pname)) = SysAllocString(pname); \
    pT->m_bRequiresSave = TRUE; \
    pT->FireOnChanged(dispid); \
    pT->FireViewChange(); \
    return S_OK; \
  } \
  HRESULT STDMETHODCALLTYPE get_##fname(BSTR* p##pname) \
  { \
    T* pT = (T*) this; \
    *p##pname = SysAllocString(pT->m_##pname); \
    return S_OK; \
  }

//////////////////////////////////////////////////////////////////////////////////////////
//
// MFC macros (WINNT.H)

#define DECLARE_HANDLE(name) typedef HANDLE name
#define DECLARE_HANDLE32    DECLARE_HANDLE


#define //MFC Global Declaration Level
#define DECLARE_DIAG_GROUP(g, qual) TDiagGroup & qual _GetDiagGroup##g()

//////////////////////////////////////////////////////////////////////////////////////////
//
// MAPI macros (MAPIDEFS.H)

#define STDINITMETHODIMP        HRESULT STDMAPIINITCALLTYPE
#define STDINITMETHODIMP_(type) type STDMAPIINITCALLTYPE

#define DECLARE_MAPI_INTERFACE(iface)                                   \
        DECLARE_INTERFACE(iface)
#define DECLARE_MAPI_INTERFACE_(iface, baseiface)                       \
        DECLARE_INTERFACE_(iface, baseiface)

#define DECLARE_MAPI_INTERFACE_PTR(iface, piface)                       \
        interface iface; typedef iface FAR * piface

#define MAPIMETHOD(method)              MAPIMETHOD_(HRESULT, method)
#define MAPIMETHOD_(type, method)       STDMETHOD_(type, method)
#define MAPIMETHOD_DECLARE(type, method, prefix) \
        STDMETHODIMP_(type) prefix##method
#define MAPIMETHOD_TYPEDEF(type, method, prefix) \
        typedef type (STDMETHODCALLTYPE prefix##method##_METHOD)


//////////////////////////////////////////////////////////////////////////////////////////
//
// Telephony  API (TAPI.H)

#define DECLARE_OPAQUE32(name)  struct name##__ { int unused; }; \
                typedef const struct name##__ FAR* name

//////////////////////////////////////////////////////////////////////////////////////////
//
// OWL (EVENTHAN.H)

//
// Macros to declare a response table
//
#define DECLARE_RESPONSE_TABLE(cls)\
  private:\
    static TResponseTableEntry< cls > __RTFAR __entries[];\
    typedef TResponseTableEntry< cls >::PMF   TMyPMF;\
    typedef cls                               TMyClass;\
  public:\
    bool  Find(TEventInfo&, TEqualOperator = 0)

#define END_RESPONSE_TABLE\
  {0, 0, 0, 0}}

#define DEFINE_RESPONSE_TABLE_ENTRIES(cls)\
  TResponseTableEntry< cls > __RTFAR  cls::__entries[] = {

//
// Macro to define a response table for a class with no base response tables
//
// Use it like this:
//    DEFINE_RESPONSE_TABLE(cls)
//      EV_WM_PAINT,
//      EV_WM_LBUTTONDOWN,
//    END_RESPONSE_TABLE;
//
#define DEFINE_RESPONSE_TABLE(cls)\
  bool  cls::Find(TEventInfo& eventInfo, TEqualOperator equal)\
      {eventInfo.Object = (GENERIC*)this;\
       return SearchEntries((TGenericTableEntry __RTFAR*)__entries, eventInfo, equal);}\
  DEFINE_RESPONSE_TABLE_ENTRIES(cls)

//
// Macro to define a response table for a class with one base. Use this macro
// exactly like macro DEFINE_RESPONSE_TABLE
//
#define DEFINE_RESPONSE_TABLE1(cls, base)\
  bool  cls::Find(TEventInfo& eventInfo, TEqualOperator equal)\
      {eventInfo.Object = (GENERIC*)this;\
       return SearchEntries((TGenericTableEntry __RTFAR*)__entries, eventInfo, equal) ||\
              base::Find(eventInfo, equal);}\
  DEFINE_RESPONSE_TABLE_ENTRIES(cls)

//
// Macro to define a response table for a class with two bases. Use this macro
// exactly like macro DEFINE_RESPONSE_TABLE
//
#define DEFINE_RESPONSE_TABLE2(cls, base1, base2)\
  bool  cls::Find(TEventInfo& eventInfo, TEqualOperator equal)\
      {eventInfo.Object = (GENERIC*)this;\
       return SearchEntries((TGenericTableEntry __RTFAR*)__entries, eventInfo, equal) ||\
              base1::Find(eventInfo, equal) ||\
              base2::Find(eventInfo, equal);}\
  DEFINE_RESPONSE_TABLE_ENTRIES(cls)

//
// Macro to define a response table for a class with three bases. Use this macro
// exactly like macro DEFINE_RESPONSE_TABLE
//
#define DEFINE_RESPONSE_TABLE3(cls, base1, base2, base3)\
  bool  cls::Find(TEventInfo& eventInfo, TEqualOperator equal)\
      {eventInfo.Object = (GENERIC*)this;\
       return SearchEntries((TGenericTableEntry __RTFAR*)__entries, eventInfo, equal) ||\
              base1::Find(eventInfo, equal) ||\
              base2::Find(eventInfo, equal) ||\
              base3::Find(eventInfo, equal);}\
  DEFINE_RESPONSE_TABLE_ENTRIES(cls)


//////////////////////////////////////////////////////////////////////////////////////////
//
// Video for Windows (VFW.H)

#define DEFINE_AVIGUID(name, l, w1, w2) \
    DEFINE_GUID(name, l, w1, w2, 0xC0,0,0,0,0,0,0,0x46)

//////////////////////////////////////////////////////////////////////////////////////////
//
// DAO macros (ADOID.H)


#define DEFINE_DAOGUID(name, l) \
    DEFINE_GUID(name, l, 0, 0x10, 0x80,0,0,0xAA,0,0x6D,0x2E,0xA4)

//////////////////////////////////////////////////////////////////////////////////////////
//
// Exchange (EXCHEXT.H)

#define DEFINE_EXCHEXTGUID(name, b) \
    DEFINE_GUID(name, 0x00020D00 | (b), 0, 0, 0xC0,0,0,0,0,0,0,0x46)

//////////////////////////////////////////////////////////////////////////////////////////
//
// Exchange (EXCHFORM.H)

#define DEFINE_EXCHFORMGUID(name, b) \
    DEFINE_GUID(name, 0x00020D00 | (b), 0, 0, 0xC0,0,0,0,0,0,0,0x46)

//////////////////////////////////////////////////////////////////////////////////////////
//
// MAPI (INITOID.H)

#define DEFINE_OID_1(name, b0, b1) \
    EXTERN_C const BYTE __based(__segname("_CODE")) name[] =\
        { MAPI_PREFIX, b0, b1 }

#define DEFINE_OID_2(name, b0, b1, b2) \
    EXTERN_C const BYTE __based(__segname("_CODE")) name[] =\
        { MAPI_PREFIX, b0, b1, b2 }

#define DEFINE_OID_3(name, b0, b1, b2, b3) \
    EXTERN_C const BYTE __based(__segname("_CODE")) name[] =\
        { MAPI_PREFIX, b0, b1, b2, b3 }

#define DEFINE_OID_4(name, b0, b1, b2, b3, b4) \
    EXTERN_C const BYTE __based(__segname("_CODE")) name[] =\
        { MAPI_PREFIX, b0, b1, b2, b3, b4 }


//////////////////////////////////////////////////////////////////////////////////////////
//
// Shell (SHL*.H)

#define SHSTDAPI          STDAPI
#define SHSTDAPI_(type)   STDAPI_(type)
#define SHDOCAPI          STDAPI
#define SHDOCAPI_(type)   STDAPI_(type)
#define LWSTDAPI          STDAPI
#define LWSTDAPI_(type)   STDAPI_(type)

#define DEFINE_SHLGUID(name, l, w1, w2) DEFINE_GUID(name, l, w1, w2, 0xC0,0,0,0,0,0,0,0x46)


//////////////////////////////////////////////////////////////////////////////////////////
//
// D3D (D3DRMOBJ.H)


/*
 * The methods for IUnknown
 */
#define IUNKNOWN_METHODS(kind) \
    STDMETHOD(QueryInterface)	   	(THIS_ REFIID riid, LPVOID *ppvObj) kind; \
    STDMETHOD_(ULONG, AddRef)	   	(THIS) kind; \
    STDMETHOD_(ULONG, Release)	   	(THIS) kind

/*
 * The methods for IDirect3DRMObject
 */
#define IDIRECT3DRMOBJECT_METHODS(kind) \
    STDMETHOD(Clone)			(THIS_ LPUNKNOWN pUnkOuter, REFIID riid, LPVOID *ppvObj) kind; \
    STDMETHOD(AddDestroyCallback)  	(THIS_ D3DRMOBJECTCALLBACK, LPVOID argument) kind; \
    STDMETHOD(DeleteDestroyCallback)	(THIS_ D3DRMOBJECTCALLBACK, LPVOID argument) kind; \
    STDMETHOD(SetAppData)	   	(THIS_ DWORD data) kind; \
    STDMETHOD_(DWORD, GetAppData)  	(THIS) kind; \
    STDMETHOD(SetName)		   	(THIS_ LPCSTR) kind; \
    STDMETHOD(GetName)			(THIS_ LPDWORD lpdwSize, LPSTR lpName) kind; \
    STDMETHOD(GetClassName)		(THIS_ LPDWORD lpdwSize, LPSTR lpName) kind


#define WIN_TYPES(itype, ptype) \
    typedef interface itype FAR *LP##ptype, FAR **LPLP##ptype

