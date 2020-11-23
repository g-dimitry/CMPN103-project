#include "OCL.h"
#include "kernels/resize_kernel.h"
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

void OCL::resizeCL(ExtendedImage* inImage, ExtendedImage* outImage)
{
 OCL::ocl.resizeImage(inImage, outImage);
}

void OCL::rotateCL(ExtendedImage* inImage, ExtendedImage* outImage, int rotation)
{
 OCL::ocl.rotateImage(inImage, outImage, rotation);
}