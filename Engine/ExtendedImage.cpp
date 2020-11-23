#pragma once

#include "./ExtendedImage.h"
#include "./ResizerModule/Resizer.h"
using namespace std;

ExtendedImage::ExtendedImage(vector<unsigned char>* data, int width, int height) {
 this->data = data;
 this->width = width;
 this->height = height;
}
void ExtendedImage::resize(ExtendedImage* inImage, ExtendedImage* outImage) {
 OCL::resizeCL(inImage, outImage);
}
void ExtendedImage::rotate(ExtendedImage* inImage, ExtendedImage* outImage, int rotation) {
 OCL::rotateCL(inImage, outImage, rotation);
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