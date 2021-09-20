#include "ZnccC/SpecialImagesC.h"
int SpecialImagesC_Create(pSpecialImagesC_t si, const Size2iC_t size, const RectC_t roi)
{
    int err = EXIT_SUCCESS;
    do {
        srand((int)(intptr_t)&si);
        if ((EXIT_SUCCESS != (err = ImageC_New(&si->AllRand, size, roi))) ||
            (EXIT_SUCCESS != (err = ImageC_New(&si->FltMaxBgZeroFg, size, roi))) ||
            (EXIT_SUCCESS != (err = ImageC_New(&si->OneBgZeroFg, size, roi))) ||
            (EXIT_SUCCESS != (err = ImageC_New(&si->RandBgZeroFg, size, roi))) ||
            (EXIT_SUCCESS != (err = ImageC_New(&si->ZeroBgOneFg, size, roi))))
        {
            break;
        }
        int area = AREA_SIZE(size);
        for (int i = 0; i != area; i++)
        {
            si->AllRand.elements[i] = (float)(rand() & 0xff);
        }
        for (int iRow = 0; iRow != size[1]; iRow++)
        {
            for (int iCol = 0; iCol != size[0]; iCol++)
            {
                Point2iC_t point = { iCol, iRow };
                int linearIndex = iCol + iRow * size[0];
                if (IS_IN_RECT(point, roi))
                {
                    si->FltMaxBgZeroFg.elements[linearIndex] = 0.0f;
                    si->OneBgZeroFg.elements[linearIndex] = 0.0f;
                    si->RandBgZeroFg.elements[linearIndex] = 0.0f;
                    si->ZeroBgOneFg.elements[linearIndex] = 1.0f;
                }
                else
                {
                    si->FltMaxBgZeroFg.elements[linearIndex] = __FLT_MAX__;
                    si->OneBgZeroFg.elements[linearIndex] = 1.0f;
                    si->RandBgZeroFg.elements[linearIndex] = si->AllRand.elements[linearIndex];
                    si->ZeroBgOneFg.elements[linearIndex] = 0.0f;
                }
            }
        }
    } while (0);
    return err;
}


void SpecialImagesC_Delete(pSpecialImagesC_t si)
{
    ImageC_Delete(&si->ZeroBgOneFg);
    ImageC_Delete(&si->OneBgZeroFg);
    ImageC_Delete(&si->RandBgZeroFg);
    ImageC_Delete(&si->FltMaxBgZeroFg);
    ImageC_Delete(&si->AllRand);
}