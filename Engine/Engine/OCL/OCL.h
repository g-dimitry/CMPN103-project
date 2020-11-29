//
// Created by Vaclav Samec on 5/7/15 AD.
// Copyright (c) 2015 Venca. All rights reserved.
//

#pragma once

#include <string>
#include "OCLManager.h"
#include <vector>
#include <unordered_map>

class OCL
{
public:
 static oclManager ocl;
 static void initializeOCL();
};
