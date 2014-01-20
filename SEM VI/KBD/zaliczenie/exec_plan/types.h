#ifndef __DATATYPES_H__
#define __DATATYPES_H__

#include <malloc.h>
#include "proto/operations.pb.h"

#define MEMALIGN 16

enum DataType { INT = ScanOperation_Type_INT, DOUBLE = ScanOperation_Type_DOUBLE, BOOL = ScanOperation_Type_BOOL };
enum ExprType { NOTHING = 0, LOG = -1, BASE_MATH = -2, COUNT = -3, SUM = -4 };

template <int T>
struct TypeTraits {};

template <>
struct TypeTraits <INT> {
  typedef int32_t T;
};

template <>
struct TypeTraits <BOOL> {
  typedef bool T;
};

template <>
struct TypeTraits <DOUBLE> {
  typedef double T;
};

template <int T1, int T2, int T3=-1>
struct TypeResolver { enum DataType { T = INT }; };

// BASE_MATH

template <>
struct TypeResolver<INT, DOUBLE, BASE_MATH> { enum { T = DOUBLE }; };

template <>
struct TypeResolver<DOUBLE, DOUBLE, BASE_MATH> { enum { T = DOUBLE }; };

template <>
struct TypeResolver<DOUBLE, INT, BASE_MATH> { enum { T = DOUBLE }; };

template <>
struct TypeResolver<INT, INT, BASE_MATH> { enum { T = INT }; };

template <>
struct TypeResolver<INT, SUM> { enum { T = INT }; };

template <>
struct TypeResolver<DOUBLE, SUM> { enum { T = DOUBLE }; };

template <>
struct TypeResolver<BOOL, SUM> { enum { T = BOOL }; };


// TYPED CALL MACROS

#define DOUBLE_TYPED_CREATOR2(function, t1, t2, dest, args...) {     \
  switch (t2) {                                                      \
    case INT:                                                        \
      dest = new function<t1, INT>(args);                            \
      break;                                                         \
    case DOUBLE:                                                     \
      dest = new function<t1, DOUBLE>(args);                         \
      break;                                                         \
    case BOOL:                                                       \
      dest = new function<t1, BOOL>(args);                           \
      break;                                                         \
  }                                                                  \
}

#define DOUBLE_TYPED_CREATOR(function, t1, t2, dest, args...) {      \
  switch (t1) {                                                      \
    case INT:                                                        \
      DOUBLE_TYPED_CREATOR2(function, INT, t2, dest, args...)        \
      break;                                                         \
    case DOUBLE:                                                     \
      DOUBLE_TYPED_CREATOR2(function, DOUBLE, t2, dest, args...)     \
      break;                                                         \
    case BOOL:                                                       \
      DOUBLE_TYPED_CREATOR2(function, BOOL, t2, dest, args...)       \
      break;                                                         \
  }                                                                  \
}

#define BUFF_CREATOR(t, dest, size) {                                \
  switch (t) {                                                       \
    case INT:                                                        \
      dest = memalign(MEMALIGN, sizeof(typename TypeTraits<INT>::T) * size);\
      break;                                                         \
    case DOUBLE:                                                     \
      dest = memalign(MEMALIGN, sizeof(typename TypeTraits<DOUBLE>::T) * size);\
      break;                                                         \
    case BOOL:                                                       \
      dest = memalign(MEMALIGN, sizeof(typename TypeTraits<BOOL>::T) * size); \
      break;                                                         \
    default:                                                         \
      assert(false);                                                 \
      break;                                                         \
  }                                                                  \
}

#define GROUP_BY_TYPE_RESOLVE2(t, op, dest) {                        \
  switch (op) {                                                      \
    case Aggregation_Type_COUNT:                                     \
      dest = (DataType) CountOperator<t>::R;                         \
      break;                                                         \
    case Aggregation_Type_SUM:                                       \
      dest = (DataType) SumOperator<t>::R;                           \
      break;                                                         \
    default:                                                         \
      assert(false);                                                 \
      break;                                                         \
  }                                                                  \
}

#define GROUP_BY_TYPE_RESOLVE(t, op, dest) {                         \
  switch (t) {                                                       \
    case INT:                                                        \
      GROUP_BY_TYPE_RESOLVE2(INT, op, dest);                         \
      break;                                                         \
    case DOUBLE:                                                     \
      GROUP_BY_TYPE_RESOLVE2(DOUBLE, op, dest);                      \
      break;                                                         \
    case BOOL:                                                       \
      GROUP_BY_TYPE_RESOLVE2(BOOL, op, dest);                        \
      break;                                                         \
    default:                                                         \
      assert(false);                                                 \
      break;                                                         \
  }                                                                  \
}

#define AGGVEC_PUSH_CREATOR2(t, op, ix, dest) {                      \
  switch (t) {                                                       \
    case INT:                                                        \
      dest = new TypedAggregationColumn<INT, op>(ix);                \
      break;                                                         \
    case DOUBLE:                                                     \
      dest = new TypedAggregationColumn<DOUBLE, op>(ix);             \
      break;                                                         \
    case BOOL:                                                       \
      dest = new TypedAggregationColumn<BOOL, op>(ix);               \
      break;                                                         \
    default:                                                         \
      assert(false);                                                 \
      break;                                                         \
  }                                                                  \
}


#define AGGVEC_PUSH_CREATOR(t, op, ix, dest) {                       \
  switch (op) {                                                      \
    case Aggregation_Type_COUNT:                                     \
      AGGVEC_PUSH_CREATOR2(t, CountOperator, ix, dest);              \
      break;                                                         \
    case Aggregation_Type_SUM:                                       \
      AGGVEC_PUSH_CREATOR2(t, SumOperator, ix, dest);                \
      break;                                                         \
    default:                                                         \
      assert(false);                                                 \
      break;                                                         \
  }                                                                  \
}


/* simd type patch */

template <typename T1>
struct SimdTypePatch {
  typedef T1 T;
};

template <>
struct SimdTypePatch<bool> {
  typedef char T;
};

#endif
