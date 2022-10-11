//===- APDecimalFloat.h - Decimal Float constant handling -----------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Defines the decimal float number interface.
/// This is a class for abstracting various operations performed on decimal float 
/// types.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_ADT_APDECIMALFLOAT_H
#define LLVM_ADT_APDECIMALFLOAT_H

#include "llvm/ADT/FoldingSet.h"
#include "llvm/ADT/APSInt.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/Support/raw_ostream.h"

namespace llvm {

class APFloat;
struct fltSemantics;

struct decFltSemantics {
public:
  decFltSemantics(unsigned Width) 
      : Width(Width) {
  }

  unsigned getWidth() const { return Width; }

  decFltSemantics getCommonSemantics(const decFltSemantics &Other) const;

private:
  unsigned Width          : 16;
};

/// The APDecimalFloat class 
class APDecimalFloat {
public:
  APDecimalFloat(const decFltSemantics &Sema) :
      Sema(Sema) {
  }

  APDecimalFloat(APDecimalFloat Val, const decFltSemantics &Sema) :
      Sema(Sema) {
    assert(Val.getWidth() == Sema.getWidth() &&
           "The value should have a bit width that matches the Sema width");
  }

  APDecimalFloat(const APInt &Val, const decFltSemantics &Sema) :
      Sema(Sema) {
    assert(Val.getBitWidth() == Sema.getWidth() &&
           "The value should have a bit width that matches the Sema width");
  }

  inline unsigned getWidth() const { return Sema.getWidth(); }
  decFltSemantics getSemantics() const { return Sema; }

  bool needsCleanup() const { return false; }

  APDecimalFloat add(const APDecimalFloat &Other, bool *Overflow = nullptr) const {
    APDecimalFloat Result(*this);

    return Result;
  }

  APDecimalFloat sub(const APDecimalFloat &Other, bool *Overflow = nullptr) const {
    APDecimalFloat Result(*this);

    return Result;
  }

  APDecimalFloat mul(const APDecimalFloat &Other, bool *Overflow = nullptr) const {
    APDecimalFloat Result(*this);

    return Result;
  }

  APDecimalFloat div(const APDecimalFloat &Other, bool *Overflow = nullptr) const {
    APDecimalFloat Result(*this);

    return Result;
  }

  static APDecimalFloat getMax(const decFltSemantics &Sema);
  static APDecimalFloat getMin(const decFltSemantics &Sema);

  static APDecimalFloat getFromIntValue(const APInt &Value,
                                const decFltSemantics &DstFXSema,
                                bool *Overflow) {
    APDecimalFloat Result(DstFXSema);

    return Result;
   }


  static APDecimalFloat getFromFloatValue(const APFloat &Value,
                                const decFltSemantics &DstFXSema,
                                bool *Overflow) {
    APDecimalFloat Result(DstFXSema);

    return Result;
   }

  APDecimalFloat convert(const decFltSemantics &DstFXSema,
                          bool *Overflow) {
    APDecimalFloat Result(*this);

    return Result;
   }

  APDecimalFloat& operator++() { return *this; }
  APDecimalFloat operator++(int) { 
    APDecimalFloat APDF(*this);
    ++(*this);
    return APDF;
  }

  bool isZero() const { return false; }

  bool getBoolValue() const { return !isZero(); }

  void flipAllBits(){}

  APDecimalFloat negate(bool *Overflow = nullptr) {
    flipAllBits();
    ++(*this);

    return *this;
  }

  APInt bitcastToAPInt() const {
    return APInt{};
  }

  void Profile(FoldingSetNodeID& ID) const {
    ID.AddInteger(getWidth());
  }

  void toString(SmallVectorImpl<char> &Str) const;
  std::string toString() const {
    SmallString<40> S;
    toString(S);
    return std::string(S.str());
  }

private:
  decFltSemantics Sema;
};

inline raw_ostream &operator<<(raw_ostream &OS, const APDecimalFloat &FX) {
  OS << FX.toString();
  return OS;
}

} // namespace llvm

#endif
