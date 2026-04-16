#include <addincmn.h>
#include "drivers/drv_user.h"

int Init() {
    PRINTLN("init Test Functional Block");

    return 0;
}

EXTERN_C MPLC_DRIVER_API int InitAddin(ProcessRequestCallback func, int nInFlags, int* pnOutFlags) {
    return Init();
}
