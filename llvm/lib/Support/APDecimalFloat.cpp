//===- APDecimalFloat.cpp - Decimal float constant handling ---------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
/// \file
/// Defines the implementation for the decimal float number interface.
//
//===----------------------------------------------------------------------===//

#include "/Users/shafik/Downloads/IntelRDFPMathLib20U2/LIBRARY/src/bid_conf.h"
#include "/Users/shafik/Downloads/IntelRDFPMathLib20U2/LIBRARY/src/bid_functions.h"
#include "/Users/shafik/Downloads/IntelRDFPMathLib20U2/LIBRARY/src/bid_internal.h"

#include "llvm/ADT/APDecimalFloat.h"
#include "llvm/ADT/APFloat.h"

namespace llvm {

APDecimalFloat APDecimalFloat::getMax(const decFltSemantics &Sema) {
  return APDecimalFloat(Sema);
}

APDecimalFloat APDecimalFloat::getMin(const decFltSemantics &Sema) {
  return APDecimalFloat(Sema);
}

bool decFltSemantics::fitsInFloatSemantics(
    const fltSemantics &FloatSema) const {
  // A fixed point semantic fits in a floating point semantic if the maximum
  // and minimum values as integers of the fixed point semantic can fit in the
  // floating point semantic.

  // If these values do not fit, then a floating point rescaling of the true
  // maximum/minimum value will not fit either, so the floating point semantic
  // cannot be used to perform such a rescaling.

  APSInt MaxInt = APDecimalFloat::getMax(*this).getValue();
  APFloat F(FloatSema);
  APFloat::opStatus Status = F.convertFromAPInt(MaxInt, MaxInt.isSigned(),
                                                APFloat::rmNearestTiesToAway);
  if ((Status & APFloat::opOverflow) || !isSigned())
    return !(Status & APFloat::opOverflow);

  APSInt MinInt = APDecimalFloat::getMin(*this).getValue();
  Status = F.convertFromAPInt(MinInt, MinInt.isSigned(),
                              APFloat::rmNearestTiesToAway);
  return !(Status & APFloat::opOverflow);
}

void APDecimalFloat::toString(SmallVectorImpl<char> &Str) const {
  char output[100]{0};
  BID_UINT32 result2 = Val.getZExtValue();
  unsigned int x;

  bid32_to_string(output, result2, &x);

  char *p = output;

  while (*p) {
    Str.push_back(*p);
    ++p;
  }

  // Str.push_back('0');
}

} // namespace llvm
