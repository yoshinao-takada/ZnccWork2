#ifndef SPECIALIMAGESC_H_
#define SPECIALIMAGESC_H_
#include "ZnccC/ImageC.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct {
    ImageC_t ZeroBgOneFg; 
    ImageC_t OneBgZeroFg; 
    ImageC_t RandBgZeroFg; 
    ImageC_t FltMaxBgZeroFg; 
    ImageC_t AllRand; 
} SpecialImagesC_t, *pSpecialImagesC_t;
typedef const SpecialImagesC_t *pcSpecialImagesC_t;
#define NULLSPECIALIMAGE_C { NULLIMAGE_C, NULLIMAGE_C, NULLIMAGE_C, NULLIMAGE_C, NULLIMAGE_C }

int SpecialImagesC_Create(pSpecialImagesC_t si, const Size2iC_t size, const RectC_t roi);

void SpecialImagesC_Delete(pSpecialImagesC_t si);
#ifdef __cplusplus
}
#endif
#endif /* SPECIALIMAGESC_H_ */