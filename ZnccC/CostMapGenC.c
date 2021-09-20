#include "ZnccC/CostMapGenC.h"
#include <stdio.h>

static int CostMapGen_CreateXProd(pcImageC_t base, pcImageC_t shifted, 
    const RectC_t searchRect, const Point2iC_t searchPoint, pImageC_t xprod)
{
    const int stride = base->size[0];
    int err = EXIT_SUCCESS;
    do {
        if (EXIT_SUCCESS != (err = ImageC_New(xprod, base->size, base->roi)))
        {
            break;
        }
        int linearSearchOffset = searchPoint[0] + searchPoint[1] * stride;
        const float* basePtr = base->elements;
        const float* shiftedPtr = shifted->elements + linearSearchOffset;
        float* xprodPtr = xprod->elements;
        int loopBegin = 0, loopEnd = AREA_SIZE(base->size);
        if (linearSearchOffset >= 0)
        {
            loopEnd -= linearSearchOffset;
        }
        else
        {
            loopBegin -= linearSearchOffset;
        }
        for (int i = loopBegin; i!= loopEnd; i++)
        {
            xprodPtr[i] = basePtr[i] * shiftedPtr[i];
        }
        ImageC_Integrate(xprod);
    } while (0);
    return err;
}

static int CostMapGen_FillCostMapAtSearchPoint(
    pcZnccHalfC_t base, pcZnccHalfC_t shifted,
    pcZnccMatchingSizesC_t sizes, pCostMapC_t costmap, const Point2iC_t searchPoint
) {
    const int stride = base->rcpstddev.size[0];
    const int packedStride = base->rcpstddev.roi[2];
    const int rows = base->rcpstddev.roi[3];
    const float rcpArea1 = 1.0f / (float)(AREA_RECT(sizes->sumRect) - 1);
    int err = EXIT_SUCCESS;
    ImageC_t xprod = NULLIMAGE_C;
    do {
        if (EXIT_SUCCESS != 
            (err = CostMapGen_CreateXProd(&base->lumdev, &shifted->lumdev, sizes->searchRect, searchPoint, &xprod)))
        {
            break;
        }
        const int searchPointOffset = searchPoint[0] + searchPoint[1] * stride;
        const float* baseRcpStdDev = ImageC_BeginC(&base->rcpstddev);
        const float* shiftedRcpStdDev = ImageC_BeginC(&shifted->rcpstddev) + searchPointOffset;
        const Point2iC_t packedSearchPoint = 
            { searchPoint[0] - sizes->searchRect[0], searchPoint[1] - sizes->searchRect[1] };
        const int linearPackedSearchPoint = packedSearchPoint[0] + packedSearchPoint[1] * sizes->searchRect[2];
        float* costs = costmap->costs + linearPackedSearchPoint * AREA_RECT(costmap->pixelRoi);
        SumKernelC_t skXProd = MK_SUMKERNEL(&xprod, sizes->sumRect);
        for (int iRow = 0; iRow != rows; iRow++)
        {
            for (int iCol = 0; iCol != packedStride; iCol++)
            {
                costs[iCol] = rcpArea1 * SumKernelC_SATSum(&skXProd, iCol) * baseRcpStdDev[iCol] * shiftedRcpStdDev[iCol];
                // printf("costs[]=%f, rcpArea1=%f, SATSum=%f, baseRcpStdDev=%f, shiftedRcpStdDev=%f\n", 
                //     costs[iCol], rcpArea1, SumKernelC_SATSum(&skXProd, iCol), baseRcpStdDev[iCol], shiftedRcpStdDev[iCol]);
            }
            costs += packedStride;
            skXProd.base += stride;
            baseRcpStdDev += stride;
            shiftedRcpStdDev += stride;
        }
    } while (0);
    ImageC_Delete(&xprod);
    return err;
}

int CostMapGenC_FillCostMap(
    pcZnccHalfC_t base, pcZnccHalfC_t shifted,
    pcZnccMatchingSizesC_t sizes, pCostMapC_t costmap
) {
    int err = EXIT_SUCCESS;
    do {
        if (EXIT_SUCCESS != (err = CostMapC_New(costmap, sizes->imageRoi, sizes->searchRect)))
        {
            break;
        }
        for (int iSearchV = 0; iSearchV != sizes->searchRect[3]; iSearchV++)
        {
            for (int iSearchH = 0; iSearchH != sizes->searchRect[2]; iSearchH++)
            {
                const Point2iC_t searchPoint = { iSearchH + sizes->searchRect[0], iSearchV + sizes->searchRect[1] };
                err = CostMapGen_FillCostMapAtSearchPoint(base,shifted, sizes, costmap, searchPoint);
            }
        }
    } while (0);
    return err;
}
