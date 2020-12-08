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
  uint strucSize = 1 + 3 + (4*4) + (2*4);
  EXPECT_EQ(struc.size(), strucSize);
  auto alldata = net.serialiseModel();
  for(int i=0; i < alldata.size(); i++) {
    cout << alldata[i] << ",";
  }
  cout << endl;
  EXPECT_EQ(alldata.size(), strucSize + 6);

}
int main(int argc, char **argv) {
    cout << "LUTNet library tests\n";
    ::testing::InitGoogleTest(&argc, argv);
    int res =  RUN_ALL_TESTS();
//    unsigned int n = std::thread::hardware_concurrency();
//    std::cout << n << " concurrent threads are supported.\n";
    //perf testing
//    clock_t t = clock();
//    for(int i=0; i < 86; i++) {
//        ivec x(86);
//        for(int j=0; j<x.size(); j++) x[j] = rand() % 16;
//        cout << ETC::calc(x) << endl;
//    }
//    const double work_time = (clock() - t) / double(CLOCKS_PER_SEC) * 1000;
//    cout << work_time << " ms" << endl;

//    ivec test = {0,1,2,3,4,5,6,7,8,9};
//    int offset=9, dx=2, past=3;
//    cout << test.subvec(offset-dx-past+1,offset) << endl;
//    cout << test.subvec(offset-dx-past+1,offset-dx) << endl;
//    ivec test = {1,2,3,4,5,6,7,8,9};
//    ivec test2 = {10,11,12,13,14,15};
//    ivec comb = test.subvec(0,6);
//    comb(span(4,5)) = test2(span(4,5));
//    cout << test.t() << endl;
//    cout << test2.t() << endl;
//    cout << comb.t() << endl;
    return res;
}
