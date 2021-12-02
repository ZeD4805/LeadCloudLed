cmake_minimum_required(VERSION 3.20)
project(LeadCloudDDriver)

set(CMAKE_CXX_STANDARD 14)

add_library(LeadCloudDDriver library.cpp GPIOConfig.h MatrixHardwareDriver.cpp MatrixHardwareDriver.cpp main.cpp)
