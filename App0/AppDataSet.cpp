#include "App0/AppDataSet.h"
#include <vector>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <filesystem>

struct AppDataSet
{
    std::vector<std::string> paths;
};

pcAppDataSet_t AppDataSet_NewFromFile(const char* filePath)
{
    pAppDataSet_t pobj = new AppDataSet;
    FILE* pf = fopen(filePath, "r");
    std::filesystem::path filePath_(filePath);
    std::filesystem::path parentDir_ = filePath_.parent_path();
    char lineBuf[256];
    while (lineBuf == fgets(lineBuf, sizeof(lineBuf), pf))
    {
    }

    if (pf)
    {
        fclose(pf);
    }
    return pobj;
}

void AppDataSet_Delete(pcAppDataSet_t dataset)
{
    delete dataset;
}

int AppDataSet_Count(pcAppDataSet_t dataset)
{
    return static_cast<int>(dataset->paths.size());
}

const char* AppDataSet_Path(pcAppDataSet_t dataset, int index)
{
    return dataset->paths[index].c_str();
}
