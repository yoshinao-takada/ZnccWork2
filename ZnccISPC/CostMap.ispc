#include "CostMap.isph"

export uniform ZnccError CostMap_New(
    uniform CostMap& costmap, uniform const Size2i& pixelSize, uniform const Size2i& searchSize
) {
    uniform ZnccError err = errSuccess;
    do {
        if (AreNotEqual(costmap.pixelSize, pixelSize) || AreNotEqual(costmap.searchSize, searchSize))
        {
            CostMap_Delete(costmap);
            uniform const int costmapVolume = Area(pixelSize) * Area(searchSize);
            costmap.costs = uniform new float[costmapVolume];
            if (costmap.costs == NULL)
            {
                err = errNoMem;
                break;
            }
        }
        costmap.pixelSize = pixelSize;
        costmap.searchSize = searchSize;
    } while (0);
    return err;
}

export void CostMap_Delete(uniform CostMap& costmap)
{
    if (costmap.costs)
    {
        delete[] costmap.costs;
    }
    costmap.costs = NULL;
    Zero(costmap.pixelSize);
    Zero(costmap.searchSize);
}

export uniform ZnccError CostSearchTable_New(uniform CostSearchTable& costSearchTable, uniform const Rect& searchRect
) {
    if (AreNotEqual(costSearchTable.searchRect, searchRect))
    {
        CostSearchTable_Delete(costSearchTable);
        costSearchTable.costs = uniform new float[Area(searchRect)];
        if (costSearchTable.costs == NULL)
        {
            return errNoMem;
        }
        costSearchTable.searchRect = searchRect;
    }
    return errSuccess;
}

export void CostSearchTable_Delete(uniform CostSearchTable& costSearchTable)
{
    if (costSearchTable.costs != NULL)
    {
        delete[] costSearchTable.costs;
        costSearchTable.costs = NULL;
    }
    Zero(costSearchTable.searchRect);
}

export void CostMap_Gather(
    uniform const CostMap& costmap, uniform CostSearchTable& costSearchTable, uniform const Point2i& pixelPoint
) {
    assert((costmap.searchSize[0] == costSearchTable.searchRect[2]) &&
        (costmap.searchSize[1] == costSearchTable.searchRect[3]));
    uniform const int step = Area(costmap.pixelSize);
    uniform const int offset = pixelPoint[0] + pixelPoint[1] * costmap.pixelSize[0];
    uniform const int searchRectArea = Area(costmap.searchSize);
    for (uniform int i = 0; i != searchRectArea; i++)
    {
        costSearchTable.costs[i] = costmap.costs[offset + step * i];
    }
}

export void CostSearchTable_FindMaximum(
    uniform const CostSearchTable& costSearchTable, uniform const float minimumCost, uniform Point2i& foundPoint
) {
    uniform Point2i foundMax = { __INT32_MIN__, __INT32_MIN__ };
    uniform float foundMaxCost = minimumCost;
    for (uniform int iV = 0; iV != costSearchTable.searchRect[3]; ++iV)
    {
        for (uniform int iH = 0; iH != costSearchTable.searchRect[2]; ++iH)
        {
            uniform const int linearIndex = iH + iV * costSearchTable.searchRect[2];
            uniform const float currentCostValue = costSearchTable.costs[linearIndex];
            if (costSearchTable.costs[linearIndex] > foundMaxCost)
            {
                foundMaxCost = currentCostValue;
                foundMax[0] = iH;
                foundMax[1] = iV;
            }
        }
    }
    if (foundMax[0] != __INT32_MIN__)
    {
        foundPoint[0] = foundMax[0] + costSearchTable.searchRect[0];
        foundPoint[1] = foundMax[1] + costSearchTable.searchRect[1];
    }
    else
    {
        foundPoint[0] = foundMax[0];
        foundPoint[1] = foundMax[1];
    }
}