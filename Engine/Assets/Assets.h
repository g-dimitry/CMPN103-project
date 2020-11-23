#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <CL/cl.hpp>
using namespace std;
#include "../Utilities/ExtendedImage/ExtendedImage.h";
#include "../OCL/OCL.h";

#include "../Utilities/LodePNG/LodePNG.h";

class Assets {
public:
 static cl::Image2D* AND_2;
 static void preloadImages() {
  AND_2 = preloadImage("./Assets/Images/AND_2.png");
 };
 static cl::Image2D* preloadImage(string filename) {
  vector<unsigned char> image;
  unsigned int width;
  unsigned int height;
  unsigned error = lodepng::decode(image, width, height, filename);
  if (error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
  ExtendedImage extendedImage = ExtendedImage(&image, width, height);
  return OCL::ocl.preloadImage(&extendedImage);
 }
};

cl::Image2D* Assets::AND_2 = nullptr;
