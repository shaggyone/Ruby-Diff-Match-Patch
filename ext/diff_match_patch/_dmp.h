#ifndef ruby_DMP_HPP
#define ruby_DMP_HPP

#ifdef __cplusplus
#include <string>
#include "diff-match-patch-cpp-stl/diff_match_patch.h"
#endif

#include <ruby.h>
#include <string.h>

#define dmp diff_match_patch<std::string>
typedef VALUE (ruby_method_vararg)(...);

extern "C" void Init_diff_match_patch(void);

#ifndef RARRAY_CONST_PTR
  #define RARRAY_CONST_PTR(a) \
    ((const VALUE *)((RBASIC(a)->flags & RARRAY_EMBED_FLAG) ? \
      RARRAY(a)->as.ary : \
      RARRAY(a)->as.heap.ptr))
#endif

#ifndef RARRAY_AREF
  #define RARRAY_AREF(a, i)    (RARRAY_CONST_PTR(a)[i])
#endif

#endif
