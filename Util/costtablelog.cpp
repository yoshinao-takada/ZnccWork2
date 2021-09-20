#include "Util/costtablelog.h"
#include <filesystem>
#include <cstdio>

void CostTableLog_SaveCSV(
    const ispc::CostSearchTable& costSearchTable, 
    const char* dir, 
    const char* subdirBase, 
    const ispc::int32_t2& pixelPoint
) {
    if (!std::filesystem::exists(dir))    
    {
        std::filesystem::create_directories(dir);
    }
    char subdir[64];
    snprintf(subdir, sizeof(subdir), "%s(%d,%d)", subdirBase, pixelPoint.v[0], pixelPoint.v[1]);
    std::filesystem::path dirpath = std::filesystem::path(dir) / std::filesystem::path(subdir);
    if (!std::filesystem::exists(dirpath))
    {
        std::filesystem::create_directories(dirpath);
    }
    if (std::filesystem::exists(dirpath) == false)
    {
        fprintf(stderr, "%s,%d,fail to create directory: %s\n", __FUNCTION__, __LINE__, dirpath.c_str());
        exit(1);
    }
    char filename[64];
    snprintf(filename, sizeof(filename), "CostSearchTable(%d,%d).csv", pixelPoint.v[0], pixelPoint.v[1]);
    std::filesystem::path filepath = dirpath / filename;
    FILE *pf = fopen(filepath.c_str(), "w");
    for (int iCol = 0; iCol != costSearchTable.searchRect.v[2];iCol++)
    {
        fprintf(pf, ",[%d]", iCol + costSearchTable.searchRect.v[0]);
    }
    fprintf(pf, "\n");
    for (int iRow = 0; iRow != costSearchTable.searchRect.v[3]; iRow++)
    {
        fprintf(pf, "[%d]", iRow + costSearchTable.searchRect.v[1]);
        for (int iCol = 0; iCol != costSearchTable.searchRect.v[2]; iCol++)
        {
            const int linearIndex = iCol + iRow * costSearchTable.searchRect.v[2];
            fprintf(pf, ",%f", costSearchTable.costs[linearIndex]);
        }
        fprintf(pf, "\n");
    }
    if (pf) 
    {
        fclose(pf);
    }
}

void CostTableLog_SaveCSVC(
    pcCostSearchTableC_t costSearchTable,
    const char* dir,
    const char* subdirBase,
    const Point2iC_t pixelPoint
) {
    ispc::CostSearchTable cst_ = { {{0,0,0,0}}, nullptr };
    COPY4(cst_.searchRect.v, costSearchTable->searchRect);
    cst_.costs = costSearchTable->costs;
    const ispc::int32_t2 pixelPoint_ = {{ pixelPoint[0], pixelPoint[1] }};
    CostTableLog_SaveCSV(cst_, dir, subdirBase, pixelPoint_);
}