#include "Resizer.h"
#include "OpenCL/oclManager.h"
#include "OpenCL/kernels/resize_kernel.h"
using namespace std;

void Resizer::resizeCL(ExtendedImage* inImage, ExtendedImage* outImage)
{
 // init OpenCL
 oclManager ocl;

 if (!ocl.createContext(oclManager::GPU))
 {
  return;
 }

 // compile program
 if (!ocl.addKernelProgram(resizeKernel))
 {
  std::cerr << "Error building kernel." << std::endl;
  return;
 }

 ocl.resizeImage(inImage, outImage);
}