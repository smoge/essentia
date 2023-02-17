/*
 * Copyright (C) 2006-2023  Music Technology Group - Universitat Pompeu Fabra
 *
 * This file is part of Essentia
 *
 * Essentia is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License as published by the Free
 * Software Foundation (FSF), either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the Affero GNU General Public License
 * version 3 along with this program.  If not, see http://www.gnu.org/licenses/
 */

#ifndef ESSENTIA_TENSORFLOWPREDICTFSDSINET_H
#define ESSENTIA_TENSORFLOWPREDICTFSDSINET_H


#include "streamingalgorithmcomposite.h"
#include "algorithmfactory.h"
#include "algorithm.h"
#include "network.h"

namespace essentia {
namespace streaming {

class TensorflowPredictFSDSINet : public AlgorithmComposite {
 protected:
  Algorithm* _frameCutter;
  Algorithm* _tensorflowInputFSDSINet;
  Algorithm* _vectorRealToTensor;
  Algorithm* _tensorToPool;
  Algorithm* _tensorTranspose;
  Algorithm* _tensorflowPredict;
  Algorithm* _poolToTensor;
  Algorithm* _tensorToVectorReal;

  SinkProxy<Real> _signal;
  SourceProxy<std::vector<Real> > _predictions;

  scheduler::Network* _network;
  bool _configured;

  void createInnerNetwork();
  void clearAlgos();

 public:
  TensorflowPredictFSDSINet();
  ~TensorflowPredictFSDSINet();

  void declareParameters() {
    declareParameter("graphFilename", "the name of the file from which to load the TensorFlow graph", "", "");
    declareParameter("savedModel", "the name of the TensorFlow SavedModel. Overrides parameter `graphFilename`", "", "");
    declareParameter("input", "the name of the input node in the TensorFlow graph", "", "x");
    declareParameter("output", "the name of the node from which to retrieve the output tensors", "", "model/predictions/Sigmoid");
    declareParameter("patchHopSize", "the number of frames between the beginnings of adjacent patches. 0 to avoid overlap", "[0,inf)", 50);
    declareParameter("lastPatchMode", "what to do with the last frames: `repeat` them to fill the last patch or `discard` them", "{discard,repeat}", "discard");
    declareParameter("batchSize", "the batch size for prediction. This allows parallelization when GPUs are available. Set it to -1 or 0 to accumulate all the patches and run a single TensorFlow session at the end of the stream", "[-1,inf)", 64);
  }

  void declareProcessOrder() {
    declareProcessStep(ChainFrom(_frameCutter));
  }

  void configure();
  void reset();

  static const char* name;
  static const char* category;
  static const char* description;

};

} // namespace streaming
} // namespace essentia

#include "vectorinput.h"
#include "pool.h"
#include "poolstorage.h"

namespace essentia {
namespace standard {

// Standard non-streaming algorithm comes after the streaming one as it
// depends on it
class TensorflowPredictFSDSINet : public Algorithm {
 protected:
  Input<std::vector<Real> > _signal;
  Output<std::vector<std::vector<Real> > > _predictions;

  streaming::Algorithm* _tensorflowPredictFSDSINet;
  streaming::VectorInput<Real>* _vectorInput;
  scheduler::Network* _network;
  Pool _pool;
  bool _normalize;

  void createInnerNetwork();
  void normalizeFSDSINet(std::vector<Real> &x);

 public:
  TensorflowPredictFSDSINet();
  ~TensorflowPredictFSDSINet();

  void declareParameters() {
    declareParameter("graphFilename", "the name of the file from which to load the TensorFlow graph", "", "");
    declareParameter("savedModel", "the name of the TensorFlow SavedModel. Overrides parameter `graphFilename`", "", "");
    declareParameter("input", "the name of the input node in the TensorFlow graph", "", "x");
    declareParameter("output", "the name of the node from which to retrieve the output tensors", "", "model/predictions/Sigmoid");
    declareParameter("patchHopSize", "number of frames between the beginnings of adjacent patches. 0 to avoid overlap", "[0,inf)", 50);
    declareParameter("lastPatchMode", "what to do with the last frames: `repeat` them to fill the last patch or `discard` them", "{discard,repeat}", "discard");
    declareParameter("batchSize", "the batch size for prediction. This allows parallelization when GPUs are available. Set it to -1 or 0 to accumulate all the patches and run a single TensorFlow session at the end of the stream", "[-1,inf)", 64);
    declareParameter("normalize", "whether to normalize the input audio signal. Note that this parameter is only available in standard mode", "{false,true}", true);
  }

  void configure();
  void compute();
  void reset();

  static const char* name;
  static const char* category;
  static const char* description;
};

} // namespace standard
} // namespace essentia

#endif // ESSENTIA_TENSORFLOWPREDICTFSDSINET_H
