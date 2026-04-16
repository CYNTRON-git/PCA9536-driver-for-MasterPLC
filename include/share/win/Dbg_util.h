//
#ifndef __DBG_UTIL_H__
#define __DBG_UTIL_H__

#include <string>
#include <sstream>
#include "debug_util.h"
#include "commonstructs.h"
#include <comdef.h>
#include "tstring.h"

#if !defined(PROTECT_DEMO_VERSION) && !defined(MSEH_DEBUG_TRACK_STACK)
#    define MSEH_DEBUG_NO_STACK
#endif
//#define		MSEH_DEBUG_NO_DUMP
//#define		MSEH_DEBUG_NO_REPORT

extern IDebugManager* g_pMSEHManager;
extern bool g_bMSEHCanCreate;
EXTERN_C const CLSID CLSID_DebugManager_MSEH;
// MasterSACDA error handling namespace
namespace MSEH {
#define MSEH_FULL_SUPPORT

#define MSEH_DEFINE_DEBUG()                                                                                            \
    EXTERN_C const CLSID CLSID_DebugManager_MSEH = {0x8CB9E815,                                                        \
                                                    0x9580,                                                            \
                                                    0x449C,                                                            \
                                                    {0xA6, 0x24, 0x87, 0xE4, 0xE4, 0xF3, 0xF0, 0xCA}};                 \
    IDebugManager* g_pMSEHManager = NULL;                                                                              \
    bool g_bMSEHCanCreate = true;

#define CHECK_DEBUG()                                                                                                  \
    if (g_bMSEHCanCreate && !g_pMSEHManager) {                                                                         \
        ::CoCreateInstance(CLSID_DebugManager_MSEH,                                                                    \
                           NULL,                                                                                       \
                           CLSCTX_INPROC_SERVER,                                                                       \
                           IID_IDebugManager,                                                                          \
                           (void**)&g_pMSEHManager);                                                                   \
    }

#define MSEH_RELEASE()                                                                                                 \
    g_bMSEHCanCreate = false;                                                                                          \
    g_pMSEHManager = NULL;

#define THROWS_MSEH() HRESULT MSEH_ExceptionOccured = S_OK;

    inline void RegisterCurrentThread() {
        CHECK_DEBUG();
        CComQIPtr<IDebugManagerEx> pDMEx(g_pMSEHManager);
        DWORD res;
        if (pDMEx != NULL)
            pDMEx->DoAction(dmaRegisterNewThread, 0, &res);
    }

    inline void RegisterCurrentThread(LPCSTR szName) {
        CHECK_DEBUG();
        CComQIPtr<IDebugManagerEx2> pDMEx(g_pMSEHManager);
        if (pDMEx != NULL)
            pDMEx->RegisterThread(0, CComBSTR(szName), 0);
    }

    inline void SetVectoredExceptionMode(DWORD mode) {
        CHECK_DEBUG();
        CComQIPtr<IDebugManagerEx> pDMEx(g_pMSEHManager);
        if (pDMEx != NULL)
            pDMEx->DoAction(dmaSetVectoredExceptionMode, mode, NULL);
    }

    class CDebugFunction {
    public:
        CComPtr<IUnknown> old_dump_obj;
        bool changed_dump;

        CDebugFunction(LPCWSTR name, IUnknown* object, IUnknown* dump_obj) {
            CHECK_DEBUG();
            if (g_pMSEHManager) {
                g_pMSEHManager->EnterMethod(name, object);

                changed_dump = (dump_obj != NULL);
                if (changed_dump) {
                    g_pMSEHManager->GetDumpObject(&old_dump_obj);
                    g_pMSEHManager->SetDumpObject(dump_obj, 0);
                }
            }
        }
        ~CDebugFunction() {
            if (g_pMSEHManager) {
                g_pMSEHManager->LeaveMethod();

                if (changed_dump) {
                    g_pMSEHManager->SetDumpObject(old_dump_obj, 0);
                }
            }
        }
    };

#ifndef MSEH_DEBUG_NO_STACK
#    define MSEH_FUNCTION(a, dump_obj) MSEH::CDebugFunction debug_function(a, GetUnknown(), dump_obj)

#    define MSEH_STATIC_FUNCTION(a, dump_obj) MSEH::CDebugFunction debug_function(a, dump_obj, dump_obj)
#else
#    define MSEH_FUNCTION(a, dump_obj)
#    define MSEH_STATIC_FUNCTION(a, dump_obj)
#endif

#ifndef MSEH_DEBUG_NO_REPORT
#    define MSEH_ERROR(hr, st) (MSEH::DbgError(hr, st, _T(__FILE__), __LINE__, NULL))
#    define MSEH_ERROR_DUMP(hr, st) (MSEH::DbgError(hr, st, _T(__FILE__), __LINE__, NULL, dmdfSaveSystemDump))
#    define MSEH_ERROR_FULL_DUMP(hr, st)                                                                               \
        (MSEH::DbgError(hr, st, _T(__FILE__), __LINE__, NULL, dmdfSaveSystemDump | dmdfSaveSystemDumpFull))
#else
#    define MSEH_ERROR(hr, st) (hr)
#    define MSEH_ERROR_DUMP(hr, st) (hr)
#    define MSEH_ERROR_FULL_DUMP(hr, st) (hr)
#endif

#define MSEH_ENABLE() ((g_pMSEHManager) ? g_pMSEHManager->Enable() : 0)

#define MSEH_ERROR_ENABLE(hr, st) (MSEH_ERROR(hr, st), MSEH_ENABLE(), hr)

    inline HRESULT DbgError(HRESULT hr,
                            LPCTSTR st,
                            LPCTSTR file_name,
                            int line,
                            IDebugObject* piObject,
                            DWORD dwFlags = 0) {
        CHECK_DEBUG();
        std::tstringstream oss;
        oss << st << _T(" (") << file_name << _T(", ") << line << _T(")");

        if (g_pMSEHManager)
            g_pMSEHManager->Error(hr, CComBSTR(oss.str().c_str()), piObject, dwFlags);
        return hr;
    }

#define RAISE_ERR_INFO(hr, msg)                                                                                        \
    {                                                                                                                  \
        AtlReportError(GUID_NULL, msg, GUID_NULL, hr);                                                                 \
        throw _com_error((MSEH_ExceptionOccured = hr, MSEH_ERROR(hr, msg)));                                           \
    }

#define RAISE_ERR(hr, msg) throw _com_error((MSEH_ExceptionOccured = hr, MSEH_ERROR(hr, msg)));
#define RAISE_ERR_DUMP(hr, msg) throw _com_error((MSEH_ExceptionOccured = hr, MSEH_ERROR_DUMP(hr, msg)));

#define E_MS_CATCHED MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x0666)
#define E_MS_UNHANDLED MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x0667)

#ifdef _AFX
#    define RAISE_MSG(msg)                                                                                             \
        {                                                                                                              \
            ::AfxMessageBox(msg, MB_ICONSTOP | MB_TASKMODAL | MB_TOPMOST);                                             \
            RAISE_ERR(E_MS_CATCHED, msg);                                                                              \
        }

#    define RAISE_MSG_ID(msg_id)                                                                                       \
        {                                                                                                              \
            ::AfxMessageBox(msg_id, MB_ICONSTOP | MB_TASKMODAL | MB_TOPMOST);                                          \
            RAISE_ERR(E_MS_CATCHED, #msg_id);                                                                          \
        }
#endif

#define VERIFY_TEST(hres)                                                                                              \
    {                                                                                                                  \
        HRESULT hr_ = hres;                                                                                            \
        if (FAILED(hr_))                                                                                               \
            MSEH_ERROR(hr_, _T("\"") _T(#hres) _T("\""));                                                              \
    }

#define VERIFY_RET(hres)                                                                                               \
    {                                                                                                                  \
        HRESULT hr_ = hres;                                                                                            \
        if (FAILED(hr_))                                                                                               \
            RAISE_ERR(hr_, _T("\"") _T(#hres) _T("\""));                                                               \
    }

#define VERIFY_RET_WIN32(res)                                                                                          \
    {                                                                                                                  \
        if (res == 0)                                                                                                  \
            RAISE_ERR(HRESULT_FROM_WIN32(::GetLastError()), _T("\"") _T(#res) _T("\""));                               \
    }

#define VERIFY_TEST_WIN32(res)                                                                                         \
    {                                                                                                                  \
        if (res == 0)                                                                                                  \
            MSEH_ERROR(HRESULT_FROM_WIN32(::GetLastError()), _T("\"") _T(#res) _T("\""));                              \
    }

#define MSEH_ExcPtr(ptr, ex)                                                                                           \
    {                                                                                                                  \
        if (!(ptr))                                                                                                    \
            RAISE_ERR(ex, _T("\"") _T(#ptr) _T("\""));                                                                 \
    }

#define MSEH_ChkPtr(ptr, ex)                                                                                           \
    {                                                                                                                  \
        if (!(ptr))                                                                                                    \
            MSEH_ERROR(ex, _T("\"") _T(#ptr) _T("\""));                                                                \
    }

#define PTR_TEST(ptr) MSEH_ChkPtr(ptr, E_NOINTERFACE)
#define PTR_RET(ptr) MSEH_ExcPtr(ptr, E_NOINTERFACE)

#define BOOL_TEST(ptr) MSEH_ChkPtr(ptr, E_INVALIDARG)
#define BOOL_RET(ptr) MSEH_ExcPtr(ptr, E_INVALIDARG)

#define IFOK(hres) if (hr = (hres), (SUCCEEDED(hr) ? true : (MSEH_ERROR(hr, _T("\"") _T(#hres) _T("\"")), false)))

#define IFOK_PTR(hres, ptr)                                                                                            \
    if (hr = (hres), ((SUCCEEDED(hr) && ptr) ? true : (MSEH_ERROR(hr, _T("\"") _T(#hres) _T("\"")), false)))

#define IF_FAILED(hres) if (hr = (hres), (SUCCEEDED(hr) ? false : (MSEH_ERROR(hr, _T("\"") _T(#hres) _T("\"")), true)))

#define IF_FAILED_PTR(hres, ptr)                                                                                       \
    if (hr = (hres), ((SUCCEEDED(hr) && ptr) ? false : (MSEH_ERROR(hr, _T("\"") _T(#hres) _T("\"")), true)))

#define BEGIN_METHOD(function_name)                                                                                    \
    HRESULT MSEH_ExceptionOccured = S_OK;                                                                              \
    MSEH_FUNCTION(function_name, NULL);                                                                                \
    try {
#define BEGIN_STATIC_METHOD(function_name)                                                                             \
    HRESULT MSEH_ExceptionOccured = S_OK;                                                                              \
    MSEH_STATIC_FUNCTION(function_name, NULL);                                                                         \
    try {
#define BEGIN_METHOD_DUMP(function_name, dump_obj)                                                                     \
    HRESULT MSEH_ExceptionOccured = S_OK;                                                                              \
    MSEH_FUNCTION(function_name, dump_obj);                                                                            \
    try {
#define BEGIN_STATIC_METHOD_DUMP(function_name, dump_obj)                                                              \
    HRESULT MSEH_ExceptionOccured = S_OK;                                                                              \
    MSEH_STATIC_FUNCTION(function_name, dump_obj);                                                                     \
    try {
    ////////////////////////////////////////////

#define END_METHOD()                                                                                                   \
    }                                                                                                                  \
    catch (const _com_error& hr) {                                                                                     \
        return hr.Error();                                                                                             \
    }                                                                                                                  \
    MSEH_BLOCK_RET_MFC()                                                                                               \
    catch (...) {                                                                                                      \
        return MSEH_ERROR(E_MS_UNHANDLED, _T("Unhandled exception"));                                                  \
    }

#define END_RES_METHOD(res)                                                                                            \
    }                                                                                                                  \
    catch (const _com_error&) {                                                                                        \
        return res;                                                                                                    \
    }                                                                                                                  \
    catch (...) {                                                                                                      \
        MSEH_ERROR(E_MS_UNHANDLED, _T("Unhandled exception"));                                                         \
        return res;                                                                                                    \
    }

#define END_VOID_METHOD()                                                                                              \
    }                                                                                                                  \
    catch (const _com_error&) {                                                                                        \
        return;                                                                                                        \
    }                                                                                                                  \
    catch (...) {                                                                                                      \
        MSEH_ERROR(E_MS_UNHANDLED, _T("Unhandled exception"));                                                         \
    }

#define END_NULL_METHOD() END_RES_METHOD(NULL)

    ////////////////////////////////////////////

#define MSEH_BLOCK_BEGIN()                                                                                             \
    try {                                                                                                              \
        MSEH_ExceptionOccured = S_OK;

#define MSEH_BLOCK_CATCH()                                                                                             \
    }                                                                                                                  \
    catch (const _com_error& ex) {                                                                                     \
        MSEH_ExceptionOccured = ex.Error();                                                                            \
    }                                                                                                                  \
    MSEH_BLOCK_CATCH_MFC()                                                                                             \
    catch (...) {                                                                                                      \
        MSEH_ExceptionOccured = MSEH_ERROR(E_MS_UNHANDLED, _T("Unhandled exception"));                                 \
    }                                                                                                                  \
    if (MSEH_ExceptionOccured != S_OK) {
#define MSEH_BLOCK_END()                                                                                               \
    }                                                                                                                  \
    MSEH_ExceptionOccured = S_OK;

// For MFC
#ifdef _AFX
#    define MSEH_BLOCK_CATCH_MFC()                                                                                     \
        catch (COleException * pEx) {                                                                                  \
            MSEH_ExceptionOccured = MSEH_ERROR(pEx->m_sc, _T("MFC"));                                                  \
            pEx->Delete();                                                                                             \
        }                                                                                                              \
        catch (CException * pEx) {                                                                                     \
            TCHAR szError[1000];                                                                                       \
            pEx->GetErrorMessage(szError, 999);                                                                        \
            if (pEx->GetRuntimeClass() != NULL) {                                                                      \
                strcat(szError, " class:");                                                                            \
                strcat(szError, pEx->GetRuntimeClass()->m_lpszClassName);                                              \
            }                                                                                                          \
            MSEH_ERROR_DUMP(E_UNEXPECTED, szError);                                                                    \
            pEx->Delete();                                                                                             \
        }

#    define MSEH_BLOCK_RET_MFC()                                                                                       \
        catch (COleException * pEx) {                                                                                  \
            MSEH_ExceptionOccured = MSEH_ERROR(pEx->m_sc, _T("MFC"));                                                  \
            pEx->Delete();                                                                                             \
            return MSEH_ExceptionOccured;                                                                              \
        }                                                                                                              \
        catch (CException * pEx) {                                                                                     \
            TCHAR szError[1000];                                                                                       \
            pEx->GetErrorMessage(szError, 999);                                                                        \
            if (pEx->GetRuntimeClass() != NULL) {                                                                      \
                strcat(szError, " class:");                                                                            \
                strcat(szError, pEx->GetRuntimeClass()->m_lpszClassName);                                              \
            }                                                                                                          \
            MSEH_ERROR_DUMP(E_UNEXPECTED, szError);                                                                    \
            pEx->Delete();                                                                                             \
            return E_UNEXPECTED;                                                                                       \
        }

#else
#    define MSEH_BLOCK_CATCH_MFC()
#    define MSEH_BLOCK_RET_MFC()
#endif

    ///////////////////////////////////////

#define MSEH_HANDLE_MFC_EXCEPTION()                                                                                    \
    catch (CException * pEx) {                                                                                         \
        TCHAR szError[1000];                                                                                           \
        pEx->GetErrorMessage(szError, 999);                                                                            \
        pEx->Delete();                                                                                                 \
        RAISE_ERR(E_UNEXPECTED, szError);                                                                              \
    }

///////////////////////////////////////

///////////////////////////////////////
#define START_ERROR_HANDLER()                                                                                          \
    goto DbgErrorHandlerStart;                                                                                         \
DbgErrorHandlerClean : {
#define END_ERROR_HANDLER(res)                                                                                         \
    return res;                                                                                                        \
    }                                                                                                                  \
DbgErrorHandlerStart:;

#define END_ERROR_HANDLER_VOID()                                                                                       \
    return;                                                                                                            \
    }                                                                                                                  \
DbgErrorHandlerStart:;

#define END_METHOD_HANDLER()                                                                                           \
    }                                                                                                                  \
    catch (const _com_error&) {                                                                                        \
        goto DbgErrorHandlerClean;                                                                                     \
    }                                                                                                                  \
    catch (...) {                                                                                                      \
        MSEH_ERROR(E_FAIL, _T("Unhandled exception"));                                                                 \
        goto DbgErrorHandlerClean;                                                                                     \
    }

}  // namespace MSEH

#endif  //__DBG_UTIL_H__
