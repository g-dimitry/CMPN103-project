//
// Created by Vaclav Samec on 5/7/15 AD.
// Copyright (c) 2015 Venca. All rights reserved.
//

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include "../ExtendedImage.h"
class ExtendedImage;

class Resizer
{
public:
 static void resizeCL(ExtendedImage* inImage, ExtendedImage* outImage);
};
