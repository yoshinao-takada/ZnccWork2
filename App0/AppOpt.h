#ifndef APPOPT_H_
#define APPOPT_H_
#include "ZnccC/Basic2DTypesC.h"
#ifdef __cplusplus
#include <cstdio>
extern "C" {
#else
#include <stdio.h>
#endif
#define DEFAULT_SEARCHRECT { -64, -15, 80, 31 }
#define DEFAULT_SCALE { 1, 1 }
#define DEFAULT_SUMRECT { -10, -10, 21, 21 }
typedef struct {
    RectC_t searchRect; // is applied to scaled images.
    RectC_t sumRect; // is applied to scaled images.
    Rational scale; // image pre-scaling
} AppOpt_t, *pAppOpt_t;
typedef const AppOpt_t *pcAppOpt_t;

int AppOpt_Read(pAppOpt_t opt, FILE* pf);

int AppOpt_ReadFile(pAppOpt_t opt, const char* filepath);

void AppOpt_Write(pAppOpt_t opt, FILE* pf);

#ifdef __cplusplus
}
#endif
#endif /* APPOPT_H_ */