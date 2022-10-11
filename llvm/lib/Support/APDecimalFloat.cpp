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

#include "llvm/ADT/APDecimalFloat.h"
#include "llvm/ADT/APFloat.h"

namespace llvm {

APDecimalFloat APDecimalFloat::getMax(const decFltSemantics &Sema) {
  return APDecimalFloat(Sema);
}

APDecimalFloat APDecimalFloat::getMin(const decFltSemantics &Sema) {
  return APDecimalFloat(Sema);
}

void APDecimalFloat::toString(SmallVectorImpl<char> &Str) const {
  Str.push_back('0');
}

} // namespace llvm
