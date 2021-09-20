#ifndef COSTMAPC_H_
#define COSTMAPC_H_
#include "ZnccC/Basic2DTypesC.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct {
    RectC_t pixelRoi;
    RectC_t searchRect;
    float* costs;
} CostMapC_t, *pCostMapC_t;
typedef const CostMapC_t *pcCostMapC_t;
#define NULLCOSTMAP_C   { {0,0,0,0}, {0,0,0,0}, NULL }

int CostMapC_New(pCostMapC_t costmap, const RectC_t roi, const RectC_t searchRect);

void CostMapC_Delete(pCostMapC_t costmap);

int CostMapC_PixelPlaneArea(pcCostMapC_t costmap);

int CostMapC_SearchPlaneArea(pcCostMapC_t costmap);

typedef struct {
    RectC_t searchRect;
    float* costs;
} CostSearchTableC_t, *pCostSearchTableC_t;
typedef const CostSearchTableC_t *pcCostSearchTableC_t;
#define NULLCOSTSEARCHTABLE_C   { {0,0,0,0}, NULL }

int CostSearchTableC_New(pCostSearchTableC_t costSearchTable, pcCostMapC_t costmap);

void CostSearchTableC_Delete(pCostSearchTableC_t costSearchTable);

void CostSearchTableC_Gather(
    pCostSearchTableC_t costSearchTable, pcCostMapC_t costmap, const Point2iC_t packedPixelPoint);

void CostSearchTableC_FindMaximumPoint(pcCostSearchTableC_t costSearchTable, Point2iC_t pointOfMaxmum);
#ifdef __cplusplus
}
#endif
#endif /* COSTMAPC_H_ */