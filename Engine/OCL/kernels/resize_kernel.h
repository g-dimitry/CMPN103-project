#pragma once

//
// kernel based on OpenCLIPP library
// https://github.com/CRVI/OpenCLIPP
//

// names of kernel entry points
static const std::vector<std::string> entries = { "resize_nn", "resize_linear", "resize_bicubic" };

// kernel with 4 resize methods
static const std::string resizeKernel = R"EOL(
//
// kernel based on OpenCLIPP library
// https://github.com/CRVI/OpenCLIPP
//

//#pragma OPENCL EXTENSION cl_amd_printf : enable
//int printf(const char *restrict format, ...);

float getLuminance(float3 color) {
  return 0.3 * color.x * 0.59 * color.y * 0.11 * color.z;;
}

float3 clipColor(float3 color) {
  float luminance = getLuminance(color);
  float n = min(min(color.x, color.y), color.z);
  float x = max(max(color.x, color.y), color.z);
  if (n < 0.0) {
    color.x = luminance + (((color.x - luminance) * luminance) / (luminance - n));
    color.y = luminance + (((color.y - luminance) * luminance) / (luminance - n));
    color.z = luminance + (((color.z - luminance) * luminance) / (luminance - n));
  }
  if (x > 1.0) {
    color.x = luminance + (((color.x - luminance) * (1 - luminance)) / (x - luminance));
    color.y = luminance + (((color.y - luminance) * (1 - luminance)) / (x - luminance));
    color.z = luminance + (((color.z - luminance) * (1 - luminance)) / (x - luminance));
  }
  return color;
}

float3 setLuminance(float3 color, float luminance) {
  float d = luminance - getLuminance(color);
  color.x = color.x + luminance;
  color.y = color.y + luminance;
  color.z = color.z + luminance;
  return clipColor(color);
}

float getSaturation(float3 color) {
 float n = min(min(color.x, color.y), color.z);
 float x = max(max(color.x, color.y), color.z);
 return x - n;
}

float3 setSaturation(float3 color, float saturation) {
 float values[3] = {color.x, color.y, color.z};
 int3 sortedIndices = {0, 1, 2};
 if (color.x >= color.y && color.x >= color.z) {
  sortedIndices.x = 0;
  if (color.y >= color.z) {
   sortedIndices.y = 1;
   sortedIndices.z = 2;
  } else {
   sortedIndices.y = 2;
   sortedIndices.z = 1;
  }
 } else if (color.y >= color.x && color.y >= color.z) {
  sortedIndices.x = 1;
  if (color.x >= color.z) {
   sortedIndices.y = 0;
   sortedIndices.z = 2;
  } else {
   sortedIndices.y = 2;
   sortedIndices.z = 0;
  }
 } else {
  sortedIndices.x = 2;
  if (color.x >= color.y) {
   sortedIndices.y = 0;
   sortedIndices.z = 1;
  } else {
   sortedIndices.y = 1;
   sortedIndices.z = 0;
  }
 }
 float* cMax = &values[sortedIndices.x];
 float* cMid = &values[sortedIndices.y];
 float* cMin = &values[sortedIndices.z];
 if (cMax > cMin) {
  *cMid = (((*cMid - *cMin) * saturation) / (*cMax - *cMin));
  *cMax = saturation;
 } else {
  *cMid = 0;
  *cMax = 0;
 }
 *cMin = 0;
 float3 result = {
  values[0],
  values[1],
  values[2],
 };
 return result;
}

float3 colorInvert(float3 color) {
 float3 result = {
  255 - color.x,
  255 - color.y,
  255 - color.z,
 };
 return result;
}

float3 colorBlend(float3 backdropColor, float3 sourceColor) {
  //return setLuminance(backdropColor, getLuminance(sourceColor));
  //return setLuminance(setSaturation(sourceColor, getSaturation(backdropColor)), getLuminance(backdropColor));
  float3 result = {
   fabs(backdropColor.z - sourceColor.x),
   fabs(backdropColor.y - sourceColor.y),
   fabs(backdropColor.x - sourceColor.z),
  };
  return result;
}

int2 get_rotated_image_pixel(int gx, int gy, int width, int height, int rotation)
{
 const int2 pos = { gx, gy };
 int2 destPos = {gy, gx};
 if (rotation == 0 || (rotation != 90 && rotation != 180 && rotation != 270)) {
  destPos.x = gx;
  destPos.y = gy;
 } else if (rotation == 90) {
  destPos.x = gy;
  destPos.y = height - 1 - gx;
 } else if (rotation == 180) {
  destPos.x = width - 1 - gx;
  destPos.y = height - 1 - gy;
 } else {
  destPos.x = width - 1 - gy;
  destPos.y = gx;
 }
 return destPos;
}

float4 get_value_at(__global uchar* textureBuffer, int start, int end, int width, int height, int x, int y) {
  const int imageIndex = start + width * y * 4 + x * 4;
  float4 result = {
    textureBuffer[imageIndex],
    textureBuffer[imageIndex + 1],
    textureBuffer[imageIndex + 2],
    textureBuffer[imageIndex + 3],
  };
  return result;
}

float4 sample_bicubic_border(__global uchar* textureBuffer, int start, int end, int width, int height, float2 pos, int2 SrcSize)
{
   int2 isrcpos = convert_int2(pos);
   float dx = pos.x - isrcpos.x;
   float dy = pos.y - isrcpos.y;

   float4 C[4] = {0, 0, 0, 0};

   if (isrcpos.x < 0 || isrcpos.x >= SrcSize.x)
      return 0;

   if (isrcpos.y < 0 || isrcpos.y >= SrcSize.y)
      return 0;

   for (int i = 0; i < 4; i++)
   {
      int y = isrcpos.y - 1 + i;
      if (y < 0)
         y = 0;

      if (y >= SrcSize.y)
         y = SrcSize.y - 1;

      int Middle = clamp(isrcpos.x, 0, SrcSize.x - 1);

      const int2 pos0 = { Middle, y };
      float4 center = get_value_at(textureBuffer, start, end, width, height, pos0.x, pos0.y);

      float4 left = 0, right1 = 0, right2 = 0;
      if (isrcpos.x - 1 >= 0)
      {
         const int2 pos1 = { isrcpos.x - 1, y };
         left = get_value_at(textureBuffer, start, end, width, height, pos1.x, pos1.y);
      }
      else
      {
         left = center;
      }

      if (isrcpos.x + 1 < SrcSize.x)
      {
         const int2 pos2 = { isrcpos.x + 1, y };
         right1 = get_value_at(textureBuffer, start, end, width, height, pos2.x, pos2.y);
      }
      else
      {
         right1 = center;
      }

      if (isrcpos.x + 2 < SrcSize.x)
      {
         const int2 pos3 = { isrcpos.x + 2, y };
         right2 = get_value_at(textureBuffer, start, end, width, height, pos3.x, pos3.y);
      }
      else
      {
         right2 = right1;
      }

      float4 a0 = center;
      float4 d0 = left - a0;
      float4 d2 = right1 - a0;
      float4 d3 = right2 - a0;

      float4 a1 = -1.0f / 3 * d0 + d2 - 1.0f / 6 * d3;
      float4 a2 =  1.0f / 2 * d0 + 1.0f / 2 * d2;
      float4 a3 = -1.0f / 6 * d0 - 1.0f / 2 * d2 + 1.0f / 6 * d3;
      C[i] = a0 + a1 * dx + a2 * dx * dx + a3 * dx * dx * dx;
   }

   float4 d0 = C[0] - C[1];
   float4 d2 = C[2] - C[1];
   float4 d3 = C[3] - C[1];
   float4 a0 = C[1];
   float4 a1 = -1.0f / 3 * d0 + d2 -1.0f / 6 * d3;
   float4 a2 = 1.0f / 2 * d0 + 1.0f / 2 * d2;
   float4 a3 = -1.0f / 6 * d0 - 1.0f / 2 * d2 + 1.0f / 6 * d3;

   return a0 + a1 * dy + a2 * dy * dy + a3 * dy * dy * dy;
}

float4 resize_bicubic(__global uchar* textureBuffer, int start, int end, int width, int height, int gx, int gy, float ratio)
{
   const int2 pos = { gx, gy };

   if (pos.x >= width || pos.y >= height) {
    float4 result = {0, 0, 0, 0};
    return result;
   }

   float2 srcpos = {(pos.x + 0.4995f) / ratio, (pos.y + 0.4995f) / ratio};
   int2 SrcSize = { width, height };

   float4 value;

   srcpos -= (float2)(0.5f, 0.5f);

   int2 isrcpos = convert_int2(srcpos);
   float dx = srcpos.x - isrcpos.x;
   float dy = srcpos.y - isrcpos.y;

   if (isrcpos.x <= 0 || isrcpos.x >= SrcSize.x - 2)
      value = sample_bicubic_border(textureBuffer, start, end, width, height, srcpos, SrcSize);

   if (isrcpos.y <= 0 || isrcpos.y >= SrcSize.y - 2)
      value = sample_bicubic_border(textureBuffer, start, end, width, height, srcpos, SrcSize);

   float4 C[4] = {0, 0, 0, 0};

   for (int i = 0; i < 4; i++)
   {
      const int y = isrcpos.y - 1 + i;

      const int2 pos0 = { isrcpos.x, y };
      const int2 pos1 = { isrcpos.x - 1, y };
      const int2 pos2 = { isrcpos.x + 1, y };
      const int2 pos3 = { isrcpos.x + 2, y };

      float4 a0 = get_value_at(textureBuffer, start, end, width, height, pos0.x, pos0.y);
      float4 d0 = get_value_at(textureBuffer, start, end, width, height, pos1.x, pos1.y) - a0;
      float4 d2 = get_value_at(textureBuffer, start, end, width, height, pos2.x, pos2.y) - a0;
      float4 d3 = get_value_at(textureBuffer, start, end, width, height, pos3.x, pos3.y) - a0;

      float4 a1 = -1.0f / 3 * d0 + d2 - 1.0f / 6 * d3;
      float4 a2 =  1.0f / 2 * d0 + 1.0f / 2 * d2;
      float4 a3 = -1.0f / 6 * d0 - 1.0f / 2 * d2 + 1.0f / 6 * d3;
      C[i] = a0 + a1 * dx + a2 * dx * dx + a3 * dx * dx * dx;
   }

   float4 d0 = C[0] - C[1];
   float4 d2 = C[2] - C[1];
   float4 d3 = C[3] - C[1];
   float4 a0 = C[1];
   float4 a1 = -1.0f / 3 * d0 + d2 -1.0f / 6 * d3;
   float4 a2 = 1.0f / 2 * d0 + 1.0f / 2 * d2;
   float4 a3 = -1.0f / 6 * d0 - 1.0f / 2 * d2 + 1.0f / 6 * d3;
   value = a0 + a1 * dy + a2 * dy * dy + a3 * dy * dy * dy;

   return value;
}

__kernel void render_shapes(__global float* camInfo, __global float* shapesBuffer, int shapesSize, __global float* screenShapesBuffer, int screenShapesSize, __global uchar* textureBuffer, int width, int height, __global uchar* outBuffer)
{
 const int camStartX = convert_int(camInfo[0]);
 const int camStartY = convert_int(camInfo[1]);
 const float camZoom = camInfo[2];
 const int screenX = get_global_id(0);
 const int screenY = get_global_id(1);
 const int x = camStartX + round(screenX / camZoom);
 const int y = camStartY + round(screenY / camZoom);
 const int currentIndex = width * screenY * 3 + screenX * 3;
 const int shapesCount = shapesSize / 14;
 outBuffer[currentIndex] = 255;
 outBuffer[currentIndex + 1] = 255;
 outBuffer[currentIndex + 2] = 255;
 for(int i = 0; i < shapesSize; i += 14) {
  float shapeType = shapesBuffer[i];
  float shapeStartX = shapesBuffer[i + 1];
  float shapeStartY = shapesBuffer[i + 2];
  float shapeEndX = shapesBuffer[i + 3];
  float shapeEndY = shapesBuffer[i + 4];
  float rotation = shapesBuffer[i + 5];
  float imageWidth = shapesBuffer[i + 6];
  float imageHeight = shapesBuffer[i + 7];
  float imageStart = shapesBuffer[i + 8];
  float imageEnd = shapesBuffer[i + 9];
  float colorR = shapesBuffer[i + 10];
  float colorG = shapesBuffer[i + 11];
  float colorB = shapesBuffer[i + 12];
  float colorA = shapesBuffer[i + 13];
  if (x > shapeStartX && x < shapeEndX && y > shapeStartY && y < shapeEndY) {
   if (shapeType == 0) {
    const int2 rotatedImagePos = get_rotated_image_pixel(x - shapeStartX, y - shapeStartY, imageWidth, imageHeight, 360 - rotation);
    const int imageX = rotatedImagePos.x;
    const int imageY = rotatedImagePos.y;
    const int imageIndex = imageStart + imageWidth * imageY * 4 + imageX * 4;
    // const float4 scaledImageValue = resize_bicubic(textureBuffer, imageStart, imageEnd, imageWidth, imageHeight, imageX, imageY, camZoom);
    // float r = scaledImageValue.x;
    // float g = scaledImageValue.y;
    // float b = scaledImageValue.z;
    // float a = scaledImageValue.w;
    float r = textureBuffer[imageIndex];
    float g = textureBuffer[imageIndex + 1];
    float b = textureBuffer[imageIndex + 2];
    float a = textureBuffer[imageIndex + 3];
    outBuffer[currentIndex] = (1 - a / 255) * outBuffer[currentIndex] + (a / 255) * r;
    outBuffer[currentIndex + 1] = (1 - a / 255) * outBuffer[currentIndex + 1] + (a / 255) * g;
    outBuffer[currentIndex + 2] = (1 - a / 255) * outBuffer[currentIndex + 2] + (a / 255) * b;
   }
  }
 }
for(int i = 0; i < screenShapesSize; i += 14) {
  float shapeType = screenShapesBuffer[i];
  float shapeStartX = screenShapesBuffer[i + 1];
  float shapeStartY = screenShapesBuffer[i + 2];
  float shapeEndX = screenShapesBuffer[i + 3];
  float shapeEndY = screenShapesBuffer[i + 4];
  float rotation = screenShapesBuffer[i + 5];
  float imageWidth = screenShapesBuffer[i + 6];
  float imageHeight = screenShapesBuffer[i + 7];
  float imageStart = screenShapesBuffer[i + 8];
  float imageEnd = screenShapesBuffer[i + 9];
  float colorR = screenShapesBuffer[i + 10];
  float colorG = screenShapesBuffer[i + 11];
  float colorB = screenShapesBuffer[i + 12];
  float colorA = screenShapesBuffer[i + 13];
  if (screenX > shapeStartX && screenX < shapeEndX && screenY > shapeStartY && screenY < shapeEndY) {
   if (shapeType == 0) {
    const int2 rotatedImagePos = get_rotated_image_pixel(screenX - shapeStartX, screenY - shapeStartY, imageWidth, imageHeight, 360 - rotation);
    const int imageX = rotatedImagePos.x;
    const int imageY = rotatedImagePos.y;
    const int imageIndex = imageStart + imageWidth * imageY * 4 + imageX * 4;
    // const float4 scaledImageValue = resize_bicubic(textureBuffer, imageStart, imageEnd, imageWidth, imageHeight, imageX, imageY, camZoom);
    // float r = scaledImageValue.x;
    // float g = scaledImageValue.y;
    // float b = scaledImageValue.z;
    // float a = scaledImageValue.w;
    float r = textureBuffer[imageIndex];
    float g = textureBuffer[imageIndex + 1];
    float b = textureBuffer[imageIndex + 2];
    float3 color1 = {r, g, b};
    float3 color2 = {255, 255, 0};
    float3 colorResult = colorBlend(color2, color1);
    r = colorResult.x;
    g = colorResult.y;
    b = colorResult.z;
    float a = textureBuffer[imageIndex + 3];
    outBuffer[currentIndex] = (1 - a / 255) * outBuffer[currentIndex] + (a / 255) * r;
    outBuffer[currentIndex + 1] = (1 - a / 255) * outBuffer[currentIndex + 1] + (a / 255) * g;
    outBuffer[currentIndex + 2] = (1 - a / 255) * outBuffer[currentIndex + 2] + (a / 255) * b;
   }
  }
 }
}

)EOL";
