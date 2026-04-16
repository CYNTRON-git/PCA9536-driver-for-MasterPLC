#ifndef __DBG_UTIL_SHORT_H__
#define __DBG_UTIL_SHORT_H__

#ifndef MSEH_FULL_SUPPORT

namespace MSEH //MasterSACDA error handling namespace
{

class HRes
{
protected:
	HRESULT hr;
public:
	HRes(HRESULT hr): hr (hr){}
	operator HRESULT() const {return hr;}
};

#define	THROWS_MSEH()

#define MSEH_ERROR(hr, st)	(OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "%s returns 0x%08X\n", st, hr), hr)

#define RAISE_ERR(hr, msg) \
	throw MSEH::HRes(MSEH_ERROR(hr, msg));

#define E_MS_CATCHED	MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x0666)
#define E_MS_UNHANDLED	MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x0667)

#ifdef _AFX

	#define RAISE_MSG(msg)	\
		{	\
			::AfxMessageBox(msg, MB_ICONSTOP);	\
			RAISE_ERR(E_MS_CATCHED, msg);	\
		}

	#define RAISE_MSG_ID(msg_id)	\
		{	\
			::AfxMessageBox(msg_id, MB_ICONSTOP);	\
			RAISE_ERR(E_MS_CATCHED, #msg_id);	\
		}

#endif

#define VERIFY_TEST(hres) \
{ \
	HRESULT hr_ = hres; \
	if (FAILED(hr_)) \
	    MSEH_ERROR(hr_, "\"" #hres "\""); \
}

#define VERIFY_RET(hres) \
{ \
	HRESULT hr_ = hres; \
	if (FAILED(hr_)) \
	    RAISE_ERR (hr_, "\"" #hres "\""); \
}

#define MSEH_ExcPtr(ptr, ex) \
{ \
	if (!(ptr)) \
	    RAISE_ERR(ex, "\"" #ptr "\""); \
}

#define MSEH_ChkPtr(ptr, ex) \
{ \
	if (!(ptr)) \
	    MSEH_ERROR(ex, "\"" #ptr "\""); \
}

#define PTR_TEST(ptr) MSEH_ChkPtr(ptr, E_NOINTERFACE)
#define PTR_RET(ptr) MSEH_ExcPtr(ptr, E_NOINTERFACE)

#define BOOL_TEST(ptr) MSEH_ChkPtr(ptr, E_INVALIDARG)
#define BOOL_RET(ptr) MSEH_ExcPtr(ptr, E_INVALIDARG)

#define IFOK(hres) \
	if (hr = (hres), (SUCCEEDED(hr) ? true : (MSEH_ERROR(hr, "\"" #hres "\""), false)))

#define IFOK_PTR(hres, ptr) \
	if (hr = (hres), ((SUCCEEDED(hr) && ptr) ? true	: (MSEH_ERROR(hr, "\"" #hres "\""), false)))

#define IF_FAILED(hres) \
	if (hr = (hres), (SUCCEEDED(hr) ? false	: (MSEH_ERROR(hr, "\"" #hres "\""), true)))

#define IF_FAILED_PTR(hres, ptr) \
	if (hr = (hres), ((SUCCEEDED(hr) && ptr) ? false : (MSEH_ERROR(hr, "\"" #hres "\""), true)))

#define BEGIN_METHOD(function_name) \
	HRESULT MSEH_ExceptionOccured = S_OK;	\
	try {

#define BEGIN_STATIC_METHOD(function_name) BEGIN_METHOD(function_name)

#define END_METHOD() } \
	catch (const MSEH::HRes &hr) \
	{ \
		return hr; \
	} \
	catch (...) \
	{ \
		return MSEH_ERROR(E_FAIL, "Unhandled exception"); \
	} 

#define END_RES_METHOD(res) } \
	catch (const MSEH::HRes &) \
	{ \
		return res; \
	} \
	catch (...) \
	{ \
		MSEH_ERROR(E_FAIL, "Unhandled exception"); \
		return res; \
	} 

#define END_VOID_METHOD() } \
	catch (const MSEH::HRes &) \
	{ \
		return; \
	} \
	catch (...) \
	{ \
		MSEH_ERROR (E_FAIL, "Unhandled exception"); \
	} 

#define END_NULL_METHOD() END_RES_METHOD(NULL)

#define MSEH_BLOCK_BEGIN() \
	try { MSEH_ExceptionOccured = S_OK;

#define MSEH_BLOCK_CATCH() \
	} \
	catch (const _com_error& ex) \
	{	MSEH_ExceptionOccured = ex.Error(); } \
	catch (...) \
	{	MSEH_ExceptionOccured = E_MS_UNHANDLED; } \
	if (MSEH_ExceptionOccured != S_OK) \
	{

#define MSEH_BLOCK_END() \
	} \
	MSEH_ExceptionOccured = S_OK;

}

#endif

#endif //__DBG_UTIL_SHORT_H__