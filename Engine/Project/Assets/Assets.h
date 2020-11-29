#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <CL/cl.hpp>
#include "../../Engine/Utils/LodePNG/LodePNG.h";
#include "../../Engine/OCL/OCL.h";
using namespace std;
using namespace lodepng;

enum class IMAGES {
 AND_2,
 BUFFER,
 NAND_2,
 NOR_2,
 NOT,
 OR_2,
 XNOR_2,
 XOR_2,
};

class Assets {
public:
 static cl::Buffer* textureBuffer;
 static vector<unsigned char> texture;
 static vector<int> widths;
 static vector<int> heights;
 static vector<int> startIndices;
 static vector<int> endIndices;
 static void preloadImages();
 static int getImageWidth(IMAGES image);
 static int getImageHeight(IMAGES image);
 static int getImageStart(IMAGES image);
 static int getImageEnd(IMAGES image);
 static void preloadImage(string filename);
};
