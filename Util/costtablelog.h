#ifndef COSTTABLELOG_H_
#define COSTTABLELOG_H_
#include <ZnccISPC/ZnccHalf.hpp>
#include <ZnccISPC/costmap.hpp>

void CostTableLog_SaveCSV(
    const ispc::CostSearchTable& costSearchTable, 
    const char* dir, 
    const char* subdirBase, 
    const ispc::int32_t2& pixelPoint);

#endif /*  COSTTABLELOG_H_ */
