#pragma once

#include <iostream>
#include <vector>
using namespace std;

class Resizer;

class ExtendedImage {
private:
 vector<unsigned char>* data;
 int width;
 int height;
public:
 ExtendedImage(vector<unsigned char>* data, int width, int height);
 static void resize(ExtendedImage* inImage, ExtendedImage* outImage);
 int getWidth();
 int getHeight();
 void setData(vector<unsigned char>* data);
 vector<unsigned char>* getData();
};
