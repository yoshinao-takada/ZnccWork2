#include "CostMapGen.isph"
#include "SumKernel.isph"

void CostMapGen_FillXProd(
    uniform Image& xprod, uniform const Image& baseLumdev, uniform const Image& shiiftedLumdev,
    uniform const Point2i& searchPoint
) {
    uniform const int linearSearchOffset = searchPoint[0] + searchPoint[1] * xprod.size[0];
    uniform const bool shiftPositive = (linearSearchOffset >= 0);
    uniform const int shiftedBeginOffset = select(shiftPositive, 0, -linearSearchOffset);
    uniform const int shiftedEndOffset = select(shiftPositive, -linearSearchOffset, 0);
    uniform float* uniform xprodElement = xprod.elements;
    uniform const float* uniform baseElement = baseLumdev.elements;
    uniform const float* uniform shiftedElement = shiiftedLumdev.elements + linearSearchOffset;
    uniform const int loopEnd = Area(xprod.size) + shiftedEndOffset;
    foreach (i = shiftedBeginOffset ... loopEnd)
    {
        xprodElement[i] = baseElement[i] * shiftedElement[i];
    }
    Image_Integrate(xprod);
}


uniform ZnccError CostMapGen_FillPixelPlane(
    uniform const ZnccHalf& base, uniform const ZnccHalf& shifted,
    uniform const ZnccMatchingSizes& sizes, uniform const Point2i& searchPoint,
    uniform float* uniform costPlane
) {
    uniform Image xprod = IMAGE_INIT;
    uniform ZnccError err = errSuccess;
    uniform const int roiRows = base.mean.roi[3]; // roi rows
    uniform const int roiCols = base.mean.roi[2]; // roi columns
    uniform const int srcStride = base.mean.size[0];
    uniform const int costStride = roiCols;
    uniform const int linearSearchOffset = searchPoint[0] + searchPoint[1] * srcStride;
    do {
        if (errSuccess != (err = Image_Clone(xprod, base.random)))
        {
            break;
        }
        xprod.roi = base.mean.roi;
        CostMapGen_FillXProd(xprod, base.lumdev, shifted.lumdev, searchPoint);
        uniform float* uniform costs = costPlane;
        uniform const float* uniform rcpsdBase = Image_BeginC(base.rcpsd);
        uniform const float* uniform rcpsdShifted = Image_BeginC(shifted.rcpsd) + linearSearchOffset;
        uniform SumKernel skxprod = Image_GetSumKernel(xprod, sizes.sumRect);
        uniform const float rcpSumArea = 1.0f / (float)Area(sizes.sumRect);
        for (uniform int iRow = 0; iRow != roiRows; iRow++)
        {
            foreach (iCol = 0 ... roiCols)
            {
                costs[iCol] = rcpSumArea * SumKernel_SATSum(skxprod, iCol) * rcpsdBase[iCol] * rcpsdShifted[iCol];
            }
            skxprod.base += srcStride;
            rcpsdBase += srcStride;
            rcpsdShifted += srcStride;
            costs += costStride;
        }
    } while (0);
    Image_Delete(xprod);
    return err;
}

export uniform ZnccError CostMapGen_Fill(
    uniform const ZnccHalf& base, uniform const ZnccHalf& shifted,
    uniform const ZnccMatchingSizes& sizes, uniform CostMap& cm
) {
    uniform const Size2i searchSize = { sizes.searchRect[2], sizes.searchRect[3] }; // { search width, search height }
    uniform const Size2i packedPixelSize = { base.mean.roi[2], base.mean.roi[3] };
    uniform const int pixelPlaneArea = Area(packedPixelSize);
    uniform ZnccError err = errSuccess;
    do {
        if (errSuccess != (err = CostMap_New(cm, packedPixelSize, searchSize)))
        {
            break;
        }
        for (uniform int iSearchV = 0; iSearchV != searchSize[1]; iSearchV++)
        {
            for (uniform int iSearchH = 0; iSearchH != searchSize[0]; iSearchH++)
            {
                uniform const int searchPointLinearIndex = iSearchH + iSearchV * searchSize[0];
                uniform float* uniform costPlane = cm.costs + pixelPlaneArea * searchPointLinearIndex;
                uniform const Point2i searchPoint = { sizes.searchRect[0] + iSearchH, sizes.searchRect[1] + iSearchV }; // point relative to search rect reference point
                err |= CostMapGen_FillPixelPlane(base, shifted, sizes, searchPoint, costPlane);
            }
        }
    } while (0);
    return err;
}


task void CostMapGen_FillTask(
    uniform const ZnccHalf& base, uniform const ZnccHalf& shifted,
    uniform const ZnccMatchingSizes& sizes, uniform CostMap& cm, uniform ZnccError& taskReturn
) {
    uniform const Size2i searchSize = { sizes.searchRect[2], sizes.searchRect[3] }; // { search width, search height }
    uniform const Size2i packedPixelSize = { base.mean.roi[2], base.mean.roi[3] };
    uniform const int pixelPlaneArea = Area(packedPixelSize);
    uniform ZnccError err = errSuccess;
    do {
        if (errSuccess != (err = CostMap_New(cm, packedPixelSize, searchSize)))
        {
            break;
        }

        for (uniform int iSearchV = taskIndex; iSearchV < searchSize[1]; iSearchV += taskCount)
        {
            for (uniform int iSearchH = 0; iSearchH != searchSize[0]; iSearchH++)
            {
                uniform const int searchPointLinearIndex = iSearchH + iSearchV * searchSize[0];
                uniform float* uniform costPlane = cm.costs + pixelPlaneArea * searchPointLinearIndex;
                uniform const Point2i searchPoint = { sizes.searchRect[0] + iSearchH, sizes.searchRect[1] + iSearchV }; // point relative to search rect reference point
                err |= CostMapGen_FillPixelPlane(base, shifted, sizes, searchPoint, costPlane);
            }
        }
    } while (0);
    taskReturn = err;
}


export uniform ZnccError CostMapGen_FillByTasks(
    uniform const ZnccHalf& base, uniform const ZnccHalf& shifted,
    uniform const ZnccMatchingSizes& sizes, uniform CostMap& cm
) {
    uniform ZnccError err = errSuccess;
    launch[4] CostMapGen_FillTask(base, shifted, sizes, cm, err);
    return err;
}
