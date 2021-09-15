#include "ZnccC/ZnccHalfC.h"

int ZnccHalfC_SetImage(pZnccHalfC_t znccHalf, pcImageC_t image,
    pcZnccMatchingSizesC_t sizes, pcSpecialImagesC_t si
)  {
    int err = EXIT_SUCCESS;
    const int stride = image->size[0];
    ImageC_t imageForMean = NULLIMAGE;
    ImageC_t imageForScattering = NULLIMAGE;
    const float areaSumRect1 = (float)(AREA_RECT(sizes->sumRect) - 1);
    do {
        if ((EXIT_SUCCESS != (err = ImageC_Clone(&imageForMean, image))) ||
            (EXIT_SUCCESS != (err = ImageC_Clone(&znccHalf->lumdev, image))) ||
            (EXIT_SUCCESS != (err = ImageC_Clone(&znccHalf->random, &si->AllRand))) ||
            (EXIT_SUCCESS != (err = ImageC_New(&znccHalf->rcpstddev, image->size, image->roi))))
        {
            break;
        }
        ImageC_Add(&imageForMean, &si->RandBgZeroFg);
        ImageC_Add(&znccHalf->lumdev, &si->RandBgZeroFg);
        {
            const RectC_t marginRect = {
                sizes->sumRect[0] - 1, sizes->sumRect[1] - 1, sizes->sumRect[2] + 1, sizes->sumRect[3] + 1 };
            const RectC_t tempRoi = ROI(imageForMean.size, marginRect);
            COPY4(imageForMean.roi, tempRoi);
            COPY4(znccHalf->mean.roi, tempRoi);
            COPY4(znccHalf->rcpstddev.roi, tempRoi);
        }
        ImageC_Integrate(&imageForMean);
        SumKernelC_t skMean = MK_SUMKERNEL(&imageForMean, sizes->sumRect);
        float* meanPtr = ImageC_Begin(&znccHalf->mean);
        float* lumdevPtr = ImageC_Begin(&znccHalf->lumdev);
        for (int iRow = 0; iRow != imageForMean.roi[3]; iRow++)
        {
            for (int iCol = 0; iCol != imageForMean.roi[2]; iCol++)
            {
                meanPtr[iCol] = SumKernelC_SATSum(&skMean, iCol);
                lumdevPtr[iCol] -= meanPtr[iCol];
            }
            skMean.base += stride;
            meanPtr += stride;
            lumdevPtr += stride;
        }
        if (EXIT_SUCCESS != (err = ImageC_Clone(&imageForScattering, &znccHalf->lumdev)))
        {
            break;
        }
        ImageC_Multiply(&imageForScattering, &znccHalf->lumdev);
        ImageC_Integrate(&imageForScattering);
        float* rcpstddevPtr = ImageC_Begin(&znccHalf->rcpstddev);
        SumKernelC_t skScattering = MK_SUMKERNEL(&imageForScattering, sizes->sumRect);
        for (int iRow = 0; iRow != imageForMean.roi[3]; iRow++)
        {
            for (int iCol = 0; iCol != imageForMean.roi[2]; iCol++)
            {
                rcpstddevPtr[iCol] = areaSumRect1 / SumKernelC_SATSum(&skScattering, iCol);
            }
            skScattering.base += stride;
            rcpstddevPtr += stride;
        }
        COPY4(znccHalf->lumdev.roi, image->roi);
        COPY4(znccHalf->mean.roi, image->roi);
        COPY4(znccHalf->rcpstddev.roi, image->roi);
    } while (0);
    ImageC_Delete(&imageForMean);
    ImageC_Delete(&imageForScattering);
    return err;
}

void ZnccHalfC_Delete(pZnccHalfC_t znccHalf)
{
    ImageC_Delete(&znccHalf->lumdev);
    ImageC_Delete(&znccHalf->mean);
    ImageC_Delete(&znccHalf->rcpstddev);
    ImageC_Delete(&znccHalf->random);
}