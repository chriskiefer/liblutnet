#include <vector>
#include <cmath>
#include <stdexcept>
#include <random>
// #include <thread>
// #include <future>

#ifdef PYTHON_BUILD

#include <pybind11/pybind11.h>
namespace py = pybind11;

#endif

#ifndef uint
using uint = unsigned int;
#endif

struct LUT4 {
  uint ttable[16];
  uint output=0;
  LUT4 *inputs[4];
  uint idx=0;
};


using LUT4Layer = std::vector<LUT4>;

class FFLUT4Net {
public:
  std::random_device rd{};
  std::mt19937_64 mersenne{rd()};
  std::bernoulli_distribution bd{0.5};

  FFLUT4Net(uint numInputs) {
    LUT4Layer inputLayer(numInputs);
    for(uint i=0; i < numInputs; i++) {
      inputLayer[i].idx = i;
    }
    layers.push_back(inputLayer);
  }

  void addLayer(uint numNodes) {
    LUT4Layer newLayer(numNodes);
    //make connections
    uint topLayerNodeCount = layers.back().size();
    uint stride = ceil(double(topLayerNodeCount) / double(numNodes));
    // std::cout << "stride: " << stride << std::endl;
// #ifdef PYTHON_BUILD
// py::print("stride", stride);
// #endif
    for(uint i=0; i < numNodes; i++) {
      //make connections
      for(uint i_input =0; i_input < 4; i_input++) {
        uint inputNodeIndex = ((i * stride) + i_input) % topLayerNodeCount;
        newLayer[i].inputs[i_input] = &layers.back().at(inputNodeIndex);
      }
      //init ttable
      for(uint i_t=0; i_t < 16; i_t++) {
        newLayer[i].ttable[i_t] =  bd(mersenne);
      }
      //record node idx in layer
      newLayer[i].idx = i;
    }
    layers.push_back(newLayer);
  }

  void setTtable(uint layer, uint nodeNumber, uint ttable[16]) {
    for(uint i=0; i < 16; i++) {
        setTtableElement(layer, nodeNumber, i, ttable[i]);
    }
  }

  inline uint calcNodeLUTAddress(uint layer, uint nodeNumber) {
    uint addr = 0;
    for(uint i_input=0; i_input < 4; i_input++) {
      addr += (layers[layer][nodeNumber].inputs[3-i_input]->output << i_input);
    }
    return addr;
  }

  void calc(std::vector<unsigned int> &input) {
    if (input.size() != layers[0].size()) throw std::out_of_range("input size OOR");
    for(uint i=0; i < input.size(); i++) {
      layers[0][i].output = input[i];
    }
    auto calcLayerBlock = [this](uint layer, uint start, uint blockSize) {
      for(uint i_node=start; i_node < start + blockSize; i_node++) {
        uint addr = calcNodeLUTAddress(layer, i_node);
        layers[layer][i_node].output = layers[layer][i_node].ttable[addr];
      }
    };
    for(uint i=1; i < layers.size(); i++) {
      //at this stage, threading just slows it down
      // if (layers[i].size()   > 80000000) {
      //   uint numThreads = 16;
      //   uint blockSize = ceil(double(layers[i].size()) / double(numThreads));
      //   uint lastBlockSize = layers[i].size() - ((numThreads-1) * blockSize);
      //   std::vector<std::future<void> > future_threads;
      //   future_threads.reserve(numThreads);
      //   for(uint i_thread = 0; i_thread < numThreads - 1; i_thread++) {
      //     future_threads.emplace_back(std::async(std::launch::async, [&calcLayerBlock, i_thread, blockSize, i]() {
      //       calcLayerBlock(i, i_thread * blockSize, blockSize);
      //     }));
      //   }
      //   // auto last_thread_future = std::async(std::launch::async, calcLayerBlock, (numThreads - 1) * blockSize, lastBlockSize);
      //   future_threads.emplace_back(std::async(std::launch::async, [&calcLayerBlock, numThreads, blockSize, i, lastBlockSize]() {
      //     calcLayerBlock(i, (numThreads - 1) * blockSize, lastBlockSize);
      //   }));
      //   // future_threads.emplace_back(last_thread_future);
      //   for(auto& fut : future_threads) {
      //     fut.get();
      //   }
      // }else{
      //   calcLayerBlock(i, 0, layers[i].size());
      // }
      calcLayerBlock(i, 0, layers[i].size());
    }
  }

  int getTtableElement(unsigned int layer, unsigned int node, unsigned int bit) {
    if (bit > 15) throw std::out_of_range("bit index over 15");
    return layers.at(layer).at(node).ttable[bit];
  }

  void setTtableElement(unsigned int layer, unsigned int node, unsigned int bit, unsigned int value) {
    if (bit > 15) throw std::out_of_range("bit index over 15");
    layers.at(layer).at(node).ttable[bit] = value;
  }

  uint getLayerSize(uint idx) {
    return layers.at(idx).size();
  }

  uint getLayerCount() {
    return layers.size();
  }

  uint getOutput(uint layer, uint node) {
    return layers.at(layer).at(node).output;
  }

  uint getTopLayerOutput(uint node) {
    return layers.back().at(node).output;
  }

  uint getInputIndex(uint layer, uint node, uint input) {
    if (input > 3) {throw std::out_of_range("input index over 3"); return 0;}
    if (layer ==0) {throw std::out_of_range("no input to layer 0"); return 0;}
    return layers.at(layer).at(node).inputs[input]->idx;
  }

  vector<uint> serialiseLayer(uint layerNum) {

  }

  vector<uint> serialiseStructure() {

  }

  vector<uint> serialise() {

  }

  bool unserialise(vector<uint> &bin) {

  }

private:

  std::vector<LUT4Layer > layers;

};
