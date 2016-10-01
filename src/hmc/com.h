/*
	COM サポートクラス
	Copyright (C)1999 H.Mutsuki
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __INC_HMC_COM__
#define __INC_HMC_COM__

#include "util.h"
#include <unknwn.h>
#pragma comment(lib, "ole32.lib")

/*****************************************************************
*****************************************************************/
_HMC_BEGIN
/*****************************************************************
*****************************************************************/
#ifdef _DEBUG
#define CHECKRESULT_COM(exp) (HMC::CDelegateUnknown::CheckResult((exp),__FILE__,__LINE__))
#else
#define CHECKRESULT_COM(exp) (exp)
#endif
/*****************************************************************
*****************************************************************/
class CDelegateUnknown;
class CDelegateClassFactory;
class CDelegateDispatch;
class CDelegatePersist;
class CDelegateStorage;
class CDelegateSequentialStream;
class CDelegateStream;

/*****************************************************************
*****************************************************************/
DEFINE_GUID(CLSID_Unknown, 0x00000000, 0x0000, 0x0000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0, 0x0);

/*****************************************************************
	IUnknown(unknwn.h)
*****************************************************************/
class CDelegateUnknown //: public IUnknown // ※継承してはいけないことに注意
{
public:
	DECLARE_SYNONYM(CDelegateUnknown,TYPE);
	DECLARE_SYNONYM(IUnknown,OBJ);
	operator LPTYPE() { return this; }
	operator LPVOID() { return object(); }
	operator LPOBJ () { return object(); }
	operator LPOBJ *() { return receptor(); }
	operator LPVOID*() { return (LPVOID*)receptor(); }
public:
	static REFIID iid() { return IID_IUnknown; }
	static REFCLSID clsid() { return CLSID_Unknown; }
public:
	CDelegateUnknown() { init(); }
	CDelegateUnknown(TYPE &t) { init(); let(t); }
	CDelegateUnknown(LPUNKNOWN pUnk) { init(); map(pUnk); }
	CDelegateUnknown(LPUNKNOWN pUnk, REFIID riid) { init(); create(pUnk,riid); }
	CDelegateUnknown(REFCLSID rclsid, REFIID riid, DWORD dwClsContext) { init(); create(rclsid,riid,dwClsContext); }
	virtual ~CDelegateUnknown() { destroy(); }
private:
	void init();
public:
	void destroy();
public:
	HRESULT create(LPUNKNOWN pUnk, REFIID riid);
	HRESULT create(REFCLSID rclsid, REFIID riid, DWORD dwClsContext=CLSCTX_ALL);
	void let(LPOBJ pObj) { destroy(); if(pObj) pObj->AddRef(); m_pObject=pObj; }
	void map(LPOBJ pObj) { destroy(); m_bOwner=FALSE; m_pObject=pObj; }
public:
	// IUnknown
	STDMETHODIMP QueryInterface(REFIID riid, void **ppvObject);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();
public:
	// リザルトコード検証
	static HRESULT CheckResult(HRESULT hResult, LPCTSTR szFileName, UINT nLineNumber);
public:
	// クラス情報の取得/設定
	LPOBJ iptr() const { return dynamic_cast<LPOBJ>(const_cast<LPTYPE>(this)); }
	LPOBJ object() const { return m_pObject; }
	LPOBJ *receptor() { return &m_pObject; }
	BOOL isValid() const { return !!object(); }
	BOOL isOwner() const { return m_bOwner; }
	BOOL ownership(BOOL f) { return m_bOwner=f; }
private:
	BOOL m_bOwner;		// 所有権フラグ
	ULONG m_cRef;		// 参照カウンタ
	LPOBJ m_pObject;	// インターフェース
};

/*****************************************************************
	IClassFactory(unknwn.h)
*****************************************************************/
class CDelegateClassFactory : public IClassFactory, public CDelegateUnknown
{
public:
	DECLARE_SYNONYM(CDelegateClassFactory,TYPE);
	DECLARE_SYNONYM(CDelegateUnknown,BASE);
	DECLARE_SYNONYM(IClassFactory,OBJ);
	operator LPTYPE() { return this; }
	operator LPOBJ*() { return (LPOBJ*)BASE::receptor(); }
	LPOBJ object() const { return (LPOBJ)BASE::object(); }
public:
	static REFIID iid() { return IID_IClassFactory; }
	static REFCLSID clsid() { return CLSID_Unknown; }
public:
	CDelegateClassFactory() {}
	CDelegateClassFactory(LPUNKNOWN pUnk) : BASE(pUnk) {}
	CDelegateClassFactory(LPUNKNOWN pUnk, REFIID riid) : BASE(pUnk,riid) {}
	CDelegateClassFactory(REFCLSID rclsid, REFIID riid, DWORD dwClsContext) : BASE(rclsid,riid,dwClsContext) {}
public:
	// IUnknown
	STDMETHODIMP QueryInterface(REFIID riid, void **ppvObject) { return BASE::QueryInterface(riid,ppvObject); }
	STDMETHODIMP_(ULONG) AddRef() { return BASE::AddRef(); }
	STDMETHODIMP_(ULONG) Release() { return BASE::Release(); }
public:
	// IFilterFactory
	STDMETHODIMP CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppvObject) { LPOBJ p=object(); HRESULT h=!p?E_POINTER:p==this?E_NOTIMPL:p->CreateInstance(pUnkOuter,riid,ppvObject); CHECKRESULT_COM(h); return h; }
	STDMETHODIMP LockServer(BOOL fLock) { LPOBJ p=object(); HRESULT h=!p?E_POINTER:p==this?E_NOTIMPL:p->LockServer(fLock); CHECKRESULT_COM(h); return h; }
};

/*****************************************************************
	IDispatch(oaidl.h)
*****************************************************************/
class CDelegateDispatch : public IDispatch, public CDelegateUnknown
{
public:
	DECLARE_SYNONYM(CDelegateDispatch,TYPE);
	DECLARE_SYNONYM(CDelegateUnknown,BASE);
	DECLARE_SYNONYM(IDispatch,OBJ);
	operator LPTYPE() { return this; }
	operator LPOBJ*() { return (LPOBJ*)BASE::receptor(); }
	LPOBJ object() const { return (LPOBJ)BASE::object(); }
public:
	static REFIID iid() { return IID_IDispatch; }
	static REFCLSID clsid() { return CLSID_Unknown; }
public:
	CDelegateDispatch() {}
	CDelegateDispatch(LPUNKNOWN pUnk) : BASE(pUnk) {}
	CDelegateDispatch(LPUNKNOWN pUnk, REFIID riid) : BASE(pUnk,riid) {}
	CDelegateDispatch(REFCLSID rclsid, REFIID riid, DWORD dwClsContext) : BASE(rclsid,riid,dwClsContext) {}
public:
	// IUnknown
	STDMETHODIMP QueryInterface(REFIID riid, void **ppvObject) { return BASE::QueryInterface(riid,ppvObject); }
	STDMETHODIMP_(ULONG) AddRef() { return BASE::AddRef(); }
	STDMETHODIMP_(ULONG) Release() { return BASE::Release(); }
public:
	// IDispatch
	STDMETHODIMP GetTypeInfoCount(UINT *pctinfo) { LPOBJ p=object(); HRESULT h=!p?E_POINTER:p==this?E_NOTIMPL:p->GetTypeInfoCount(pctinfo); CHECKRESULT_COM(h); return h; }
	STDMETHODIMP GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo) { LPOBJ p=object(); HRESULT h=!p?E_POINTER:p==this?E_NOTIMPL:p->GetTypeInfo(iTInfo,lcid,ppTInfo); CHECKRESULT_COM(h); return h; }
	STDMETHODIMP GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId) { LPOBJ p=object(); HRESULT h=!p?E_POINTER:p==this?E_NOTIMPL:p->GetIDsOfNames(riid,rgszNames,cNames,lcid,rgDispId); CHECKRESULT_COM(h); return h; }
	STDMETHODIMP Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr) { LPOBJ p=object(); HRESULT h=!p?E_POINTER:p==this?E_NOTIMPL:p->Invoke(dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr); CHECKRESULT_COM(h); return h; }
};

/*****************************************************************
	IPersist(objidl.h)
*****************************************************************/
class CDelegatePersist : public IPersist, public CDelegateUnknown
{
public:
	DECLARE_SYNONYM(CDelegatePersist,TYPE);
	DECLARE_SYNONYM(CDelegateUnknown,BASE);
	DECLARE_SYNONYM(IPersist,OBJ);
	operator LPTYPE() { return this; }
	operator LPOBJ*() { return (LPOBJ*)BASE::receptor(); }
	LPOBJ object() const { return (LPOBJ)BASE::object(); }
public:
	static REFIID iid() { return IID_IPersist; }
	static REFCLSID clsid() { return CLSID_Unknown; }
public:
	CDelegatePersist() {}
	CDelegatePersist(LPUNKNOWN pUnk) : BASE(pUnk) {}
	CDelegatePersist(LPUNKNOWN pUnk, REFIID riid) : BASE(pUnk,riid) {}
	CDelegatePersist(REFCLSID rclsid, REFIID riid, DWORD dwClsContext) : BASE(rclsid,riid,dwClsContext) {}
public:
	// IUnknown
	STDMETHODIMP QueryInterface(REFIID riid, void **ppvObject) { return BASE::QueryInterface(riid,ppvObject); }
	STDMETHODIMP_(ULONG) AddRef() { return BASE::AddRef(); }
	STDMETHODIMP_(ULONG) Release() { return BASE::Release(); }
public:
	// IPersist
	STDMETHODIMP GetClassID(CLSID *pClassID) { LPOBJ p=object(); HRESULT h=!p?E_POINTER:p==this?E_NOTIMPL:p->GetClassID(pClassID); CHECKRESULT_COM(h); return h; }
};

/*****************************************************************
	IStorage(objidl.h)
*****************************************************************/
class CDelegateStorage : public IStorage, public CDelegateUnknown
{
public:
	DECLARE_SYNONYM(CDelegateStorage,TYPE);
	DECLARE_SYNONYM(CDelegateUnknown,BASE);
	DECLARE_SYNONYM(IStorage,OBJ);
	operator LPTYPE() { return this; }
	operator LPOBJ*() { return (LPOBJ*)BASE::receptor(); }
	LPOBJ object() const { return (LPOBJ)BASE::object(); }
public:
	static REFIID iid() { return IID_IStorage; }
	static REFCLSID clsid() { return CLSID_Unknown; }
public:
	CDelegateStorage() {}
	CDelegateStorage(LPUNKNOWN pUnk) : BASE(pUnk) {}
	CDelegateStorage(LPUNKNOWN pUnk, REFIID riid) : BASE(pUnk,riid) {}
	CDelegateStorage(REFCLSID rclsid, REFIID riid, DWORD dwClsContext) : BASE(rclsid,riid,dwClsContext) {}
public:
	// IUnknown
	STDMETHODIMP QueryInterface(REFIID riid, void **ppvObject) { return BASE::QueryInterface(riid,ppvObject); }
	STDMETHODIMP_(ULONG) AddRef() { return BASE::AddRef(); }
	STDMETHODIMP_(ULONG) Release() { return BASE::Release(); }
public:
	// IStorage
	STDMETHODIMP CreateStream(const OLECHAR *pwcsName, DWORD grfMode, DWORD reserved1, DWORD reserved2, IStream **ppstm) { LPOBJ p=object(); HRESULT h=!p?E_POINTER:p==this?E_NOTIMPL:p->CreateStream(pwcsName,grfMode,reserved1,reserved2,ppstm); CHECKRESULT_COM(h); return h; }
	STDMETHODIMP OpenStream(const OLECHAR *pwcsName, void *reserved1, DWORD grfMode, DWORD reserved2, IStream **ppstm) { LPOBJ p=object(); HRESULT h=!p?E_POINTER:p==this?E_NOTIMPL:p->OpenStream(pwcsName,reserved1,grfMode,reserved2,ppstm); CHECKRESULT_COM(h); return h; }
	STDMETHODIMP CreateStorage(const OLECHAR *pwcsName, DWORD grfMode, DWORD reserved1, DWORD reserved2, IStorage **ppstg) { LPOBJ p=object(); HRESULT h=!p?E_POINTER:p==this?E_NOTIMPL:p->CreateStorage(pwcsName,grfMode,reserved1,reserved2,ppstg); CHECKRESULT_COM(h); return h; }
	STDMETHODIMP OpenStorage(const OLECHAR *pwcsName, IStorage *pstgPriority, DWORD grfMode, SNB snbExclude, DWORD reserved, IStorage **ppstg) { LPOBJ p=object(); HRESULT h=!p?E_POINTER:p==this?E_NOTIMPL:p->OpenStorage(pwcsName,pstgPriority,grfMode,snbExclude,reserved,ppstg); CHECKRESULT_COM(h); return h; }
	STDMETHODIMP CopyTo(DWORD ciidExclude, const IID *rgiidExclude, SNB snbExclude, IStorage *pstgDest) { LPOBJ p=object(); HRESULT h=!p?E_POINTER:p==this?E_NOTIMPL:p->CopyTo(ciidExclude,rgiidExclude,snbExclude,pstgDest); CHECKRESULT_COM(h); return h; }
	STDMETHODIMP MoveElementTo(const OLECHAR *pwcsName, IStorage *pstgDest, const OLECHAR *pwcsNewName, DWORD grfFlags) { LPOBJ p=object(); HRESULT h=!p?E_POINTER:p==this?E_NOTIMPL:p->MoveElementTo(pwcsName,pstgDest,pwcsNewName,grfFlags); CHECKRESULT_COM(h); return h; }
	STDMETHODIMP Commit(DWORD grfCommitFlags) { LPOBJ p=object(); HRESULT h=!p?E_POINTER:p==this?E_NOTIMPL:p->Commit(grfCommitFlags); CHECKRESULT_COM(h); return h; }
	STDMETHODIMP Revert() { LPOBJ p=object(); HRESULT h=!p?E_POINTER:p==this?E_NOTIMPL:p->Revert(); CHECKRESULT_COM(h); return h; }
	STDMETHODIMP EnumElements(DWORD reserved1, void *reserved2, DWORD reserved3, IEnumSTATSTG **ppenum) { LPOBJ p=object(); HRESULT h=!p?E_POINTER:p==this?E_NOTIMPL:p->EnumElements(reserved1,reserved2,reserved3,ppenum); CHECKRESULT_COM(h); return h; }
	STDMETHODIMP DestroyElement(const OLECHAR *pwcsName) { LPOBJ p=object(); HRESULT h=!p?E_POINTER:p==this?E_NOTIMPL:p->DestroyElement(pwcsName); CHECKRESULT_COM(h); return h; }
	STDMETHODIMP RenameElement(const OLECHAR *pwcsOldName, const OLECHAR *pwcsNewName) { LPOBJ p=object(); HRESULT h=!p?E_POINTER:p==this?E_NOTIMPL:p->RenameElement(pwcsOldName,pwcsNewName); CHECKRESULT_COM(h); return h; }
	STDMETHODIMP SetElementTimes(const OLECHAR *pwcsName, const FILETIME *pctime, const FILETIME *patime, const FILETIME *pmtime) { LPOBJ p=object(); HRESULT h=!p?E_POINTER:p==this?E_NOTIMPL:p->SetElementTimes(pwcsName,pctime,patime,pmtime); CHECKRESULT_COM(h); return h; }
	STDMETHODIMP SetClass(REFCLSID clsid) { LPOBJ p=object(); HRESULT h=!p?E_POINTER:p==this?E_NOTIMPL:p->SetClass(clsid); CHECKRESULT_COM(h); return h; }
	STDMETHODIMP SetStateBits(DWORD grfStateBits, DWORD grfMask) { LPOBJ p=object(); HRESULT h=!p?E_POINTER:p==this?E_NOTIMPL:p->SetStateBits(grfStateBits,grfMask); CHECKRESULT_COM(h); return h; }
	STDMETHODIMP Stat(STATSTG *pstatstg, DWORD grfStatFlag) { LPOBJ p=object(); HRESULT h=!p?E_POINTER:p==this?E_NOTIMPL:p->Stat(pstatstg,grfStatFlag); CHECKRESULT_COM(h); return h; }
};

/*****************************************************************
	ISequentialStream(objidl.h)
*****************************************************************/
class CDelegateSequentialStream : public ISequentialStream, public CDelegateUnknown
{
public:
	DECLARE_SYNONYM(CDelegateSequentialStream,TYPE);
	DECLARE_SYNONYM(CDelegateUnknown,BASE);
	DECLARE_SYNONYM(ISequentialStream,OBJ);
	operator LPTYPE() { return this; }
	operator LPOBJ*() { return (LPOBJ*)BASE::receptor(); }
	LPOBJ object() const { return (LPOBJ)BASE::object(); }
public:
	static REFIID iid() { return IID_ISequentialStream; }
	static REFCLSID clsid() { return CLSID_Unknown; }
public:
	CDelegateSequentialStream() {}
	CDelegateSequentialStream(LPUNKNOWN pUnk) : BASE(pUnk) {}
	CDelegateSequentialStream(LPUNKNOWN pUnk, REFIID riid) : BASE(pUnk,riid) {}
	CDelegateSequentialStream(REFCLSID rclsid, REFIID riid, DWORD dwClsContext) : BASE(rclsid,riid,dwClsContext) {}
public:
	// IUnknown
	STDMETHODIMP QueryInterface(REFIID riid, void **ppvObject) { return BASE::QueryInterface(riid,ppvObject); }
	STDMETHODIMP_(ULONG) AddRef() { return BASE::AddRef(); }
	STDMETHODIMP_(ULONG) Release() { return BASE::Release(); }
public:
	// ISequentialStream
	STDMETHODIMP Read(void *pv, ULONG cb, ULONG *pcbRead) { LPOBJ p=object(); HRESULT h=!p?E_POINTER:p==this?E_NOTIMPL:p->Read(pv,cb,pcbRead); CHECKRESULT_COM(h); return h; }
	STDMETHODIMP Write(const void *pv, ULONG cb, ULONG *pcbWritten) { LPOBJ p=object(); HRESULT h=!p?E_POINTER:p==this?E_NOTIMPL:p->Write(pv,cb,pcbWritten); CHECKRESULT_COM(h); return h; }
};

/*****************************************************************
	IStream(objidl.h)
*****************************************************************/
class CDelegateStream : public IStream, public CDelegateUnknown
{
public:
	DECLARE_SYNONYM(CDelegateStream,TYPE);
	DECLARE_SYNONYM(CDelegateUnknown,BASE);
	DECLARE_SYNONYM(IStream,OBJ);
	operator LPTYPE() { return this; }
	operator LPOBJ*() { return (LPOBJ*)BASE::receptor(); }
	LPOBJ object() const { return (LPOBJ)BASE::object(); }
public:
	static REFIID iid() { return IID_IStream; }
	static REFCLSID clsid() { return CLSID_Unknown; }
public:
	CDelegateStream() {}
	CDelegateStream(LPUNKNOWN pUnk) : BASE(pUnk) {}
	CDelegateStream(LPUNKNOWN pUnk, REFIID riid) : BASE(pUnk,riid) {}
	CDelegateStream(REFCLSID rclsid, REFIID riid, DWORD dwClsContext) : BASE(rclsid,riid,dwClsContext) {}
public:
	// IUnknown
	STDMETHODIMP QueryInterface(REFIID riid, void **ppvObject) { return BASE::QueryInterface(riid,ppvObject); }
	STDMETHODIMP_(ULONG) AddRef() { return BASE::AddRef(); }
	STDMETHODIMP_(ULONG) Release() { return BASE::Release(); }
public:
	// ISequentialStream
	STDMETHODIMP Read(void *pv, ULONG cb, ULONG *pcbRead) { LPOBJ p=object(); HRESULT h=!p?E_POINTER:p==this?E_NOTIMPL:p->Read(pv,cb,pcbRead); CHECKRESULT_COM(h); return h; }
	STDMETHODIMP Write(const void *pv, ULONG cb, ULONG *pcbWritten) { LPOBJ p=object(); HRESULT h=!p?E_POINTER:p==this?E_NOTIMPL:p->Write(pv,cb,pcbWritten); CHECKRESULT_COM(h); return h; }
public:
	// IStream
	STDMETHODIMP Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER *plibNewPosition) { LPOBJ p=object(); HRESULT h=!p?E_POINTER:p==this?E_NOTIMPL:p->Seek(dlibMove,dwOrigin,plibNewPosition); CHECKRESULT_COM(h); return h; }
	STDMETHODIMP SetSize(ULARGE_INTEGER libNewSize) { LPOBJ p=object(); HRESULT h=!p?E_POINTER:p==this?E_NOTIMPL:p->SetSize(libNewSize); CHECKRESULT_COM(h); return h; }
	STDMETHODIMP CopyTo(IStream *pstm, ULARGE_INTEGER cb, ULARGE_INTEGER *pcbRead, ULARGE_INTEGER *pcbWritten) { LPOBJ p=object(); HRESULT h=!p?E_POINTER:p==this?E_NOTIMPL:p->CopyTo(pstm,cb,pcbRead,pcbWritten); CHECKRESULT_COM(h); return h; }
	STDMETHODIMP Commit(DWORD grfCommitFlags) { LPOBJ p=object(); HRESULT h=!p?E_POINTER:p==this?E_NOTIMPL:p->Commit(grfCommitFlags); CHECKRESULT_COM(h); return h; }
	STDMETHODIMP Revert() { LPOBJ p=object(); HRESULT h=!p?E_POINTER:p==this?E_NOTIMPL:p->Revert(); CHECKRESULT_COM(h); return h; }
	STDMETHODIMP LockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType) { LPOBJ p=object(); HRESULT h=!p?E_POINTER:p==this?E_NOTIMPL:p->LockRegion(libOffset,cb,dwLockType); CHECKRESULT_COM(h); return h; }
	STDMETHODIMP UnlockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType) { LPOBJ p=object(); HRESULT h=!p?E_POINTER:p==this?E_NOTIMPL:p->UnlockRegion(libOffset,cb,dwLockType); CHECKRESULT_COM(h); return h; }
	STDMETHODIMP Stat(STATSTG *pstatstg, DWORD grfStatFlag) { LPOBJ p=object(); HRESULT h=!p?E_POINTER:p==this?E_NOTIMPL:p->Stat(pstatstg,grfStatFlag); CHECKRESULT_COM(h); return h; }
	STDMETHODIMP Clone(IStream **ppstm) { LPOBJ p=object(); HRESULT h=!p?E_POINTER:p==this?E_NOTIMPL:p->Clone(ppstm); CHECKRESULT_COM(h); return h; }
};

/*****************************************************************
*****************************************************************/
_HMC_END
/*****************************************************************
*****************************************************************/

#endif	/* __INC_HMC_COM__ */

/* Local Variables:						*/
/* tab-width:4							*/
/* c-basic-offset:4						*/
/* c-indent-level:4						*/
/* c-label-offset:-4					*/
/* c-continued-statement-offset:4		*/
/* file-coding-system:*sjis*dos			*/
/* buffer-file-coding-system:sjis-dos	*/
/* End:									*/
