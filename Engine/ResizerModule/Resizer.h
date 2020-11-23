//
// Created by Vaclav Samec on 5/7/15 AD.
// Copyright (c) 2015 Venca. All rights reserved.
//

#pragma once

#include <string>
#include "OpenCL/oclManager.h"
#include <vector>
#include <unordered_map>
#include "../ExtendedImage.h"
class ExtendedImage;

class OCL
{
public:
 static oclManager ocl;
 static void initializeOCL();
 static void resizeCL(ExtendedImage* inImage, ExtendedImage* outImage);
 static void rotateCL(ExtendedImage* inImage, ExtendedImage* outImage, int rotation);
};
