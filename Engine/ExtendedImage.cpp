#pragma once

#include "./ExtendedImage.h"
#include "./ResizerModule/Resizer.h"
using namespace std;

class Resizer;

ExtendedImage::ExtendedImage(vector<unsigned char>* data, int width, int height) {
 this->data = data;
 this->width = width;
 this->height = height;
}
void ExtendedImage::resize(ExtendedImage* inImage, ExtendedImage* outImage) {
 Resizer::resizeCL(inImage, outImage);
}
int ExtendedImage::getWidth() {
 return this->width;
}
int ExtendedImage::getHeight() {
 return this->height;
}
void ExtendedImage::setData(vector<unsigned char>* data) {
 delete this->data;
 this->data = data;
}
vector<unsigned char>* ExtendedImage::getData() {
 return this->data;
}