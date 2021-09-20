#include "ZnccC/ZnccHalfC.h"
#include <math.h>

int ZnccHalfC_SetImage(pZnccHalfC_t znccHalf, pcImageC_t image,
    pcZnccMatchingSizesC_t sizes, pcSpecialImagesC_t si
)  {
    int err = EXIT_SUCCESS;
    const int stride = image->size[0];
    ImageC_t imageForScattering = NULLIMAGE_C;
    const float areaSumRect1 = (float)(AREA_RECT(sizes->sumRect) - 1);
    do {
        if ((EXIT_SUCCESS != (err = ImageC_Clone(&znccHalf->lumdev, image))) ||
            (EXIT_SUCCESS != (err = ImageC_Clone(&znccHalf->random, &si->AllRand))) ||
            (EXIT_SUCCESS != (err = ImageC_New(&znccHalf->rcpstddev, image->size, image->roi))))
        {
            break;
        }
        ImageC_Add(&znccHalf->lumdev, &si->RandBgZeroFg);
        ImageC_Mean(&znccHalf->mean, &znccHalf->lumdev, sizes->sumRect);
        ImageC_Subtract(&znccHalf->lumdev, &znccHalf->mean);
        if (EXIT_SUCCESS != (err = ImageC_Clone(&imageForScattering, &znccHalf->lumdev)))
        {
            break;
        }
        ImageC_Multiply(&imageForScattering, &znccHalf->lumdev);
        ImageC_Integrate(&imageForScattering);
        float* rcpstddevPtr = ImageC_Begin(&znccHalf->rcpstddev);
        SumKernelC_t skScattering = MK_SUMKERNEL(&imageForScattering, sizes->sumRect);
        for (int iRow = 0; iRow != imageForScattering.roi[3]; iRow++)
        {
            for (int iCol = 0; iCol != imageForScattering.roi[2]; iCol++)
            {
                rcpstddevPtr[iCol] = sqrtf(areaSumRect1 / SumKernelC_SATSum(&skScattering, iCol));
            }
            skScattering.base += stride;
            rcpstddevPtr += stride;
        }
        COPY4(znccHalf->lumdev.roi, image->roi);
        COPY4(znccHalf->mean.roi, image->roi);
        COPY4(znccHalf->rcpstddev.roi, image->roi);
    } while (0);
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