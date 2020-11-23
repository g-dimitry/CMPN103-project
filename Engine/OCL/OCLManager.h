//
// Created by Vaclav Samec on 4/28/15 AD.
// Copyright (c) 2015 Venca. All rights reserved.
//

#pragma once
#include "../Utilities/ExtendedImage/ExtendedImage.h"

#define __CL_ENABLE_EXCEPTIONS
#if defined(__APPLE__) || defined(__MACOSX)
#include "cl.hpp"
#else
#include <CL/cl.hpp>
#endif

#include <string>
class ExtendedImage;

class oclManager
{
public:

 enum DeviceType
 {
  CPU = CL_DEVICE_TYPE_CPU,
  GPU = CL_DEVICE_TYPE_GPU
 };

 bool createContext(DeviceType type);

 bool addKernelProgram(const std::string& kernel);

 void resizeImage(ExtendedImage* in, ExtendedImage* out);

 void rotateImage(ExtendedImage* in, ExtendedImage* out, int rotation);

 void renderShapes(vector<float>* shapes, cl::Buffer* textureBuffer, int width, int height, vector<unsigned char>* out);

 cl::Image2D* preloadImage(ExtendedImage* in);

 cl::Buffer* preloadTexture(vector<unsigned char>* v);

protected:

 void setupPlatform(DeviceType type);

 static char* getCLErrorString(cl_int err);

 static const std::string preferredDeviceVendors[];

 cl::Platform        m_platform;
 cl::Device          m_device;
 cl::Context         m_context;
 cl::Program         m_program;
 cl::CommandQueue    m_queue;
};
