#ifndef IMAGEC_H_
#define IMAGEC_H_
#include "ZnccC/Basic2DTypesC.h"

#ifdef __cplusplus
extern "C" {
#endif
typedef struct {
    Size2iC_t size; // whole buffer size
    RectC_t roi; // region of interest
    float* elements; // buffer pointer
} ImageC_t, *pImageC_t;
typedef const ImageC_t *pcImageC_t;
#define NULLIMAGE_C { {0,0}, {0,0,0,0}, NULL }

/**
 * @brief Initialize a image object. malloc() is underlying.
 * 
 * @param image [in,out] The object under initialization
 * @param size [in] whole buffer size
 * @param roi [in] region of interest
 * @return EXIT_SUCCESS if successfully initialized. or ENOMEM.
 */
int ImageC_New(pImageC_t image, const Size2iC_t size, const RectC_t roi);

/**
 * @brief 
 * Release the memory block allocated by ImageC_New(). free() is underlying.
 * @param image [in,out] The object to release. It is filled with NULLIMAGE_C by the function.
 */
void ImageC_Delete(pImageC_t image);

/**
 * @brief Convert a uint8_t pixel packed image to an ImageC_t object.
 * Packed image means that the image has no edge margin.
 * 
 * @param image [in,out] The object is resized if its original size does not match.
 * @param packedImage [in] pointer to the beginning element of the packed image.
 * @param size [in] whole buffer size of image after the operation.
 * @param roi [in] region of interest of image after the operation.
 * @return result of underlying ImageC_New().
 */
int ImageC_SetPackedImage(pImageC_t image, const uint8_t* packedImage, const Size2iC_t size, const RectC_t roi);

/**
 * @brief Convert a float pixel packed image to an ImageC_t object.
 * 
 * @param image [in,out] The object is resized if its original size does not match.
 * @param packedImage [in] pointer to the beginning element of the packed image.
 * @param size [in] whole buffer size of image after the operation.
 * @param roi [in] region of interest of image after the operation.
 * @return result of underlying ImageC_New().
 */
int ImageC_SetPackedImageF(pImageC_t image, const float* packedImage, const Size2iC_t size, const RectC_t roi);

/**
 * @brief create a clone of the original image.
 * 
 * @param image [in,out] clone image.
 * @param source [in] original image
 * @return result of underlying ImageC_New().
 */
int ImageC_Clone(pImageC_t image, pcImageC_t source);

/**
 * @brief apply 2-D summing operation preparing a summed area table. Refer to
 * https://en.wikipedia.org/wiki/Summed-area_table
 * The summing operation is applied to both inside and outside roi preventing pointer overrun
 * by SumKernelC_SATSum() macro scanning over roi. Refer to SumKernelC.h for details.
 * SumKernelC_SATSum() is a macro to calculate a sum over a region defined by sumRect.
 * @param image [in,out]
 */
void ImageC_Integrate(pImageC_t image);

/**
 * @brief element-by-element addition is applied over whole buffer.
 * symbolic meaning is 'destination->elements[*] += source->elements[*];'.
 * @param destination [in,out]
 * @param source [in]
 */
void ImageC_Add(pImageC_t destination, pcImageC_t source);

/**
 * @brief element-by-element multiplication is applied over whole buffer.
 * symbolic meaning is 'destination->elements[*] *= source->elements[*];'.
 * @param destination [in,out]
 * @param source [in]
 */
void ImageC_Multiply(pImageC_t destination, pcImageC_t source);

/**
 * @brief element-by-element subtraction is applied over whole buffer.
 * symbolic meaning is 'destination->elements[*] -= source->elements[*];'.
 * @param destination [in,out]
 * @param source [in]
 */
void ImageC_Subtract(pImageC_t destination, pcImageC_t source);

/**
 * @brief Calculate the local mean of of source pixels over sumRect.
 * The operation is applied to both insde and outside roi except minimum edge margin
 * reqired by sumRect.
 * @param mean [out] mean image is resized if its original size does not match.
 * @param source [in]
 * @param sumRect [in] region of averaging
 * @return result of underlying ImageC_New().
 */
int ImageC_Mean(pImageC_t mean, pcImageC_t source, const RectC_t sumRect);

/**
 * @brief Return the pointer to the top-left element for read/write operation.
 * 
 */
#define ImageC_Begin(image) ((image)->elements + (image)->roi[0] + (image)->roi[1] * (image)->size[0])

/**
 * @brief Return the pointer to the top-left element for read-only operation.
 * 
 */
#define ImageC_BeginC(image) ((const float*)((image)->elements + (image)->roi[0] + (image)->roi[1] * (image)->size[0]))

/**
 * @brief negate all elements
 * 
 * @param img 
 */
void ImageC_Negate(pImageC_t img);
#ifdef __cplusplus
}
#endif
#endif /* IMAGEC_H_ */