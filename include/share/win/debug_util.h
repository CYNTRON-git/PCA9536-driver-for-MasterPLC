

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Wed Aug 19 23:03:14 2015
 */
/* Compiler settings for debug_util.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 7.00.0555 
    protocol : dce , ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __debug_util_h__
#define __debug_util_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IDebugObject_FWD_DEFINED__
#define __IDebugObject_FWD_DEFINED__
typedef interface IDebugObject IDebugObject;
#endif 	/* __IDebugObject_FWD_DEFINED__ */


#ifndef __IDebugManager_FWD_DEFINED__
#define __IDebugManager_FWD_DEFINED__
typedef interface IDebugManager IDebugManager;
#endif 	/* __IDebugManager_FWD_DEFINED__ */


#ifndef __IDebugManagerEx_FWD_DEFINED__
#define __IDebugManagerEx_FWD_DEFINED__
typedef interface IDebugManagerEx IDebugManagerEx;
#endif 	/* __IDebugManagerEx_FWD_DEFINED__ */


#ifndef __IDebugManagerEx2_FWD_DEFINED__
#define __IDebugManagerEx2_FWD_DEFINED__
typedef interface IDebugManagerEx2 IDebugManagerEx2;
#endif 	/* __IDebugManagerEx2_FWD_DEFINED__ */


#ifndef __IErrorReportingService_FWD_DEFINED__
#define __IErrorReportingService_FWD_DEFINED__
typedef interface IErrorReportingService IErrorReportingService;
#endif 	/* __IErrorReportingService_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IDebugObject_INTERFACE_DEFINED__
#define __IDebugObject_INTERFACE_DEFINED__

/* interface IDebugObject */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IDebugObject;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("5904F7D9-5C13-4ab5-A36E-88F51552D226")
    IDebugObject : public IUnknown
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE get_Description( 
            /* [out] */ BSTR *pbstrDescr) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Dump( 
            /* [in] */ IStream *pStream,
            /* [in] */ DWORD dwFlags) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugObjectVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugObject * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugObject * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugObject * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *get_Description )( 
            IDebugObject * This,
            /* [out] */ BSTR *pbstrDescr);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Dump )( 
            IDebugObject * This,
            /* [in] */ IStream *pStream,
            /* [in] */ DWORD dwFlags);
        
        END_INTERFACE
    } IDebugObjectVtbl;

    interface IDebugObject
    {
        CONST_VTBL struct IDebugObjectVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugObject_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IDebugObject_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IDebugObject_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IDebugObject_get_Description(This,pbstrDescr)	\
    ( (This)->lpVtbl -> get_Description(This,pbstrDescr) ) 

#define IDebugObject_Dump(This,pStream,dwFlags)	\
    ( (This)->lpVtbl -> Dump(This,pStream,dwFlags) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IDebugObject_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_debug_util_0000_0001 */
/* [local] */ 

typedef 
enum EDebugManagerDumpFlags
    {	dmdfSaveSystemDump	= 0x1,
	dmdfSaveSystemDumpFull	= 0x2
    } 	EDebugManagerDumpFlags;



extern RPC_IF_HANDLE __MIDL_itf_debug_util_0000_0001_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_debug_util_0000_0001_v0_0_s_ifspec;

#ifndef __IDebugManager_INTERFACE_DEFINED__
#define __IDebugManager_INTERFACE_DEFINED__

/* interface IDebugManager */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IDebugManager;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("FCBCC312-9231-4125-9D85-B6B1F6F955C4")
    IDebugManager : public IUnknown
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE EnterMethod( 
            /* [in] */ LPCWSTR wszMethodName,
            /* [in] */ IUnknown *piUnkObj) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE LeaveMethod( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Error( 
            /* [in] */ HRESULT hr,
            /* [in] */ BSTR bstrDescr,
            /* [in] */ IUnknown *piDumpObject,
            /* [in] */ DWORD dwFlags) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Init( 
            /* [in] */ BSTR bstrLogFolder) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Dump( 
            /* [in] */ BSTR bstrFileName,
            /* [in] */ BSTR bstrDescr,
            /* [in] */ IUnknown *piDumpObject,
            /* [in] */ DWORD dwFlags) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetDumpObject( 
            /* [in] */ IUnknown *piDumpObject,
            /* [in] */ DWORD dwFlags) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetDumpObject( 
            /* [out] */ IUnknown **ppDumpObject) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetState( 
            /* [in] */ BOOL bEnable,
            /* [in] */ long nMaxDumpCount) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Enable( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugManagerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugManager * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugManager * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugManager * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *EnterMethod )( 
            IDebugManager * This,
            /* [in] */ LPCWSTR wszMethodName,
            /* [in] */ IUnknown *piUnkObj);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *LeaveMethod )( 
            IDebugManager * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Error )( 
            IDebugManager * This,
            /* [in] */ HRESULT hr,
            /* [in] */ BSTR bstrDescr,
            /* [in] */ IUnknown *piDumpObject,
            /* [in] */ DWORD dwFlags);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Init )( 
            IDebugManager * This,
            /* [in] */ BSTR bstrLogFolder);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Dump )( 
            IDebugManager * This,
            /* [in] */ BSTR bstrFileName,
            /* [in] */ BSTR bstrDescr,
            /* [in] */ IUnknown *piDumpObject,
            /* [in] */ DWORD dwFlags);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetDumpObject )( 
            IDebugManager * This,
            /* [in] */ IUnknown *piDumpObject,
            /* [in] */ DWORD dwFlags);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetDumpObject )( 
            IDebugManager * This,
            /* [out] */ IUnknown **ppDumpObject);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetState )( 
            IDebugManager * This,
            /* [in] */ BOOL bEnable,
            /* [in] */ long nMaxDumpCount);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Enable )( 
            IDebugManager * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IDebugManager * This);
        
        END_INTERFACE
    } IDebugManagerVtbl;

    interface IDebugManager
    {
        CONST_VTBL struct IDebugManagerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugManager_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IDebugManager_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IDebugManager_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IDebugManager_EnterMethod(This,wszMethodName,piUnkObj)	\
    ( (This)->lpVtbl -> EnterMethod(This,wszMethodName,piUnkObj) ) 

#define IDebugManager_LeaveMethod(This)	\
    ( (This)->lpVtbl -> LeaveMethod(This) ) 

#define IDebugManager_Error(This,hr,bstrDescr,piDumpObject,dwFlags)	\
    ( (This)->lpVtbl -> Error(This,hr,bstrDescr,piDumpObject,dwFlags) ) 

#define IDebugManager_Init(This,bstrLogFolder)	\
    ( (This)->lpVtbl -> Init(This,bstrLogFolder) ) 

#define IDebugManager_Dump(This,bstrFileName,bstrDescr,piDumpObject,dwFlags)	\
    ( (This)->lpVtbl -> Dump(This,bstrFileName,bstrDescr,piDumpObject,dwFlags) ) 

#define IDebugManager_SetDumpObject(This,piDumpObject,dwFlags)	\
    ( (This)->lpVtbl -> SetDumpObject(This,piDumpObject,dwFlags) ) 

#define IDebugManager_GetDumpObject(This,ppDumpObject)	\
    ( (This)->lpVtbl -> GetDumpObject(This,ppDumpObject) ) 

#define IDebugManager_SetState(This,bEnable,nMaxDumpCount)	\
    ( (This)->lpVtbl -> SetState(This,bEnable,nMaxDumpCount) ) 

#define IDebugManager_Enable(This)	\
    ( (This)->lpVtbl -> Enable(This) ) 

#define IDebugManager_Reset(This)	\
    ( (This)->lpVtbl -> Reset(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IDebugManager_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_debug_util_0000_0002 */
/* [local] */ 

typedef 
enum EDebugManagerStateFlags
    {	dmsfThreadHangup	= 0x1
    } 	EDebugManagerStateFlags;

typedef 
enum EDebugManagerAction
    {	dmaResetErrorThread	= 1,
	dmaRegisterNewThread	= 2,
	dmaRegisterNetThread	= 3,
	dmaDumpWithExceptionPointers	= 4,
	dmaSetVectoredExceptionMode	= 5
    } 	EDebugManagerAction;



extern RPC_IF_HANDLE __MIDL_itf_debug_util_0000_0002_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_debug_util_0000_0002_v0_0_s_ifspec;

#ifndef __IDebugManagerEx_INTERFACE_DEFINED__
#define __IDebugManagerEx_INTERFACE_DEFINED__

/* interface IDebugManagerEx */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IDebugManagerEx;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B9107422-B9A3-43b8-B6A8-68837A3B9BF9")
    IDebugManagerEx : public IUnknown
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetThreadStates( 
            /* [out] */ DWORD *pdwStateFlags,
            /* [out] */ DWORD *pdwErrorThread,
            /* [out] */ DWORD *pdwHangupThreads) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE DoAction( 
            /* [in] */ EDebugManagerAction action,
            /* [in] */ DWORD dwInParam,
            /* [retval][out] */ DWORD *pdwOutParam) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugManagerExVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugManagerEx * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugManagerEx * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugManagerEx * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetThreadStates )( 
            IDebugManagerEx * This,
            /* [out] */ DWORD *pdwStateFlags,
            /* [out] */ DWORD *pdwErrorThread,
            /* [out] */ DWORD *pdwHangupThreads);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *DoAction )( 
            IDebugManagerEx * This,
            /* [in] */ EDebugManagerAction action,
            /* [in] */ DWORD dwInParam,
            /* [retval][out] */ DWORD *pdwOutParam);
        
        END_INTERFACE
    } IDebugManagerExVtbl;

    interface IDebugManagerEx
    {
        CONST_VTBL struct IDebugManagerExVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugManagerEx_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IDebugManagerEx_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IDebugManagerEx_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IDebugManagerEx_GetThreadStates(This,pdwStateFlags,pdwErrorThread,pdwHangupThreads)	\
    ( (This)->lpVtbl -> GetThreadStates(This,pdwStateFlags,pdwErrorThread,pdwHangupThreads) ) 

#define IDebugManagerEx_DoAction(This,action,dwInParam,pdwOutParam)	\
    ( (This)->lpVtbl -> DoAction(This,action,dwInParam,pdwOutParam) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IDebugManagerEx_INTERFACE_DEFINED__ */


#ifndef __IDebugManagerEx2_INTERFACE_DEFINED__
#define __IDebugManagerEx2_INTERFACE_DEFINED__

/* interface IDebugManagerEx2 */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IDebugManagerEx2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C124D4F3-F503-4b9e-8F28-7EBED26813AA")
    IDebugManagerEx2 : public IUnknown
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Version( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE RegisterThread( 
            /* [in] */ DWORD dwThreadID,
            /* [in] */ BSTR bstrName,
            /* [in] */ DWORD dwFlags) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugManagerEx2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugManagerEx2 * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugManagerEx2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugManagerEx2 * This);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Version )( 
            IDebugManagerEx2 * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *RegisterThread )( 
            IDebugManagerEx2 * This,
            /* [in] */ DWORD dwThreadID,
            /* [in] */ BSTR bstrName,
            /* [in] */ DWORD dwFlags);
        
        END_INTERFACE
    } IDebugManagerEx2Vtbl;

    interface IDebugManagerEx2
    {
        CONST_VTBL struct IDebugManagerEx2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugManagerEx2_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IDebugManagerEx2_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IDebugManagerEx2_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IDebugManagerEx2_get_Version(This,pVal)	\
    ( (This)->lpVtbl -> get_Version(This,pVal) ) 

#define IDebugManagerEx2_RegisterThread(This,dwThreadID,bstrName,dwFlags)	\
    ( (This)->lpVtbl -> RegisterThread(This,dwThreadID,bstrName,dwFlags) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IDebugManagerEx2_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_debug_util_0000_0004 */
/* [local] */ 

typedef 
enum EErrorReportingAction
    {	eraOnCriticalError	= 1
    } 	EErrorReportingAction;



extern RPC_IF_HANDLE __MIDL_itf_debug_util_0000_0004_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_debug_util_0000_0004_v0_0_s_ifspec;

#ifndef __IErrorReportingService_INTERFACE_DEFINED__
#define __IErrorReportingService_INTERFACE_DEFINED__

/* interface IErrorReportingService */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IErrorReportingService;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C9EE4D44-D902-4986-B177-0655B301FAE5")
    IErrorReportingService : public IUnknown
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE DoAction( 
            /* [in] */ EErrorReportingAction action,
            /* [in] */ DWORD dwInParam,
            /* [retval][out] */ DWORD *pdwOutParam) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IErrorReportingServiceVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IErrorReportingService * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IErrorReportingService * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IErrorReportingService * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *DoAction )( 
            IErrorReportingService * This,
            /* [in] */ EErrorReportingAction action,
            /* [in] */ DWORD dwInParam,
            /* [retval][out] */ DWORD *pdwOutParam);
        
        END_INTERFACE
    } IErrorReportingServiceVtbl;

    interface IErrorReportingService
    {
        CONST_VTBL struct IErrorReportingServiceVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IErrorReportingService_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IErrorReportingService_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IErrorReportingService_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IErrorReportingService_DoAction(This,action,dwInParam,pdwOutParam)	\
    ( (This)->lpVtbl -> DoAction(This,action,dwInParam,pdwOutParam) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IErrorReportingService_INTERFACE_DEFINED__ */



#ifndef __DebugLib_LIBRARY_DEFINED__
#define __DebugLib_LIBRARY_DEFINED__

/* library DebugLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_DebugLib;
#endif /* __DebugLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


