#ifndef IMAGESOURCE_H_
#define IMAGESOURCE_H_
#include <ZnccISPC/Image.hpp>
#include <ZnccC/ImageC.h>
#include <vector>
/**
 * @brief Create random mosaic pattern with noise.
 * 
 * @param image [in,out] is initialized with an adequate size and a roi.
 * @param patternScale [in] specifies mosaic pattern size relative to image size.
 * horizontal pattern pitch is image.size.v[0]/patternScale.v[0],
 * vertical pattern pitch is image.size.v[1]/patternScale.v[1].
 */
void ImageSource_Create(ispc::Image& image, const ispc::int32_t2& patternScale);

/**
 * @brief Remap imageSource int imageShifted. shifts is a four element vector conaining shifting vector of each square of 
 * the images.
 * 
 * @param imageShifted [in,out] is initialized with an adequate size and a roi.
 * @param imageSource [in] contains a meaningful image.
 * @param shifts [in] is a four elements vector specifying shihft from imageSource to imageShifted.
 * shifts[0] corresponds to top-left square.
 * shifts[1] corresponds to top-right square.
 * shifts[2] corresponds to bottom-left square.
 * shifts[3] corresponds to bottom-right square
 */
void ImageSource_Shift(ispc::Image& imageShifted, const ispc::Image& imageSource, const std::vector<ispc::int32_t2>& shifts);

void ImageSource_MonoralSample(ispc::Image& image);
void ImageSource_MonotalSample(pImageC_t image);

/**
 * @brief Creates a default sample stereo images. Their sizes are (640,480). Their rois are (0,0,640,480).
 * They are filled with a random mosaic pattern with noise.
 * 
 * @param imageShifted [out]
 * @param imageSource [out]
 */
void ImageSource_StereoSamples(ispc::Image& imageShifted, ispc::Image& imageSource);
void ImageSource_StereoSamples(pImageC_t imageShifted, pImageC_t imageSource);

/**
 * @brief Divide a image with overlapping region
 * 
 * @param filePath [in] source image file path
 * @param saveDir [in] save directory of resulted images
 * @param nDiv [in] number of resulted images
 * @param overlappingRatio [in] overlapping ratio of each adjacent image pair. 1.0 fully overlap (impossible)
 *  0.1: 10 % overlap
 * @return EINVAL: invalid overlapping ratio or nDiv, ENOENT: file or directory not exist, EPERM: file permission
 */
int ImageSourrce_StitchSamples(const char* filePath, const char* saveDir, int nDiv, float overlappingRatio);
#endif /* IMAGESOURCE_H_ */