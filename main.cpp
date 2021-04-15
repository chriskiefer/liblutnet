#include <iostream>
#include "gtest/gtest.h"
#include <thread>
#include "LUTNet.hpp"

using namespace std;


TEST(LUTNETTEST, LayerAddTest) {
  FFLUT4Net net(4);
  net.addLayer(15);
  net.addLayer(1);
  EXPECT_EQ(net.getLayerSize(0), 4);
  EXPECT_EQ(net.getLayerSize(1), 15);
  EXPECT_EQ(net.getLayerSize(2), 1);
  try {
    net.getLayerSize(100);
    FAIL() << "should have thrown exception:     net.getLayerSize(100);" << std::endl;
  }
  catch(std::exception e) {}

}

TEST(LUTNETTEST, LayerTtableTest) {
  FFLUT4Net net(4);
  net.addLayer(4);
  net.addLayer(1);
  uint ttable[16] = {0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,1};
  net.setTtable(1,3,ttable);

  uint ttable2[16] = {1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0};
  net.setTtable(2,0,ttable2);
  EXPECT_EQ(net.getTtableElement(1,3,0), 0);
  EXPECT_EQ(net.getTtableElement(1,3,1), 1);
  EXPECT_EQ(net.getTtableElement(1,3,15), 1);

  EXPECT_EQ(net.getTtableElement(2,0,1), 1);
  EXPECT_EQ(net.getTtableElement(2,0,4), 0);
  EXPECT_EQ(net.getTtableElement(2,0,15), 0);

  net.setTtableElement(1,2,13,0);
  EXPECT_EQ(net.getTtableElement(1,2,13), 0);
  net.setTtableElement(2,0,1,0);
  EXPECT_EQ(net.getTtableElement(2,0,1), 0);

  try {
    net.setTtableElement(1,0,17,0);
    FAIL() << "should have thrown exception" << std::endl;
  }
  catch(std::exception e) {}
  try {
    net.setTtableElement(1,199,1,0);
    FAIL() << "should have thrown exception" << std::endl;
  }
  catch(std::exception e) {}
  try {
    net.setTtableElement(9,1,1,0);
    FAIL() << "should have thrown range exception" << std::endl;
  }
  catch(std::exception e) {}
  try {
    net.setTtable(1,200,ttable2);;
    FAIL() << "should have thrown range exception:    net.setTtable(1,200,ttable2);;" << std::endl;
  }
  catch(std::exception e) {}
  try {
    net.setTtable(200,0,ttable2);;
    FAIL() << "should have thrown range exception:    net.setTtable(200,0,ttable2);;" << std::endl;
  }
  catch(std::exception e) {}
}

TEST(LUTNETTEST, RandInitTest) {
  //check that there are some ttable elements that aren't 0
  FFLUT4Net net(4);
  net.addLayer(1);
  int oneCount=0;
  for(int i=0; i < 16; i++) {
    oneCount += net.getTtableElement(1,0,i);
    // cout << net.getTtableElement(1,0,i) << endl;
  }
  EXPECT_GT(oneCount,0);
}

TEST(LUTNETTEST, CalcNetTest) {
  FFLUT4Net net(4);
  net.addLayer(1);
  EXPECT_EQ(net.getOutput(1,0), 0); //default output=0

  uint ttable[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  net.setTtable(1,0,ttable);
  vector<uint> x = {0,0,0,0};
  net.calc(x);
  EXPECT_EQ(net.getOutput(1,0), 0);

  uint ttable2[16] = {0,0,1,0,0,1,0,0,1,0,0,1,0,1,0,1};
  net.setTtable(1,0,ttable2);
  {
    vector<uint> x2 = {0,0,1,0};
    net.calc(x2);
    EXPECT_EQ(net.getOutput(1,0), 1);
  }
  {
    vector<uint> x2 = {0,0,1,1};
    net.calc(x2);
    EXPECT_EQ(net.getOutput(1,0), 0);
  }
  {
    vector<uint> x2 = {1,1,1,1};
    net.calc(x2);
    EXPECT_EQ(net.getOutput(1,0), 1);
  }
  {
    vector<uint> x2 = {1,1,1,1};
    net.calc(x2);
    try {
      EXPECT_EQ(net.getOutput(1,73), 1);
      FAIL() << "output OOR  - node\n";
    }
    catch(exception e) {}
    try {
      EXPECT_EQ(net.getOutput(7234,0), 1);
      FAIL() << "output OOR  - layer \n";
    }
    catch(exception e) {}
  }
}

TEST(LUTNETTEST, InputIdxtest) {
  FFLUT4Net net(4);
  net.addLayer(4);
  net.addLayer(1);
  EXPECT_EQ(net.getInputIndex(1,0,0),0);
  EXPECT_EQ(net.getInputIndex(1,0,1),1);
  EXPECT_EQ(net.getInputIndex(1,0,2),2);
  EXPECT_EQ(net.getInputIndex(1,0,3),3);

  EXPECT_EQ(net.getInputIndex(2,0,0),0);
  EXPECT_EQ(net.getInputIndex(2,0,1),1);
  EXPECT_EQ(net.getInputIndex(2,0,2),2);
  EXPECT_EQ(net.getInputIndex(2,0,3),3);
  try {
    uint idx = net.getInputIndex(1,0,4);
    FAIL() << "input index OOR \n";
  }
  catch(exception e) {}
  try {
    uint idx = net.getInputIndex(0,0,1);
    FAIL() << "input layer 0 \n";
  }
  catch(exception e) {}
}

TEST(LUTNETTEST, InputIdxtest2) {
  FFLUT4Net net(8);
  net.addLayer(2);
  EXPECT_EQ(net.getInputIndex(1,1,1), 5);
}

TEST(LUTNETTEST, InputIdxtest3) {
  FFLUT4Net net(8);
  net.addLayer(16);
  // for(int i=0; i < 16; i++) {
  //   for(int j=0; j < 4; j++) {
  //     cout << i << "/" << j << ":" << net.getInputIndex(1,i,j) << endl;
  //   }
  // }
  EXPECT_EQ(net.getInputIndex(1,1,2),3);
  EXPECT_EQ(net.getInputIndex(1,7,0),7);
  EXPECT_EQ(net.getInputIndex(1,4,1),5);
  EXPECT_EQ(net.getInputIndex(1,5,2),7);
}

TEST(LUTNETTEST, CalcNetTest2Layer) {
  FFLUT4Net net(16);
  net.addLayer(4);
  net.addLayer(1);
  EXPECT_EQ(net.getOutput(1,0), 0); //default output=0

  uint ttable[16] = {0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1};
  net.setTtable(1,0,ttable);
  net.setTtable(1,1,ttable);
  net.setTtable(1,2,ttable);
  net.setTtable(1,3,ttable);
  net.setTtable(2,0,ttable);
  {
    vector<uint> x = {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};
    net.calc(x);
    EXPECT_EQ(net.getOutput(2,0), 0);
  }
  {
    vector<uint> x = {1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1};
    net.calc(x);
    EXPECT_EQ(net.getOutput(2,0), 1);
  }
  {
    vector<uint> x = {1,1,1,1, 0,0,0,0, 0,0,0,0, 0,0,0,0,};
    net.calc(x);
    EXPECT_EQ(net.getOutput(2,0), 1);
  }
  {
    vector<uint> x = {0,0,0,0, 1,1,1,1, 1,1,1,1, 1,1,1,1};
    net.calc(x);
    EXPECT_EQ(net.getOutput(2,0), 0);
  }
}
TEST(LUTNETTEST, CalcTopLayerTest) {
  FFLUT4Net net(4);
  net.addLayer(2);
  EXPECT_EQ(net.getTopLayerOutput(0), 0); //default output=0

  uint ttable[16] = {0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1};
  net.setTtable(1,0,ttable);
  net.setTtable(1,1,ttable);
  {
    vector<uint> x = {0,0,0,0};
    net.calc(x);
    cout << "test1\n";
    EXPECT_EQ(net.getTopLayerOutput(0), 0);
    EXPECT_EQ(net.getTopLayerOutput(1), 0);
  }
  {
    vector<uint> x = {1,1,1,1};
    net.calc(x);
    cout << "test2\n";
    EXPECT_EQ(net.getTopLayerOutput(0), 1);
    EXPECT_EQ(net.getTopLayerOutput(1), 1);
  }
}

TEST(LUTNETTEST, CalcNetTestSpeed) {
  FFLUT4Net net(1024);
  net.addLayer(2048);
  net.addLayer(8192);
  net.addLayer(1024);
  net.addLayer(512);
  net.addLayer(256);
  net.addLayer(128);
  net.addLayer(64);
  net.addLayer(32);
  net.addLayer(16);
  net.addLayer(8);
  net.addLayer(4);
  net.addLayer(2);
  vector<uint> x(net.getLayerSize(0));
  for(int i=0; i < 10000; i++) {
    net.calc(x);
  }
}
TEST(LUTNETTEST, LayerSizeTest) {
  FFLUT4Net net(1024);
  net.addLayer(1024);
  net.addLayer(512);
  net.addLayer(256);
  net.addLayer(2);
  EXPECT_EQ(net.getLayerSize(0),1024);
  EXPECT_EQ(net.getLayerSize(1),1024);
  EXPECT_EQ(net.getLayerSize(2),512);
  EXPECT_EQ(net.getLayerSize(3),256);
  EXPECT_EQ(net.getLayerSize(4),2);
  EXPECT_EQ(net.getLayerCount(),5);
}

TEST(LUTNETTEST, Serialisation) {
  FFLUT4Net net(2);
  net.addLayer(4);
  net.addLayer(2);
  auto data = net.serialiseAllLayers();
  for(int i=0; i < data.size(); i++) {
    cout << data[i] << ",";
  }
  cout << endl;
  EXPECT_EQ(data.size(),6);
  auto struc = net.serialiseStructure();
  for(int i=0; i < struc.size(); i++) {
    cout << struc[i] << ",";
  }
  cout << endl;
  uint strucSize = 1 + 3;
  EXPECT_EQ(struc.size(), strucSize);
  auto alldata = net.serialiseModel();
  for(int i=0; i < alldata.size(); i++) {
    cout << alldata[i] << ",";
  }
  cout << endl;
  EXPECT_EQ(alldata.size(), strucSize + 6);

}

TEST(LUTNETTEST, SerialiseUnserialisation) {
  std::srand(std::time(nullptr));
  uint inCount = 16;
  uint outCount = 1;
  for(uint test=0; test < 16; test++) {
    //init random network and and collect in-out-pairs
    FFLUT4Net net(inCount);
    net.addLayer(4);
    net.addLayer(outCount);
    auto alldata = net.serialiseModel();
    cout << "Data 1: ";
    for(int i=0; i < alldata.size(); i++) {
      cout << alldata[i] << ",";
    }
    cout << endl;
    vector<vector<uint>> testins;
    vector<vector<uint>> testouts;
    for(uint i=0; i < 5; i++) {
      vector<uint> ins(inCount);
      vector<uint> outs(outCount);
      for(uint j=0; j < inCount; j++) {
        ins[j] = std::rand() > (RAND_MAX/2);
        cout << ins[j] << ",";
      }
      net.calc(ins);
      cout << " : ";

      for(uint j=0; j < outCount; j++) {
        outs[j] = net.getTopLayerOutput(j);
        cout << outs[j] << ",";
      }
      cout << endl;
      testins.push_back(ins);
      testouts.push_back(outs);
    }

    //check test data
    net.unserialise(alldata);
    auto alldata2 = net.serialiseModel();

    cout << "Data 2: ";
    for(uint i=0; i < alldata2.size(); i++) {
      cout << alldata2[i] << ",";
    }
    cout << endl;
    for(uint i=0; i < testins.size(); i++) {
      net.calc(testins[i]);
      for(uint j=0; j < inCount; j++) {
        cout << testins[i][j] << ",";
      }
      cout << " : ";

      for(uint j=0; j < outCount; j++) {
        uint output = net.getTopLayerOutput(j);
        cout << output << ",";
        EXPECT_EQ(output, testouts[i][j]);
      }
      cout << endl;
    }
  }


}
int main(int argc, char **argv) {
    cout << "LUTNet library tests\n";
    ::testing::InitGoogleTest(&argc, argv);
    int res =  RUN_ALL_TESTS();
    return res;
}
