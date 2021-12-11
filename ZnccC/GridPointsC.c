#include "ZnccC/GridPointsC.h"
#include <errno.h>

/**
 * @brief count needed size of grid point for the baseImage.
 * 
 * @param gridStep [in] beggining point offset and grid step
 * @param baseImage [in] base image
 * @param size [out] resulted counts in x-directioni and y-direction
 */
static void GridPointsC_Count(const RectC_t gridStep, pcImageC_t baseImage, Size2iC_t size)
{
    int32_t imageEnd = baseImage->roi[0] + baseImage->roi[2];
    for (size[0] = 0; ; size[0]++)
    {
        int32_t x = gridStep[0] + size[0] * gridStep[2];
        int32_t xEnd = x + gridStep[2];
        if (xEnd > imageEnd) break;
    }
    size[0]--;
    imageEnd = baseImage->roi[1] + baseImage->roi[3];
    for (size[1] = 0; ; size[1]++)
    {
        int32_t y = gridStep[1] + size[1] * gridStep[3];
        int32_t yEnd = y + gridStep[3];
        if (yEnd > imageEnd) break;
    }
    size[1]--;
}

void GridPointsC_Fill(pGridPointsC_t gridPoints, const RectC_t gridStep, pcImageC_t baseImage)
{
    GridPointsC_Count(gridStep, baseImage, gridPoints->size);
    COPY4(gridPoints->gridStep, gridStep);
    COPY4(gridPoints->baseRoi, baseImage->roi);
}
