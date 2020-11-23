#pragma once

#include <iostream>
#include <vector>
using namespace std;

class OCL;

class ExtendedImage {
private:
 vector<unsigned char>* data;
 int width;
 int height;
public:
 ExtendedImage(vector<unsigned char>* data, int width, int height);
 static void resize(ExtendedImage* inImage, ExtendedImage* outImage);
 static void rotate(ExtendedImage* inImage, ExtendedImage* outImage, int rotation);
 int getWidth();
 int getHeight();
 void setData(vector<unsigned char>* data);
 vector<unsigned char>* getData();
};
