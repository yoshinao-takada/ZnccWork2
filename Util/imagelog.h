#ifndef IMAGELOG_H_
#define IMAGELOG_H_
#include <ZnccISPC/Image.hpp>
#include <ZnccISPC/CostMap.hpp>
#ifdef __cplusplus
extern "C" {
#endif
typedef uint8_t (*FPIXTOU8)(float pixelValue);

typedef uint8_t (*FPIXTOU8RANGED)(float pixelValue, float range0, float range1);

void ImageLog_SaveImage(const char* dir, const char* subdir, const ispc::Image* image, const FPIXTOU8 pixconv);

void ImageLog_SaveColorMap(const char* dir, const char* subdir, const ispc::Image* image, const FPIXTOU8RANGED pixconv, float range0, float range1);

void ImageLog_SaveCSV(const char* dir, const char* filename, const ispc::Image* image);

void ImageLog_SaveMatrixImageCSV(const char* dir, const char* filename, const ispc::Image* image);

void ImageLog_SaveCostSearchTableCSV(const char* dir, const char* filename, const ispc::CostSearchTable* cst);
#ifdef __cplusplus
}
#endif
#endif /* IMAGELOG_H_ */