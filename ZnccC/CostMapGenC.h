#ifndef COSTMAPGENC_H_
#define COSTMAPGENC_H_
#include "ZnccC/CostMapC.h"
#include "ZnccC/ZnccHalfC.h"
#ifdef __cplusplus
extern "C" {
#endif
int CostMapGenC_FillCostMap(
    pcZnccHalfC_t base, pcZnccHalfC_t shifted,
    pcZnccMatchingSizesC_t sizes, pCostMapC_t costmap);

/**
 * @brief find maximums in search rect
 * 
 * @param base 
 * @param shifted 
 * @param sizes 
 * @param dispx 
 * @param dispy 
 * @return int 
 */
int CostMapGen_FindMax(
    pcZnccHalfC_t base, pcZnccHalfC_t shifted,
    pcZnccMatchingSizesC_t sizes, pImageC_t dispx, pImageC_t dispy);

#define COST_LOWER_LIMIT    0.0f
#define NO_MATCH_FOUND      (-1.0e10f);
#ifdef __cplusplus
}
#endif
#endif /* COSTMAPGENC_H_ */