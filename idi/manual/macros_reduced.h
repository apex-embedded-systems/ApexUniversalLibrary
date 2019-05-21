// This file contains reduced MCF, ATL and VCL macros to use
// in Doc-O-Matic's macro expansion mechanism.
//
// The macros have been either reduced so that they expand to
// empty or to a minimal representation which is more appropriate
// than the "full" version. Additionally, reducing macros
// to the absolute minimum necessary can increase Doc-O-Matic's
// parser speed.
//
// Note that Doc-O-Matic does not recursively expand macros.
// The macros in this file have been pre-expanded where it was
// necessary. Some macros (like STDMETHODCALLTYPE) have not been
// expanded because from the documentation reader's point of
// view it's more important to see what actually is in the code
// rather than what the pre-processor generates from the code
// whenever this is possible.


//////////////////////////////////////////////////////////////////////////////////////////
//
// Borland Macros

#define BEGIN_MESSAGE_MAP   virtual void __fastcall Dispatch(void *Message);
#define END_MESSAGE_MAP(base)

#define VCL_MESSAGE_HANDLER(msg,type,meth)

//////////////////////////////////////////////////////////////////////////////////////////
//
// Borland Macros (SYSCLASS.H)


#define USEDATAMODULE(FileName, DataModuleName)

#define USEDATAMODULENS(FileName, UnitName, DataModuleName)

#define USEFORM(FileName, FormName)

#define USEFORMRES(FileName, FormName, AncestorName)


#define USEFORMNS(FileName, UnitName, FormName)

#define USEUNIT(ModName)

#define USEOBJ(FileName)

#define USERC(FileName)

#define USEASM(FileName)

#define USEDEF(FileName)

#define USERES(FileName)

#define USETLB(FileName)

#define USELIB(FileName)

#define USEFILE(FileName)

#define USEPACKAGE(FileName)

#define USEIDL(FileName)

#define DECLARE_DINTERFACE_TYPE(iface) typedef System::DelphiInterface< iface >  _di_##iface;

//////////////////////////////////////////////////////////////////////////////////////////
//
// ATL macros (ATLWIN.H)

#define BEGIN_MSG_MAP(theClass) \
public: \
	BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0);

#define ALT_MSG_MAP(msgMapID)


#define MESSAGE_HANDLER(msg, func)

#define MESSAGE_RANGE_HANDLER(msgFirst, msgLast, func)


#define COMMAND_HANDLER(id, code, func)

#define COMMAND_ID_HANDLER(id, func)

#define COMMAND_CODE_HANDLER(code, func)

#define COMMAND_RANGE_HANDLER(idFirst, idLast, func)

#define NOTIFY_HANDLER(id, cd, func)

#define NOTIFY_ID_HANDLER(id, func)

#define NOTIFY_CODE_HANDLER(cd, func)

#define NOTIFY_RANGE_HANDLER(idFirst, idLast, func)

#define CHAIN_MSG_MAP(theChainClass)

#define CHAIN_MSG_MAP_MEMBER(theChainMember)

#define CHAIN_MSG_MAP_ALT(theChainClass, msgMapID)

#define CHAIN_MSG_MAP_ALT_MEMBER(theChainMember, msgMapID)

#define CHAIN_MSG_MAP_DYNAMIC(dynaChainID)

#define CHAIN_MSG_MAP_ALT_DYNAMIC(dynaChainID, msgMapID)

#define END_MSG_MAP()

#define DECLARE_WND_CLASS(WndClassName)

#define DECLARE_WND_SUPERCLASS(WndClassName, OrigWndClassName)

//////////////////////////////////////////////////////////////////////////////////////////
//
// MFC macros (AFXWIN.H)


#define DECLARE_MESSAGE_MAP()
#define BEGIN_MESSAGE_MAP(theClass, baseClass)
#define END_MESSAGE_MAP()
#define DECLARE_INTERFACE_MAP()
#define DECLARE_DISPATCH_MAP()
#define DECLARE_OLECMD_MAP()
#define BEGIN_OLECMD_MAP(theClass, baseClass)
#define END_OLECMD_MAP()
#define DECLARE_EVENTSINK_MAP()
#define DECLARE_CONNECTION_MAP()

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

#define ASSERT_KINDOF(class_name, object)

#define DECLARE_DYNCREATE(class_name) \
	static const AFX_DATA CRuntimeClass class##class_name; \
	virtual CRuntimeClass* GetRuntimeClass() const; \
	static CObject* PASCAL CreateObject();

#define _DECLARE_DYNCREATE(class_name) \
	static AFX_DATA CRuntimeClass class##class_name; \
	virtual CRuntimeClass* GetRuntimeClass() const; \
	static CObject* PASCAL CreateObject();

#define DECLARE_SERIAL(class_name) \
	static AFX_DATA CRuntimeClass class##class_name; \
	virtual CRuntimeClass* GetRuntimeClass() const; \
	static CObject* PASCAL CreateObject(); \
	friend CArchive& AFXAPI operator>>(CArchive& ar, class_name* &pOb);

#define IMPLEMENT_RUNTIMECLASS(class_name, base_class_name, wSchema, pfnNew)

#define _IMPLEMENT_RUNTIMECLASS(class_name, base_class_name, wSchema, pfnNew)

#define IMPLEMENT_DYNAMIC(class_name, base_class_name)
#define IMPLEMENT_DYNCREATE(class_name, base_class_name)

#define IMPLEMENT_SERIAL(class_name, base_class_name, wSchema)


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
	static HRESULT WINAPI _Cache(void* pv, REFIID iid, void** ppvObject, DWORD dw); \
	IUnknown* GetUnknown(); \
	HRESULT _InternalQueryInterface(REFIID iid, void** ppvObject); \
	const static _ATL_INTMAP_ENTRY* WINAPI _GetEntries();

#define DECLARE_GET_CONTROLLING_UNKNOWN() public:\
	virtual IUnknown* GetControllingUnknown();

#define COM_INTERFACE_ENTRY_BREAK(x)

#define COM_INTERFACE_ENTRY_NOINTERFACE(x)

#define COM_INTERFACE_ENTRY(x)

#define COM_INTERFACE_ENTRY_IID(iid, x)

#define COM_INTERFACE_ENTRY_IMPL(x)

#define COM_INTERFACE_ENTRY_IMPL_IID(iid, x)

#define COM_INTERFACE_ENTRY2(x, x2)

#define COM_INTERFACE_ENTRY2_IID(iid, x, x2)

#define COM_INTERFACE_ENTRY_FUNC(iid, dw, func)

#define COM_INTERFACE_ENTRY_FUNC_BLIND(dw, func)

#define COM_INTERFACE_ENTRY_TEAR_OFF(iid, x)

#define COM_INTERFACE_ENTRY_CACHED_TEAR_OFF(iid, x, punk)

#define COM_INTERFACE_ENTRY_AGGREGATE(iid, punk)

#define COM_INTERFACE_ENTRY_AGGREGATE_BLIND(punk)

#define COM_INTERFACE_ENTRY_AUTOAGGREGATE(iid, punk, clsid)

#define COM_INTERFACE_ENTRY_AUTOAGGREGATE_BLIND(punk, clsid)

#define COM_INTERFACE_ENTRY_CHAIN(classname)

#define END_COM_MAP()

#define BEGIN_OBJECT_MAP(x)
#define END_OBJECT_MAP()
#define OBJECT_ENTRY(clsid, class)

#define DECLARE_CLASSFACTORY_EX(cf) typedef CComCreator< CComObjectNoLock< cf > > _ClassFactoryCreatorClass;
#define DECLARE_CLASSFACTORY() typedef CComCreator< CComObjectNoLock< CComClassFactory > > _ClassFactoryCreatorClass;
#define DECLARE_CLASSFACTORY2(lic) typedef CComCreator< CComObjectNoLock< CComClassFactory2<lic> > > _ClassFactoryCreatorClass;
#define DECLARE_CLASSFACTORY_AUTO_THREAD() typedef CComCreator< CComObjectNoLock< CComClassFactoryAutoThread > > _ClassFactoryCreatorClass;
#define DECLARE_CLASSFACTORY_SINGLETON(obj) typedef CComCreator< CComObjectNoLock< CComClassFactorySingleton<obj> > > _ClassFactoryCreatorClass; 

#define DECLARE_OBJECT_DESCRIPTION(x)

#define DECLARE_NO_REGISTRY()\
	static HRESULT WINAPI UpdateRegistry(BOOL /*bRegister*/);

#define DECLARE_REGISTRY(class, pid, vpid, nid, flags)\
	static HRESULT WINAPI UpdateRegistry(BOOL bRegister);

#define DECLARE_REGISTRY_RESOURCE(x)\
	static HRESULT WINAPI UpdateRegistry(BOOL bRegister);

#define DECLARE_REGISTRY_RESOURCEID(x)\
	static HRESULT WINAPI UpdateRegistry(BOOL bRegister);

#define BEGIN_CONNECTION_POINT_MAP(x)\
	typedef x _atl_conn_classtype;\
	static const _ATL_CONNMAP_ENTRY* GetConnMap(int* pnEntries);
    
#define CONNECTION_POINT_ENTRY(iid)

#define END_CONNECTION_POINT_MAP()



//////////////////////////////////////////////////////////////////////////////////////////
//
// MFC macros (AFXCTL.H)

#define DECLARE_EVENT_MAP()

#define BEGIN_EVENT_MAP(theClass, baseClass)
#define END_EVENT_MAP()

#define EVENT_CUSTOM(pszName, pfnFire, vtsParams)

#define EVENT_CUSTOM_ID(pszName, dispid, pfnFire, vtsParams)

#define EVENT_PARAM(vtsParams) 

/////////////////////////////////////////////////////////////////////////////
// Stock events

#define EVENT_STOCK_CLICK()

#define EVENT_STOCK_DBLCLICK()

#define EVENT_STOCK_KEYDOWN()

#define EVENT_STOCK_KEYPRESS()

#define EVENT_STOCK_KEYUP()

#define EVENT_STOCK_MOUSEDOWN()

#define EVENT_STOCK_MOUSEMOVE()

#define EVENT_STOCK_MOUSEUP()

#define EVENT_STOCK_ERROREVENT()

#define EVENT_STOCK_READYSTATECHANGE()

#define DISP_PROPERTY_STOCK(theClass, szExternalName, dispid, pfnGet, pfnSet, vtPropType)

#define DISP_STOCKPROP_APPEARANCE()

#define DISP_STOCKPROP_BACKCOLOR()

#define DISP_STOCKPROP_BORDERSTYLE()

#define DISP_STOCKPROP_CAPTION()

#define DISP_STOCKPROP_ENABLED()

#define DISP_STOCKPROP_FONT()

#define DISP_STOCKPROP_FORECOLOR()

#define DISP_STOCKPROP_HWND()

#define DISP_STOCKPROP_TEXT()

#define DISP_STOCKPROP_READYSTATE()

/////////////////////////////////////////////////////////////////////////////
// Stock methods

#define DISP_FUNCTION_STOCK(theClass, szExternalName, dispid, pfnMember, vtRetVal, vtsParams)

#define DISP_STOCKFUNC_REFRESH()

#define DISP_STOCKFUNC_DOCLICK()

/////////////////////////////////////////////////////////////////////////////
// Macros for object factory and class ID

#define BEGIN_OLEFACTORY(class_name)

#define END_OLEFACTORY(class_name)

#define DECLARE_OLECREATE_EX(class_name)

#define IMPLEMENT_OLECREATE_EX(class_name, external_name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8)

/////////////////////////////////////////////////////////////////////////////
// Macros for type name and misc status

#define DECLARE_OLECTLTYPE(class_name)

#define IMPLEMENT_OLECTLTYPE(class_name, idsUserTypeName, dwOleMisc)

/////////////////////////////////////////////////////////////////////////////
// Macros for property page IDs

#define DECLARE_PROPPAGEIDS(class_name)

#define BEGIN_PROPPAGEIDS(class_name, count)

#define PROPPAGEID(clsid)

#define END_PROPPAGEIDS(class_name)

//////////////////////////////////////////////////////////////////////////////////////////
//
// OLE COM macros (AFXDISP.H)

#define METHOD_PROLOGUE(theClass, localClass)

#define METHOD_PROLOGUE_(theClass, localClass)

#define METHOD_PROLOGUE_EX(theClass, localClass)

#define METHOD_PROLOGUE_EX_(theClass, localClass)

#define BEGIN_INTERFACE_PART(localClass, baseClass) \
	class X##localClass : public baseClass \
	{ \
	public: \
		virtual ULONG STDMETHODCALLTYPE AddRef(); \
		virtual ULONG STDMETHODCALLTYPE Release(); \
		virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, LPVOID* ppvObj); \

#define BEGIN_INTERFACE_PART_DERIVE(localClass, baseClass) \
	class X##localClass : public baseClass \
	{ \
	public: \
		virtual ULONG STDMETHODCALLTYPE AddRef(); \
		virtual ULONG STDMETHODCALLTYPE Release(); \
		virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, LPVOID* ppvObj); \

#define INIT_INTERFACE_PART(theClass, localClass)
#define INIT_INTERFACE_PART_DERIVE(theClass, localClass)

#define END_INTERFACE_PART(localClass) \
	} m_x##localClass; \
	friend class X##localClass;

#define BEGIN_INTERFACE_MAP(theClass, theBase)

#define INTERFACE_PART(theClass, iid, localClass)

#define INTERFACE_AGGREGATE(theClass, theAggr)

#define END_INTERFACE_MAP()

#define BEGIN_DISPATCH_MAP(theClass, baseClass)


#define END_DISPATCH_MAP()

#define DISP_FUNCTION(theClass, szExternalName, pfnMember, vtRetVal, vtsParams) \


#define DISP_PROPERTY(theClass, szExternalName, memberName, vtPropType)

#define DISP_PROPERTY_NOTIFY(theClass, szExternalName, memberName, pfnAfterSet, vtPropType)

#define DISP_PROPERTY_EX(theClass, szExternalName, pfnGet, pfnSet, vtPropType)

#define DISP_PROPERTY_PARAM(theClass, szExternalName, pfnGet, pfnSet, vtPropType, vtsParams)


#define DISP_FUNCTION_ID(theClass, szExternalName, dispid, pfnMember, vtRetVal, vtsParams)

#define DISP_PROPERTY_ID(theClass, szExternalName, dispid, memberName, vtPropType)

#define DISP_PROPERTY_NOTIFY_ID(theClass, szExternalName, dispid, memberName, pfnAfterSet, vtPropType)


#define DISP_PROPERTY_EX_ID(theClass, szExternalName, dispid, pfnGet, pfnSet, vtPropType)

#define DISP_PROPERTY_PARAM_ID(theClass, szExternalName, dispid, pfnGet, pfnSet, vtPropType, vtsParams)

#define DISP_DEFVALUE(theClass, szExternalName)

#define DISP_DEFVALUE_ID(theClass, dispid)

/////////////////////////////////////////////////////////////////////////////
// Macros for creating "creatable" automation classes.

#define DECLARE_OLECREATE(class_name)

#define IMPLEMENT_OLECREATE(class_name, external_name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8)

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

#define BEGIN_CONNECTION_MAP(theClass, theBase)

#define CONNECTION_PART(theClass, iid, localClass)

#define END_CONNECTION_MAP()

#define BEGIN_EVENTSINK_MAP(theClass, baseClass)

#define END_EVENTSINK_MAP()

#define ON_EVENT(theClass, id, dispid, pfnHandler, vtsParams)

#define ON_EVENT_RANGE(theClass, idFirst, idLast, dispid, pfnHandler, vtsParams)

#define ON_PROPNOTIFY(theClass, id, dispid, pfnRequest, pfnChanged)

#define ON_PROPNOTIFY_RANGE(theClass, idFirst, idLast, dispid, pfnRequest, pfnChanged)

#define ON_DSCNOTIFY(theClass, id, pfnNotify)

#define ON_DSCNOTIFY_RANGE(theClass, idFirst, idLast, pfnNotify)

#define ON_EVENT_REFLECT(theClass, dispid, pfnHandler, vtsParams)

#define ON_PROPNOTIFY_REFLECT(theClass, dispid, pfnRequest, pfnChanged)


/////////////////////////////////////////////////////////////////////////////
// Macros for type library information

#define DECLARE_OLETYPELIB(class_name) \
	protected: \
		virtual UINT GetTypeInfoCount(); \
		virtual HRESULT GetTypeLib(LCID, LPTYPELIB*); \
		virtual CTypeLibCache* GetTypeLibCache(); \

#define IMPLEMENT_OLETYPELIB(class_name, tlid, wVerMajor, wVerMinor)

//////////////////////////////////////////////////////////////////////////////////////////
//
// MFC macros (AFXISAPI.H)



#define DECLARE_PARSE_MAP() 
#define BEGIN_PARSE_MAP(theClass, baseClass)

#define ON_PARSE_COMMAND(FnName, mapClass, Args)

#define ON_PARSE_COMMAND_PARAMS(Params)

#define DEFAULT_PARSE_COMMAND(FnName, mapClass)

#define END_PARSE_MAP(theClass)


//////////////////////////////////////////////////////////////////////////////////////////
//
// MFC macros (BASETYPS.H)

#define EXTERN_C    extern "C"

#define STDMETHODCALLTYPE       __stdcall
#define STDMETHODVCALLTYPE      __cdecl

#define STDAPICALLTYPE          __stdcall
#define STDAPIVCALLTYPE         __cdecl

#define STDAPI                  extern "C" HRESULT STDAPICALLTYPE
#define STDAPI_(type)           extern "C" type STDAPICALLTYPE

#define STDMETHODIMP            HRESULT STDMETHODCALLTYPE
#define STDMETHODIMP_(type)     type STDMETHODCALLTYPE

#define STDAPIV                 extern "C" HRESULT STDAPIVCALLTYPE
#define STDAPIV_(type)          extern "C" type STDAPIVCALLTYPE

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
    extern "C" const GUID FAR name

#define DEFINE_OLEGUID(name, l, w1, w2) \
    extern "C" const GUID FAR name

#define BEGIN_INTERFACE
#define END_INTERFACE


//////////////////////////////////////////////////////////////////////////////////////////
//
// OWL macros (OCBOCOLE.H)

#define DEFINE_BOLEGUID(name, l) extern "C" const GUID CDECL FAR name


//////////////////////////////////////////////////////////////////////////////////////////
//
// MFC macros (AFXTLS_.H)

#define THREAD_LOCAL(class_name, ident_name) \
	AFX_DATADEF CThreadLocal<class_name> ident_name;
#define EXTERN_THREAD_LOCAL(class_name, ident_name) \
	extern AFX_DATA AFX_DATADEF CThreadLocal<class_name> ident_name;

#define PROCESS_LOCAL(class_name, ident_name) \
	AFX_DATADEF CProcessLocal<class_name> ident_name;
#define EXTERN_PROCESS_LOCAL(class_name, ident_name) \
	extern AFX_DATA AFX_DATADEF CProcessLocal<class_name> ident_name;


//////////////////////////////////////////////////////////////////////////////////////////
//
// MFC macros (AFXMOD.H)

#define DECLARE_OLEMISC_FLAGS(flags)  static DWORD   _GetObjectMiscStatus();
#define DECLARE_PROGID(progid)        static LPCTSTR GetProgID();
#define DECLARE_DESCRIPTION(desc)     static LPCTSTR GetDescription();
#define DECLARE_THREADING_MODEL(thrd) static ObjectThreadingModel GetThreadModel();

#define BEGIN_VERB_MAP()

#define VERB_ENTRY_EX(verbId, verbString, mfFlags, verbAttrib)

#define VERB_ENTRY(verbId, verbString)

#define END_VERB_MAP()


// Automation Object Macros
//
#define  AUTOOBJECT_COM_INTERFACE_ENTRIES(intf)

// Entry of the default dispatch interface of a COCLASS
//
#define   COM_INTERFACE_ENTRY_OF_DEF_DISPINTF(intf)

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

#define DUALINTERFACE_ENTRY(i)


#define UPDATE_REGISTRY_METHOD(code)

#define DECLARE_COMSERVER_REGISTRY(progid, desc)

#define DECLARE_TYPED_COMSERVER_REGISTRY(progid)


//////////////////////////////////////////////////////////////////////////////////////////
//
// ATL/VCL macros (ATLVCL.H)

// VCL_CONTROL_COM_INTERFACE_ENTRIES
//
//  This macro defines the entries of the required Interfaces for exposing a VCL
//  component as an ActiveX Control
//
#define  VCL_CONTROL_COM_INTERFACE_ENTRIES(intf) 

// VCLCONTROL_IMPL
// This macro is used to encapsulate the various base classes an ActiveX VCL Control derives from.
//
#define VCLCONTROL_IMPL(cppClass, CoClass, VclClass, intf, EventID) \
   public TVclControlImpl<cppClass, VclClass, &CLSID_##CoClass, &IID_##intf, &EventID, LIBID_OF_##CoClass>,\
   public IDispatchImpl<intf, &IID_##intf, LIBID_OF_##CoClass>, \
   public TEvents_##CoClass<cppClass>

// COM_MAP entry for VCL-based Property Page
//
#define  PROPERTYPAGE_COM_INTERFACE_ENTRIES

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

#define DECLARE_REMOTEDATAMODULE_REGISTRY(progid)

#define DECLARE_ACTIVEXCONTROL_REGISTRY(progid, idbmp)


//////////////////////////////////////////////////////////////////////////////////////////
//
// ATL macros (ATLCTL.H)

#define BEGIN_PROPERTY_MAP(theClass)

#define PROP_ENTRY(szDesc, dispid, clsid)

#define PROP_ENTRY_EX(szDesc, dispid, clsid, iidDispatch)

#define PROP_PAGE(clsid)

#define END_PROPERTY_MAP()

#define IMPLEMENT_STOCKPROP(type, fname, pname, dispid)

#define IMPLEMENT_BOOL_STOCKPROP(fname, pname, dispid)

#define IMPLEMENT_BSTR_STOCKPROP(fname, pname, dispid)

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

#define DECLARE_MAPI_INTERFACE(iface)                                   interface iface
#define DECLARE_MAPI_INTERFACE_(iface, baseiface)                       interface iface : public baseiface
#define DECLARE_MAPI_INTERFACE_PTR(iface, piface)                       interface iface; typedef iface FAR * piface

#define MAPIMETHOD(method)                                  virtual HRESULT STDMETHODCALLTYPE method
#define MAPIMETHOD_(type, method)                           virtual type STDMETHODCALLTYPE method
#define MAPIMETHOD_DECLARE(type, method, prefix)            type STDMETHODCALLTYPE prefix##method
#define MAPIMETHOD_TYPEDEF(type, method, prefix)            typedef type (STDMETHODCALLTYPE prefix##method##_METHOD)


//////////////////////////////////////////////////////////////////////////////////////////
//
// Telephony  API (TAPI.H)

#define DECLARE_OPAQUE32(name)  struct name##__ { int unused; };

//////////////////////////////////////////////////////////////////////////////////////////
//
// OWL (EVENTHAN.H)

//
// Macros to declare a response table
//
#define DECLARE_RESPONSE_TABLE(cls)

#define END_RESPONSE_TABLE

#define DEFINE_RESPONSE_TABLE_ENTRIES(cls)

//
// Macro to define a response table for a class with no base response tables
//
// Use it like this:
//    DEFINE_RESPONSE_TABLE(cls)
//      EV_WM_PAINT,
//      EV_WM_LBUTTONDOWN,
//    END_RESPONSE_TABLE;
//
#define DEFINE_RESPONSE_TABLE(cls)

//
// Macro to define a response table for a class with one base. Use this macro
// exactly like macro DEFINE_RESPONSE_TABLE
//
#define DEFINE_RESPONSE_TABLE1(cls, base)

//
// Macro to define a response table for a class with two bases. Use this macro
// exactly like macro DEFINE_RESPONSE_TABLE
//
#define DEFINE_RESPONSE_TABLE2(cls, base1, base2)


//
// Macro to define a response table for a class with three bases. Use this macro
// exactly like macro DEFINE_RESPONSE_TABLE
//
#define DEFINE_RESPONSE_TABLE3(cls, base1, base2, base3)



//////////////////////////////////////////////////////////////////////////////////////////
//
// Video for Windows (VFW.H)

#define DEFINE_AVIGUID(name, l, w1, w2)

//////////////////////////////////////////////////////////////////////////////////////////
//
// DAO macros (ADOID.H)


#define DEFINE_DAOGUID(name, l)

//////////////////////////////////////////////////////////////////////////////////////////
//
// Exchange (EXCHEXT.H)

#define DEFINE_EXCHEXTGUID(name, b)

//////////////////////////////////////////////////////////////////////////////////////////
//
// Exchange (EXCHFORM.H)

#define DEFINE_EXCHFORMGUID(name, b)

//////////////////////////////////////////////////////////////////////////////////////////
//
// MAPI (INITOID.H)

#define DEFINE_OID_1(name, b0, b1)

#define DEFINE_OID_2(name, b0, b1, b2)

#define DEFINE_OID_3(name, b0, b1, b2, b3)

#define DEFINE_OID_4(name, b0, b1, b2, b3, b4)

//////////////////////////////////////////////////////////////////////////////////////////
//
// Shell (SHL*.H)

#define SHSTDAPI          extern "C" HRESULT STDAPICALLTYPE
#define SHSTDAPI_(type)   extern "C" type STDAPICALLTYPE
#define SHDOCAPI          extern "C" HRESULT STDAPICALLTYPE
#define SHDOCAPI_(type)   extern "C" type STDAPICALLTYPE
#define LWSTDAPI          extern "C" HRESULT STDAPICALLTYPE
#define LWSTDAPI_(type)   extern "C" type STDAPICALLTYPE

#define DEFINE_SHLGUID(name, l, w1, w2) extern "C" const GUID FAR name


//////////////////////////////////////////////////////////////////////////////////////////
//
// D3D (D3DRMOBJ.H)


/*
 * The methods for IUnknown
 */
#define IUNKNOWN_METHODS(kind) \
    virtual HRESULT STDMETHODCALLTYPE QueryInterface	   	(THIS_ REFIID riid, LPVOID *ppvObj) kind; \
    virtual ULONG STDMETHODCALLTYPE AddRef	   	(THIS) kind; \
    virtual ULONG STDMETHODCALLTYPE Release	   	(THIS) kind

/*
 * The methods for IDirect3DRMObject
 */
#define IDIRECT3DRMOBJECT_METHODS(kind) \
    virtual HRESULT STDMETHODCALLTYPE Clone			(THIS_ LPUNKNOWN pUnkOuter, REFIID riid, LPVOID *ppvObj) kind; \
    virtual HRESULT STDMETHODCALLTYPE AddDestroyCallback  	(THIS_ D3DRMOBJECTCALLBACK, LPVOID argument) kind; \
    virtual HRESULT STDMETHODCALLTYPE DeleteDestroyCallback	(THIS_ D3DRMOBJECTCALLBACK, LPVOID argument) kind; \
    virtual HRESULT STDMETHODCALLTYPE SetAppData	   	(THIS_ DWORD data) kind; \
    virtual DWORD STDMETHODCALLTYPE GetAppData  	(THIS) kind; \
    virtual HRESULT STDMETHODCALLTYPE SetName		   	(THIS_ LPCSTR) kind; \
    virtual HRESULT STDMETHODCALLTYPE GetName			(THIS_ LPDWORD lpdwSize, LPSTR lpName) kind; \
    virtual HRESULT STDMETHODCALLTYPE GetClassName		(THIS_ LPDWORD lpdwSize, LPSTR lpName) kind


#define WIN_TYPES(itype, ptype) \
    typedef interface itype FAR *LP##ptype, FAR **LPLP##ptype

