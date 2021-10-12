#include "ZnccC/CostMapGenC.h"
#include <stdio.h>
#include <errno.h>

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

typedef struct {
    Size2iC_t packedSize;
    int32_t *dispx, *dispy;
    float* costs;
} TempCostMap_t, *pTempCostMap_t;
typedef const TempCostMap_t *pcTempCostMap_t;

#define NULL_TEMPCOSTMAP    { {0,0}, NULL, NULL, NULL }

static void TempCostMap_Delete(pTempCostMap_t tempCostMap)
{
    if (tempCostMap->dispx)
    {
        free(tempCostMap->dispx);
        tempCostMap->dispx = tempCostMap->dispy = NULL;
    }
    if (tempCostMap->costs)
    {
        free(tempCostMap->costs);
        tempCostMap->costs = NULL;
    }
    tempCostMap->packedSize[0] = tempCostMap->packedSize[1] = 0;
}

static int TempCostMap_New(pTempCostMap_t tempCostMap, const Size2iC_t packedSize)
{
    int err = EXIT_SUCCESS;
    do {
        size_t packedArea = AREA_SIZE(packedSize);
        size_t allocSizeIntArray = sizeof(int32_t) * packedArea * 2;
        size_t allocSizeFloatArray = sizeof(float) * packedArea;
        tempCostMap->dispx = (int32_t*)malloc(allocSizeIntArray);
        if (tempCostMap->dispx == NULL)
        {
            err = ENOMEM;
            break;
        }
        tempCostMap->costs = (float*)malloc(allocSizeFloatArray);
        if (tempCostMap->costs == NULL)
        {
            err = ENOMEM;
            break;
        }
        tempCostMap->dispy = tempCostMap->dispx + packedArea;
        COPY2(tempCostMap->packedSize, packedSize);
    } while (0);
    if (err)
    {
        TempCostMap_Delete(tempCostMap);
    }
    return err;
}

static void TempCostMap_Set(pTempCostMap_t tempCostMap, float costIni)
{
    size_t packedArea = AREA_SIZE(tempCostMap->packedSize);
    for (size_t i = 0; i != packedArea; i++)
    {
        tempCostMap->dispx[i] = tempCostMap->dispy[i] = INT32_MIN;
        tempCostMap->costs[i] = costIni;
    }
}

static int TempCostMap_ToDispmap(pcTempCostMap_t tempCostMap, pImageC_t dispx, pImageC_t dispy, pcZnccMatchingSizesC_t sizes)
{
    const int invalidDispXLimit = sizes->searchRect[0];
    int err = EXIT_SUCCESS;
    do {
        if ((EXIT_SUCCESS != (err = ImageC_New(dispx, sizes->imageBufferSize, sizes->imageRoi))) ||
            (EXIT_SUCCESS != (err = ImageC_New(dispy, sizes->imageBufferSize, sizes->imageRoi))))
        {
            break;
        }
        float* dispXPtr = ImageC_Begin(dispx);
        float* dispYPtr = ImageC_Begin(dispy);
        const int stride = dispx->size[0];
        int linearPackedIndex = 0;
        for (int iRow = 0; iRow != tempCostMap->packedSize[1]; iRow++)
        {
            for (int iCol = 0; iCol != tempCostMap->packedSize[0]; iCol++)
            {
                if (tempCostMap->dispx[linearPackedIndex] >= invalidDispXLimit)
                {
                    dispXPtr[iCol] = (float)(tempCostMap->dispx[linearPackedIndex]);
                    dispYPtr[iCol] = (float)(tempCostMap->dispy[linearPackedIndex]);
                }
                else
                {
                    dispXPtr[iCol] = dispYPtr[iCol] = NO_MATCH_FOUND;
                }
                linearPackedIndex++;
            }
            dispXPtr += stride;
            dispYPtr += stride;
        }
    } while (0);
    return err;
}

static void SetLatestCosts(
    pcZnccHalfC_t base, pcZnccHalfC_t shifted,
    pcImageC_t xprod, pImageC_t latestCosts, const Point2iC_t searchPoint, pcZnccMatchingSizesC_t sizes
) {
    const int area1 = AREA_RECT(sizes->sumRect) - 1;
    const float rcpArea1 = 1.0f / (float)area1;
    const int32_t* roi = base->rcpstddev.roi;
    const int stride = base->rcpstddev.size[0];
    const int shiftOffset = searchPoint[0] + searchPoint[1] * stride;
    const float* baseRcpStdDevPtr = ImageC_BeginC(&base->rcpstddev);
    const float* shiftedRcpStdDevPtr = ImageC_BeginC(&shifted->rcpstddev) + shiftOffset;
    SumKernelC_t skXProd = MK_SUMKERNEL(xprod, sizes->sumRect);
    float* latestCostPtr = ImageC_Begin(latestCosts);
    for (int iRow = 0; iRow != roi[3]; iRow++)
    {
        for (int iCol = 0; iCol != roi[2]; iCol++)
        {
            latestCostPtr[iCol] = rcpArea1 * SumKernelC_SATSum(&skXProd, iCol) * baseRcpStdDevPtr[iCol] * shiftedRcpStdDevPtr[iCol];
        }
        baseRcpStdDevPtr += stride;
        shiftedRcpStdDevPtr += stride;
        latestCostPtr += stride;
        skXProd.base += stride;
    }
}

static void TempCostMap_Update(pTempCostMap_t tempCostMap, pcImageC_t latestCosts, const Point2iC_t searchPoint)
{
    const float* latestCostPtr = ImageC_BeginC(latestCosts);
    int linearPackedIndex = 0;
    for (int iRow = 0; iRow != tempCostMap->packedSize[1]; iRow++)
    {
        for (int iCol = 0; iCol != tempCostMap->packedSize[0]; iCol++)
        {
            if (tempCostMap->costs[linearPackedIndex] < latestCostPtr[iCol])
            {
                tempCostMap->costs[linearPackedIndex] = latestCostPtr[iCol];
                tempCostMap->dispx[linearPackedIndex] = searchPoint[0];
                tempCostMap->dispy[linearPackedIndex] = searchPoint[1];
            }
            linearPackedIndex++;
        }
        latestCostPtr += latestCosts->size[0];
    }
}

int CostMapGen_FindMax(
    pcZnccHalfC_t base, pcZnccHalfC_t shifted,
    pcZnccMatchingSizesC_t sizes, pImageC_t dispx, pImageC_t dispy
) {
    TempCostMap_t currentMax = NULL_TEMPCOSTMAP;
    ImageC_t xprod = NULLIMAGE_C, latestCosts = NULLIMAGE_C;
    int err = EXIT_SUCCESS;
    do {
        if ((EXIT_SUCCESS != (err = TempCostMap_New(&currentMax, sizes->packedSize))) ||
            (EXIT_SUCCESS != (err = ImageC_New(&latestCosts, sizes->imageBufferSize, sizes->imageRoi))))
        {
            break;
        }
        TempCostMap_Set(&currentMax, COST_LOWER_LIMIT); // initialize with the lower limit of cost function.
        // scan over the search rect
        for (int iSearchV = 0; iSearchV != sizes->searchRect[3]; iSearchV++)
        {
            for (int iSearchH = 0; iSearchH != sizes->searchRect[2]; iSearchH++)
            {
                // calculate cost function values at the search point
                const Point2iC_t searchPoint = { iSearchH + sizes->searchRect[0], iSearchV + sizes->searchRect[1] };
                if (EXIT_SUCCESS != (err = CostMapGen_CreateXProd(&base->lumdev, &shifted->lumdev, sizes->searchRect, searchPoint, &xprod)))
                {
                    break;
                }
                SetLatestCosts(base, shifted, &xprod, &latestCosts, searchPoint, sizes);
                // update the maximum costs
                TempCostMap_Update(&currentMax, &latestCosts, searchPoint);
            }
            if (err) { break; }
        }
        if (err) { break; }
        if (EXIT_SUCCESS != (err = TempCostMap_ToDispmap(&currentMax, dispx, dispy, sizes)))
        {
            break;
        }
    } while (0);
    ImageC_Delete(&xprod);
    ImageC_Delete(&latestCosts);
    TempCostMap_Delete(&currentMax);
    return err;
}
