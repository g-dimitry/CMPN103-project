#include "./Assets.h"

cl::Buffer* Assets::textureBuffer = nullptr;
vector<unsigned char> Assets::texture;
vector<int> Assets::widths;
vector<int> Assets::heights;
vector<int> Assets::startIndices;
vector<int> Assets::endIndices;
void Assets::preloadImages()
{
 preloadImage("./Project/Assets/Images/AND_2.png");
 preloadImage("./Project/Assets/Images/BUFFER.png");
 preloadImage("./Project/Assets/Images/NAND_2.png");
 preloadImage("./Project/Assets/Images/NOR_2.png");
 preloadImage("./Project/Assets/Images/NOT.png");
 preloadImage("./Project/Assets/Images/OR_2.png");
 preloadImage("./Project/Assets/Images/XNOR_2.png");
 preloadImage("./Project/Assets/Images/XOR_2.png");
 textureBuffer = OCL::ocl.preloadTexture(&texture);
};
int Assets::getImageWidth(IMAGES image)
{
 return widths[(int)image];
}
int Assets::getImageHeight(IMAGES image)
{
 return heights[(int)image];
}
int Assets::getImageStart(IMAGES image)
{
 return startIndices[(int)image];
}
int Assets::getImageEnd(IMAGES image)
{
 return endIndices[(int)image];
}
void Assets::preloadImage(string filename)
{
 vector<unsigned char> image;
 unsigned int width;
 unsigned int height;
 unsigned error = lodepng::decode(image, width, height, filename);
 if (error)
  std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
 widths.push_back(width);
 heights.push_back(height);
 startIndices.push_back(texture.size());
 endIndices.push_back(texture.size() + image.size() - 1);
 texture.insert(texture.end(), image.begin(), image.end());
}