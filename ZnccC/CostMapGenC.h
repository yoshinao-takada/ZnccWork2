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
#ifdef __cplusplus
}
#endif
#endif /* COSTMAPGENC_H_ */