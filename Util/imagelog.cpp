#include "Util/imagelog.h"
#include <filesystem>
#include <cstdio>
#include <cstdlib>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

struct CvPixelConv {
    const FPIXTOU8 pixconv;
    const ispc::Image* image;
    CvPixelConv(FPIXTOU8 pixconv_, const ispc::Image* image_)
        : pixconv(pixconv_), image(image_) {}
    
    void operator () (uint8_t& pixel, const int* loc) const {
        int stride = image->size.v[0];
        int linearIndex = loc[1] + loc[0] * stride;
        pixel = (*pixconv)(image->elements[linearIndex]);
    }
};

void ImageLog_SaveImage(const char* dir, const char* subdir, const ispc::Image* image, const FPIXTOU8 pixconv)
{
    if (!std::filesystem::exists(dir))
    {
        std::filesystem::create_directories(dir);
    }
    std::filesystem::path dirpath = std::filesystem::path(dir) / std::filesystem::path(subdir);
    if (!std::filesystem::exists(dirpath))
    {
        std::filesystem::create_directories(dirpath);
        if (!std::filesystem::exists(dirpath))
        {
            fprintf(stderr, "%s,%d,fail to create directory: %s\n", __FUNCTION__, __LINE__, dirpath.c_str());
            abort();
        }
    }
    { // save attributes
        std::filesystem::path filepath = dirpath / "attrib.txt";
        FILE *pf = fopen(filepath.c_str(), "w");
        if (pf == nullptr)
        {
            fprintf(stderr, "%s,%d,fail to open file:%s\n", __FUNCTION__, __LINE__, filepath.c_str());
            abort();
        }
        fprintf(pf, "size=(%d,%d)\n", image->size.v[0], image->size.v[1]);
        fprintf(pf, "roi=(%d,%d,%d,%d)\n", image->roi.v[0], image->roi.v[1], image->roi.v[2], image->roi.v[3]);
        fclose(pf);
    }
    { // save grayscale map
        std::filesystem::path filepath = dirpath / "grayscale.jpg";
        cv::Mat1b u8image(image->size.v[1], image->size.v[0]);
        u8image.forEach(CvPixelConv(pixconv, image));
        cv::imwrite(filepath.c_str(), u8image);
    }
    { // save CSV
        ImageLog_SaveCSV(dirpath.c_str(), "pixelvalues.csv", image);
    }
}

struct CvPixelConvRanged {
    const FPIXTOU8RANGED pixconv;
    const float range0, range1;
    const ispc::Image* image;
    CvPixelConvRanged(const FPIXTOU8RANGED pixconv_, const ispc::Image* image_, float range0_, float range1_)
        : pixconv(pixconv_), image(image_), range0(range0_), range1(range1_) {}
    
    void operator () (uint8_t& pixel, const int* loc) const {
        int stride = image->size.v[0];
        int linearIndex = loc[1] + loc[0] * stride;
        pixel = (*pixconv)(image->elements[linearIndex], range0, range1);
    }
};

void ImageLog_SaveColorMap(const char* dir, const char* subdir, const ispc::Image* image, const FPIXTOU8RANGED pixconv, float range0, float range1)
{
    if (!std::filesystem::exists(dir))
    {
        std::filesystem::create_directories(dir);
    }
    std::filesystem::path dirpath = std::filesystem::path(dir) / std::filesystem::path(subdir);
    if (!std::filesystem::exists(dirpath))
    {
        std::filesystem::create_directories(dirpath);
        if (!std::filesystem::exists(dirpath))
        {
            fprintf(stderr, "%s,%d,fail to create directory: %s\n", __FUNCTION__, __LINE__, dirpath.c_str());
            abort();
        }
    }
    if (!std::filesystem::exists(dirpath))
    {
        std::filesystem::create_directories(dirpath);
        if (!std::filesystem::exists(dirpath))
        {
            fprintf(stderr, "%s,%d,fail to create directory: %s\n", __FUNCTION__, __LINE__, dirpath.c_str());
            abort();
        }
    }
    { // save attributes
        std::filesystem::path filepath = dirpath / "attrib.txt";
        FILE *pf = fopen(filepath.c_str(), "w");
        if (pf == nullptr)
        {
            fprintf(stderr, "%s,%d,fail to open file:%s\n", __FUNCTION__, __LINE__, filepath.c_str());
            abort();
        }
        fprintf(pf, "size=(%d,%d)\n", image->size.v[0], image->size.v[1]);
        fprintf(pf, "roi=(%d,%d,%d,%d)\n", image->roi.v[0], image->roi.v[1], image->roi.v[2], image->roi.v[3]);
        fclose(pf);
    }
    { // save color map
        std::filesystem::path filepath = dirpath / "color.jpg";
        cv::Mat1b u8image(image->size.v[1], image->size.v[0]);
        u8image.forEach(CvPixelConvRanged(pixconv, image, range0, range1));
        cv::Mat3b colorimage;
        cv::applyColorMap(u8image, colorimage, cv::COLORMAP_RAINBOW);
        cv::imwrite(filepath.c_str(), colorimage);
    }
    { // save CSV
        ImageLog_SaveCSV(dirpath.c_str(), "pixelvalues.csv", image);
    }
}

void ImageLog_SaveCSV(const char* dir, const char* filename, const ispc::Image* image)
{
    if (!std::filesystem::exists(dir))
    {
        std::filesystem::create_directories(dir);
    }
    std::filesystem::path filepath = std::filesystem::path(dir) / std::filesystem::path(filename);
    const char* cfilepath = filepath.c_str();
    FILE* pf = fopen(cfilepath, "w");
    if (!pf)
    {
        fprintf(stderr, "%s,%d,fail to open: %s\n", __FUNCTION__, __LINE__, cfilepath);
        abort();
    }
    fprintf(pf, "roi,%d,%d,%d,%d\n", image->roi.v[0], image->roi.v[1], image->roi.v[2], image->roi.v[3]);
    fprintf(pf, "ix,iy,value\n");
    int index = 0;
    for (int iy = 0; iy != image->size.v[0]; iy++)
    {
        for (int ix = 0; ix != image->size.v[1]; ix++)
        {
            fprintf(pf, "%d,%d,%f\n", ix, iy, image->elements[index++]);
        }
    }
    if (pf) fclose(pf);
}

void ImageLog_SaveMatrixImageCSV(const char* dir, const char* filename, const ispc::Image* image)
{
    if (!std::filesystem::exists(dir))
    {
        std::filesystem::create_directories(dir);
    }
    std::filesystem::path filepath = std::filesystem::path(dir) / std::filesystem::path(filename);
    const char* cfilepath = filepath.c_str();
    FILE* pf = fopen(cfilepath, "w");
    if (!pf)
    {
        fprintf(stderr, "%s,%d,fail to open: %s\n", __FUNCTION__, __LINE__, cfilepath);
        abort();
    }
    fprintf(pf, "size=(%d,%d)\n", image->size.v[0], image->size.v[1]);
    fprintf(pf, "roi=(%d,%d,%d,%d)\n", image->roi.v[0], image->roi.v[1], image->roi.v[2], image->roi.v[3]);
    for (int iCol = 0; iCol != image->size.v[0]; iCol++)
    {
        fprintf(pf, ",[%d]", iCol);
    }
    fprintf(pf, "\n");
    for (int iRow = 0; iRow != image->size.v[1]; iRow++)
    {
        fprintf(pf, "[%d]", iRow);
        for (int iCol = 0; iCol != image->size.v[0]; iCol++)
        {
            const int linearIndex = iCol + iRow * image->size.v[0];
            fprintf(pf, ",%f", image->elements[linearIndex]);
        }
        fprintf(pf, "\n");
    }
    fclose(pf);
}

void ImageLog_SaveCostSearchTableCSV(const char* dir, const char* filename, const ispc::CostSearchTable* cst)
{
    if (!std::filesystem::exists(dir))
    {
        std::filesystem::create_directories(dir);
    }
    std::filesystem::path filepath = std::filesystem::path(dir) / std::filesystem::path(filename);
    const char* cfilepath = filepath.c_str();
    FILE* pf = fopen(cfilepath, "w");
    if (!pf)
    {
        fprintf(stderr, "%s,%d,fail to open: %s\n", __FUNCTION__, __LINE__, cfilepath);
        abort();
    }
    for (int iCol = 0; iCol != cst->searchRect.v[2]; iCol++)
    {
        const int xpos = iCol + cst->searchRect.v[0];
        fprintf(pf, ",[%d]", xpos);
    }
    fprintf(pf, "\n");
    for (int iRow = 0; iRow != cst->searchRect.v[3]; iRow++)
    {
        const int ypos = iRow + cst->searchRect.v[1];
        fprintf(pf, "[%d]", ypos);
        for (int iCol = 0; iCol != cst->searchRect.v[2]; iCol++)
        {
            const int xpos = iCol + cst->searchRect.v[0];
            const int linearIndex = iCol + iRow * cst->searchRect.v[2];
            fprintf(pf, ",%f", cst->costs[linearIndex]);
        }
        fprintf(pf, "\n");
    }
    fclose(pf);
}


void ImageLog_SaveImageC(const char* dir, const char* subdir, pcImageC_t image, const FPIXTOU8 pixconv)
{
    const ispc::Image image_ = {
        {{image->size[0], image->size[1]}},
        {{image->roi[0], image->roi[1], image->roi[2], image->roi[3] }},
        image->elements
    };
    ImageLog_SaveImage(dir, subdir, &image_, pixconv);
}

void ImageLog_SaveColorMapC(const char* dir, const char* subdir, pcImageC_t image, const FPIXTOU8RANGED pixconv, float range0, float range1)
{
    const ispc::Image image_ = {
        {{image->size[0], image->size[1]}},
        {{image->roi[0], image->roi[1], image->roi[2], image->roi[3] }},
        image->elements
    };
    ImageLog_SaveColorMap(dir, subdir, &image_, pixconv, range0, range1);
}

void ImageLog_SaveCSVC(const char* dir, const char* filename, pcImageC_t image)
{
    const ispc::Image image_ = {
        {{image->size[0], image->size[1]}},
        {{image->roi[0], image->roi[1], image->roi[2], image->roi[3] }},
        image->elements
    };
    ImageLog_SaveCSV(dir, filename, &image_);
}

void ImageLog_SaveMatrixImageCSVC(const char* dir, const char* filename, pcImageC_t image)
{
    const ispc::Image image_ = {
        {{image->size[0], image->size[1]}},
        {{image->roi[0], image->roi[1], image->roi[2], image->roi[3] }},
        image->elements
    };
    ImageLog_SaveMatrixImageCSV(dir, filename, &image_);
}

void ImageLog_SaveCostSearchTableCSVC(const char* dir, const char* filename, pcCostSearchTableC_t cst)
{
    const ispc::CostSearchTable cst_ = {
        {{ cst->searchRect[0], cst->searchRect[1], cst->searchRect[2], cst->searchRect[3] }},
        cst->costs
    };
    ImageLog_SaveCostSearchTableCSV(dir, filename, &cst_);
}
