#include "ZnccC/CostMapC.h"
#include <stdlib.h>
#include <errno.h>

int CostMapC_New(pCostMapC_t costmap, const RectC_t roi, const RectC_t searchRect)
{
    int err = EXIT_SUCCESS;
    do {
        if (ARE_NE_RECT(roi, costmap->pixelRoi) || ARE_NE_RECT(searchRect, costmap->searchRect))
        {
            size_t allocBytes = sizeof(float) * AREA_RECT(roi) * AREA_RECT(searchRect);
            CostMapC_Delete(costmap);
            costmap->costs = (float*)malloc(allocBytes);
            if (NULL == costmap->costs)
            {
                err = ENOMEM;
                break;
            }
            COPY4(costmap->pixelRoi, roi);
            COPY4(costmap->searchRect, searchRect);
        }
    } while (0);
    return err;
}

void CostMapC_Delete(pCostMapC_t costmap)
{
    if (costmap->costs)
    {
        free(costmap->costs);
        costmap->costs = NULL;
    }
    ZERO_RECT(costmap->pixelRoi);
    ZERO_RECT(costmap->searchRect);
}

int CostMapC_PixelPlaneArea(pcCostMapC_t costmap)
{
    return AREA_RECT(costmap->pixelRoi);
}

int CostMapC_SearchPlaneArea(pcCostMapC_t costmap)
{
    return AREA_RECT(costmap->searchRect);
}


int CostSearchTableC_New(pCostSearchTableC_t costSearchTable, pcCostMapC_t costmap)
{
    int err = EXIT_SUCCESS;
    do {
        if (ARE_NE_RECT(costmap->searchRect, costSearchTable->searchRect))
        {
            size_t allocBytes = AREA_RECT(costmap->searchRect) * sizeof(float);
            CostSearchTableC_Delete(costSearchTable);
            costSearchTable->costs = (float*)malloc(allocBytes);
            if (NULL == costSearchTable->costs)
            {
                err = ENOMEM;
                break;
            }
        }
        COPY4(costSearchTable->searchRect, costmap->searchRect);
    } while (0);
    return err;
}

void CostSearchTableC_Delete(pCostSearchTableC_t costSearchTable)
{
    if (costSearchTable->costs)
    {
        free(costSearchTable->costs);
        costSearchTable->costs = NULL;
    }
    ZERO_RECT(costSearchTable->searchRect);
}

void CostSearchTableC_Gather(
    pCostSearchTableC_t costSearchTable, pcCostMapC_t costmap, const Point2iC_t packedPixelPoint
) {
    assert(ARE_EQ_RECT(costSearchTable->searchRect, costmap->searchRect));
    int pixelPlaneArea = CostMapC_PixelPlaneArea(costmap);
    int pixelOffset = packedPixelPoint[0] + packedPixelPoint[1] * costmap->pixelRoi[2];
    const float* costsSrc = costmap->costs + pixelOffset;
    float* costsDst = costSearchTable->costs;
    const int linearSearchTableSize = AREA_RECT(costSearchTable->searchRect);
    for (int i = 0; i != linearSearchTableSize; i++)
    {
        costsDst[i] = *costsSrc;
        costsSrc += pixelPlaneArea;
    }
}