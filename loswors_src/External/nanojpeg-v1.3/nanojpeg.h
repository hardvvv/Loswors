#ifndef NANOJPEG_H
#define NANOJPEG_H

#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

    typedef enum _nj_result {
        NJ_OK = 0,
        NJ_NO_JPEG,
        NJ_UNSUPPORTED,
        NJ_OUT_OF_MEM,
        NJ_INTERNAL_ERR,
        NJ_SYNTAX_ERROR,
        __NJ_FINISHED
    } nj_result_t;

    void njInit(void);
    nj_result_t njDecode(const void* mainbuf, const int size);
    int njGetWidth(void);
    int njGetHeight(void);
    int njIsColor(void);
    unsigned char* njGetImage(void);
    int njGetImageSize(void);
    void njDone(void);

#ifdef __cplusplus
}
#endif

#endif // NANOJPEG_H