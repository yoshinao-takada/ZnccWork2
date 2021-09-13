#include "Image.isph"
#include "SumKernel.isph"

#pragma region Prototypes_Non_Inline_Functions
export uniform ZnccError Image_New(uniform Image& image, uniform const Size2i& size, uniform const Rect& roi)
{
    uniform ZnccError err = errSuccess;
    uniform const int newArea = Area(size);
    do {
        if (AreNotEqual(image.size, size) || AreNotEqual(image.roi, roi))
        {
            if (image.elements != NULL)
            {
                Image_Delete(image);
            }
            image.elements = uniform new float[newArea];
            if (NULL == image.elements)
            {
                err = errNoMem;
                break;
            }
        }
        image.size = size;
        image.roi = roi;
        foreach (i = 0 ... newArea)
        {
            image.elements[i] = 0.0f;
        }
    } while (0);
    return err;
}

export void Image_Delete(uniform Image& image)
{
    if (NULL != image.elements)
    {
        delete[] image.elements;
    }
    image.elements = NULL;
    Zero(image.size);
    Zero(image.roi);
}

export uniform ZnccError Image_SetPackedImage(
    uniform Image& image, uniform const uint8* uniform packedImage,
    uniform const Size2i& packedSize, uniform const Rect& margin
) {
    uniform ZnccError err = errSuccess;
    do {
        uniform Size2i newSize = { packedSize[0] + margin[2], packedSize[1] + margin[3] };
        uniform Rect newRoi = { -margin[0], -margin[1], packedSize[0], packedSize[1] };
        if (errSuccess != (err = Image_New(image, newSize, newRoi)))
        {
            break;
        }
        uniform float* uniform dstF = Image_Begin(image);
        uniform const int dstStride = image.size[0];
        uniform const uint8* uniform srcU8 = packedImage;
        uniform const int srcStride = packedSize[0];
        for (uniform int iRow = 0 ; iRow != packedSize[1]; ++iRow)
        {
            for (uniform int iCol = 0; iCol != packedSize[0]; iCol++)
            {
                dstF[iCol] = (float)srcU8[iCol];
            }
            dstF += dstStride;
            srcU8 += srcStride;
        }
    } while (0);
    return err;
}

export uniform ZnccError Image_SetPackedImageF(
    uniform Image& image, uniform const float* uniform packedImage,
    uniform const Size2i& packedSize, uniform const Rect& margin
) {
    uniform ZnccError err = errSuccess;
    do {
        uniform Size2i newSize = { packedSize[0] + margin[2], packedSize[1] + margin[3] };
        uniform Rect newRoi = { -margin[0], -margin[1], packedSize[0], packedSize[1] };
        if (errSuccess != (err = Image_New(image, newSize, newRoi)))
        {
            break;
        }
        uniform float* uniform dstF = Image_Begin(image);
        uniform const float* uniform srcF = packedImage;
        uniform const int dstStride = image.size[0];
        uniform const int srcStride = packedSize[0];
        uniform const int copySize = sizeof(uniform float) * srcStride;
        foreach (iRow = 0 ... packedSize[1])
        {
            void* varying pvDst = (void* varying)(dstF + iRow * dstStride);
            void* varying pvSrc = (void* varying)(srcF + iRow * srcStride);
            memcpy(pvDst, pvSrc, copySize);
        }
    } while (0);
    return err;
}

export uniform ZnccError Image_GetPackedImage(uniform uint8* uniform packedImage, const uniform Image& image)
{
    uniform ZnccError err = errSuccess;
    do {
        uniform const float* uniform srcF = Image_BeginC(image);
        uniform uint8* uniform dstU8 = packedImage;
        uniform const int srcStride = image.size[0];
        uniform const int dstStride = image.roi[2];
        for (uniform int iRow = 0; iRow != image.roi[3]; iRow++)
        {
            foreach (iCol = 0 ... image.roi[2])
            {
                dstU8[iCol] = ClampFtoU8(srcF[iCol]);
            }
            dstU8 += dstStride;
            srcF += srcStride;
        }
    } while (0);
    return err;
}

export uniform ZnccError Image_GetPackedImageF(float* uniform packedImage, const uniform Image& image)
{
    uniform ZnccError err = errSuccess;
    do {
        uniform const float* uniform srcF = Image_BeginC(image);
        uniform const int srcStride = image.size[0];
        uniform float* uniform dstF = packedImage;
        uniform const int dstStride = image.roi[2];
        uniform const int32 copySize = sizeof(uniform float) * dstStride;
        foreach (i = 0 ... image.roi[3])
        {
            void* varying dstFv = (void* varying)(dstF + i * dstStride);
            void* varying srcFv = (void* varying)(srcF + i * srcStride);
            memcpy(dstFv, srcFv, copySize);
        }
    } while (0);
    return err;
}


export void Image_Integrate(uniform Image& image)
{
    uniform const int stride = image.size[0];
    uniform float* uniform row = image.elements + 1 + stride;
    uniform const int osLeft = -1;
    uniform const int osUp = -stride;
    uniform const int osUpLeft = osUp + osLeft;
    // fill the top-most row and left-most column with 0s.
    foreach (iCol = 0 ... image.size[0])
    {
        image.elements[iCol] = 0.0f;
    }
    for (uniform int iRow = 0; iRow != image.size[1]; ++iRow)
    {
        image.elements[iRow * stride] = 0.0f;
    }

    // integration
    for (uniform int iRow = 1; iRow != image.size[1]; ++iRow)
    {
        uniform float* uniform element = row;
        for (uniform int iCol = 1; iCol != stride; ++iCol)
        {
            *element += *(element + osLeft) + *(element + osUp) - *(element + osUpLeft);
            ++element;
        }
        row += stride;
    }
}

export void Image_GetAttributes(uniform const Image& image, uniform Size2i& size, uniform Rect& rect)
{
    size[0] = image.size[0];
    size[1] = image.size[1];
    rect[0] = image.roi[0];
    rect[1] = image.roi[1];
    rect[2] = image.roi[2];
    rect[3] = image.roi[3];
}
export uniform ZnccError Image_Clone(uniform Image& destination, uniform const Image& source)
{
    Image_Delete(destination);
    uniform ZnccError result = Image_New(destination, source.size, source.roi);
    if (result == errSuccess)
    {
        uniform const int32 copySize = source.size[0] * source.size[1] * sizeof(uniform float);
        memcpy(
            (void* uniform)destination.elements, 
            (void* uniform)source.elements, 
            copySize);
    }
    return result;
}

// add souorce to destination including margin area
export void Image_Add(uniform Image& destination, uniform const Image& source)
{
    if (AreNotEqual(destination.size, source.size))
    {
        print("error: size mismatch in Image_Add(), destination.size=(%,%), source.size=(%,%)\n",
            destination.size[0], destination.size[1], source.size[0], source.size[1]);
        assert(false);
    }
    uniform int area = Area(destination.size);
    foreach (i = 0 ... area)
    {
        destination.elements[i] += source.elements[i];
    }
}

export void Image_Multiply(uniform Image& destination, uniform const Image& source)
{
    if (AreNotEqual(destination.size, source.size))
    {
        print("error: size mismatch in Image_Multiply(), destination.size=(%,%), source.size=(%,%)\n",
            destination.size[0], destination.size[1], source.size[0], source.size[1]);
        assert(false);
    }
    uniform int area = Area(destination.size);
    foreach (i = 0 ... area)
    {
        destination.elements[i] *= source.elements[i];
    }
}

export void Image_Subtract(uniform Image& destination, uniform const Image& source)
{
    if (AreNotEqual(destination.size, source.size))
    {
        print("error: size mismatch in Image_Subtract(), destination.size=(%,%), source.size=(%,%)\n",
            destination.size[0], destination.size[1], source.size[0], source.size[1]);
        assert(false);
    }
    uniform int area = Area(destination.size);
    foreach (i = 0 ... area)
    {
        destination.elements[i] -= source.elements[i];
    }
}

void Image_Mean(uniform Image& mean, uniform const Image& source, uniform const Rect& sumRect)
{
    if (AreNotEqual(mean.size, source.size))
    {
        print("error: size mismatch in Image_Subtract(), mean.size=(%,%), source.size=(%,%)\n",
            mean.size[0], mean.size[1], source.size[0], source.size[1]);
        assert(false);
    }
    uniform Image satForMean = IMAGE_INIT;
    Image_Clone(satForMean, source);
    Image_Integrate(satForMean);  
    uniform const Rect satRoi = CreateRoi(source.size, SumKernel_MarginRect(sumRect));
    satForMean.roi = satRoi;
    mean.roi = satRoi;
    uniform SumKernel sk = Image_GetSumKernel(satForMean, sumRect);
    uniform float* uniform meanPtr = Image_Begin(mean);
    uniform const int stride = satForMean.size[0];
    uniform const float rcpArea = 1.0f / (float)Area(sumRect);

    for (uniform int iRow = 0; iRow != satRoi[3]; iRow++)
    {
        foreach (iCol = 0 ... satRoi[2])
        {
            meanPtr[iCol] = rcpArea * SumKernel_SATSum(sk, iCol);
        }
        meanPtr += stride;
        sk.base += stride;
    }
    Image_Delete(satForMean);
    mean.roi = source.roi;
}

export void Image_Sum(uniform const Image& src, uniform const Rect& sumRect, uniform Image& sums)
{
    uniform SumKernel sk = Image_GetSumKernel(src, sumRect);
    Image_New(sums, src.size, src.roi);
    uniform float* uniform sumsPtr = Image_Begin(sums);
    for (uniform int iRow = 0; iRow != sums.roi[3]; iRow++)
    {
        foreach (iCol = 0 ... sums.roi[2])
        {
            sumsPtr[iCol] = SumKernel_SATSum(sk, iCol);
        }
        sumsPtr += sums.size[0];
        sk.base += sums.size[0];
    }
}