#include "mplc_cyntron.h"
#include "license_publish.h"
#include "drivers/drv_user.h"
#include "addincmn.h"
EXTERN_C MPLC_DRIVER_API int InitAddin(ProcessRequestCallback func, int nInFlags, int* pnOutFlags) {
    try { mplc::cyntron::startLicensePublisher(); }
    catch (...) { return E_FAIL; }
    return S_OK;
}

EXTERN_C MPLC_DRIVER_API void DisposeAddin() { mplc::cyntron::stopLicensePublisher(); }
