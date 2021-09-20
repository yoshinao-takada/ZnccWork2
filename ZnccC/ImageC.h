#ifndef IMAGEC_H_
#define IMAGEC_H_
#include "ZnccC/Basic2DTypesC.h"

#ifdef __cplusplus
extern "C" {
#endif
typedef struct {
    Size2iC_t size;
    RectC_t roi;
    float* elements;
} ImageC_t, *pImageC_t;
typedef const ImageC_t *pcImageC_t;
#define NULLIMAGE_C { {0,0}, {0,0,0,0}, NULL }

int ImageC_New(pImageC_t image, const Size2iC_t size, const RectC_t roi);

void ImageC_Delete(pImageC_t image);

int ImageC_SetPackedImage(pImageC_t image, const uint8_t* packedImage, const Size2iC_t size, const RectC_t roi);

int ImageC_SetPackedImageF(pImageC_t image, const float* packedImage, const Size2iC_t size, const RectC_t roi);

int ImageC_Clone(pImageC_t image, pcImageC_t source);

void ImageC_Integrate(pImageC_t image);

void ImageC_Add(pImageC_t destination, pcImageC_t source);
void ImageC_Multiply(pImageC_t destination, pcImageC_t source);
void ImageC_Subtract(pImageC_t destination, pcImageC_t source);

int ImageC_Mean(pImageC_t mean, pcImageC_t source, const RectC_t sumRect);

#define ImageC_Begin(image) ((image)->elements + (image)->roi[0] + (image)->roi[1] * (image)->size[0])

#define ImageC_BeginC(image) ((const float*)((image)->elements + (image)->roi[0] + (image)->roi[1] * (image)->size[0]))

#ifdef __cplusplus
}
#endif
#endif /* IMAGEC_H_ */