#ifndef ZNCCHALFC_H_
#define ZNCCHALFC_H_
#include "ZnccC/SpecialImagesC.h"
#include "ZnccC/SumKernelC.h"

#ifdef __cplusplus
extern "C" {
#endif
typedef struct {
    ImageC_t lumdev;
    ImageC_t mean;
    ImageC_t rcpstddev;
    ImageC_t random;
} ZnccHalfC_t, *pZnccHalfC_t;
typedef const ZnccHalfC_t *pcZnccHalfC_t;
#define NULLZNCCHALF_C { NULLIMAGE_C, NULLIMAGE_C, NULLIMAGE_C, NULLIMAGE_C }

typedef struct {
    Size2iC_t packedSize;
    Size2iC_t gridSize;
    RectC_t searchRect;
    RectC_t sumRect;
    RectC_t marginRect;
    Size2iC_t imageBufferSize;
    RectC_t imageRoi;
} ZnccMatchingSizesC_t, *pZnccMatchingSizesC_t;
typedef const ZnccMatchingSizesC_t *pcZnccMatchingSizesC_t;

int ZnccHalfC_SetImage(pZnccHalfC_t znccHalf, pcImageC_t image,
    pcZnccMatchingSizesC_t sizes, pcSpecialImagesC_t si);

void ZnccHalfC_Delete(pZnccHalfC_t znccHalf);
#ifdef __cplusplus
}
#endif
#endif /* ZNCCHALFC_H_ */