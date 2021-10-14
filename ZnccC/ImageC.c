#include "ZnccC/ImageC.h"
#include "ZnccC/SumKernelC.h"
#include <errno.h>
#include <stdlib.h>
#include <memory.h>

int ImageC_New(pImageC_t image, const Size2iC_t size, const RectC_t roi)
{
    int err = EXIT_SUCCESS;
    int area = AREA_SIZE(size);
    do {
        if (ARE_NE_SIZE(image->size, size))
        {
            ImageC_Delete(image);
            image->elements = (float*)malloc(sizeof(float) * area);
            if (NULL == image->elements)
            {
                err = ENOMEM;
                break;
            }
        }
        COPY2(image->size, size);
        COPY4(image->roi, roi);
        for (int i = 0; i != area; i++)
        {
            image->elements[i] = 0.0f;
        }
    } while (0);
    return err;
}

void ImageC_Delete(pImageC_t image)
{
    if (image->elements)
    {
        free(image->elements);
        image->elements = NULL;
    }
    ZERO_SIZE(image->size);
    ZERO_RECT(image->roi);
}

int ImageC_SetPackedImage(pImageC_t image, const uint8_t* packedImage, const Size2iC_t size, const RectC_t roi)
{
    int err = EXIT_SUCCESS;
    do {
        if (EXIT_SUCCESS != (err = ImageC_New(image,size, roi)))
        {
            break;
        }
        float* dstF = ImageC_Begin(image);
        const uint8_t* srcU8 = packedImage;
        for (int iRow = 0; iRow != roi[3]; iRow++)
        {
            for (int iCol = 0; iCol != roi[2]; iCol++)
            {
                dstF[iCol] = (float)srcU8[iCol];
            }
            dstF += size[0];
            srcU8 += roi[2];
        }
    } while (0);
    return err;
}

int ImageC_SetPackedImageF(pImageC_t image, const float* packedImage, const Size2iC_t size, const RectC_t roi)
{
    int err = EXIT_SUCCESS;
    do {
        if (EXIT_SUCCESS != (err = ImageC_New(image,size, roi)))
        {
            break;
        }
        float* dstF = ImageC_Begin(image);
        const float* srcF = packedImage;
        size_t copySize = roi[2] * sizeof(float);
        for (int iRow = 0; iRow != roi[3]; iRow++)
        {
            memcpy(dstF, srcF, copySize);
            dstF += size[0];
            srcF += roi[2];
        }
    } while (0);
    return err;
}

int ImageC_Clone(pImageC_t image, pcImageC_t source)
{
    int err = EXIT_SUCCESS;
    do {
        if (EXIT_SUCCESS != (err = ImageC_New(image, source->size, source->roi)))
        {
            break;
        }
        memcpy(image->elements, source->elements, sizeof(float) * AREA_SIZE(source->size));
    } while (0);
    return err;
}

void ImageC_Integrate(pImageC_t image)
{
    float* ptr = image->elements;
    int offsetRight = 1;
    int offsetLower = image->size[0];
    int offsetLowerRight = offsetLower + offsetRight;
    int stride = image->size[0];
    int endRow = image->size[1] - 1;
    int endCol = image->size[0] - 1;
    for (int iRow = 0; iRow != endRow; iRow++)
    {
        for (int iCol = 0; iCol != endCol; iCol++)
        {
            ptr[iCol + offsetLowerRight] += ((ptr[iCol + offsetRight] - ptr[iCol]) + ptr[iCol + offsetLower]);
        }
        ptr += stride;
    }
}

void ImageC_Add(pImageC_t destination, pcImageC_t source)
{
    assert(ARE_EQ_SIZE(destination->size, source->size));
    assert(ARE_EQ_RECT(destination->roi, source->roi));
    int area = AREA_SIZE(destination->size);
    for (int i = 0; i != area; i++)
    {
        destination->elements[i] += source->elements[i];
    }
}

void ImageC_Multiply(pImageC_t destination, pcImageC_t source)
{
    assert(ARE_EQ_SIZE(destination->size, source->size));
    assert(ARE_EQ_RECT(destination->roi, source->roi));
    int area = AREA_SIZE(destination->size);
    for (int i = 0; i != area; i++)
    {
        destination->elements[i] *= source->elements[i];
    }
}

void ImageC_Subtract(pImageC_t destination, pcImageC_t source)
{
    assert(ARE_EQ_SIZE(destination->size, source->size));
    assert(ARE_EQ_RECT(destination->roi, source->roi));
    int area = AREA_SIZE(destination->size);
    for (int i = 0; i != area; i++)
    {
        destination->elements[i] -= source->elements[i];
    }
}

int ImageC_Mean(pImageC_t mean, pcImageC_t source, const RectC_t sumRect)
{
    const int stride = source->size[0];
    int err = EXIT_SUCCESS;
    ImageC_t imageForMean = NULLIMAGE_C;
    const float rcpSumArea = 1.0f / (float)AREA_RECT(sumRect);
    do {
        const RectC_t marginRect = { sumRect[0] - 1, sumRect[1] - 1, sumRect[2] + 1, sumRect[3] + 1 };
        const RectC_t tempRoi = {
            -marginRect[0], -marginRect[1],
            source->size[0] - marginRect[2], source->size[1] - marginRect[3] };
        if (EXIT_SUCCESS != (err = ImageC_Clone(&imageForMean, source)))
        { // heap used up
            break;
        }
        COPY4(imageForMean.roi, tempRoi);
        if (EXIT_SUCCESS != (err = ImageC_New(mean, source->size, tempRoi)))
        { // heap used up
            break;
        }
        ImageC_Integrate(&imageForMean);
        SumKernelC_t sk = MK_SUMKERNEL(&imageForMean, sumRect);
        float* meanPtr = ImageC_Begin(mean);
        for (int iRow = 0; iRow != tempRoi[3]; iRow++)
        {
            for (int iCol = 0; iCol != tempRoi[2]; iCol++)
            {
                meanPtr[iCol] = rcpSumArea * SumKernelC_SATSum(&sk, iCol);
            }
            sk.base += stride;
            meanPtr += stride;
        }
        COPY4(mean->roi, source->roi);
    }  while (0);
    ImageC_Delete(&imageForMean);
    return err;
}

void ImageC_Negate(pImageC_t img)
{
    int area = AREA_SIZE(img->size);
    if (area == 0) return;
    for (int i = 0; i != area; i++)
    {
        img->elements[i] = -(img->elements[i]);
    }
}
