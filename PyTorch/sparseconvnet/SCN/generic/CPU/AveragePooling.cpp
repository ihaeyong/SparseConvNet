// Copyright 2016-present, Facebook, Inc.
// All rights reserved.
//
// This source code is licensed under the license found in the
// LICENSE file in the root directory of this source tree.

#ifndef TH_GENERIC_FILE_
#define TH_GENERIC_FILE_ "generic/CPU/AveragePooling.cpp"
#else
#include "AveragePooling.h"

extern "C" void scn_DR_(AveragePooling_updateOutput)(
    THLongTensor *inputSize, THLongTensor *outputSize, THLongTensor *poolSize,
    THLongTensor *poolStride, void **m, THTensor *input_features,
    THTensor *output_features, long nFeaturesToDrop, void *rulesBuffer) {

  SCN_INITIALIZE_AND_REFERENCE(Metadata<Dimension>, m)
  uInt nPlanes = input_features->size[1] - nFeaturesToDrop;
  auto _rules =
      _m.getRuleBook(inputSize, outputSize, poolSize, poolStride, true);
  uInt nActive = _m.getNActive(outputSize);
  THTensor_(resize2d)(output_features, nActive,
                      input_features->size[1] - nFeaturesToDrop);
  THTensor_(zero)(output_features);

  auto iF = THTensor_(data)(input_features) + nFeaturesToDrop;
  auto oF = THTensor_(data)(output_features);

  for (auto &r : _rules) {
    uInt nHot = r.size() / 2;
    AveragePooling_ForwardPass<real>(iF, oF, nPlanes, input_features->stride[0],
                                     output_features->stride[0], &r[0], nHot,
                                     _rules.size());
  }
}
extern "C" void scn_DR_(AveragePooling_updateGradInput)(
    THLongTensor *inputSize, THLongTensor *outputSize, THLongTensor *poolSize,
    THLongTensor *poolStride, void **m, THTensor *input_features,
    THTensor *d_input_features, THTensor *d_output_features,
    long nFeaturesToDrop, void *rulesBuffer) {

  SCN_INITIALIZE_AND_REFERENCE(Metadata<Dimension>, m)
  uInt nPlanes = input_features->size[1] - nFeaturesToDrop;
  auto _rules =
      _m.getRuleBook(inputSize, outputSize, poolSize, poolStride, true);
  uInt nActive = _m.getNActive(outputSize);
  THTensor_(resizeAs)(d_input_features, input_features);
  THTensor_(zero)(d_input_features);

  auto diF = THTensor_(data)(d_input_features) + nFeaturesToDrop;
  auto doF = THTensor_(data)(d_output_features);

  for (auto &r : _rules) {
    uInt nHot = r.size() / 2;
    AveragePooling_BackwardPass<real>(
        diF, doF, nPlanes, input_features->stride[0],
        d_output_features->stride[0], &r[0], nHot, _rules.size());
  }
}
#endif
