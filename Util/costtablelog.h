#ifndef COSTTABLELOG_H_
#define COSTTABLELOG_H_
#include <ZnccISPC/ZnccHalf.hpp>
#include <ZnccISPC/CostMap.hpp>
#include <ZnccC/Basic2DTypesC.h>
#include <ZnccC/CostMapC.h>

void CostTableLog_SaveCSV(
    const ispc::CostSearchTable& costSearchTable, 
    const char* dir, 
    const char* subdirBase, 
    const ispc::int32_t2& pixelPoint);

void CostTableLog_SaveCSVC(
    pcCostSearchTableC_t costSearchTable,
    const char* dir,
    const char* subdirBase,
    const Point2iC_t pixelPoint
);
#endif /*  COSTTABLELOG_H_ */
