# Design of image container considering summed-area table and template matching
The image container is implemented in `ZnccC/ImageC.c` for C-language and
`ZnccISPC/Image.isph` for ISPC language. Implemented algorithms are same for both.
I explain mainly C version which data structure is a little simpler than ISPC version
because of C language simplicity.

## Basic data structure
Very low level data structures are implemented in `Basic2DTypesC.h`.

Table 1. Low level data structure
Name | Description | base data type
--|--|--
`Size2iC_t` | 2-D size, similar to OpenCV `cv::Size2i` | two elemnt array of `int32_t`
`Point2iC_t` | 2-D point, similar to OpenCV `cv::Point2i` | two element array of `int32_t`
`RectC_t` | 2D rectangle, similar to OpenCV `cv::Rect2i` | four element array of `int32_t`
<br>

Note: `RectC_t`[0], [1]: The origin of the rectangle, `RectC_t`[2], [3]: The width and the height of the rectangle.

Related algorithms are implemented as macros in C version and inline functions in ISPC version.
<br>
<br>


Table 2. Low level algorithm macros
Name | Description
--|--
`AREA_SIZE(s)` | return the product of `s`[0] and `s`[1].
`AREA_RECT(r)` | return the area of rectangle `r`, i.e. `r`[2]*r[3].
`ZERO_SIZE(s)` | fill s with zeros.
`ZERO_RECT(r)` | fill r with zeros.
`ARE_EQ_SIZE(s0, s1)` | return true if `s0` is equal to `s1`.
`ARE_EQ_RECT(r0, r1)` | return true if `r0` is equal to `r1`.
`ARE_NE_SIZE(s0, s1)` | return true if `s0` is not equal to `s1`.
`ARE_NE_RECT(r0, r1)` | return true if `r0` is not equal to `r1`.
`IS_IN_RECT(pnt, r)` | return true if `pnt` is in `r`. `pnt`: a point, `r`: a rectangle
`IS_NIN_RECT(pnt, r)` | return true if `pnt` is not in `r`.
`ROI(wholeSize, marginRect)` | create an initializer of `Rect2iC_t`. wholeSize: whole buffer size, `marginRect`: edge margin definition
`F2U8(f)` | convert a `float` number to `uint8_t` number with min(0)/max(255) clamping.
<br>

## Image container structures
It is declared in `ZnccC/ImageC.h` for C language and is declared in `ZnccISPC/Image.isph`.
The declaration is show below.
```
typedef struct {
    Size2iC_t size; // whole buffer size
    RectC_t roi; // region of interest
    float* elements; // buffer pointer
} ImageC_t, *pImageC_t;
typedef const ImageC_t *pcImageC_t;
```
Corresponding pointer type and const pointer type are also declared.
Closely related functions are declared in the header.

Table 3. Functions related to image
Name | Description
--|--
`ImageC_New` | Initialize an image object.
`ImageC_Delete` | Delete an image object initialized by `ImageC_New`.
`ImageC_SetPackedImage` | Convert `uint8_t` packed image to an `ImageC_t` object with a given edge margin.
`ImageC_SetPackedImageF` | Convert float packed image to an `ImageC_t` object with a given edge margin.
`ImageC_Clone` | Clone an image.
`ImageC_Integrate` | Create a summed area table.
`ImageC_Add` | pixel-by-pixel '`+=`' operation
`ImageC_Multiply` | pixel-by-pixel '`*=`' operation
`ImageC_Subtract` | pixel-by-pixel '`-=`' operation
`ImageC_Mean` | Averaging over sumRect around each pixel.
`ImageC_Begin` | return read/write pointer to the top-left element.
`ImageC_BeginC` | return read-only pointer to the top-left element.
<br>

Note: 'Packed image', 'packed size', etc. means 'without edge margin'.

## Sum kernel for summed area table
It is declared in `SumKernelC.h` which is a header only library for retrieving a local sum of
a summed area table. The implementaion is aware of index addressing of modern computers.
An example in `ImageC_Mean` is shown below.
```
001     ImageC_Integrate(&imageForMean);
002     SumKernelC_t sk = MK_SUMKERNEL(&imageForMean, sumRect);
003     float* meanPtr = ImageC_Begin(mean);
004     for (int iRow = 0; iRow != tempRoi[3]; iRow++)
005     {
006         for (int iCol = 0; iCol != tempRoi[2]; iCol++)
007         {
008             meanPtr[iCol] = rcpSumArea * SumKernelC_SATSum(&sk, iCol);
009         }
010         sk.base += stride;
011         meanPtr += stride;
012     }
```
, where `iRow` and `iCol` are 0-base index in `tempRoi`.
`tempRoi` is a region of interest maximally applicable to the images under operation without
pointer over-run by `SumKernelC_SATSum` macro. `tempRoi` is usually larger than the roi of the original image.  
<br>
001: Create a summed area table. 'imageForMean' was a clone of an original image to get local sums.  
002: Build a summing kernel object. `MK_SUMKERNEL` macro is declared in `SumKernelC.h`.  
003: Get the pointer to the top-left element of the destination image where the mean is put into.  
008: Calculate a sum over `sumRect`.  
010, 011: move `sk` and `meanPtr` to the next row.

Table 4. Macros defined in `SumKernelC.h`
Name | Description
--|--
`SumKernelC_SATSum(sk, index)` | Calculate local sum over `sumRect` around a point of interest.
`MK_SUMKERNEL(image, sumRect)` | Build a `SumKernelC_t` object. The initial `base` is adjusted to `ImageC_BeginC(image)`.
<br>

For example, `sumRect` is `{-15, -15, 31, 31}` when a summing region is inclusive (-15, 15) to inclusive (+15, +15).