#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <CL/cl.hpp>
using namespace std;
#include "../Utilities/ExtendedImage/ExtendedImage.h";
#include "../OCL/OCL.h";

#include "../Utilities/LodePNG/LodePNG.h";

enum class IMAGES {
 AND_2,
};

class Assets {
public:
 static cl::Buffer* textureBuffer;
 static vector<unsigned char> texture;
 static vector<int> widths;
 static vector<int> startIndices;
 static vector<int> endIndices;
 static void preloadImages() {
  preloadImage("./Assets/Images/AND_2.png");
  textureBuffer = OCL::ocl.preloadTexture(&texture);
 };
 static int getImageWidth(IMAGES image) {
  return widths[(int)image];
 }
 static int getImageStart(IMAGES image) {
  return startIndices[(int)image];
 }
 static int getImageEnd(IMAGES image) {
  return endIndices[(int)image];
 }
 static void preloadImage(string filename) {
  vector<unsigned char> image;
  unsigned int width;
  unsigned int height;
  unsigned error = lodepng::decode(image, width, height, filename);
  if (error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
  widths.push_back(width);
  startIndices.push_back(texture.size());
  endIndices.push_back(texture.size() + image.size() - 1);
  texture.insert(texture.end(), image.begin(), image.end());
 }
};

cl::Buffer* Assets::textureBuffer = nullptr;
vector<unsigned char> Assets::texture;
vector<int> Assets::widths;
vector<int> Assets::startIndices;
vector<int> Assets::endIndices;