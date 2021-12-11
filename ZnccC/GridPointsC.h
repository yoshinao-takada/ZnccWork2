#ifndef GRIDPOINTSC_H_
#define GRIDPOINTSC_H_
#include "ZnccC/ImageC.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct {
    Size2iC_t size;
    RectC_t baseRoi;
    RectC_t gridStep;
} GridPointsC_t, *pGridPointsC_t;
typedef const GridPointsC_t *pcGridPointsC_t;
#define NULLGRIDPOINTS_C    { NULLSIZE_C, NULLRECT_C, NULLRECT_C, (int32_t*)NULL, (int32_t*)NULL, (int32_t*)NULL }

/**
 * @brief fill the grid points
 * 
 * @param gridPoints [out] GridPointsC_t object to fill
 * @param gridStep [in] (gridStep[0], gridStep[1]): beginning of the grid, (gridStep[2], gridStep[3]): pitch
 * @param baseImage [in] the grid is defined to cover the image.
 * @return int unix errno compatible error code
 */
void GridPointsC_Fill(pGridPointsC_t gridPoints, const RectC_t gridStep, pcImageC_t baseImage);

#ifdef __cplusplus
}
#endif
#endif /* GRIDPOINTSC_H_ */
