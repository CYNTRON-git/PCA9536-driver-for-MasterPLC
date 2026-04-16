#ifndef __UTIL_H__
#define __UTIL_H__
//
#include <vector>
#include <string>
#include <sstream>
#include <math.h>
#include <atltime.h>
#include "commonstructs.h"
#include "dbg_util_short.h"

namespace MS //MasterSACDA namespace
{

inline void IncFileTime(FILETIME *pft, DWORD dwMilleseconds)
{
	*((ULONGLONG*) pft) += ((ULONGLONG)dwMilleseconds) * 10000;
}
inline void DecFileTime(FILETIME *pft, DWORD dwMilleseconds)
{
	*((ULONGLONG*) pft) -= ((ULONGLONG)dwMilleseconds) * 10000;
}
inline DWORD DiffFileTime (FILETIME *pft1, FILETIME *pft2)
{
	LONGLONG diff = 
		(*((LONGLONG*)pft1) - *((LONGLONG*)pft2)) / 10000;
	if (diff < 0)
		return 0;
	_ASSERTE (diff <= MAXLONG);
	return (diff <= MAXLONG) ? diff : 0;
}

#define ARRAY_SIZE(T)	(sizeof(T)/sizeof(T[0]))

struct CFlagSet
{
	bool& bFlag;
	bool bOldValue;
	CFlagSet(bool& b, bool bNewValue = true) : bFlag(b)
	{
		bOldValue = bFlag;
		bFlag = bNewValue;
	}
	~CFlagSet()
	{
		bFlag = bOldValue;
	}
};

struct CAssignValue
{
	long* plValue;
	long lOldValue;
	CAssignValue(long* pl, long l) : plValue(pl)
	{
		lOldValue = *plValue;
		*plValue = l;
	}
	~CAssignValue()
	{
		*plValue = lOldValue;
	}
};

struct CIncrementValue
{
	int* pnValue;
	CIncrementValue(int* pl) : pnValue(pl)
	{
		(*pnValue)++;
	}
	~CIncrementValue()
	{
		(*pnValue)--;
	}
};

//created and destroyed in the base thread
//used in the work or in the base thread
template <class T>
class CThreadPtr
{
public:
	CComQIPtr<T> pBase;
	CComPtr<T> pWork;	//weak

	CThreadPtr()
	{
		piMarshalStm = NULL;
		pWork = NULL;
		dwBaseThreadId = 0;
	}
	~CThreadPtr()
	{
		Release();
	}
	void Release()
	{
		HRESULT hr;
		pBase.Release();
		pWork.Release();
		if (piMarshalStm)
		{
			hr = ::CoReleaseMarshalData (piMarshalStm);
			piMarshalStm->Release();
			piMarshalStm = NULL;
		}
		dwBaseThreadId = 0;
	}

	T* operator = (IUnknown* lp)
	{
		_ASSERTE (!pBase);
		dwBaseThreadId = ::GetCurrentThreadId();

		pBase = lp;
		Marshal();

		return pBase.p;
	}

	bool operator ! ()
	{
		return pBase.p == NULL;
	}

	_NoAddRefReleaseOnCComPtr<T>* operator->()
	{
		T* res = p();
		_ASSERTE (res != NULL);
		return (_NoAddRefReleaseOnCComPtr<T>*)res;
	}

	operator T*()
	{
		return p();
	}

	T* p()
	{
		DWORD dwThreadId = ::GetCurrentThreadId();
		if (dwThreadId == dwBaseThreadId)
		{
			return pBase.p;
		}
		else
		{
			HRESULT hr = Unmarshal();
			return pWork.p;
		}		
	}

	CComQIPtr<T> &work()
	{
		HRESULT hr = Unmarshal();
		_ASSERTE (pWork.p != NULL);
		return pWork;
	}
	HRESULT Marshal()
	{
		if (piMarshalStm)
			return S_OK;
		if (!pBase)
			return E_FAIL;
		
		HRESULT hr  = ::CoMarshalInterThreadInterfaceInStream 
			( __uuidof(T)
			, pBase
			, &piMarshalStm);
		return hr;
	}
	HRESULT Unmarshal()
	{
		if (pWork)
			return S_OK;
		if (!piMarshalStm)
			return E_FAIL;
		
		HRESULT hr  = ::CoGetInterfaceAndReleaseStream
			( piMarshalStm
			, __uuidof(T)
			, (void**) &pWork);
		piMarshalStm = NULL;
		return hr;
	}
private:
	IStream* piMarshalStm;
	DWORD dwBaseThreadId;
};

class CComAutoCritSection : public CComAutoCriticalSection
{
public:
	CComAutoCritSection(){}
	CComAutoCritSection(const CComAutoCritSection& other)
	{
		CComAutoCritSection();
	}
};

class CComEvent
{
public:
	HANDLE h;
	BOOL m_bManualReset, m_bInitialState;
	CComEvent()
	{
		h = NULL;
	}
	CComEvent (BOOL bManualReset, BOOL bInitialState)
		: m_bManualReset (bManualReset), m_bInitialState (bInitialState)
	{
		h = ::CreateEvent (NULL, bManualReset, bInitialState, NULL);
	}
	~CComEvent()
	{
		::CloseHandle (h);
	}
	CComEvent (const CComEvent& src)
	{
		*this = src;
	}
	CComEvent& operator = (const CComEvent& src)
	{
		if (src.h)
		{
			m_bManualReset = src.m_bManualReset;
			m_bInitialState = src.m_bInitialState;
			h = ::CreateEvent (NULL
				, m_bManualReset
				, m_bInitialState
				, NULL);
		}
		else
			h = NULL;
		return *this;
	}
	operator HANDLE ()
	{
		_ASSERTE (h);
		return h;
	}
};

class CComOLESTR
{
public:
	CComOLESTR():m_str(NULL)
	{
	}
	operator LPCWSTR() const
	{
		return m_str;
	}
	LPWSTR* operator&()
	{
		return &m_str;
	}
	~CComOLESTR()
	{
		Empty();
	}
	void Empty()
	{
		if (m_str)
			::CoTaskMemFree (m_str);
		m_str = NULL;
	}
	inline bool operator == (const CComBSTR &bstr) const
	{
		return (bstr == (BSTR) m_str);
	}
	
	//protected:
	LPWSTR m_str;
};

inline TCHAR GetDecimalPoint()
{
	TCHAR szBuff[10];
	if (!GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, szBuff, 10))
		return _T(',');
	return szBuff[0];
}

inline HRESULT ChangeVarType(CComVariant &var, VARTYPE vt, VARIANT* pvarSrc = NULL)
{
	CComVariant varTemp;

	if (pvarSrc == NULL)
		pvarSrc = &var;
	if ((vt == VT_R8 || vt == VT_R4) && pvarSrc->vt == VT_BSTR)
	{
		//special case
		TCHAR chDecimal = GetDecimalPoint();
		TCHAR chOther = (chDecimal == _T('.')) ? _T(',') : _T('.');

		CString str(pvarSrc->bstrVal);
		if (str.Replace(chOther, chDecimal) > 0)
		{
			//Replace pointer to the source string
			varTemp = str;
			pvarSrc = &varTemp;
		}
	}
	//analog -> bool
	else if (vt == VT_BOOL && IsAnalogType(pvarSrc->vt))
	{
		if (pvarSrc->vt == VT_R4 || pvarSrc->vt == VT_R8 || pvarSrc->vt == VT_DECIMAL)
		{
			var.ChangeType(VT_R8, pvarSrc);
			var = (var.dblVal != 0.0);
		}
		else
		{
			var.ChangeType(VT_I4, pvarSrc);
			var = (var.lVal != 0);
		}
		return S_OK;
	}
	//bool -> analog
	else if (pvarSrc->vt == VT_BOOL && IsAnalogType(vt))
	{
		var = (pvarSrc->boolVal != VARIANT_FALSE) ? 1L : 0L;
		return var.ChangeType(vt);
	}
#ifdef __ATLCOMTIME_H__
	else if (pvarSrc->vt == VT_DATE && vt == VT_BSTR)
	{
		COleDateTime dateTime(*pvarSrc);
		SYSTEMTIME system_time;
		dateTime.GetAsSystemTime(system_time);

		CString strText;
		if ((DATE)dateTime < 1)
			strText.Format(_T("%02hd:%02hd:%02hd"), system_time.wHour, system_time.wMinute, system_time.wSecond);
		else if (fabs((DATE)dateTime - (long)(DATE)dateTime) < 1.0/24.0/3600.0/1000.0)
			strText.Format(_T("%02hd.%02hd.%04hd"), system_time.wDay, system_time.wMonth, system_time.wYear);
		else
		{
			strText.Format(_T("%02hd.%02hd.%04hd %02hd:%02hd:%02hd"),
				system_time.wDay, system_time.wMonth, system_time.wYear, 
				system_time.wHour, system_time.wMinute, system_time.wSecond);
		}

		var = strText;
		return S_OK;
	}
#endif

	return var.ChangeType(vt, pvarSrc);
}

inline double StringToDouble(LPCTSTR szValue)
{
	CComVariant varValue = szValue;
	if (FAILED(ChangeVarType(varValue, VT_R8)))
		return false;
	return varValue.dblVal;
}

inline CString GetComObjectName(REFCLSID clsid)
{
	CString strName;
	LPWSTR wszUserType;
	if (::OleRegGetUserType (clsid, USERCLASSTYPE_FULL, &wszUserType) == S_OK)
	{
		strName = wszUserType;
		::CoTaskMemFree (wszUserType);
	}
	if (strName.Find(_T("Class")) >= 0 || strName.IsEmpty())
	{
		LPWSTR wszProgID;
		if (::ProgIDFromCLSID(clsid, &wszProgID) != S_OK)
			return CString();

		strName = wszProgID;
		::CoTaskMemFree (wszProgID);

		CRegKey regKey;
		if (regKey.Open (HKEY_CLASSES_ROOT, strName, KEY_READ) == ERROR_SUCCESS)
		{
			ULONG nCount = 300;
			CString strVal;
			regKey.QueryValue (strVal.GetBuffer (300), NULL, &nCount);
			strVal.ReleaseBuffer();

			if (strVal.Find(_T("Class")) < 0 && !strVal.IsEmpty())
				strName = strVal;
		}
	}
	return strName;
}

inline BOOL GetVarValueStr(const VARIANT* pvarValue, CString& strValue, short nDigits = 3)
{
	HRESULT hr;
	CString format;
	bool bExpForm = nDigits < 0;
	if (nDigits < 0)
		nDigits = -nDigits;

	if (pvarValue->vt == VT_R4 || pvarValue->vt == VT_R8)
	{
		if (bExpForm)
			format.Format(_T("%%.%de"), nDigits);
		else
			format.Format(_T("%%.%dlf"), nDigits);
	}

	if ((pvarValue->vt & VT_ARRAY) != 0)
	{
		SAFEARRAY* psa = pvarValue->parray;

		VARTYPE vtBase;
		if (FAILED(hr = ::SafeArrayGetVartype(psa, &vtBase)))
			return FALSE;

		strValue = _T("{");
		for (long i = 0; i < psa->rgsabound[0].cElements; i++)
		{
			CComVariant varValue;
			if (FAILED(hr = ::SafeArrayGetElement(psa, &i, &varValue.bVal)))
				continue;
			varValue.vt = vtBase;

			if (i > 0)
				strValue += _T(", ");

			varValue.ChangeType(VT_BSTR);
			if (varValue.vt == VT_BSTR)
				strValue += CString(varValue.bstrVal);
		}
		strValue += _T("}");
		return TRUE;
	}

	switch (pvarValue->vt)
	{
		case VT_BOOL:
			if (pvarValue->boolVal)
				strValue = _T("Вкл");
			else 
				strValue = _T("Выкл");
			break;
		case VT_ERROR:
			strValue = _T("Ошибка");
			break;
		case VT_R4:
			strValue.Format(format, (double)pvarValue->fltVal);
			break;
		case VT_R8:
			strValue.Format(format, pvarValue->dblVal);
			break;
		case VT_UNKNOWN:
		case VT_DISPATCH:
			strValue = _T(" ");
			break;
		case VT_ARRAY:
			{
				int iDim=::SafeArrayGetDim(pvarValue->parray);
				strValue.Format(_T("Размерность %d"), iDim);
				break;	
			}
		default:
			{
				CComVariant varVal;
				varVal.ChangeType(VT_BSTR, pvarValue);
				if (varVal.vt == VT_BSTR)
					strValue = varVal.bstrVal;
				else
				{
					strValue = _T("");
					return FALSE;
				}
			}
	}

	if (pvarValue->vt != VT_BSTR)
		strValue.TrimLeft();
	return TRUE;
}

inline BOOL GetVarValueStr(const VARIANT* pvarValue, CString& strValue, CString strFormat)
{
	HRESULT hr;

	if ((pvarValue->vt & VT_ARRAY) != 0)
	{
		SAFEARRAY* psa = pvarValue->parray;

		VARTYPE vtBase;
		if (FAILED(hr = ::SafeArrayGetVartype(psa, &vtBase)))
			return FALSE;

		strValue = _T("{");
		for (long i = 0; i < psa->rgsabound[0].cElements; i++)
		{
			CComVariant varValue;
			if (FAILED(hr = ::SafeArrayGetElement(psa, &i, &varValue.bVal)))
				continue;
			varValue.vt = vtBase;

			if (i > 0)
				strValue += _T(", ");

			varValue.ChangeType(VT_BSTR);
			if (varValue.vt == VT_BSTR)
				strValue += CString(varValue.bstrVal);
		}
		strValue += _T("}");
		return TRUE;
	}

	if (strFormat.GetLength() == 0)
		strFormat = _T("f3");

	if (IsAnalogType(pvarValue->vt))
	{
		double fVal;
		long lVal;
		bool bDouble = pvarValue->vt == VT_R4 || pvarValue->vt == VT_R8;
		if (bDouble)
			fVal = pvarValue->vt == VT_R4 ? (double)pvarValue->fltVal : pvarValue->dblVal;
		else
		{
			CComVariant varVal;
			varVal.ChangeType(VT_I4, pvarValue);
			lVal = varVal.lVal;
		}

		CString format;
		int nPrecision = _ttol(strFormat.Mid(1));
		if (nPrecision < 0)
			nPrecision = 0;
		switch (strFormat[0])
		{
		case 'e':
		case 'E':
			format.Format(_T("%%.%d%c"), nPrecision, strFormat[0]);
			if (!bDouble)
			{
				bDouble = true;
				if (pvarValue->vt == VT_UI4)
					fVal = pvarValue->ulVal;
				else
					fVal = lVal;
			}
			break;

		default:
			if (bDouble)
				format.Format(_T("%%.%dlf"), nPrecision);
			else
				format = pvarValue->vt == VT_UI4 ? _T("%u") : _T("%d");
			break;
		}
		if (bDouble)
			strValue.Format(format, fVal);
		else if (pvarValue->vt == VT_UI4)
			strValue.Format(format, pvarValue->ulVal);
		else 
			strValue.Format(format, lVal);

		if ((strFormat[0] == 'e' || strFormat[0] == 'E') && strValue[strValue.GetLength() - 3] == '0')
			strValue.Delete(strValue.GetLength() - 3);

		return TRUE;
	}

	switch (pvarValue->vt)
	{
		case VT_BOOL:
			if (pvarValue->boolVal)
				strValue = _T("Вкл");
			else 
				strValue = _T("Выкл");
			break;
		case VT_ERROR:
			strValue = _T("Ошибка");
			break;
		case VT_UNKNOWN:
		case VT_DISPATCH:
			strValue = _T(" ");
			break;
		default:
			{
				CComVariant varVal;
				varVal.ChangeType(VT_BSTR, pvarValue);
				if (varVal.vt == VT_BSTR)
					strValue = varVal.bstrVal;
				else
				{
					strValue = _T("");
					return FALSE;
				}
			}
	}

	if (pvarValue->vt != VT_BSTR)
		strValue.TrimLeft();
	return TRUE;
}

template<class T>
inline HRESULT ReadFromStream (IStream *istm, T&t)
{
	ULONG tmp;
	return istm->Read ((void*)&t, sizeof (T), &tmp);
}

template<class T>
inline HRESULT WriteToStream (IStream *istm, T t)
{
	ULONG tmp;
	return istm->Write ((void*)&t, sizeof (T), &tmp);
}

template<>
inline HRESULT ReadFromStream<CComBSTR> (IStream *istm, CComBSTR &t)
{
	CComBSTR str;
	HRESULT hr = str.ReadFromStream (istm);
	t = str;
	return hr;
}

template<>
inline HRESULT WriteToStream<CComBSTR> (IStream *istm, CComBSTR t)
{
	return t.WriteToStream (istm);
}

inline HRESULT ReadVarFromStream(IStream *pStream, CComVariant &t)
{
	ATLASSERT(pStream != NULL);
	HRESULT hr;
	hr = t.Clear();
	if (FAILED(hr))
		return hr;
	VARTYPE vtRead;
	hr = pStream->Read(&vtRead, sizeof(VARTYPE), NULL);
	if (hr == S_FALSE)
		hr = E_FAIL;
	if (FAILED(hr))
		return hr;

	t.vt = vtRead;
	int cbRead = 0;
	switch (vtRead)
	{
	case VT_UNKNOWN:
	case VT_DISPATCH:
		{
			t.punkVal = NULL;

			CLSID clsid;
			hr = ::ReadClassStm(pStream, &clsid);
			if (FAILED(hr))
				return hr;

			if (clsid == GUID_NULL)
				return S_OK;

			CComPtr<IUnknown> pUnk;
			hr = ::CoCreateInstance(clsid, NULL, CLSCTX_INPROC_SERVER, IID_IUnknown, (void**)&pUnk);
			if (FAILED(hr))
			{
				CString str;
				str.Format(_T("Ошибка при создании %s"), MS::GetComObjectName(clsid));
				MSEH_ERROR (hr, str);
				return hr;
			}

			CComQIPtr<IPersistStreamInit> spStreamInit(pUnk);
			if (spStreamInit)
			{
				hr = spStreamInit->Load(pStream);
			}
			else
			{
				CComQIPtr<IPersistStream> spStream(pUnk);
				if (!spStream)
					return E_NOINTERFACE;
				hr = spStream->Load(pStream);
			}
			if (FAILED(hr))
			{
				CString str;
				str.Format(_T("Ошибка при загрузке %s"), GetComObjectName(clsid));
				MSEH_ERROR (hr, str);
				return hr;
			}

			hr = pUnk->QueryInterface((vtRead == VT_UNKNOWN) ? IID_IUnknown : IID_IDispatch, (void**)&t.punkVal);
			if (FAILED(hr))
			{
				CString str;
				str.Format(_T("Ошибка при приведении к IDispatch %s"), GetComObjectName(clsid));
				MSEH_ERROR (hr, str);
				return hr;
			}
			return S_OK;
		}
	case VT_UI1:
	case VT_I1:
		cbRead = sizeof(BYTE);
		break;
	case VT_I2:
	case VT_UI2:
	case VT_BOOL:
		cbRead = sizeof(short);
		break;
	case VT_I4:
	case VT_UI4:
	case VT_R4:
	case VT_INT:
	case VT_UINT:
	case VT_ERROR:
		cbRead = sizeof(long);
		break;
	case VT_I8:
	case VT_UI8:
		cbRead = sizeof(LONGLONG);
		break;
	case VT_R8:
	case VT_CY:
	case VT_DATE:
		cbRead = sizeof(double);
		break;
	default:
		break;
	}
	if (cbRead != 0)
	{
		hr = pStream->Read((void*) &t.bVal, cbRead, NULL);
		if (hr == S_FALSE)
			hr = E_FAIL;
		return hr;
	}
	CComBSTR bstrRead;

	hr = bstrRead.ReadFromStream(pStream);
	if (FAILED(hr))
		return hr;
	t.vt = VT_BSTR;
	t.bstrVal = bstrRead.Detach();
	if (vtRead != VT_BSTR)
		hr = t.ChangeType(vtRead);
	return hr;
}

template<>
inline HRESULT ReadFromStream<CComVariant> (IStream *istm, CComVariant &t)
{
	return ReadVarFromStream(istm, t);
}

inline HRESULT WriteVarToStream(IStream *pStream, const CComVariant &t)
{
	HRESULT hr;
	if (t.vt == VT_UNKNOWN || t.vt == VT_DISPATCH)
	{
		CComQIPtr<IPersistStreamInit> spStreamInit(t.punkVal);
		CComQIPtr<IPersistStream> spStream(t.punkVal);
		if (spStream || spStreamInit)
		{
			hr = pStream->Write (&t.vt, sizeof (VARTYPE), NULL);
			if (FAILED(hr))
				return hr;

			CLSID clsid;
			if (spStreamInit)
				hr = spStreamInit->GetClassID (&clsid);
			else
				hr = spStream->GetClassID (&clsid);
			if (FAILED (hr))
				return hr;
			hr = WriteClassStm (pStream, clsid);
			if (FAILED (hr))
				return hr;
			
			if (spStreamInit)
				hr = spStreamInit->Save(pStream, TRUE);
			else
				hr = spStream->Save(pStream, TRUE);
			return hr;
		}
	}
	return const_cast<CComVariant &>(t).WriteToStream (pStream);
}
template<>
inline HRESULT WriteToStream<const CComVariant &> 
							(IStream *istm, const CComVariant &t)
{
	return WriteVarToStream (istm, t);
}

template<>
inline HRESULT WriteToStream<CComVariant &> 
							(IStream *istm, CComVariant &t)
{
	return WriteVarToStream (istm, t);
}

template<>
inline HRESULT WriteToStream<CComVariant> 
							(IStream *istm, CComVariant t)
{
	return WriteVarToStream (istm, t);
}

template<>
inline HRESULT ReadFromStream<CString> (IStream *istm, CString &t)
{
	CComBSTR str;
	HRESULT hr = str.ReadFromStream (istm);
	t = str;
	return hr;
}

template<>
inline HRESULT WriteToStream<CString> (IStream *istm, CString t)
{
	CComBSTR str = t.GetBuffer();
	return str.WriteToStream (istm);
}

inline HRESULT WriteObjectToStream(IStream *istm, IUnknown *iunk, bool clear_dirty)
{
	CComQIPtr<IPersistStreamInit> ipersist_init(iunk);
	if (ipersist_init)
	{
		return ipersist_init->Save(istm, clear_dirty);
	}

	CComQIPtr<IPersistStream> persist(iunk);
	if (persist)
	{
		return persist->Save(istm, clear_dirty);
	}

	return E_NOINTERFACE;
}

inline HRESULT ReadObjectFromStream(IStream *istm, IUnknown *iunk)
{
	CComQIPtr<IPersistStreamInit> ipersist_init (iunk);
	if (ipersist_init)
	{
		return ipersist_init->Load (istm);
	}

	CComQIPtr<IPersistStream> persist (iunk);
	if (persist)
	{
		return persist->Load (istm);
	}

	return E_NOINTERFACE;
}

inline bool IsObjectDirty (IUnknown *iunk)
{
	CComQIPtr<IPersistStreamInit> ipersist_init (iunk);
	if (ipersist_init)
	{
		return ipersist_init->IsDirty() == S_OK;
	}

	CComQIPtr<IPersistStream> ipersist (iunk);
	if (ipersist)
	{
		return ipersist->IsDirty() == S_OK;
	}

	CComQIPtr<IPersistStorage> ipersist_stg (iunk);
	if (ipersist_stg)
	{
		return ipersist_stg->IsDirty() == S_OK;
	}

	return false;
}

///////Variant edition
template<class T>
inline HRESULT WriteToStreamV (IStream *istm, T t)
{
	CComVariant var = t;
	return MS::WriteToStream<CComVariant>(istm, var);
}

inline HRESULT ReadFromStreamV (IStream *istm, long &l)
{
	CComVariant var;
	HRESULT hr = var.ReadFromStream (istm);
	if (FAILED (hr))
		return hr;
	if (var.vt != VT_I4)
		return E_FAIL;

	l = var.lVal;
	return S_OK;
}

inline HRESULT ReadFromStreamV (IStream *istm, double &d)
{
	CComVariant var;
	HRESULT hr = var.ReadFromStream (istm);
	if (FAILED (hr))
		return hr;
	if (var.vt != VT_R8)
		return E_FAIL;

	d = var.dblVal;
	return S_OK;
}

inline HRESULT ReadFromStreamV (IStream *istm, bool &b)
{
	CComVariant var;
	HRESULT hr = var.ReadFromStream (istm);
	if (FAILED (hr))
		return hr;
	if (var.vt != VT_BOOL)
		return E_FAIL;

	b = (var.boolVal) ? true : false;
	return S_OK;
}

inline HRESULT ReadFromStreamV (IStream *istm, short &s)
{
	CComVariant var;
	HRESULT hr = var.ReadFromStream (istm);
	if (FAILED (hr))
		return hr;
	if (var.vt != VT_I2)
		return E_FAIL;

	s = var.iVal;
	return S_OK;
}

inline HRESULT ReadFromStreamV (IStream *istm, float &f)
{
	CComVariant var;
	HRESULT hr = var.ReadFromStream (istm);
	if (FAILED (hr))
		return hr;
	if (var.vt != VT_R4)
		return E_FAIL;

	f = var.fltVal;
	return S_OK;
}

inline HRESULT CreateStream (IStorage* pStorage
							 , LPCWSTR wszName
							 , IStream** ppStream
							 , BOOL bTryOpen = TRUE)
{
	HRESULT hr;
	if (pStorage == NULL)
		return E_POINTER;
	if (bTryOpen)
	{
		hr = pStorage->OpenStream (wszName
			, NULL
			, STGM_READWRITE | STGM_SHARE_EXCLUSIVE
			, 0
			, ppStream);
		if (SUCCEEDED (hr))
			return S_OK;
	}

	hr = pStorage->CreateStream (wszName
		, STGM_CREATE | STGM_READWRITE | STGM_SHARE_EXCLUSIVE
		, 0
		, 0
		, ppStream);
	return  hr;
}

inline HRESULT OpenStream (IStorage* pStorage
						   , LPCWSTR wszName
						   , IStream** ppStream
						   , bool bReadOnly = true)
{
	HRESULT hr;
	if (pStorage == NULL)
		return E_POINTER;
	DWORD dwMode;
	if (bReadOnly)
		dwMode = STGM_READ | STGM_SHARE_EXCLUSIVE;
	else
		dwMode = STGM_READWRITE | STGM_SHARE_EXCLUSIVE;

	hr = pStorage->OpenStream (wszName
		, NULL
		, dwMode
		, 0
		, ppStream);
	return  hr;
}

inline HRESULT CreateStorage (IStorage* pStorage
							 , LPCWSTR wszName
							 , IStorage** ppNewStg
							 , BOOL bTryOpen = TRUE)
{
	HRESULT hr;
	if (pStorage == NULL)
		return E_POINTER;
	if (bTryOpen)
	{
		hr = pStorage->OpenStorage (wszName
			, NULL
			, STGM_READWRITE | STGM_SHARE_EXCLUSIVE
			, NULL
			, 0
			, ppNewStg);
		if (SUCCEEDED (hr))
			return S_OK;
	}

	hr = pStorage->CreateStorage (wszName
		, STGM_CREATE | STGM_READWRITE | STGM_SHARE_EXCLUSIVE
		, 0
		, 0
		, ppNewStg);
	return  hr;
}

inline HRESULT OpenStorage (IStorage* pStorage
						   , LPCWSTR wszName
						   , IStorage** ppNewStg
						   , bool bReadOnly = true)
{
	HRESULT hr;
	if (pStorage == NULL)
		return E_POINTER;
	DWORD dwMode;
	if (bReadOnly)
		dwMode = STGM_READ | STGM_SHARE_EXCLUSIVE;
	else
		dwMode = STGM_READWRITE | STGM_SHARE_EXCLUSIVE;

	hr = pStorage->OpenStorage (wszName
		, NULL
		, dwMode
		, NULL
		, 0
		, ppNewStg);
	return  hr;
}
inline void FastVariantCopy (VARIANT* var, const VARIANT* varSrc)
{
	switch (var->vt)
	{
	case VT_BSTR:
		var->vt = VT_EMPTY;
		SysFreeString (var->bstrVal);
		break;
	case VT_DISPATCH:
	case VT_UNKNOWN:
		var->vt = VT_EMPTY;
		if (var->punkVal)
			var->punkVal->Release();
		break;
	}

	VARTYPE vt = varSrc->vt;
	switch (vt)
	{
	case VT_I4:
		var->lVal = varSrc->lVal;
		break;
	case VT_R8:
		var->dblVal = varSrc->dblVal;
		break;
	case VT_BOOL:
		var->boolVal = varSrc->boolVal;
		break;
	case VT_R4:
		var->fltVal = varSrc->fltVal;
		break;
	case VT_BSTR:
		var->bstrVal = ::SysAllocString (varSrc->bstrVal);
		break;
	case VT_I2:
		var->iVal = varSrc->iVal;
		break;
	case VT_ERROR:
		var->scode = varSrc->scode;
		break;
	case VT_DISPATCH:
	case VT_UNKNOWN:
		var->punkVal = varSrc->punkVal;
		var->punkVal->AddRef();
		break;
	case VT_UI1:
		var->bVal = varSrc->bVal;
		break;
	case VT_EMPTY:
	case VT_NULL:
		break;
	case VT_DATE:
		var->date = varSrc->date;
		break;
	case VT_CY:
		var->cyVal.int64 = varSrc->cyVal.int64;
		break;
	case VT_I1:
		var->cVal = varSrc->cVal;
		break;
	case VT_UI2:
		var->uiVal = varSrc->uiVal;
		break;
	case VT_UI4:
		var->ulVal = varSrc->ulVal;
		break;
	case VT_INT:
		var->intVal = varSrc->intVal;
		break;
	case VT_UINT:
		var->uintVal = varSrc->uintVal;
		break;
	case VT_DECIMAL:
		var->decVal = varSrc->decVal;
		break;
	}
	var->vt = vt;
}
inline bool IsVarsEqual(const VARIANT& var, const VARIANT& varSrc)
{
	// Variants not equal if types don't match
	if (var.vt != varSrc.vt)
		return false;
	
	// Check type specific values
	switch (var.vt)
	{
	case VT_EMPTY:
	case VT_NULL:
		return true;
		
	case VT_I4:
		return var.lVal == varSrc.lVal;
	case VT_R8:
		return var.dblVal == varSrc.dblVal;
	case VT_R4:
		return var.fltVal == varSrc.fltVal;
	case VT_BOOL:
		return var.boolVal == varSrc.boolVal;
	case VT_UI1:
		return var.bVal == varSrc.bVal;
	case VT_I2:
		return var.iVal == varSrc.iVal;
	case VT_BSTR:
		return		   ::SysStringByteLen (var.bstrVal) 
					== ::SysStringByteLen (varSrc.bstrVal)
				&&	   ::memcmp ( var.bstrVal
					  		    , varSrc.bstrVal
							    , ::SysStringByteLen (var.bstrVal)) 
					== 0;
	case VT_ERROR:
		return var.scode == varSrc.scode;
	case VT_DISPATCH:
		return var.pdispVal == varSrc.pdispVal;
	case VT_UNKNOWN:
		return var.punkVal == varSrc.punkVal;
	case VT_DATE:
		return var.date == varSrc.date;
	case VT_CY:
		return var.cyVal.int64 == varSrc.cyVal.int64;
	case VT_I1:
		return var.cVal == varSrc.cVal;
	case VT_UI2:
		return var.uiVal == varSrc.uiVal;
	case VT_UI4:
		return var.ulVal == varSrc.ulVal;
	case VT_INT:
		return var.intVal == varSrc.intVal;
	case VT_UINT:
		return var.uintVal == varSrc.uintVal;
	}
	return false;
}

inline HRESULT CloneObject(IUnknown* pUnkObject, IUnknown** ppUnkCopy)
{
	CComQIPtr<IPersistStreamInit> piPersist (pUnkObject);
	if (!piPersist)
		return E_POINTER;

	HGLOBAL hg = GlobalAlloc (GMEM_MOVEABLE, 0);
	CComPtr<IStream> pStream;
	HRESULT hr = CreateStreamOnHGlobal (hg, TRUE, &pStream);
	if (FAILED (hr))
		return hr;

	CLSID clsid;
	hr = piPersist->GetClassID(&clsid);
	if (FAILED (hr))
		return hr;
	hr = piPersist->Save(pStream, FALSE);
	if (FAILED (hr))
		return hr;

	CComPtr<IPersistStreamInit> piPersistCopy;
	hr = piPersistCopy.CoCreateInstance (clsid);
	if (FAILED (hr))
		return hr;

	LARGE_INTEGER dlibMove;
	dlibMove.QuadPart = 0;
	hr = pStream->Seek (dlibMove, STREAM_SEEK_SET, NULL);
	if (FAILED (hr))
		return hr;

	hr = piPersistCopy->Load (pStream);
	if (FAILED (hr))
		return hr;

	*ppUnkCopy = piPersistCopy;
	(*ppUnkCopy)->AddRef();
	return S_OK;
}

template <class T>
inline CComPtr<T> CloneObjectEx(T* pSource)
{
	THROWS_MSEH();

	CComPtr<IUnknown> pUnkMnemoInfoCopy;
	VERIFY_RET (MS::CloneObject(pSource, &pUnkMnemoInfoCopy));
	CComQIPtr<T> pCopy(pUnkMnemoInfoCopy);
	PTR_RET (pCopy);
	return pCopy;
}

class CComMemoryStorage
{
public:
	HRESULT Create()
	{
		HRESULT hr;

		Destroy();

		HGLOBAL hg = ::GlobalAlloc(GMEM_MOVEABLE, 0);
		hr = ::CreateILockBytesOnHGlobal(hg, FALSE, &pLockBytes);
		if (FAILED(hr))
			return hr;

		hr = ::StgCreateDocfileOnILockBytes(pLockBytes,
			STGM_CREATE | STGM_READWRITE | STGM_SHARE_EXCLUSIVE, 0, &pStg);
		if (FAILED(hr))
			return hr;

		return S_OK;
	}

	void Destroy()
	{
		pStg.Release();
		pLockBytes.Release();
	}

	IStorage* operator->() const
	{
		return pStg;
	}
	operator IStorage*() const
	{
		return pStg;
	}

protected:
	CComPtr<IStorage> pStg;
	CComPtr<ILockBytes> pLockBytes;
};

inline CString GetExePath()
{
	CString strPath;
	::GetModuleFileName(NULL, strPath.GetBuffer(_MAX_PATH), _MAX_PATH);
	strPath.ReleaseBuffer();

	int nPoint = strPath.ReverseFind(_T('\\'));
	_ASSERTE (nPoint >= 0);
	if (nPoint < 0)
		return _T("");
	return strPath.Left(nPoint);
}

inline CString CorrectPath(LPCTSTR szPath)
{
	CString strRes(szPath);
	bool bNetworkName = strRes.GetLength() >= 2 && strRes[0] == _T('\\') && strRes[1] == _T('\\');

	while (strRes.Replace(_T("/"), _T("\\")) > 0);
	while (strRes.Replace(_T("\\\\"), _T("\\")) > 0);

	if (bNetworkName)
		strRes.Insert(0, _T('\\')); //restore leading prefix ("\\\\")

	return strRes;
}

inline CString MSGetShortPath(LPCTSTR szPath, int nReserve = 40)
{
	CString strPath = MS::CorrectPath(szPath);
	if (strPath.GetLength() < MAX_PATH - nReserve)
		return strPath;

	int nPoint = strPath.ReverseFind(_T('\\'));
	CString strFile = strPath.Mid(nPoint + 1);
	strPath = strPath.Left(nPoint + 1);
	//strPath end by /
	
	CString strNewPath, strShortPath;
	DWORD dw;
	
	while(1)
	{
		nPoint = strPath.Find(_T('\\'));
		if (nPoint < 0)
			break; //strPath empty
		
		strNewPath += strPath.Left(nPoint + 1);
		strPath = strPath.Mid(nPoint + 1);
		//strNewPath end by /
		
		dw = ::GetShortPathName(strNewPath, strShortPath.GetBuffer(MAX_PATH), MAX_PATH);
		if (dw == 0)
			return strNewPath + strPath + strFile;
		strShortPath.ReleaseBuffer();
		strNewPath = strShortPath;
	}
	
	return strShortPath + strFile;
}

inline void ClearReadOnlyAttr(LPCTSTR cszPath)
{
	DWORD dwCurAttr = ::GetFileAttributes(cszPath);
	if (dwCurAttr != 0xFFFFFFFF && (dwCurAttr & FILE_ATTRIBUTE_READONLY) != 0)
		::SetFileAttributes(cszPath, dwCurAttr & ~FILE_ATTRIBUTE_READONLY);
}

inline bool CleanupFolder(LPCTSTR cszDir, CString& strDenied, bool bSelfRemove = false)
{
	CString strNewPath;
	strNewPath = MS::MSGetShortPath(cszDir);
	if (!::SetCurrentDirectory(strNewPath))
		return true;

	WIN32_FIND_DATA fd;
	HANDLE hf = ::FindFirstFile(_T("*.*"), &fd);
	bool find = true;
	while (find && hf != INVALID_HANDLE_VALUE)
	{
		bool is_dir = (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

		CString strName;
		if (fd.cAlternateFileName != NULL && fd.cAlternateFileName[0] != 0)
			strName = fd.cAlternateFileName;
		else
			strName = fd.cFileName;

		CString strFullName = fd.cFileName;
		if (!FindNextFile(hf, &fd))
			find = false;

		if (strName == "." || strName == "..")
			continue;

		if (is_dir)
		{
			if (!CleanupFolder(strName, strDenied))
			{
				::FindClose(hf);
				return false;
			}

			BOOL bRes = ::SetCurrentDirectory(_T("..\\"));
			_ASSERTE(bRes);

			ClearReadOnlyAttr(strName);
			if (!::RemoveDirectory(strName))
			{
				strDenied = strFullName;
				::FindClose(hf);
				return false;
			}
		}
		else
		{
			//Clear read only attribute
			DWORD dwCurAttr = ::GetFileAttributes(strName);
			dwCurAttr &= ~FILE_ATTRIBUTE_READONLY;
			::SetFileAttributes(strName, dwCurAttr);

			ClearReadOnlyAttr(strName);
			if (!::DeleteFile(strName))
			{
				strDenied = strFullName;
				::FindClose(hf);
				return false;
			}
		}
	}
	::FindClose(hf);

	if (bSelfRemove)
	{
		::SetCurrentDirectory(_T("..\\"));
		ClearReadOnlyAttr(strNewPath);
		if (!::RemoveDirectory(strNewPath))
			return false;
	}

	return true;
}

inline bool RemoveDir(LPCTSTR cszDir)
{
	CString strDenied;
	return CleanupFolder(cszDir, strDenied, true);
}

inline CString GetObjectHelpDir(CLSID clsid)
{
	CComBSTR bstrCLSID(clsid);
	CString strKey;
	CString strDir;
		
	CRegKey regKey;
	strKey.Format(_T("CLSID\\%s\\HelpDir"), CString(bstrCLSID));
	if (regKey.Open(HKEY_CLASSES_ROOT, strKey, KEY_READ) == ERROR_SUCCESS)
	{
		TCHAR szDir[MAX_PATH];
		DWORD dwLen = MAX_PATH;
		if (regKey.QueryValue(szDir, _T(""), &dwLen) == ERROR_SUCCESS)
			strDir = szDir;
	}

	if (strDir.IsEmpty())
	{
		strKey.Format(_T("CLSID\\%s\\InProcServer32"), CString(bstrCLSID));
		if (regKey.Open(HKEY_CLASSES_ROOT, strKey, KEY_READ) == ERROR_SUCCESS)
		{
			TCHAR szDir[MAX_PATH];
			DWORD dwLen = MAX_PATH;
			if (regKey.QueryValue(szDir, _T(""), &dwLen) == ERROR_SUCCESS)
			{
				CString strDir(szDir);
				int nPoint = strDir.ReverseFind(_T('\\'));
				if (nPoint >= 0)
					strDir = strDir.Left(nPoint + 1);
				if (strDir == _T("mscoree.dll"))
					strDir = GetExePath();
			}
		}
	}

	if (!strDir.IsEmpty() && strDir.Right(1) != _T("\\") && strDir.Right(1) != _T("/"))
		strDir += _T("\\");

	return strDir;
}

inline CString GetObjectModuleName(CLSID clsid)
{
	CComBSTR bstrCLSID(clsid);
	CString strKey;
		
	CRegKey regKey;
	strKey.Format(_T("CLSID\\%s\\InProcServer32"), CString(bstrCLSID));
	if (regKey.Open(HKEY_CLASSES_ROOT, strKey, KEY_READ) == ERROR_SUCCESS)
	{
		TCHAR szDir[MAX_PATH];
		DWORD dwLen = MAX_PATH;
		if (regKey.QueryValue(szDir, _T(""), &dwLen) == ERROR_SUCCESS)
		{
			CString strDir(szDir);
			int nPoint = strDir.ReverseFind(_T('\\'));
			return strDir.Mid(nPoint + 1);
		}
	}

	return CString();
}

inline CString GetObjectHelpString(CLSID clsid)
{
	CComBSTR bstrCLSID(clsid);
	CString strKey;
	CRegKey regKey;
	strKey.Format(_T("CLSID\\%s\\Images"), CString(bstrCLSID));
	if (regKey.Open(HKEY_CLASSES_ROOT, strKey, KEY_READ) != ERROR_SUCCESS)
		return CString();

	ULONG lCount = 1000;
	CString strHelpCustomFile = _T("");
	regKey.QueryValue(strHelpCustomFile.GetBuffer(1000), "HelpFile", &lCount);
	strHelpCustomFile.ReleaseBuffer();
	if (!strHelpCustomFile.IsEmpty() && strHelpCustomFile.Find(_T('.')) < 0)
		strHelpCustomFile += _T(".chm");
	int nPoint = strHelpCustomFile.ReverseFind(_T('\\'));
	if (nPoint >= 0)
		strHelpCustomFile.Delete(0, nPoint + 1);

	//Help
	DWORD dwHelpID;
	if (regKey.QueryValue(dwHelpID, "HelpID") != ERROR_SUCCESS)
		return CString();

	CString strRes;
	CString strDir = GetObjectHelpDir(clsid);
	if (strHelpCustomFile.IsEmpty())
	{
		CString strModule = GetObjectModuleName(clsid);
		int nPoint2 = strModule.ReverseFind('.');
		if (nPoint2 > 0)
			strModule = strModule.Left(nPoint2);
		strRes.Format("%s%s.chm\n%d", strDir, strModule, dwHelpID);
	}
	else
	{
		strRes.Format("%s%s\n%d", strDir, strHelpCustomFile, dwHelpID);
	}
	return strRes;
}

inline bool MSFileExists(LPCTSTR szFile)
{
	return ::GetFileAttributes(MSGetShortPath(szFile)) != 0xFFFFFFFF;
}

inline void CreateDirs(LPCTSTR cszFName)
{
	CString strFName(MSGetShortPath(cszFName));
	int iPoint = strFName.Find(_T('\\'));
	while (iPoint >= 0)
	{
		CString strCur = strFName.Left(iPoint);
		if (!MSFileExists(strCur))
			BOOL bRes = ::CreateDirectory(strCur, NULL);
//		iPoint = strFName.Find(_T('\\'), iPoint + 1);
		int nPoint2 = strFName.Mid(iPoint + 1).Find(_T('\\'));
		if (nPoint2 < 0)
			break;
		iPoint = iPoint + 1 + nPoint2;
	}
}

inline bool MSCopyDir(LPCTSTR szSourceDir, LPCTSTR szDestDir, BOOL bFailIfExist = FALSE)
{
	CString strSourceDir(MSGetShortPath(szSourceDir) + _T("\\"));

	CString strDestDir(MSGetShortPath(szDestDir) + _T("\\"));
	MS::CreateDirs(strDestDir);
	strDestDir = MSGetShortPath(strDestDir);

	WIN32_FIND_DATA fd;
	HANDLE hf = ::FindFirstFile(strSourceDir + _T("*.*"), &fd);
	bool find = true;
	while (find && hf != INVALID_HANDLE_VALUE)
	{
		bool is_dir = (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

		CString strName;
		if (fd.cAlternateFileName != NULL && fd.cAlternateFileName[0] != 0)
			strName = fd.cAlternateFileName;
		else
			strName = fd.cFileName;

		CString strFullName = fd.cFileName;
		if (!FindNextFile(hf, &fd))
			find = false;

		if (strName == "." || strName == "..")
			continue;

		if (is_dir)
		{
			if (!MSCopyDir(strSourceDir + strFullName, strDestDir + strFullName, bFailIfExist))
				return false;
		}
		else
		{
			if (!::CopyFile(strSourceDir + strFullName, strDestDir + strFullName, bFailIfExist))
				return false;
		}
	}
	::FindClose(hf);

	return true;
}

inline BOOL MSCopyFile(LPCTSTR szExistingFile, LPCTSTR szNewFile, BOOL bFailIfExist = FALSE)
{
	CString strSourceFile(MSGetShortPath(szExistingFile));
	DWORD dwSourceAttr = ::GetFileAttributes(strSourceFile);
	if (dwSourceAttr == 0xFFFFFFFF)
		return (::SetLastError(ERROR_FILE_NOT_FOUND), FALSE);
	
	if ((dwSourceAttr & FILE_ATTRIBUTE_DIRECTORY) != 0)
		return MSCopyDir(szExistingFile, szNewFile, bFailIfExist);
	
	return ::CopyFile(strSourceFile, MSGetShortPath(szNewFile), bFailIfExist);
}

inline BOOL MSMoveFile(LPCTSTR szExistingFile, LPCTSTR szNewFile, BOOL bFailIfExist = FALSE)
{
	CString strNewFile(MSGetShortPath(szNewFile));
	if (!bFailIfExist)
	{
		DWORD dwFileAttr = ::GetFileAttributes(strNewFile);
		if (dwFileAttr != 0xFFFFFFFF)
		{
			if ((dwFileAttr & FILE_ATTRIBUTE_DIRECTORY) != 0)
				RemoveDir(strNewFile);
			else
			{
				ClearReadOnlyAttr(strNewFile);
				::DeleteFile(strNewFile);
			}
		}
	}
	
	CString strSourceFile(MSGetShortPath(szExistingFile));
	DWORD dwSourceAttr = ::GetFileAttributes(strSourceFile);
	if (dwSourceAttr == 0xFFFFFFFF)
		return (::SetLastError(ERROR_FILE_NOT_FOUND), FALSE);
	
	if ((dwSourceAttr & FILE_ATTRIBUTE_DIRECTORY) != 0)
	{
		int nPoint = strSourceFile.ReverseFind(_T('\\'));
		::SetCurrentDirectory(strSourceFile.Left(nPoint));
	}
	
	return ::MoveFile(strSourceFile, strNewFile);
}

inline BOOL MSDeleteFile(LPCTSTR szFile)
{
	CString strFile = MSGetShortPath(szFile);
	ClearReadOnlyAttr(szFile);
	return ::DeleteFile(strFile);
}

inline BOOL MSRemoveFileOrDir(LPCTSTR szFile)
{
	DWORD dwSourceAttr = ::GetFileAttributes(MSGetShortPath(szFile));
	if ((dwSourceAttr & FILE_ATTRIBUTE_DIRECTORY) != 0)
		return RemoveDir(szFile);
	else
		return MSDeleteFile(szFile);
}

inline CString MSGetShortFileName(LPCTSTR szFile)
{
	int lastPos = -1;
	while (szFile[lastPos + 1])
	{
		int pos = strcspn(szFile + lastPos + 1, "\\/");
		if (pos >= 0 && szFile[lastPos + pos + 1])
			lastPos += pos + 1;
		else
			break;
	} 
	
	return szFile + lastPos + 1;
}

inline bool VariantBoolToBool(VARIANT_BOOL b)
{
	return b != VARIANT_FALSE;
}

inline VARIANT_BOOL BoolToVariantBool(BOOL b)
{
	return b ? VARIANT_TRUE : VARIANT_FALSE;
}

inline LPCTSTR BoolToString(BOOL b)
{
	return b != 0 ? _T("true") : _T("false");
}

inline CString GetMasterSCADAPath()
{
	CRegKey regKey;
	TCHAR szBuff[MAX_PATH];

	CString strKey = MS::g_szRegKeyWithMainPath;
	CString strRes;
	if (regKey.Open(HKEY_LOCAL_MACHINE, strKey, KEY_READ) == ERROR_SUCCESS)
	{
		ULONG lCount = MAX_PATH;
		if (regKey.QueryValue(szBuff, g_szRegPathValue, &lCount) == ERROR_SUCCESS)
			strRes = szBuff;
	}

	if (strRes.GetLength() == 0)
	{
		if (regKey.Open(HKEY_CURRENT_USER, strKey, KEY_READ) == ERROR_SUCCESS)
		{
			ULONG lCount = MAX_PATH;
			if (regKey.QueryValue(szBuff, g_szRegPathValue, &lCount) == ERROR_SUCCESS)
				strRes = szBuff;
		}
	}

	if (strRes.GetLength() > 0)
	{
		if (strRes[strRes.GetLength() - 1] == _T('\\'))
			strRes.Delete(strRes.GetLength() - 1);

		MS::CreateDirs(strRes + _T("\\"));
		if (::SetCurrentDirectory(strRes))
			return strRes;
	}

	//Create folder
	DWORD nSize = MAX_PATH;        
	nSize = ::GetModuleFileName(NULL, szBuff, nSize);
	_ASSERTE(nSize > 0);

	CString strPath = szBuff;
	int nPoint = strPath.ReverseFind(_T('\\'));
	strPath = strPath.Left(nPoint);

	return strPath;
}

inline void SetMasterSCADAPath(LPCTSTR strPath)
{
	CString strKey = MS::g_szRegKeyWithMainPath;
	{
		CRegKey regKey;
		if (regKey.Create(HKEY_LOCAL_MACHINE, strKey) == ERROR_SUCCESS && regKey.SetKeyValue(_T(""), strPath, g_szRegPathValue) == ERROR_SUCCESS)
			return;
	}

	{
		CRegKey regKey;
		if (regKey.Create(HKEY_CURRENT_USER, strKey) == ERROR_SUCCESS && regKey.SetKeyValue(_T(""), strPath, g_szRegPathValue) == ERROR_SUCCESS)
			return;
	}
}

//CComRetMem definition
template<class T> inline void FreeStruct(T& t) {_ASSERTE(false);}

template <class S, class SIZE_T = long>
class CComRetMem
{
public:
	CComRetMem()
	{
		p = NULL;
		size = 0;
	}
	~CComRetMem ()
	{
		Free();
	}
	void Free()
	{
		if (p)
		{
			if (size != 0)
			{
				for(int i = 0; i < size; i++) FreeStruct<S>(p[i]);
			}
			::CoTaskMemFree(p);
		}
		p = NULL;
	}
	S* Detach ()
	{
		S *tmp = p;
		p = NULL;
		return tmp;
	}
	bool operator ! ()
	{
		return p == NULL;
	}
	S& operator [](int i)
	{
		return p[i];
	}
	S** operator & ()
	{
		_ASSERTE (p == NULL);
		return &p;
	}
	S *p;
	SIZE_T size;
};

template<class S>
class COleMem
{
public:
	COleMem()
	{
		count = 0;
		p = NULL;
	}
	COleMem(SIZE_T size)
	{
		bool bRes = Alloc(size);
		_ASSERTE (bRes);
	}
	~COleMem()
	{
		if (p != NULL)
		{
			for (int i = 0; i < count; i++)
			{
				p[i].~S();
			}
			::CoTaskMemFree(p);
		}
	}

	bool Alloc(SIZE_T size)
	{
		count = size;
		p = (S*)::CoTaskMemAlloc(sizeof(S) * count);
		if (p == NULL)
			return false;
		new (p) S;
		return true;
	}

	S * Detach ()
	{
		S * tmp = p;
		p = NULL;
		return tmp;
	}
	operator S*()
	{
		return p;
	}
	S& operator [](int i)
	{
		return p[i];
	}
	S * p;
	SIZE_T count;
};

template<class T, const IID* piid = &__uuidof(T)>
class CThreadPtr2
{
public:
	CComQIPtr<T, piid> pBase;
	bool bBaseIsSTA;
	CComPtr<T> pWork;	//weak
	DWORD dwBaseThreadId;

	CThreadPtr2()
	{
		bBaseIsSTA = true;
		piMarshalStm = NULL;
		pWork = NULL;
		dwBaseThreadId = 0;
	}
	~CThreadPtr2()
	{
		Release();
	}
	void Release()
	{
		HRESULT hr;
		pBase.Release();
		pWork.Release();
		if (piMarshalStm)
		{
			hr = ::CoReleaseMarshalData (piMarshalStm);
			piMarshalStm->Release();
			piMarshalStm = NULL;
		}
		dwBaseThreadId = 0;
	}
	T* operator = (IUnknown* lp)
	{
		_ASSERTE (!pBase);
#ifdef _AFX //only for MFC
		AFX_MANAGE_STATE(AfxGetAppModuleState())
		CWinApp *pApp = ::AfxGetApp();
		if (pApp)
		{
			dwBaseThreadId = pApp->m_nThreadID;
			if (dwBaseThreadId != ::GetCurrentThreadId())
				bBaseIsSTA = false;
		}
#endif
		if (dwBaseThreadId == 0)
			dwBaseThreadId = ::GetCurrentThreadId();

		pBase = lp;
		Marshal();
		return pBase.p;
	}
	bool operator !() const
	{
		return pBase.p == NULL;
	}
	_NoAddRefReleaseOnCComPtr<T>* operator->()
	{
		T* res = p();
		_ASSERTE (res != NULL);
		return (_NoAddRefReleaseOnCComPtr<T>*)res;
	}
	operator T*()
	{
		return p();
	}
	T* p()
	{
		DWORD dwThreadId = ::GetCurrentThreadId();
		if (   dwThreadId == dwBaseThreadId && bBaseIsSTA
			|| dwThreadId != dwBaseThreadId && !bBaseIsSTA)
		{
			return pBase.p;
		}
		else
		{
			HRESULT hr = Unmarshal();
			return pWork.p;
		}		
	}
	CComQIPtr<T> &work()
	{
		HRESULT hr = Unmarshal();
		_ASSERTE (pWork.p != NULL);
		return pWork;
	}
	HRESULT Marshal()
	{
		if (piMarshalStm)
			return S_OK;
		if (!pBase)
			return E_FAIL;
		
		HRESULT hr  = ::CoMarshalInterThreadInterfaceInStream(*piid, pBase, &piMarshalStm);
		return hr;
	}
	HRESULT Unmarshal()
	{
		if (pWork)
			return S_OK;
		if (!piMarshalStm)
			return E_FAIL;
		
		HRESULT hr  = ::CoGetInterfaceAndReleaseStream(piMarshalStm, *piid, (void**)&pWork);
		piMarshalStm = NULL;
		return hr;
	}
private:
	IStream* piMarshalStm;
};

inline CString FileTimeToUniversalString(FILETIME &file_time)
{
	SYSTEMTIME system_time;
	::FileTimeToSystemTime(&file_time, &system_time);
	CString cstr;
	cstr.Format(_T("%04hd-%02hd-%02hdT%02hd:%02hd:%02hd.%03hd0000Z"),
		system_time.wYear, system_time.wMonth, system_time.wDay, 
		system_time.wHour, system_time.wMinute, system_time.wSecond, system_time.wMilliseconds);
	return cstr;
}

inline CString FileTimeToString(FILETIME &file_time)
{
	SYSTEMTIME st;
	::FileTimeToSystemTime(&file_time, &st);
	SYSTEMTIME st_local;
	if (!::SystemTimeToTzSpecificLocalTime(NULL, &st, &st_local))
		return CString();

	CString cstr;
	cstr.Format(_T("%02hd/%02hd/%04hd %02hd:%02hd:%02hd:%03hd"),
		st_local.wDay, st_local.wMonth, st_local.wYear, 
		st_local.wHour, st_local.wMinute, st_local.wSecond, st_local.wMilliseconds);
	return cstr;
}

inline BOOL StringToFileTime(CString &cstr, FILETIME &file_time)
{
	SYSTEMTIME st_local;
	int nRes = _stscanf(cstr, _T("%hd/%hd/%hd %hd:%hd:%hd:%hd"), 
		&st_local.wDay, &st_local.wMonth, &st_local.wYear, 
		&st_local.wHour, &st_local.wMinute, &st_local.wSecond, &st_local.wMilliseconds);
	if (nRes != 7)
		return FALSE;

	SYSTEMTIME st;
	if (!::TzSpecificLocalTimeToSystemTime(NULL, &st_local, &st))
		return false;
	if (!::SystemTimeToFileTime(&st, &file_time))
		return FALSE;

	return TRUE;
}

inline CString FormatErrorString(DWORD err)
{
	LPTSTR lpMsgBuf;
	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		err,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL 
	);
	CString str = lpMsgBuf;
	str.Remove(13);
	str.Remove(10);
	::LocalFree(lpMsgBuf);
	return str;
}

#ifdef PIN_TYPES_DEFINED
	inline PinTypes TITtoPT(TreeItemType sType)
	{
		switch (sType)
		{
		case TIT_PINGROUP:
			return PT_PINGROUP;
		case TIT_POUTGROUP:
			return PT_POUTGROUP;
		case TIT_GROUP:
			return PT_GROUP;
		case TIT_PIN:
			return PT_PIN;
		case TIT_POUT:
			return PT_POUT;
		case TIT_EVENT:
			return PT_EVENT;
		case TIT_PARSER:
			return PT_PARSER;
		case TIT_EVENTGROUP:
			_ASSERTE(0);
		}
		_ASSERTE(0);
		return PT_PIN;
	}

	inline TreeItemType PTtoTIT(PinTypes ptType)
	{
		switch (ptType)
		{
		case PT_PINGROUP:
			return TIT_PINGROUP;
		case PT_POUTGROUP:
			return TIT_POUTGROUP;
		case PT_GROUP:
			return TIT_GROUP;
		case PT_PIN:
			return TIT_PIN;
		case PT_POUT:
			return TIT_POUT;
		case PT_EVENT:
			return TIT_EVENT;
		case PT_PARSER:
			return TIT_PARSER;
		}
		_ASSERTE(0);
		return TIT_POUT;
	}
#endif

inline bool IsTypesCompatible(VARTYPE vt1, VARTYPE vt2)
{
	if (IsAnalogType(vt1) && IsAnalogType(vt2))
		return true;
	return vt1 == vt2;
}

template <class T>
class CLockSectionEx
{
public:
	CLockSectionEx(T* pObject, BOOL bLockReceiving, BOOL bLockSending, bool bInitiallyOwn = true)
		: m_pObject(pObject), m_bLockReceiving(bLockReceiving), m_bLockSending(bLockSending)
	{
		if (bInitiallyOwn)
		{
			m_pObject->LockEx(TRUE, m_bLockReceiving, m_bLockSending);
			m_bOwn = true;
		}
		else
			m_bOwn = false;
	}
	~CLockSectionEx()
	{
		if (m_bOwn)
			Unlock();
	}
	void Lock()
	{
		if (!m_bOwn)
			m_pObject->LockEx(TRUE, m_bLockReceiving, m_bLockSending);
		m_bOwn = true;
	}
	void Unlock()
	{
		if (m_bOwn)
			m_pObject->LockEx(FALSE, m_bLockReceiving, m_bLockSending);
		m_bOwn = false;
	}
	T* m_pObject;
private:
	bool m_bOwn;
	BOOL m_bLockReceiving;
	BOOL m_bLockSending;
};

inline CString GetErrorDescr(HRESULT hr)
{
	CComPtr<IErrorInfo> pErrorInfo;
	::GetErrorInfo(0, &pErrorInfo);
	if (pErrorInfo)
	{
		CComBSTR bstrErrDescr;
		pErrorInfo->GetDescription(&bstrErrDescr);
		::SetErrorInfo(0, pErrorInfo);
		return CString(bstrErrDescr);
	}

    LPTSTR lpMsgBuf;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
        (LPTSTR) &lpMsgBuf, 0, NULL);

	CString str;
	if (lpMsgBuf == NULL)
	{
		str.Format(_T("%#8X"), (DWORD)hr);
	}
	else
	{
		str = lpMsgBuf;
		::LocalFree(lpMsgBuf);
		str.Remove(13);
		str.Remove(10);
	}

    return str;
}

enum EVarTypeGroup
{
	vtgUnsupported = 0, vtgAnalog = 1, vtgBool = 2, vtgString = 3, vtgTime = 4
};

inline EVarTypeGroup GetVarTypeGroup(VARTYPE vt)
{
	if (vt == VT_BOOL)
		return vtgBool;
	else if (IsAnalogType(vt))
		return vtgAnalog;
	else if (vt == VT_BSTR)
		return vtgString;
	else if (vt == VT_DATE)
		return vtgTime;
	else
		return vtgUnsupported;
}

const VARTYPE g_rgVarTypes[] = {VT_R8, VT_I4, VT_UI4, VT_BSTR, VT_DATE, VT_R4, VT_I2, VT_UI2};

inline long VarTypeToIndex(VARTYPE vt)
{
	if (vt == VT_BOOL)
		return -2;
	else if (vt == VT_DATE)
		return 4;
	else if (vt == VT_BSTR)
		return 3;
	else if (vt == VT_R8)
		return 0;
	else if (vt == VT_R4)
		return 5;
	else if (vt == VT_I2 || vt == VT_I1)
		return 6;
	else if (vt == VT_UI2 || vt == VT_UI1)
		return 7;
	else if (vt == VT_UI4 || vt == VT_UINT)
		return 2;
	else
		return 1; //VT_I4
}

inline bool CanDrawValue(DWORD dwValidity)
{
	if (   (dwValidity & OPC_MASK) == OPC_QUALITY_NOT_CONNECTED
		|| (dwValidity & OPC_MASK) == OPC_QUALITY_OUT_OF_SERVICE
		|| (dwValidity & OPC_MASK) == OPC_QUALITY_COMM_FAILURE)
		return false;
	else
		return true;
}

enum OPCHDA_AGGREGATE_MS
{	
	OPCHDA_MS_COUNT	= 100,
	OPCHDA_MS_SUM
};

#ifdef BIF_RETURNONLYFSDIRS	//Include "Shlobj.h" first

#define PACKVERSION(major,minor) MAKELONG(minor,major)
inline DWORD GetDllVersion(LPCTSTR lpszDllName)
{
    HINSTANCE hinstDll;
    DWORD dwVersion = 0;

    hinstDll = LoadLibrary(lpszDllName);
    if (hinstDll)
    {
        DLLGETVERSIONPROC pDllGetVersion = (DLLGETVERSIONPROC)GetProcAddress(hinstDll, "DllGetVersion");
        if(pDllGetVersion)
        {
            DLLVERSIONINFO dvi;
            HRESULT hr;
            ZeroMemory(&dvi, sizeof(dvi));
            dvi.cbSize = sizeof(dvi);
            hr = (*pDllGetVersion)(&dvi);
            if(SUCCEEDED(hr))
                dwVersion = PACKVERSION(dvi.dwMajorVersion, dvi.dwMinorVersion);
        }
        FreeLibrary(hinstDll);
    }
    return dwVersion;
}

#ifndef BIF_NEWDIALOGSTYLE
	#define BIF_NEWDIALOGSTYLE     0x0040
#endif

struct CChooseFolderData
{
	LPCSTR szIntitialDir;
	LPCSTR szRequiredFile;
};

inline INT CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lp, LPARAM pData) 
{
   TCHAR szDir[MAX_PATH];   

   CChooseFolderData* pFolderData = (CChooseFolderData*)pData;

   switch(uMsg) 
   {
   case BFFM_INITIALIZED: 
      if (pFolderData != NULL && pFolderData->szIntitialDir != NULL)
      {
		  CString strDir = (LPCSTR)pData;

         // WParam is TRUE since you are passing a path.
         // It would be FALSE if you were passing a pidl.
		 ::SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)pFolderData->szIntitialDir);
      }
      break;   
   case BFFM_SELCHANGED: 
      // Set the status window to the currently selected path.
	   if (::SHGetPathFromIDList((LPITEMIDLIST)lp, szDir))
      {
		  ::SendMessage(hwnd, BFFM_SETSTATUSTEXT, 0, (LPARAM)szDir);
      }

      if (pFolderData != NULL && pFolderData->szRequiredFile != NULL)
	  {
		  CString strPath = szDir;
		  strPath += _T("\\");
		  strPath += pFolderData->szRequiredFile;
		  bool bExists = MSFileExists(strPath);
		  ::SendMessage(hwnd, BFFM_ENABLEOK, 0, bExists);
	  }

      break;
   }
   return 0;
}

inline CString MSChooseFolder(HWND hWnd, LPCTSTR szCaption, 
	LPCSTR szIntitialDir = NULL, LPCSTR szRequiredFile = NULL)
{
	TCHAR szPath[MAX_PATH];
	::_tcscpy(szPath, MS::GetMasterSCADAPath());

	DWORD dwStyle = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT;
	if (GetDllVersion(_T("Shell32.dll")) >= PACKVERSION(5,0))
		dwStyle |= BIF_NEWDIALOGSTYLE;

	CChooseFolderData data;
	data.szIntitialDir = szIntitialDir;
	data.szRequiredFile = szRequiredFile;

	BROWSEINFO bi = {hWnd, NULL, szPath, szCaption, dwStyle, 
		BrowseCallbackProc, (LPARAM)&data, 0};
	LPITEMIDLIST pidl = ::SHBrowseForFolder(&bi); 
 	if (pidl == NULL) 
		return _T("");
	if (!::SHGetPathFromIDList(pidl, szPath))
		return _T("");

	CComPtr<IMalloc> pMalloc;
	if (::SHGetMalloc(&pMalloc) == NOERROR) 
		pMalloc->Free(pidl); 

	return szPath;
}

#endif

inline DWORD ReadMasterSCADAUserSetting(LPCTSTR szKey, DWORD dwDefault)
{
	CRegKey regKey;
	if (regKey.Open(HKEY_CURRENT_USER, g_szRegMainKey, KEY_READ) == ERROR_SUCCESS)
	{
		DWORD dwValue;
		if (regKey.QueryValue(dwValue, szKey) == ERROR_SUCCESS)
			return dwValue;
	}
	return dwDefault;
}

inline DWORD ReadMasterSCADASetting(LPCTSTR szKey, DWORD dwDefault)
{
	CRegKey regKey;
	if (regKey.Open(HKEY_LOCAL_MACHINE, g_szRegMainKey, KEY_READ) == ERROR_SUCCESS)
	{
		DWORD dwValue;
		if (regKey.QueryValue(dwValue, szKey) == ERROR_SUCCESS)
			return dwValue;
	}
	return ReadMasterSCADAUserSetting(szKey, dwDefault);
}

inline void RemoveMasterSCADAUserSetting(LPCTSTR szKey)
{
	CRegKey regKey;
	if (regKey.Open(HKEY_CURRENT_USER, g_szRegMainKey) == ERROR_SUCCESS)
		regKey.DeleteValue(szKey);
}

inline void WriteMasterSCADAUserSetting(LPCTSTR szKey, DWORD dwValue)
{
	CRegKey regKey;
	if (regKey.Open(HKEY_CURRENT_USER, g_szRegMainKey) == ERROR_SUCCESS)
		regKey.SetValue(dwValue, szKey);
}

inline void WriteMasterSCADASetting(LPCTSTR szKey, DWORD dwValue)
{
	CRegKey regKey;
	if (regKey.Open(HKEY_LOCAL_MACHINE, g_szRegMainKey) == ERROR_SUCCESS && regKey.SetValue(dwValue, szKey) == ERROR_SUCCESS)
		return;
	WriteMasterSCADAUserSetting(szKey, dwValue);
}

inline CString ReadMasterSCADAUserSettingString(LPCTSTR szKey, CString strDefault = CString())
{
	CRegKey regKey;
	if (regKey.Open(HKEY_CURRENT_USER, g_szRegMainKey, KEY_READ) == ERROR_SUCCESS)
	{
		ULONG lCount = 1000;
		regKey.QueryStringValue(szKey, strDefault.GetBuffer(1000), &lCount);
		strDefault.ReleaseBuffer();
	}
	return strDefault;
}

inline CString ReadMasterSCADASettingString(LPCTSTR szKey, CString strDefault = CString())
{
	CRegKey regKey;
	if (regKey.Open(HKEY_LOCAL_MACHINE, g_szRegMainKey, KEY_READ) == ERROR_SUCCESS)
	{
		ULONG lCount = 1000;
		regKey.QueryStringValue(szKey, strDefault.GetBuffer(1000), &lCount);
		strDefault.ReleaseBuffer();
	}
	return ReadMasterSCADAUserSettingString(szKey, strDefault);
}

inline void WriteMasterSCADAUserSettingString(LPCTSTR szKey, CString strValue)
{
	CRegKey regKey;
	if (regKey.Open(HKEY_CURRENT_USER, g_szRegMainKey) == ERROR_SUCCESS)
		regKey.SetStringValue(szKey, strValue);
}

inline void WriteMasterSCADASettingString(LPCTSTR szKey, CString strValue)
{
	CRegKey regKey;
	if (regKey.Open(HKEY_LOCAL_MACHINE, g_szRegMainKey) == ERROR_SUCCESS && regKey.SetStringValue(szKey, strValue) == ERROR_SUCCESS)
		return;
	WriteMasterSCADAUserSettingString(szKey, strValue);
}

// Instances of this class will be accessed by multiple threads. 
template<class T>
class CInterlockedType : public CComAutoCriticalSection
{
public:
	CInterlockedType() { }
	CInterlockedType(const T& val) { data = val; }

	operator const T() const 
	{ 
		return data; 
	}
	operator T()
	{ 
		return data; 
	}
	T& operator=(const T& val) 
	{ 
		return (data = val); 
	}
	T* operator->()
	{
		return &data;
	}
	T data;
};

inline bool IsTypeSupported(VARTYPE vt)
{
	return IsAnalogType(vt) || vt == VT_BSTR || vt == VT_DATE
		|| vt == VT_BOOL || vt == VT_CY || vt == VT_VARIANT
		|| vt == VT_HRESULT || vt == VT_FILETIME;
}

#ifdef _AFX

inline CString GetExceptionStringAndDelete(CException* pEx)
{
	CString strError;
	pEx->GetErrorMessage(strError.GetBuffer(1000), 1000);
	pEx->Delete();
	return strError;
}

template<class T, const GUID* pguid = NULL, const IID* piid = &__uuidof(T)>
class CCachedFactory
{
	CComPtr<IClassFactory> m_pSTA;
	//CComPtr<IClassFactory> m_pMTA;
	//DWORD m_dwSTAThreadID;
	CLSID m_clsid;

public:
	CCachedFactory()
	{
//		m_dwSTAThreadID = 0;
		if (pguid != NULL)
			m_clsid = *pguid;
	}

	void SetCLSID(REFCLSID clsid)
	{
		m_clsid = clsid;
		m_pSTA.Release();
//		m_pMTA.Release();
	}

	HRESULT CreateInstance(T** pp)
	{
//		if (m_dwSTAThreadID == 0)
//		{
////			AFX_MANAGE_STATE(AfxGetAppModuleState())
//			CWinApp *pApp = ::AfxGetApp();
//			if (pApp)
//				m_dwSTAThreadID = pApp->m_nThreadID;
//		}
//		_ASSERTE (m_dwSTAThreadID != 0);
//
//		if (m_dwSTAThreadID == ::GetCurrentThreadId())
			return CreateInstanceImpl(pp, m_pSTA);
//else
//	return CreateInstanceImpl(pp, m_pMTA);
	}

private:
	HRESULT CreateInstanceImpl(T** pp, CComPtr<IClassFactory>& pFactory)
	{
		HRESULT hr = CreateInstanceIndirect(pp, pFactory);
		if (FAILED(hr))
		{
			//Try to recreate factory
			pFactory.Release();
			hr = CreateInstanceIndirect(pp, pFactory);
		}
		return hr;
	}

	HRESULT CreateInstanceIndirect(T** pp, CComPtr<IClassFactory>& pFactory)
	{
		if (!pFactory)
		{
			HRESULT hr = ::CoGetClassObject(*pguid, CLSCTX_SERVER, NULL, IID_IClassFactory, (void**)&pFactory);
			if (FAILED(hr))
				return hr;
		}
		return pFactory->CreateInstance(NULL, *piid, (void**)pp);
	}
};

#endif

inline bool VariantToBool(VARIANT& varValue, bool bDefValue = false)
{
	if (varValue.vt == VT_BOOL)
		return VariantBoolToBool(varValue.boolVal);
	else if (varValue.vt == VT_EMPTY)
		return bDefValue;
	else
	{
		CComVariant v;
		if (v.ChangeType(VT_BOOL, &varValue) == S_OK)
			return VariantBoolToBool(varValue.boolVal);
		else
			return bDefValue;
	}
}

inline long VariantToLong(VARIANT& varValue, long nDefValue = 0)
{
	if (varValue.vt == VT_I4)
		return varValue.lVal;
	else if (varValue.vt == VT_EMPTY)
		return nDefValue;
	else
	{
		CComVariant v;
		if (v.ChangeType(VT_I4, &varValue) == S_OK)
			return varValue.lVal;
		else
			return nDefValue;
	}
}

inline double VariantToDouble(VARIANT& varValue, double fDefValue = 0.0)
{
	if (varValue.vt == VT_R8)
		return varValue.dblVal;
	else if (varValue.vt == VT_EMPTY)
		return fDefValue;
	else
	{
		CComVariant v;
		if (v.ChangeType(VT_R8, &varValue) == S_OK)
			return varValue.dblVal;
		else
			return fDefValue;
	}
}

inline CString VariantToString(VARIANT& varValue, CString strDefValue = CString())
{
	if (varValue.vt == VT_EMPTY)
		return strDefValue;
	else
	{
		CString res;
		if (GetVarValueStr(&varValue, res))
			return res;
		else
			return strDefValue;
	}
}

inline CString FormatFileTime(FILETIME& ft)
{
	SYSTEMTIME UTCTime;
	FileTimeToSystemTime(&ft,&UTCTime);
	SYSTEMTIME st_local;
	if (!::SystemTimeToTzSpecificLocalTime(NULL, &UTCTime, &st_local))
		return CString();

	CString strTime;
	strTime.Format(_T("%02d:%02d:%02d.%03d %02d/%02d/%02d"), 
		st_local.wHour, st_local.wMinute, st_local.wSecond, st_local.wMilliseconds, st_local.wDay, st_local.wMonth, st_local.wYear % 100);

	return strTime;
}

inline CString ConvertToXML(CString str, bool bReplaceQuot = false)
{
	str.Replace(_T("&"), _T("&amp;"));
	if (bReplaceQuot)
		str.Replace(_T("\""), _T("&quot;"));
	str.Replace(_T("'"), _T("&apos;"));
	str.Replace(_T("<"), _T("&lt;"));
	str.Replace(_T(">"), _T("&gt;"));

	return str;
}

inline bool IsValidItemName(CString name)
{
	return !name.IsEmpty() && name.FindOneOf(_T(".\"")) < 0;
}

#ifdef __ITreeObject_INTERFACE_DEFINED__
inline bool IsValidItemForRename(EObjectType otParentType)
{
	return otParentType == otFolder || otParentType == otExternalModule;
}
#endif

inline bool IsValidFileName(CString name)
{
	return !name.IsEmpty() && name.FindOneOf(g_szInvalidFileChars) < 0;
}

inline CString GetErrorInfoString()
{
	CString strError;

	CComBSTR bstrErrDescr, bstrSource;
	CComPtr<IErrorInfo> pErrorInfo;
	::GetErrorInfo(0, &pErrorInfo);
	if (pErrorInfo)
	{
		pErrorInfo->GetDescription(&bstrErrDescr);
		strError = bstrErrDescr;
	}

	return strError;
}

inline CString GetItemRelativeDisplayPath(CString strNamedPath)
{
	strNamedPath.Delete(0);
	if (strNamedPath.Left(1) == _T("/"))
	{
		strNamedPath.Delete(0);
		strNamedPath.Replace(_T("/"), _T("//"));
		strNamedPath.Insert(0, _T('/'));
	}
	else
		strNamedPath.Replace(_T("/"), _T("//"));

	int n = 0;
	while (n < strNamedPath.GetLength() && strNamedPath[n] == _T('.'))
	{
		strNamedPath.Insert(n + 1, _T("./"));
		n += 3;
	}
	while (n < strNamedPath.GetLength())
	{
		if (strNamedPath[n] == _T('.'))
			strNamedPath.SetAt(n, _T('/'));
		n++;
	}
	return strNamedPath;
}

inline CLSID CLSIDFromString(const CString &s)
{
	THROWS_MSEH();

	HRESULT hr;
	CLSID clsid;
	if (FAILED(hr = ::CLSIDFromString(CComBSTR(s), &clsid)))
		RAISE_ERR(hr, CString(_T("Неверный идентификатор класса ")) + s);

	return clsid;
}

//inline CString StringFromCLSID (const CLSID &clsid)
//{
//  return CString(CComBSTR(clsid));
//}

/*
inline GUID CLSIDFromString(CComBSTR bstrCLSID)
{
	GUID clsid;
	::CLSIDFromString(bstrCLSID, &clsid);
	return clsid;
}
*/

inline int CharToHex(TCHAR ch)
{
	if (ch >= _T('0') && ch <= _T('9'))
		return ch - _T('0');
	if (ch >= _T('a') && ch <= _T('f'))
		return 10 + ch - _T('a');
	if (ch >= _T('A') && ch <= _T('F'))
		return 10 + ch - _T('A');
	return 0;
}

inline void ConvertHexStringToBuffer(CString strHex, BYTE* bytes, int len)
{
	for (int i = 0; i < len; ++i)
	{
		bytes[i] = CharToHex(strHex[2*i]) * 16 + CharToHex(strHex[2*i + 1]);
	}
} 

inline CString ConvertHexStringFromBuffer(BYTE* buf, int len)
{
	CString ss;

	for (int j = 0; j < len; j++)
	{
		ss.AppendFormat(_T("%02x"), (int)buf[j]);
	}

	return ss;
} 

inline CString ConvertHexStringFromBuffer(UINT* buf, int len)
{
	CString ss;

	for (int j = 0; j < len; j++)
	{
		ss.AppendFormat(_T("%08x"), (int)buf[j]);
	}

	return ss;
} 

#ifdef __IScale_INTERFACE_DEFINED__

inline CString FormatValueScale(IScale* pScale, VARIANT varValue, int nIntervalID, bool bShowUnit = true)
{
	EVarTypeGroup vg = MS::GetVarTypeGroup(varValue.vt);

	short sIndex = 0;
	if (nIntervalID != 0)
		pScale->GetIndexByID(nIntervalID, &sIndex);

	CComBSTR bstrValue;
	pScale->FormatValue(varValue, ComposeValidity(sIndex, OPC_QUALITY_GOOD), &bstrValue);
	CString strValue((BSTR)bstrValue);

	if (!bShowUnit && vg == vtgAnalog && sIndex > 0)
	{
		EScaleType scaleType;
		pScale->GetIndexType(sIndex, &scaleType);
		if (scaleType == ST_INTERVAL)
		{
			int i = strValue.Find(_T(' '));
			if (i >= 0)
				strValue = strValue.Left(i);
		}
	}

	return strValue;
}

#endif

#ifdef __INumProperties_INTERFACE_DEFINED__

class CNumPropertiesHlp
{
	CComQIPtr<INumProperties> _pProps;

public:
	CNumPropertiesHlp()
	{
	}

	CNumPropertiesHlp(IUnknown* pProps)
	{
		Init(pProps);
	}

	void Init(IUnknown* pProps)
	{
		THROWS_MSEH();
		_pProps = pProps;
		PTR_RET(_pProps);
	}

	CNumPropertiesHlp GetParent()
	{
		CComQIPtr<ITreeObject> pTreeObj(_pProps);
		CComPtr<ITreeItem> pParent;
		pTreeObj->get_Parent(ptInherated, &pParent);
		return CNumPropertiesHlp(pParent);
	}

	//Helpers
	inline long GetLongProperty(long lCategory, long lProp, long lDef = 0, bool bInherated = true, bool* pbIsInherated = NULL)
	{
		CComVariant varValue;
		if (bInherated)
			_pProps->GetInheratedProperty(lCategory, lProp, &varValue, FALSE);
		else
			_pProps->GetProperty(lCategory, lProp, &varValue);

		if (pbIsInherated != NULL)
		{
			if (!bInherated)
				*pbIsInherated = varValue.vt == VT_EMPTY;
			else 
			{
				CComVariant vTest;
				_pProps->GetProperty(lCategory, lProp, &vTest);
				*pbIsInherated = vTest.vt == VT_EMPTY;
			}
		}

		return varValue.vt == VT_I4 ? varValue.lVal : lDef;
	}

	inline bool GetBoolProperty(long lCategory, long lProp, bool bDef = false, bool bInherated = true, bool* pbIsInherated = NULL)
	{
		CComVariant varValue;
		if (bInherated)
			_pProps->GetInheratedProperty(lCategory, lProp, &varValue, FALSE);
		else
			_pProps->GetProperty(lCategory, lProp, &varValue);

		if (pbIsInherated != NULL)
		{
			if (!bInherated)
				*pbIsInherated = varValue.vt == VT_EMPTY;
			else 
			{
				CComVariant vTest;
				_pProps->GetProperty(lCategory, lProp, &vTest);
				*pbIsInherated = vTest.vt == VT_EMPTY;
			}
		}

		return varValue.vt == VT_BOOL ? VariantBoolToBool(varValue.boolVal) : bDef;
	}

	inline CString GetStringProperty(long lCategory, long lProp, CString strDef = CString(), bool bInherated = true, bool* pbIsInherated = NULL)
	{
		CComVariant varValue;
		if (bInherated)
			_pProps->GetInheratedProperty(lCategory, lProp, &varValue, FALSE);
		else
			_pProps->GetProperty(lCategory, lProp, &varValue);
		return varValue.vt == VT_BSTR ? CString(varValue.bstrVal) : strDef;
	}

	inline double GetDoubleProperty(long lCategory, long lProp, double fDef = 0.0, bool bInherated = true, bool* pbIsInherated = NULL)
	{
		CComVariant varValue;
		if (bInherated)
			_pProps->GetInheratedProperty(lCategory, lProp, &varValue, FALSE);
		else
			_pProps->GetProperty(lCategory, lProp, &varValue);
		return varValue.vt == VT_R8 ? varValue.dblVal : fDef;
	}

	inline CComPtr<IUnknown> GetObjectProperty(long lCategory, long lProp, bool bInherated = true, bool* pbIsInherated = NULL)
	{
		CComVariant varValue;
		if (bInherated)
			_pProps->GetInheratedProperty(lCategory, lProp, &varValue, FALSE);
		else
			_pProps->GetProperty(lCategory, lProp, &varValue);
		return varValue.vt == VT_UNKNOWN || varValue.vt == VT_DISPATCH ? varValue.punkVal : NULL;
	}

	inline void SetProperty(long lCategory, long lProp, CComVariant varValue, bool bDelete = false)
	{
		THROWS_MSEH();
		if (bDelete)
		{
			VERIFY_RET (_pProps->PutProperty(lCategory, lProp, CComVariant()));
		}
		else
		{
			VERIFY_RET (_pProps->PutProperty(lCategory, lProp, varValue));
		}
	}
};

#endif

#if defined(__ATLTYPES_H__) && defined(MONITORINFOF_PRIMARY)

inline void SetWindowPosOnCursorPos(HWND hWnd, RECT* pDialogRect = NULL)
{
	CRect rect;
	if (pDialogRect != NULL)
		rect = *pDialogRect;
	else
		GetWindowRect(hWnd, &rect);

	CPoint point(0, 0);
	GetCursorPos(&point);

	MONITORINFO mi;
	mi.cbSize = sizeof(mi);
	GetMonitorInfo(MonitorFromPoint(point, MONITOR_DEFAULTTONEAREST), &mi);
	int nHorzRez = mi.rcWork.right;
	int nVertRez = mi.rcWork.bottom;

	if (point.x + rect.Width() > nHorzRez)
		point.x = nHorzRez - rect.Width();

	if (point.y + rect.Height() > nVertRez)
		point.y = nVertRez - rect.Height();

	::MoveWindow(hWnd, point.x, point.y, rect.Width(), rect.Height(), FALSE);
}

#endif

template<class T>
inline HRESULT ReadFromBuf(BYTE* pbyData, int& nPos, T& t, int nMaxSize = 0)
{
	if (nMaxSize != 0 && nPos + sizeof(T) > nMaxSize)
		return E_FAIL;
	t = *(T*)(pbyData + nPos);
	nPos += sizeof(T);
	return S_OK;
}

template<class T>
inline HRESULT WriteToBuf(BYTE* pbyData, int& nPos, T t)
{
	*(T*)(pbyData + nPos) = t;
	nPos += sizeof(T);
	return S_OK;
}

inline CString itoa(int i)
{
	CString str;
	str.Format(_T("%d"), i);
	return str;
}

inline CString htoa(int i)
{
	CString str;
	str.Format(_T("%08x"), i);
	return str;
}

inline CString h4toa(int i)
{
	CString str;
	str.Format(_T("%04x"), i);
	return str;
}
}; //namespace MS

#ifdef __IAlarmManager_INTERFACE_DEFINED__

inline bool operator ==(const EVENT_ID& def1, const EVENT_ID& def2)
{
	return def1.ulTimeStamp == def2.ulTimeStamp && def1.nSourceID == def2.nSourceID && def1.nCondition == def2.nCondition 
		&& def1.nEventIndex == def2.nEventIndex && def1.nObjectStateId == def2.nObjectStateId;
}
inline bool operator !=(const EVENT_ID& def1, const EVENT_ID& def2)
{
	return !(def1 == def2);
}

const  EVENT_ID EVENT_ID_NULL = {0, 0, 0, 0, 0};

#endif

#ifdef __IScale_INTERFACE_DEFINED__

inline MS::ELanguageIndex GetRTLocaleId(IScale* pScale)
{
	DWORD dwLocaleId = 0;
	pScale->get_RTLocaleId(&dwLocaleId);
	return MS::GetLanguageIndex(dwLocaleId);
}

inline MS::ELanguageIndex GetRTLocaleId(IProject* pProject)
{
	CComPtr<IDispatch> pDispSysObj;
	pProject->get_SysObject(&pDispSysObj);
	CComQIPtr<IScale> pScale(pDispSysObj);
	return GetRTLocaleId(pScale);
}

#endif

#endif //__UTIL_H__
