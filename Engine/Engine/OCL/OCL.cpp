#include <iostream>
#include "OCL.h"
#include "kernels/kernels.h"
using namespace std;

oclManager OCL::ocl;

void OCL::initializeOCL() {
 if (!OCL::ocl.createContext(oclManager::GPU))
 {
  return;
 }

 // compile program
 if (!OCL::ocl.addKernelProgram(resizeKernel))
 {
  std::cerr << "Error building kernel." << std::endl;
  return;
 }
}