#include "Util/imagesource.h"
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <sys/stat.h>

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
        {{-10, -1}}, {{-5, 1}}, {{-3, 2}}, {{4,0}}
    };
    ImageSource_Shift(imageShifted, imageSource, shifts);
}

void ImageSource_MonotalSample(pImageC_t image)
{
    ispc::Image image_ = {{{0,0}}, {{0,0,0,0}}, nullptr};
    ImageSource_MonoralSample(image_);
    int err = ImageC_New(image, image_.size.v, image_.roi.v);
    assert(EXIT_SUCCESS == err);
    size_t copySize = sizeof(float) * AREA_SIZE(image->size);
    memcpy(image->elements, image_.elements, copySize);
    ispc::Image_Delete(image_);
}

void ImageSource_StereoSamples(pImageC_t imageShifted, pImageC_t imageSource)
{
    ispc::Image imageShifted_ = {{{0,0}}, {{0,0,0,0}}, nullptr},
        imageSource_ = {{{0,0}}, {{0,0,0,0}}, nullptr};
    ImageSource_StereoSamples(imageShifted_, imageSource_);
    int err = ImageC_New(imageShifted, imageShifted_.size.v, imageShifted_.roi.v);
    assert(EXIT_SUCCESS == err);
    err = ImageC_New(imageSource, imageShifted_.size.v, imageShifted_.roi.v);
    assert(EXIT_SUCCESS == err);
    size_t copySize = sizeof(float) * AREA_SIZE(imageShifted->size);
    memcpy(imageShifted->elements, imageShifted_.elements, copySize);
    memcpy(imageSource->elements, imageSource_.elements, copySize);
    ispc::Image_Delete(imageShifted_);
    ispc::Image_Delete(imageSource_);
}

struct Transpose {
    cv::Mat3b& src;

    Transpose(cv::Mat3b& src_) : src(src_) {}

    void operator ()(cv::Vec3b& pixel, const int* loc) const
    {
        pixel = src(loc[1], loc[0]);
    }
};

static int MakeResultDirectory(const char* dir)
{
    int err = EXIT_SUCCESS;
    struct stat s;
    do {
        if (EXIT_SUCCESS != stat(dir, &s))
        {
            err = errno;
            if (err == ENOENT)
            {
                if (EXIT_SUCCESS != (err = mkdir(dir, 0xffff)))
                {
                    err = errno;
                }
            }
        }
        if (0 == S_ISDIR(s.st_mode))
        {
            err = EINVAL;
            break;
        }
    } while (0);
    return err;
}

int ImageSourrce_StitchSamples(const char* filePath, const char* saveDir, int nDiv, float overlappingRatio)
{
    int err = EXIT_SUCCESS;
    do {
        cv::Mat3b srcImage = cv::imread(filePath);
        if (srcImage.cols == 0 || srcImage.rows == 0)
        {
            err = ENOENT;
            break;
        }
        cv::Mat3b srcImageWork;
        if (srcImage.cols > srcImage.rows)
        {
            srcImageWork = srcImage.clone();
        }
        else
        {
            srcImageWork = cv::Mat3b(srcImage.cols, srcImage.rows);
            srcImageWork.forEach(Transpose(srcImage));
        }
        if (EXIT_SUCCESS != (err = MakeResultDirectory(saveDir)))
        {
            break;
        }
        int rows = srcImageWork.rows;

        int iImages = 0; // The first image
        std::vector<cv::Mat3b> images;
        int rowsEach = (int)((1.0f + 2.0f * overlappingRatio) * (float)rows / (float)nDiv);
        int rowsOffsetBegin = (rowsEach - rows / nDiv)/2;
        int rowsOffsetEnd = rowsEach - rows / nDiv - rowsOffsetBegin;
        {
            cv::Mat3b srcImage(rowsEach, srcImageWork.cols);
            size_t copyBytes = (rowsEach - rowsOffsetBegin) * srcImageWork.cols * sizeof(cv::Vec3b);
            memcpy(&srcImage(rowsOffsetBegin, 0), srcImageWork.data, copyBytes);
            images.emplace_back(srcImage);
        }
        for (iImages = 1; iImages < (nDiv - 1); iImages++)
        {
            int iRowSrcBegin = (int)((iImages - overlappingRatio) * (float)rows / (float)nDiv);
            cv::Mat3b srcImage(rowsEach, srcImageWork.cols, &srcImageWork(iRowSrcBegin, 0));
            images.emplace_back(srcImage.clone());
        }
        // i == nDiv - 1 : The last image
        {
            int iRowSrcBegin = (int)((iImages - overlappingRatio) * (float)rows / (float)nDiv);
            cv::Mat3b srcImage(rowsEach, srcImageWork.cols);
            size_t copyBytes = (srcImageWork.rows - iRowSrcBegin) * srcImageWork.cols * sizeof(cv::Vec3b);
            memcpy(srcImage.data, &srcImageWork(iRowSrcBegin, 0), copyBytes);
        }
    } while (0);
    return err;
}