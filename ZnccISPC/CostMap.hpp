//
// CostMap.hpp
// (Header automatically generated by the ispc compiler.)
// DO NOT EDIT THIS FILE.
//

#pragma once
#include <stdint.h>



#ifdef __cplusplus
namespace ispc { /* namespace */
#endif // __cplusplus
///////////////////////////////////////////////////////////////////////////
// Vector types with external visibility from ispc code
///////////////////////////////////////////////////////////////////////////

#ifndef __ISPC_VECTOR_int32_t2__
#define __ISPC_VECTOR_int32_t2__
#ifdef _MSC_VER
__declspec( align(16) ) struct int32_t2 { int32_t v[2]; };
#else
struct int32_t2 { int32_t v[2]; } __attribute__ ((aligned(16)));
#endif
#endif

#ifndef __ISPC_VECTOR_int32_t4__
#define __ISPC_VECTOR_int32_t4__
#ifdef _MSC_VER
__declspec( align(16) ) struct int32_t4 { int32_t v[4]; };
#else
struct int32_t4 { int32_t v[4]; } __attribute__ ((aligned(16)));
#endif
#endif


///////////////////////////////////////////////////////////////////////////
// Enumerator types with external visibility from ispc code
///////////////////////////////////////////////////////////////////////////

#ifndef __ISPC_ENUM_ZnccError__
#define __ISPC_ENUM_ZnccError__
enum ZnccError {
    errSuccess = 0,
    errInvalid = 1,
    errNoMem = 2,
    errRange = 3,
    errSizeMismatch = 4 
};
#endif


#ifndef __ISPC_ALIGN__
#if defined(__clang__) || !defined(_MSC_VER)
// Clang, GCC, ICC
#define __ISPC_ALIGN__(s) __attribute__((aligned(s)))
#define __ISPC_ALIGNED_STRUCT__(s) struct __ISPC_ALIGN__(s)
#else
// Visual Studio
#define __ISPC_ALIGN__(s) __declspec(align(s))
#define __ISPC_ALIGNED_STRUCT__(s) __ISPC_ALIGN__(s) struct
#endif
#endif

#ifndef __ISPC_STRUCT_CostMap__
#define __ISPC_STRUCT_CostMap__
struct CostMap {
    int32_t2  pixelSize;
    int32_t2  searchSize;
    float * costs;
};
#endif

#ifndef __ISPC_STRUCT_CostSearchTable__
#define __ISPC_STRUCT_CostSearchTable__
struct CostSearchTable {
    int32_t4  searchRect;
    float * costs;
};
#endif


///////////////////////////////////////////////////////////////////////////
// Functions exported from ispc code
///////////////////////////////////////////////////////////////////////////
#if defined(__cplusplus) && (! defined(__ISPC_NO_EXTERN_C) || !__ISPC_NO_EXTERN_C )
extern "C" {
#endif // __cplusplus
    extern void CostMap_Delete(struct CostMap &costmap);
    extern void CostMap_Gather(const struct CostMap &costmap, struct CostSearchTable &costSearchTable, const int32_t2   &pixelPoint);
    extern enum ZnccError CostMap_New(struct CostMap &costmap, const int32_t2   &pixelSize, const int32_t2   &searchSize);
    extern void CostSearchTable_Delete(struct CostSearchTable &costSearchTable);
    extern enum ZnccError CostSearchTable_New(struct CostSearchTable &costSearchTable, const int32_t4   &searchRect);
#if defined(__cplusplus) && (! defined(__ISPC_NO_EXTERN_C) || !__ISPC_NO_EXTERN_C )
} /* end extern C */
#endif // __cplusplus


#ifdef __cplusplus
} /* namespace */
#endif // __cplusplus
