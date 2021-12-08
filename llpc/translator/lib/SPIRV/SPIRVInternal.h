//===- LLVMSPIRVInternal.h - SPIR-V internal header file --------*- C++ -*-===//
//
//                     The LLVM/SPIRV Translator
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
// Copyright (c) 2014 Advanced Micro Devices, Inc. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal with the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimers.
// Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimers in the documentation
// and/or other materials provided with the distribution.
// Neither the names of Advanced Micro Devices, Inc., nor the names of its
// contributors may be used to endorse or promote products derived from this
// Software without specific prior written permission.
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS WITH
// THE SOFTWARE.
//
//===----------------------------------------------------------------------===//
/// \file
///
/// This file declares classes and functions shared by SPIR-V reader/writer.
///
//===----------------------------------------------------------------------===//
#ifndef SPIRV_SPIRVINTERNAL_H
#define SPIRV_SPIRVINTERNAL_H

#include "LLVMSPIRVLib.h"
#include "libSPIRV/SPIRVEnum.h"
#include "libSPIRV/SPIRVError.h"
#include "libSPIRV/SPIRVNameMapEnum.h"
#include "libSPIRV/SPIRVType.h"
#include "libSPIRV/SPIRVUtil.h"
#include "llvm/IR/Attributes.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Instructions.h"
#include <functional>
#include <utility>

using namespace SPIRV;
using namespace llvm;

namespace SPIRV {

#ifndef LLVM_DEBUG
#define LLVM_DEBUG DEBUG
#endif

template <> inline void SPIRVMap<unsigned, Op>::init() {
#define _SPIRV_OP(x, y) add(Instruction::x, Op##y);
  /* Casts */
  _SPIRV_OP(ZExt, UConvert)
  _SPIRV_OP(SExt, SConvert)
  _SPIRV_OP(Trunc, UConvert)
  _SPIRV_OP(FPToUI, ConvertFToU)
  _SPIRV_OP(FPToSI, ConvertFToS)
  _SPIRV_OP(UIToFP, ConvertUToF)
  _SPIRV_OP(SIToFP, ConvertSToF)
  _SPIRV_OP(FPTrunc, FConvert)
  _SPIRV_OP(FPExt, FConvert)
  _SPIRV_OP(PtrToInt, ConvertPtrToU)
  _SPIRV_OP(IntToPtr, ConvertUToPtr)
  _SPIRV_OP(BitCast, Bitcast)
  _SPIRV_OP(GetElementPtr, AccessChain)
  /*Binary*/
  _SPIRV_OP(And, BitwiseAnd)
  _SPIRV_OP(Or, BitwiseOr)
  _SPIRV_OP(Xor, BitwiseXor)
  _SPIRV_OP(Add, IAdd)
  _SPIRV_OP(FAdd, FAdd)
  _SPIRV_OP(Sub, ISub)
  _SPIRV_OP(FSub, FSub)
  _SPIRV_OP(Mul, IMul)
  _SPIRV_OP(FMul, FMul)
  _SPIRV_OP(UDiv, UDiv)
  _SPIRV_OP(SDiv, SDiv)
  _SPIRV_OP(FDiv, FDiv)
  _SPIRV_OP(SRem, SRem)
  _SPIRV_OP(FRem, FRem)
  _SPIRV_OP(URem, UMod)
  _SPIRV_OP(Shl, ShiftLeftLogical)
  _SPIRV_OP(LShr, ShiftRightLogical)
  _SPIRV_OP(AShr, ShiftRightArithmetic)
#undef _SPIRV_OP
}
typedef SPIRVMap<unsigned, Op> OpCodeMap;

template <> inline void SPIRVMap<CmpInst::Predicate, Op>::init() {
#define _SPIRV_OP(x, y) add(CmpInst::x, Op##y);
  _SPIRV_OP(FCMP_OEQ, FOrdEqual)
  _SPIRV_OP(FCMP_OGT, FOrdGreaterThan)
  _SPIRV_OP(FCMP_OGE, FOrdGreaterThanEqual)
  _SPIRV_OP(FCMP_OLT, FOrdLessThan)
  _SPIRV_OP(FCMP_OLE, FOrdLessThanEqual)
  _SPIRV_OP(FCMP_ONE, FOrdNotEqual)
  _SPIRV_OP(FCMP_UEQ, FUnordEqual)
  _SPIRV_OP(FCMP_UGT, FUnordGreaterThan)
  _SPIRV_OP(FCMP_UGE, FUnordGreaterThanEqual)
  _SPIRV_OP(FCMP_ULT, FUnordLessThan)
  _SPIRV_OP(FCMP_ULE, FUnordLessThanEqual)
  _SPIRV_OP(FCMP_UNE, FUnordNotEqual)
  _SPIRV_OP(ICMP_EQ, IEqual)
  _SPIRV_OP(ICMP_NE, INotEqual)
  _SPIRV_OP(ICMP_UGT, UGreaterThan)
  _SPIRV_OP(ICMP_UGE, UGreaterThanEqual)
  _SPIRV_OP(ICMP_ULT, ULessThan)
  _SPIRV_OP(ICMP_ULE, ULessThanEqual)
  _SPIRV_OP(ICMP_SGT, SGreaterThan)
  _SPIRV_OP(ICMP_SGE, SGreaterThanEqual)
  _SPIRV_OP(ICMP_SLT, SLessThan)
  _SPIRV_OP(ICMP_SLE, SLessThanEqual)
#if SPV_VERSION >= 0x10400
  _SPIRV_OP(ICMP_EQ, PtrEqual)
  _SPIRV_OP(ICMP_NE, PtrNotEqual)
#endif
#undef _SPIRV_OP
}
typedef SPIRVMap<CmpInst::Predicate, Op> CmpMap;

class IntBoolOpMapId;
template <> inline void SPIRVMap<Op, Op, IntBoolOpMapId>::init() {
  add(OpNot, OpLogicalNot);
  add(OpBitwiseAnd, OpLogicalAnd);
  add(OpBitwiseOr, OpLogicalOr);
  add(OpBitwiseXor, OpLogicalNotEqual);
  add(OpIEqual, OpLogicalEqual);
  add(OpINotEqual, OpLogicalNotEqual);
}
typedef SPIRVMap<Op, Op, IntBoolOpMapId> IntBoolOpMap;

#define SPIR_TARGETTRIPLE32 "spir-unknown-unknown"
#define SPIR_TARGETTRIPLE64 "spir64-unknown-unknown"
#define SPIR_DATALAYOUT32                                                                                              \
  "e-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32"                                                                     \
  "-i64:64:64-f32:32:32-f64:64:64-v16:16:16-v24:32:32"                                                                 \
  "-v32:32:32-v48:64:64-v64:64:64-v96:128:128"                                                                         \
  "-v128:128:128-v192:256:256-v256:256:256"                                                                            \
  "-v512:512:512-v1024:1024:1024"
#define SPIR_DATALAYOUT64                                                                                              \
  "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32"                                                                     \
  "-i64:64:64-f32:32:32-f64:64:64-v16:16:16-v24:32:32"                                                                 \
  "-v32:32:32-v48:64:64-v64:64:64-v96:128:128"                                                                         \
  "-v128:128:128-v192:256:256-v256:256:256"                                                                            \
  "-v512:512:512-v1024:1024:1024"

enum SPIRAddressSpace {
  SPIRAS_Generic = 0,  // AMDGPUAS::FLAT_ADDRESS
  SPIRAS_Global = 1,   // AMDGPUAS::GLOBAL_ADDRESS
  SPIRAS_Local = 3,    // AMDGPUAS::LOCAL_ADDRESS
  SPIRAS_Constant = 4, // AMDGPUAS::CONSTANT_ADDRESS
  SPIRAS_Private = 5,  // AMDGPUAS::PRIVATE_ADDRESS
  SPIRAS_Uniform = 7,  // Memory buffer descriptor
  SPIRAS_Input = 64,
  SPIRAS_Output = 65,
  SPIRAS_Count,
};

template <> inline void SPIRVMap<SPIRAddressSpace, std::string>::init() {
  add(SPIRAS_Private, "Private");
  add(SPIRAS_Global, "Global");
  add(SPIRAS_Constant, "Constant");
  add(SPIRAS_Local, "Local");
  add(SPIRAS_Generic, "Generic");
  add(SPIRAS_Input, "Input");
  add(SPIRAS_Output, "Output");
  add(SPIRAS_Uniform, "Uniform");
}

template <> inline void SPIRVMap<SPIRAddressSpace, SPIRVStorageClassKind>::init() {
  add(SPIRAS_Private, StorageClassFunction);
  add(SPIRAS_Global, StorageClassCrossWorkgroup);
  add(SPIRAS_Constant, StorageClassUniformConstant);
  add(SPIRAS_Local, StorageClassWorkgroup);
  add(SPIRAS_Generic, StorageClassGeneric);
  add(SPIRAS_Input, StorageClassInput);
  add(SPIRAS_Output, StorageClassOutput);
  add(SPIRAS_Uniform, StorageClassUniform);
  add(SPIRAS_Private, StorageClassPrivate);
  add(SPIRAS_Constant, StorageClassPushConstant);
  add(SPIRAS_Uniform, StorageClassStorageBuffer);
  add(SPIRAS_Global, StorageClassPhysicalStorageBufferEXT);
}
typedef SPIRVMap<SPIRAddressSpace, SPIRVStorageClassKind> SPIRSPIRVAddrSpaceMap;

template <> inline void SPIRVMap<Attribute::AttrKind, SPIRVFunctionControlMaskKind>::init() {
  add(Attribute::ReadNone, FunctionControlPureMask);
  add(Attribute::ReadOnly, FunctionControlConstMask);
  add(Attribute::AlwaysInline, FunctionControlInlineMask);
  add(Attribute::NoInline, FunctionControlDontInlineMask);
}
typedef SPIRVMap<Attribute::AttrKind, SPIRVFunctionControlMaskKind> SPIRSPIRVFuncCtlMaskMap;

namespace kLLVMTypeName {
const static char StructPrefix[] = "struct.";
} // namespace kLLVMTypeName

namespace gSPIRVMD {
const static char Prefix[] = "spirv.";
const static char InOut[] = "spirv.InOut";
const static char Block[] = "spirv.Block";
const static char PushConst[] = "spirv.PushConst";
const static char Resource[] = "spirv.Resource";
const static char ExecutionModel[] = "spirv.ExecutionModel";
const static char ImageCall[] = "spirv.ImageCall";
const static char ImageMemory[] = "spirv.ImageMemory";
const static char BufferLoad[] = "spirv.BufferLoad";
const static char BufferStore[] = "spirv.BufferStore";
const static char AccessChain[] = "spirv.AccessChain";
const static char StorageBufferCall[] = "spirv.StorageBufferCall";
const static char NonUniform[] = "spirv.NonUniform";
} // namespace gSPIRVMD

namespace gSPIRVName {
const static char EmitVertex[] = "EmitVertex";
const static char EmitStreamVertex[] = "EmitStreamVertex";
const static char InterpolateAtCentroid[] = "interpolateAtCentroid";
const static char InterpolateAtSample[] = "interpolateAtSample";
const static char InterpolateAtOffset[] = "interpolateAtOffset";
const static char InterpolateAtVertexAMD[] = "InterpolateAtVertexAMD";
const static char NonUniform[] = "spirv.NonUniform";
const static char UnpackHalf2x16[] = "unpackHalf2x16";
} // namespace gSPIRVName

enum SPIRVBlockTypeKind {
  BlockTypeUnknown,
  BlockTypeUniform,
  BlockTypeShaderStorage,
};

enum SPIRVInterpModeKind {
  InterpModeSmooth,
  InterpModeFlat,
  InterpModeNoPersp,
  InterpModeCustom,
};

enum SPIRVInterpLocKind {
  InterpLocUnknown,
  InterpLocCenter,
  InterpLocCentroid,
  InterpLocSample,
  InterpLocCustom,
};

enum SPIRVImageOpKind {
  ImageOpSample,
  ImageOpFetch,
  ImageOpGather,
  ImageOpQueryNonLod,
  ImageOpQueryLod,
  ImageOpRead,
  ImageOpWrite,
  ImageOpAtomicLoad,
  ImageOpAtomicStore,
  ImageOpAtomicExchange,
  ImageOpAtomicCompareExchange,
  ImageOpAtomicIIncrement,
  ImageOpAtomicIDecrement,
  ImageOpAtomicIAdd,
  ImageOpAtomicISub,
  ImageOpAtomicSMin,
  ImageOpAtomicUMin,
  ImageOpAtomicSMax,
  ImageOpAtomicUMax,
  ImageOpAtomicAnd,
  ImageOpAtomicOr,
  ImageOpAtomicXor,
  ImageAtomicFMin,
  ImageAtomicFMax,
  ImageAtomicFAdd
};

template <> inline void SPIRVMap<SPIRVImageOpKind, std::string>::init() {
  add(ImageOpSample, "sample");
  add(ImageOpFetch, "fetch");
  add(ImageOpGather, "gather");
  add(ImageOpQueryNonLod, "querynonlod");
  add(ImageOpQueryLod, "querylod");
  add(ImageOpRead, "read");
  add(ImageOpWrite, "write");
  add(ImageOpAtomicLoad, "atomicload");
  add(ImageOpAtomicStore, "atomicstore");
  add(ImageOpAtomicExchange, "atomicexchange");
  add(ImageOpAtomicCompareExchange, "atomiccompexchange");
  add(ImageOpAtomicIIncrement, "atomiciincrement");
  add(ImageOpAtomicIDecrement, "atomicidecrement");
  add(ImageOpAtomicIAdd, "atomiciadd");
  add(ImageOpAtomicISub, "atomicisub");
  add(ImageOpAtomicSMin, "atomicsmin");
  add(ImageOpAtomicUMin, "atomicumin");
  add(ImageOpAtomicSMax, "atomicsmax");
  add(ImageOpAtomicUMax, "atomicumax");
  add(ImageOpAtomicAnd, "atomicand");
  add(ImageOpAtomicOr, "atomicor");
  add(ImageOpAtomicXor, "atomicxor");
  add(ImageAtomicFMin, "atomicfmin");
  add(ImageAtomicFMax, "atomicfmax");
  add(ImageAtomicFAdd, "atomicfadd");
}
typedef SPIRVMap<SPIRVImageOpKind, std::string> SPIRVImageOpKindNameMap;

class ImageQueryOpKindNameMapId;
template <> inline void SPIRVMap<Op, std::string, ImageQueryOpKindNameMapId>::init() {
  add(OpImageQuerySizeLod, ".sizelod");
  add(OpImageQuerySize, ".sizelod"); // Note: OpImageQuerySize is
                                     // implemented as OpImageQuerySizeLod
                                     // with lod = 0
  add(OpImageQueryLod, ".lod");
  add(OpImageQueryLevels, ".levels");
  add(OpImageQuerySamples, ".samples");
}
typedef SPIRVMap<Op, std::string, ImageQueryOpKindNameMapId> SPIRVImageQueryOpKindNameMap;

union SPIRVImageOpInfo {
  struct {
    SPIRVImageOpKind OpKind : 6;       // Kind of image operation
    unsigned OperMask : 3;             // Index of image operand mask
    unsigned OperDref : 3;             // Index of Dref operand
    unsigned HasProj : 1;              // Whether project is present
    unsigned IsSparse : 1;             // Is sparse image operation
    unsigned OperAtomicData : 3;       // Index of atomic value
                                       // operand
    unsigned OperAtomicComparator : 3; // Index of atomic comparator
                                       // operand (valid for
                                       // CompareExchange)
    unsigned OperScope : 3;            // Index of the scope (valid for atomics)
    unsigned Unused : 9;
  };
  unsigned U32All;
};

static const unsigned InvalidOperIdx = 0x7;

template <> inline void SPIRVMap<Op, SPIRVImageOpInfo>::init() {
  //       Image OpCode                           OpCode Kind                 Mask              ref             Proj
  //       Sparse  AtomicData      AtomicComparator    Scope
  //---------------------------------------------------------------------------------------------------------------------------------------------------------------------
  add(OpImageSampleImplicitLod,
      {ImageOpSample, 2, InvalidOperIdx, false, false, InvalidOperIdx, InvalidOperIdx, InvalidOperIdx});
  add(OpImageSampleExplicitLod,
      {ImageOpSample, 2, InvalidOperIdx, false, false, InvalidOperIdx, InvalidOperIdx, InvalidOperIdx});
  add(OpImageSampleDrefImplicitLod,
      {ImageOpSample, 3, 3, false, false, InvalidOperIdx, InvalidOperIdx, InvalidOperIdx});
  add(OpImageSampleDrefExplicitLod,
      {ImageOpSample, 3, 3, false, false, InvalidOperIdx, InvalidOperIdx, InvalidOperIdx});
  add(OpImageSampleProjImplicitLod,
      {ImageOpSample, 2, InvalidOperIdx, true, false, InvalidOperIdx, InvalidOperIdx, InvalidOperIdx});
  add(OpImageSampleProjExplicitLod,
      {ImageOpSample, 2, InvalidOperIdx, true, false, InvalidOperIdx, InvalidOperIdx, InvalidOperIdx});
  add(OpImageSampleProjDrefImplicitLod,
      {ImageOpSample, 3, 3, true, false, InvalidOperIdx, InvalidOperIdx, InvalidOperIdx});
  add(OpImageSampleProjDrefExplicitLod,
      {ImageOpSample, 3, 3, true, false, InvalidOperIdx, InvalidOperIdx, InvalidOperIdx});
  add(OpImageFetch, {ImageOpFetch, 2, InvalidOperIdx, false, false, InvalidOperIdx, InvalidOperIdx, InvalidOperIdx});
  add(OpImageGather, {ImageOpGather, 3, InvalidOperIdx, false, false, InvalidOperIdx, InvalidOperIdx, InvalidOperIdx});
  add(OpImageDrefGather, {ImageOpGather, 3, 3, false, false, InvalidOperIdx, InvalidOperIdx, InvalidOperIdx});
  add(OpImageRead, {ImageOpRead, 2, InvalidOperIdx, false, false, InvalidOperIdx, InvalidOperIdx, InvalidOperIdx});
  add(OpImageWrite, {ImageOpWrite, 3, InvalidOperIdx, false, false, InvalidOperIdx, InvalidOperIdx, InvalidOperIdx});

  add(OpImageSparseSampleImplicitLod,
      {ImageOpSample, 2, InvalidOperIdx, false, true, InvalidOperIdx, InvalidOperIdx, InvalidOperIdx});
  add(OpImageSparseSampleExplicitLod,
      {ImageOpSample, 2, InvalidOperIdx, false, true, InvalidOperIdx, InvalidOperIdx, InvalidOperIdx});
  add(OpImageSparseSampleDrefImplicitLod,
      {ImageOpSample, 3, 3, false, true, InvalidOperIdx, InvalidOperIdx, InvalidOperIdx});
  add(OpImageSparseSampleDrefExplicitLod,
      {ImageOpSample, 3, 3, false, true, InvalidOperIdx, InvalidOperIdx, InvalidOperIdx});
  add(OpImageSparseSampleProjImplicitLod,
      {ImageOpSample, 2, InvalidOperIdx, true, true, InvalidOperIdx, InvalidOperIdx, InvalidOperIdx});
  add(OpImageSparseSampleProjExplicitLod,
      {ImageOpSample, 2, InvalidOperIdx, true, true, InvalidOperIdx, InvalidOperIdx, InvalidOperIdx});
  add(OpImageSparseSampleProjDrefImplicitLod,
      {ImageOpSample, 3, 3, true, true, InvalidOperIdx, InvalidOperIdx, InvalidOperIdx});
  add(OpImageSparseSampleProjDrefExplicitLod,
      {ImageOpSample, 3, 3, true, true, InvalidOperIdx, InvalidOperIdx, InvalidOperIdx});
  add(OpImageSparseFetch,
      {ImageOpFetch, 2, InvalidOperIdx, false, true, InvalidOperIdx, InvalidOperIdx, InvalidOperIdx});
  add(OpImageSparseGather,
      {ImageOpGather, 3, InvalidOperIdx, false, true, InvalidOperIdx, InvalidOperIdx, InvalidOperIdx});
  add(OpImageSparseDrefGather, {ImageOpGather, 3, 3, false, true, InvalidOperIdx, InvalidOperIdx, InvalidOperIdx});
  add(OpImageSparseRead, {ImageOpRead, 2, InvalidOperIdx, false, true, InvalidOperIdx, InvalidOperIdx, InvalidOperIdx});

  add(OpImageQuerySizeLod, {ImageOpQueryNonLod, InvalidOperIdx, InvalidOperIdx, false, false, InvalidOperIdx,
                            InvalidOperIdx, InvalidOperIdx});
  add(OpImageQuerySize, {ImageOpQueryNonLod, InvalidOperIdx, InvalidOperIdx, false, false, InvalidOperIdx,
                         InvalidOperIdx, InvalidOperIdx});
  add(OpImageQueryLod,
      {ImageOpQueryLod, InvalidOperIdx, InvalidOperIdx, false, false, InvalidOperIdx, InvalidOperIdx, InvalidOperIdx});
  add(OpImageQueryLevels, {ImageOpQueryNonLod, InvalidOperIdx, InvalidOperIdx, false, false, InvalidOperIdx,
                           InvalidOperIdx, InvalidOperIdx});
  add(OpImageQuerySamples, {ImageOpQueryNonLod, InvalidOperIdx, InvalidOperIdx, false, false, InvalidOperIdx,
                            InvalidOperIdx, InvalidOperIdx});

  add(OpAtomicLoad,
      {ImageOpAtomicLoad, InvalidOperIdx, InvalidOperIdx, false, false, InvalidOperIdx, InvalidOperIdx, 1});
  add(OpAtomicStore, {ImageOpAtomicStore, InvalidOperIdx, InvalidOperIdx, false, false, 3, InvalidOperIdx, 1});
  add(OpAtomicExchange, {ImageOpAtomicExchange, InvalidOperIdx, InvalidOperIdx, false, false, 3, InvalidOperIdx, 1});
  add(OpAtomicCompareExchange, {ImageOpAtomicCompareExchange, InvalidOperIdx, InvalidOperIdx, false, false, 4, 5, 1});
  add(OpAtomicIIncrement,
      {ImageOpAtomicIIncrement, InvalidOperIdx, InvalidOperIdx, false, false, InvalidOperIdx, InvalidOperIdx, 1});
  add(OpAtomicIDecrement,
      {ImageOpAtomicIDecrement, InvalidOperIdx, InvalidOperIdx, false, false, InvalidOperIdx, InvalidOperIdx, 1});
  add(OpAtomicIAdd, {ImageOpAtomicIAdd, InvalidOperIdx, InvalidOperIdx, false, false, 3, InvalidOperIdx, 1});
  add(OpAtomicISub, {ImageOpAtomicISub, InvalidOperIdx, InvalidOperIdx, false, false, 3, InvalidOperIdx, 1});
  add(OpAtomicSMin, {ImageOpAtomicSMin, InvalidOperIdx, InvalidOperIdx, false, false, 3, InvalidOperIdx, 1});
  add(OpAtomicUMin, {ImageOpAtomicUMin, InvalidOperIdx, InvalidOperIdx, false, false, 3, InvalidOperIdx, 1});
  add(OpAtomicSMax, {ImageOpAtomicSMax, InvalidOperIdx, InvalidOperIdx, false, false, 3, InvalidOperIdx, 1});
  add(OpAtomicUMax, {ImageOpAtomicUMax, InvalidOperIdx, InvalidOperIdx, false, false, 3, InvalidOperIdx, 1});
  add(OpAtomicAnd, {ImageOpAtomicAnd, InvalidOperIdx, InvalidOperIdx, false, false, 3, InvalidOperIdx, 1});
  add(OpAtomicOr, {ImageOpAtomicOr, InvalidOperIdx, InvalidOperIdx, false, false, 3, InvalidOperIdx, 1});
  add(OpAtomicXor, {ImageOpAtomicXor, InvalidOperIdx, InvalidOperIdx, false, false, 3, InvalidOperIdx, 1});
  add(OpAtomicFMinEXT, {ImageAtomicFMin, InvalidOperIdx, InvalidOperIdx, false, false, 3, InvalidOperIdx, 1});
  add(OpAtomicFMaxEXT, {ImageAtomicFMax, InvalidOperIdx, InvalidOperIdx, false, false, 3, InvalidOperIdx, 1});
  add(OpAtomicFAddEXT, {ImageAtomicFAdd, InvalidOperIdx, InvalidOperIdx, false, false, 3, InvalidOperIdx, 1});
}
typedef SPIRVMap<Op, SPIRVImageOpInfo> SPIRVImageOpInfoMap;

// "<" operator overloading, just to pass compilation for "SPIRVMap::rmap",
// not actually used
inline bool operator<(const SPIRVImageOpInfo &L, const SPIRVImageOpInfo &R) {
  return L.U32All < R.U32All;
}

/// @returns : a vector of types for a collection of values.
template <class T> std::vector<Type *> getTypes(T V) {
  std::vector<Type *> Tys;
  for (auto &I : V)
    Tys.push_back(I->getType());
  return Tys;
}

/// Move elements of std::vector from [begin, end) to target.
template <typename T> void move(std::vector<T> &V, size_t Begin, size_t End, size_t Target) {
  assert(Begin < End && End <= V.size() && Target <= V.size() && !(Begin < Target && Target < End));
  if (Begin <= Target && Target <= End)
    return;
  auto B = V.begin() + Begin, E = V.begin() + End;
  if (Target > V.size())
    Target = V.size();
  if (Target > End)
    Target -= (End - Begin);
  std::vector<T> Segment(B, E);
  V.erase(B, E);
  V.insert(V.begin() + Target, Segment.begin(), Segment.end());
}

void removeFnAttr(LLVMContext *Context, CallInst *Call, Attribute::AttrKind Attr);
void addFnAttr(LLVMContext *Context, CallInst *Call, Attribute::AttrKind Attr);

Function *getOrCreateFunction(Module *M, Type *RetTy, ArrayRef<Type *> ArgTypes, StringRef Name,
                              AttributeList *Attrs = nullptr, bool TakeName = true);

void dumpUsers(Value *V, StringRef Prompt = "");

bool eraseUselessFunctions(Module *M);

/// Erase a function if it is declaration, has internal linkage and has no use.
bool eraseIfNoUse(Function *F);

void eraseIfNoUse(Value *V);

/// Metadata for shader inputs and outputs, valid for scalar or vector type.
union ShaderInOutMetadata {
  struct {
    // byte 0~1
    uint64_t Value : 16; // Generic location or SPIR-V built-in ID
    // byte 2
    uint64_t Index : 1;      // Output index for dual source blending
    uint64_t IsLoc : 1;      // Whether value is a location
    uint64_t IsBuiltIn : 1;  // Whether value is a SPIR-V built-in ID
    uint64_t Component : 2;  // Component offset of inputs and outputs
    uint64_t Signedness : 1; // Signedness of the input/output, valid
                             // for integer (0 - unsigned, 1 - signed)
    uint64_t InterpMode : 2; // Interpolation mode (fragment shader)
    // byte 3
    uint64_t InterpLoc : 3; // Interpolation location (fragment
                            // shader)
    uint64_t PerPatch : 1;  // Whether this is a per-patch input/
                            // output (tessellation shader)
    uint64_t StreamId : 2;  // ID of output stream (geometry shader)
    uint64_t XfbBuffer : 2; // Transform feedback buffer ID
    // byte 4~5
    uint64_t IsXfb : 1;      // Whether this is for transform feedback
    uint64_t XfbOffset : 15; // Transform feedback offset
    // byte 6~7
    uint64_t XfbStride : 16; // Transform feedback stride
    // byte 8~9
    uint64_t IsBlockArray : 1;    // Whether we are handling block array
    uint64_t XfbArrayStride : 16; // Transform feedback array stride (for
                                  //   block array, it's flatten dimension
                                  //   of an element (1 if element is not
                                  //   sub-array; for non block array, it's
                                  //   occupied byte count of an element)
    // byte 10~11
    uint64_t XfbExtraOffset : 16; // Transform feedback extra offset
    uint64_t PerVertexDimension : 1; // Whether this is the per-vertex dimension (outermost) for an array
  };
  uint64_t U64All[2];
};

/// Info structure for all decorations applied to shader inputs and outputs.
struct ShaderInOutDecorate {
  union {
    unsigned BuiltIn; // SPIR-V built-in ID
    unsigned Loc;     // Location of generic inputs and outputs

    unsigned U32All;
  } Value;

  unsigned Index; // Output index for dual source blending

  bool IsBuiltIn; // Whether this is a SPIR-V built-in

  bool IsXfb; // Whether this is a for transform feedback

  bool IsBlockArray; // Whether we are handling a block array

  unsigned Component; // Component offset of inputs and outputs

  bool PerPatch; // Whether this is a per-patch input/output
                 // (tessellation shader)
  bool PerVertexDimension; // Whether this is decorated by "pervertexKHR"
                           // (Fragment shader)
  struct {
    SPIRVInterpModeKind Mode; // Interpolation mode
    SPIRVInterpLocKind Loc;   // Interpolation location
  } Interp;

  unsigned StreamId;       // ID of output stream (geometry shader)
  unsigned XfbBuffer;      // Transform feedback buffer ID
  unsigned XfbOffset;      // Transform feedback offset
  unsigned XfbStride;      // Transform feedback stride
  unsigned XfbExtraOffset; // Transform feedback extra offset
  unsigned XfbArrayStride; // Transform feedback array stride (for
                           //   block array, it's flatten dimension
                           //   of an element (1 if element is not
                           //   sub-array; for non block array, it's
                           //   occupied byte count of an element)
  bool contains64BitType;  // Whether contains 64-bit type
};

/// Metadata for shader block.
union ShaderBlockMetadata {
  struct {
    unsigned offset : 32;      // Offset (bytes) in block
    unsigned IsMatrix : 1;     // Whether it is a matrix
    unsigned IsRowMajor : 1;   // Whether it is a "row_major" qualified matrix
    unsigned MatrixStride : 6; // Matrix stride, valid for matrix
    unsigned Restrict : 1;     // Whether "restrict" qualifier is present
    unsigned Coherent : 1;     // Whether "coherent" qualifier is present
    unsigned Volatile : 1;     // Whether "volatile" qualifier is present
    unsigned NonWritable : 1;  // Whether "readonly" qualifier is present
    unsigned NonReadable : 1;  // Whether "writeonly" qualifier is present
    unsigned IsPointer : 1;    // Whether it is a pointer
    unsigned IsStruct : 1;     // Whether it is a structure
    unsigned Unused : 17;
  };
  uint64_t U64All;
};

/// Info structure for all decorations applied to shader block.
struct ShaderBlockDecorate {
  unsigned Offset;       // Offset (bytes) in block
  bool IsMatrix;         // Whether it is a matrix
  bool IsRowMajor;       // Whether it is a "row_major" qualified matrix
  unsigned MatrixStride; // Matrix stride, valid for matrix
  bool Restrict;         // Whether "restrict" qualifier is present
  bool Coherent;         // Whether "coherent" qualifier is present
  bool Volatile;         // Whether "volatile" qualifier is present
  bool NonWritable;      // Whether "readonly" qualifier is present
  bool NonReadable;      // Whether "writeonly" qualifier is present
};

/// Metadata for image emulation call.
union ShaderImageCallMetadata {
  struct {
    SPIRVImageOpKind OpKind : 6;     // Kind of image operation
    unsigned Dim : 3;                // Image dimension
    unsigned Arrayed : 1;            // Whether image is arrayed
    unsigned Multisampled : 1;       // Whether image is multisampled
    unsigned NonUniformSampler : 1;  // Whether sampler is non-uniform
    unsigned NonUniformResource : 1; // Whether resource is non-uniform
    unsigned WriteOnly : 1;          // Whether it is a write-only operation
    unsigned Unused : 18;
  };
  unsigned U32All;
};

/// Metadata for image memory (memory qualifiers)
union ShaderImageMemoryMetadata {
  struct {
    unsigned Restrict : 1;    // Whether "restrict" qualifier is present
    unsigned Coherent : 1;    // Whether "coherent" qualifier is present
    unsigned Volatile : 1;    // Whether "volatile" qualifier is present
    unsigned NonWritable : 1; // Whether "readonly" qualifier is present
    unsigned NonReadable : 1; // Whether "writeonly" qualifier is present

    unsigned Unused : 27;
  };
  unsigned U32All;
};

/// Flags used for floating-point control
union ShaderFloatControlFlags {
  struct {
    unsigned DenormPreserve : 4;           // Preserve denormals
    unsigned DenormFlushToZero : 4;        // Flush denormals to zeros
    unsigned SignedZeroInfNanPreserve : 4; // Preserve signed zero, INF, NaN
    unsigned RoundingModeRTE : 4;          // Rounding to even
    unsigned RoundingModeRTZ : 4;          // Rounding to zero

    unsigned Unused : 12;
  };
  unsigned U32All;
};

/// Metadata for execution modes of each shader entry-point
union ShaderExecModeMetadata {
  struct {
    struct {
      ShaderFloatControlFlags FpControlFlags; // Floating-point control flags
    } common;

    union {
      struct {
        unsigned Xfb : 1; // Transform feedback mode
        unsigned Unused : 31;
      } vs;

      struct {
        unsigned SpacingEqual : 1;          // Layout "equal_spacing"
        unsigned SpacingFractionalEven : 1; // Layout "fractional_even_spacing"
        unsigned SpacingFractionalOdd : 1;  // Layout "fractional_odd_spacing"
        unsigned VertexOrderCw : 1;         // Layout "cw"
        unsigned VertexOrderCcw : 1;        // Layout "ccw"
        unsigned PointMode : 1;             // Layout "point_mode"
        unsigned Triangles : 1;             // Layout "triangles"
        unsigned Quads : 1;                 // Layout "quads"
        unsigned Isolines : 1;              // Layout "isolines"
        unsigned Xfb : 1;                   // Transform feedback mode
        unsigned Unused : 22;

        unsigned OutputVertices; // Layout "vertices ="
      } ts;

      struct {
        unsigned InputPoints : 1;             // Layout "points"
        unsigned InputLines : 1;              // Layout "lines"
        unsigned InputLinesAdjacency : 1;     // Layout "lines_adjacency"
        unsigned Triangles : 1;               // Layout "triangles"
        unsigned InputTrianglesAdjacency : 1; // Layout "triangles_adjacency"
        unsigned OutputPoints : 1;            // Layout "points"
        unsigned OutputLineStrip : 1;         // Layout "line_strip"
        unsigned OutputTriangleStrip : 1;     // Layout "triangle_strip"
        unsigned Xfb : 1;                     // Transform feedback mode
        unsigned Unused : 23;

        unsigned Invocations;    // Layout "invocations ="
        unsigned OutputVertices; // Layout "max_vertices ="
      } gs;

      struct {
        unsigned OriginUpperLeft : 1;    // Layout "origin_upper_left"
        unsigned PixelCenterInteger : 1; // Layout "pixel_center_integer"
        unsigned EarlyFragmentTests : 1; // Layout "early_fragment_tests"
        unsigned DepthUnchanged : 1;     // Layout "depth_unchanged"
        unsigned DepthGreater : 1;       // Layout "depth_greater"
        unsigned DepthLess : 1;          // Layout "depth_less"
        unsigned DepthReplacing : 1;     // Layout "depth_any"
        unsigned PostDepthCoverage : 1;  // Layout "post_depth_coverage"
        unsigned Unused : 24;
      } fs;

      struct {
        unsigned LocalSizeX; // Layout "local_size_x ="
        unsigned LocalSizeY; // Layout "local_size_y ="
        unsigned LocalSizeZ; // Layout "local_size_z ="
      } cs;
    };
  };
  unsigned U32All[4];
};

} // namespace SPIRV

#endif
