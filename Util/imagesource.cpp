#include "Util/imagesource.h"
#include <opencv2/core.hpp>

struct SetRand {
    SetRand(int seed) { srand(seed); }

    void operator() (uint8_t& pixel, const int* loc) const
    {
        pixel = (uint8_t)(rand() & 0x7f);
    }
};

void ImageSource_Create(ispc::Image& image, const ispc::int32_t2& patternScale)
{
    cv::Size2i patternSize(image.size.v[0]/patternScale.v[0] + 1, image.size.v[1]/patternScale.v[1] + 1);
    cv::Mat1b pattern(patternSize);
    union {
        void* ptr;
        int intint[2];
    } int_caster = { (void*)&image };
    const int seed = int_caster.intint[0];
    pattern.forEach(SetRand(seed));
    cv::Mat1f cvimage(image.size.v[1], image.size.v[0], image.elements);
    for (int iRow = 0; iRow != cvimage.rows; iRow++)
    {
        for (int iCol = 0; iCol != cvimage.cols; iCol++)
        {
            cv::Point2i patternIndex2D(iCol / patternScale.v[0], iRow / patternScale.v[1]);
            cvimage(iRow, iCol) = pattern(patternIndex2D) + (uint8_t)(rand() & 0x3f);
        }
    }
}


void ImageSource_Shift(ispc::Image& imageShifted, const ispc::Image& imageSource, const std::vector<ispc::int32_t2>& shifts)
{
    memcpy(imageShifted.elements, imageSource.elements, sizeof(float) * imageShifted.size.v[0] * imageShifted.size.v[1]);

    const int stride = imageShifted.size.v[0];
    // apply shifts[0]
    cv::Rect2i imageRect = { 0, 0, imageSource.size.v[0], imageSource.size.v[1] };
    for (int iRow = 0; iRow != imageShifted.size.v[1]; iRow++)
    {
        for (int iCol = 0; iCol != imageShifted.size.v[0]; iCol++)
        {
            cv::Point2i srcPixel(iCol - shifts[0].v[0], iRow - shifts[0].v[1]);
            if (!srcPixel.inside(imageRect)) continue;
            const int dstPixelIndex = iCol + iRow * stride;
            const int srcPixelIndex = srcPixel.x + srcPixel.y * stride;
            imageShifted.elements[dstPixelIndex] = imageSource.elements[srcPixelIndex];
        }
    }
    // apply shifts[1]
    for (int iRow = 0; iRow != imageShifted.size.v[1]; iRow++)
    {
        for (int iCol = imageShifted.size.v[0]/2; iCol != imageShifted.size.v[0]; iCol++)
        {
            cv::Point2i srcPixel(iCol - shifts[1].v[0], iRow - shifts[1].v[1]);
            if (!srcPixel.inside(imageRect)) continue;
            const int dstPixelIndex = iCol + iRow * stride;
            const int srcPixelIndex = srcPixel.x + srcPixel.y * stride;
            imageShifted.elements[dstPixelIndex] = imageSource.elements[srcPixelIndex];
        }
    }
    // apply shifts[2]
    for (int iRow = imageShifted.size.v[1]/2; iRow != imageShifted.size.v[1]; iRow++)
    {
        for (int iCol = 0; iCol != imageShifted.size.v[0]; iCol++)
        {
            cv::Point2i srcPixel(iCol - shifts[2].v[0], iRow - shifts[2].v[1]);
            if (!srcPixel.inside(imageRect)) continue;
            const int dstPixelIndex = iCol + iRow * stride;
            const int srcPixelIndex = srcPixel.x + srcPixel.y * stride;
            imageShifted.elements[dstPixelIndex] = imageSource.elements[srcPixelIndex];
        }
    }
    // apply shifts[3]
    for (int iRow = imageShifted.size.v[1]/2; iRow != imageShifted.size.v[1]; iRow++)
    {
        for (int iCol = imageShifted.size.v[0]/2; iCol != imageShifted.size.v[0]; iCol++)
        {
            cv::Point2i srcPixel(iCol - shifts[3].v[0], iRow - shifts[3].v[1]);
            if (!srcPixel.inside(imageRect)) continue;
            const int dstPixelIndex = iCol + iRow * stride;
            const int srcPixelIndex = srcPixel.x + srcPixel.y * stride;
            imageShifted.elements[dstPixelIndex] = imageSource.elements[srcPixelIndex];
        }
    }
}

void ImageSource_MonoralSample(ispc::Image& image)
{
    const ispc::int32_t2 size = {{640,480}};
    const ispc::int32_t4 roi = {{0,0,size.v[0], size.v[1]}};
    ispc::Image_Delete(image);
    ispc::Image_New(image, size, roi);
    ispc::int32_t2 patternScale = {{20,20}};
    ImageSource_Create(image, patternScale);
}

void ImageSource_StereoSamples(ispc::Image& imageShifted, ispc::Image& imageSource)
{
    ImageSource_MonoralSample(imageSource);
    ispc::Image_Delete(imageShifted);
    ispc::Image_New(imageShifted, imageSource.size, imageSource.roi);
    const std::vector<ispc::int32_t2> shifts =
    {
        {{-10, -1}}, {{-5, 1}}, {{-3, 0}}, {{0,0}}
    };
    ImageSource_Shift(imageShifted, imageSource, shifts);
}