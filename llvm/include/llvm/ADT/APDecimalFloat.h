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

#include "/Users/shafik/Downloads/IntelRDFPMathLib20U2/LIBRARY/src/bid_conf.h"
#include "/Users/shafik/Downloads/IntelRDFPMathLib20U2/LIBRARY/src/bid_functions.h"
#include "/Users/shafik/Downloads/IntelRDFPMathLib20U2/LIBRARY/src/bid_internal.h"

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/APSInt.h"
#include "llvm/ADT/FoldingSet.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/Support/raw_ostream.h"

namespace llvm {

class APFloat;
class APDecimalFloat;
struct fltSemantics;

struct decFltSemantics {
public:
  decFltSemantics(unsigned Width, unsigned Scale, bool IsSigned,
                  bool IsSaturated, bool HasUnsignedPadding)
      : Width(Width), LsbWeight(-static_cast<int>(Scale)), IsSigned(IsSigned),
        IsSaturated(IsSaturated), HasUnsignedPadding(HasUnsignedPadding) {}

  unsigned getWidth() const { return Width; }
  unsigned getScale() const { return -LsbWeight; }
  int getLsbWeight() const { return LsbWeight; }
  int getMsbWeight() const {
    return LsbWeight + Width - 1 /*Both lsb and msb are both part of width*/;
  }
  bool isSigned() const { return IsSigned; }
  bool isSaturated() const { return IsSaturated; }
  bool hasUnsignedPadding() const { return HasUnsignedPadding; }

  void setSaturated(bool Saturated) { IsSaturated = Saturated; }

  bool hasSignOrPaddingBit() const { return IsSigned || HasUnsignedPadding; }

  unsigned getIntegralBits() const {
    return std::max(getMsbWeight() + 1 - hasSignOrPaddingBit(), 0);
  }

  bool fitsInFloatSemantics(const fltSemantics &FloatSema) const;

  decFltSemantics getCommonSemantics(const decFltSemantics &Other) const {
    int CommonLsb = std::min(getLsbWeight(), Other.getLsbWeight());
    int CommonMSb =
        std::max(getMsbWeight() - hasSignOrPaddingBit(),
                 Other.getMsbWeight() - Other.hasSignOrPaddingBit());
    unsigned CommonWidth = CommonMSb - CommonLsb + 1;

    bool ResultIsSigned = isSigned() || Other.isSigned();
    bool ResultIsSaturated = isSaturated() || Other.isSaturated();
    bool ResultHasUnsignedPadding = false;
    if (!ResultIsSigned) {
      // Both are unsigned.
      ResultHasUnsignedPadding = hasUnsignedPadding() &&
                                 Other.hasUnsignedPadding() &&
                                 !ResultIsSaturated;
    }

    // If the result is signed, add an extra bit for the sign. Otherwise, if it
    // is unsigned and has unsigned padding, we only need to add the extra
    // padding bit back if we are not saturating.
    if (ResultIsSigned || ResultHasUnsignedPadding)
      CommonWidth++;

    return decFltSemantics(CommonWidth, CommonLsb, ResultIsSigned,
                           ResultIsSaturated, ResultHasUnsignedPadding);
  }

  /// Return the DecimalFloatemantics for an integer type.
  static decFltSemantics GetIntegerSemantics(unsigned Width, bool IsSigned) {
    return decFltSemantics(Width, /*Scale=*/0, IsSigned, /*IsSaturated=*/false,
                           /*HasUnsignedPadding=*/false);
  }

private:
  unsigned Width          : 16;
  signed int LsbWeight : 13;
  unsigned IsSigned : 1;
  unsigned IsSaturated : 1;
  unsigned HasUnsignedPadding : 1;
};

/// The APDecimalFloat class 
class APDecimalFloat {
public:
  APDecimalFloat(const decFltSemantics &Sema) :
      Sema(Sema) {
  }

  APDecimalFloat(APDecimalFloat Val, const decFltSemantics &Sema)
      : Val(Val.getValue(), !Sema.isSigned()), Sema(Sema) {
    assert(Val.getWidth() == Sema.getWidth() &&
           "The value should have a bit width that matches the Sema width");
  }

  APDecimalFloat(const APInt &Val, const decFltSemantics &Sema)
      : Val(Val, !Sema.isSigned()), Sema(Sema) {
    assert(Val.getBitWidth() == Sema.getWidth() &&
           "The value should have a bit width that matches the Sema width");
  }

  inline unsigned getWidth() const { return Sema.getWidth(); }
  decFltSemantics getSemantics() const { return Sema; }
  APSInt getValue() const { return APSInt(Val, !Sema.isSigned()); }

  const static fltSemantics *promoteFloatSemantics(const fltSemantics *S) {
    if (S == &APFloat::BFloat())
      return &APFloat::IEEEdouble();
    else if (S == &APFloat::IEEEhalf())
      return &APFloat::IEEEsingle();
    else if (S == &APFloat::IEEEsingle())
      return &APFloat::IEEEdouble();
    else if (S == &APFloat::IEEEdouble())
      return &APFloat::IEEEquad();
    llvm_unreachable("Could not promote float type!");
  }

  bool needsCleanup() const { return false; }

  APDecimalFloat add(const APDecimalFloat &Other, bool *Overflow = nullptr) const {
    unsigned int x = 0;
    BID_UINT32 Tmp;
    BID_UINT32 RHSVal = Val.getZExtValue();
    BID_UINT32 LHSVal = Other.getValue().getZExtValue();
    BID_UINT32 ResultVal = bid32_add(RHSVal, LHSVal, x, &Tmp);

    printf("Add: %08x\n", ResultVal);
    llvm::APInt store(32u, ResultVal, false);

    APDecimalFloat Result = llvm::APDecimalFloat(
        store, llvm::decFltSemantics(32u, /*Scale=*/1u, /*IsSigned=*/0u,
                                     /*IsSaturated=*/false,
                                     /*HasUnsignedPadding=*/false));

    return Result;
  }

  APDecimalFloat sub(const APDecimalFloat &Other, bool *Overflow = nullptr) const {
    unsigned int x = 0;
    BID_UINT32 Tmp;
    BID_UINT32 RHSVal = Val.getZExtValue();
    BID_UINT32 LHSVal = Other.getValue().getZExtValue();
    BID_UINT32 ResultVal = bid32_sub(RHSVal, LHSVal, x, &Tmp);

    printf("Sub: %08x\n", ResultVal);
    llvm::APInt store(32u, ResultVal, false);

    APDecimalFloat Result = llvm::APDecimalFloat(
        store, llvm::decFltSemantics(32u, /*Scale=*/1u, /*IsSigned=*/0u,
                                     /*IsSaturated=*/false,
                                     /*HasUnsignedPadding=*/false));

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

  APInt bitcastToAPInt() const { return APInt{getWidth(), Val.getZExtValue()}; }

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
  APSInt Val;
  decFltSemantics Sema;
};

inline raw_ostream &operator<<(raw_ostream &OS, const APDecimalFloat &FX) {
  OS << FX.toString();
  return OS;
}

} // namespace llvm

#endif
