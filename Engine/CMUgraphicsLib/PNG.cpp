#include "../CMUgraphicsLib/PNG.h"
#include <iostream>
#include "../ExtendedImage.h"

void ReadPNG(vector<unsigned char>& Image, unsigned int& width, unsigned int& height, const char* filename)
{
 //decode
 unsigned error = lodepng::decode(Image, width, height, filename);
 //if there's an error, display it
 if (error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;

 //the pixels are now in the vector "image", 4 bytes per pixel, ordered RGBARGBA..., use it as texture, draw it, ...
}

void ResizeImage(vector<unsigned char>& inputImage, vector<unsigned char>& newImage, unsigned int width, unsigned int height, int newWidth, int newHeight) {
 int newSize = newWidth * newHeight;
 float horizontalStep = float(newWidth) / width;
 int horizontalTotalSteps = ceil(horizontalStep);
 int horizontalWholeStep = int(horizontalStep);
 float horizontalFractionalStep = horizontalStep - horizontalWholeStep;
 float verticalStep = float(newHeight) / height;
 int verticalTotalSteps = ceil(verticalStep);
 int verticalWholeStep = int(verticalStep);
 float verticalFractionalStep = verticalStep - verticalWholeStep;
 newImage = vector<unsigned char>(newSize * 4);
 for (long long l = 0; l < newSize; l++)
 {
  int row = floor(l / newWidth);
  int column = l % newWidth;

  float horizontalStart = row * horizontalStep;
  float horizontalEnd = (row + 1) * horizontalStep;
  float verticalStart = column * verticalStep;
  float verticalEnd = (column + 1) * verticalStep;

  int horizontalStartPixel = floor(horizontalStart);
  int horizontalEndPixel = ceil(horizontalEnd);
  int verticalStartPixel = floor(verticalStart);
  int verticalEndPixel = ceil(verticalEnd);

  float horizontalStartEdgeFraction = 1 - (horizontalStart - horizontalStartPixel);
  float horizontalEndEdgeFraction = 1 - (horizontalEndPixel - horizontalEnd);
  float verticalStartEdgeFraction = 1 - (verticalStart - verticalStartPixel);
  float verticalEndEdgeFraction = 1 - (verticalEndPixel - verticalEnd);

  float averageTotal[] = { 0,0,0,0 };
  for (int j = verticalStartPixel; j <= verticalEndPixel; j++) {
   for (int i = horizontalStartPixel; i <= horizontalEndPixel; i++) {
	float fraction = 1;
	if (i == horizontalStartPixel && j == verticalStartPixel) { // top left
	 fraction = 1 - ((1 - horizontalStartEdgeFraction) * (1 - verticalStartEdgeFraction));
	}
	else if (i == horizontalEndPixel && j == verticalStartPixel) {
	 fraction = 1 - ((1 - horizontalEndEdgeFraction) * (1 - verticalStartEdgeFraction));
	} // top right
	else if (i == horizontalStartPixel && j == verticalEndPixel) {
	 fraction = 1 - ((1 - horizontalStartEdgeFraction) * (1 - verticalEndEdgeFraction));
	} // bottom left
	else if (i == horizontalEndPixel && j == verticalEndPixel) {
	 fraction = 1 - ((1 - horizontalEndEdgeFraction) * (1 - verticalEndEdgeFraction));
	} // bottom right
	else if (i == horizontalStartPixel) {
	 fraction = horizontalStartEdgeFraction;
	} // left
	else if (i == horizontalEndPixel) {
	 fraction = horizontalEndEdgeFraction;
	} // right
	else if (j == verticalStartPixel) {
	 fraction = verticalStartEdgeFraction;
	} // top
	else if (j == verticalEndPixel) {
	 fraction = verticalEndEdgeFraction;
	} // bottom
	for (int k = 0; k < 4; k++) {
	 int index = 4 * i + k + j * width * 4;
	 if (index >= width * height * 4) {
	  cout << "batata";
	 }
	 unsigned char value = inputImage[index];
	 averageTotal[k] += value * fraction;
	}
   }
  }
  for (int i = 0; i < 4; i++) {
   averageTotal[i] = round(averageTotal[i] / ((horizontalEndPixel - horizontalStartPixel + 1) * (verticalEndPixel - verticalStartPixel + 1)));
   newImage[4 * row + i + column * newWidth * 4] = averageTotal[i];
  }
 }
 lodepng::save_file(newImage, "./test.png");
 cout << "hena";
};

class Color {
private:
 int red = 0;
 int green = 0;
 int blue = 0;
 float alpha = 1;
public:
 Color(int red = 0, int green = 0, int blue = 0, float alpha = 1) {
  this->setValue(red, green, blue, alpha);
 }
 void setRed(int red) {
  this->red = red;
 }
 int getRed() {
  return this->red;
 }
 void setGreen(int green) {
  this->green = green;
 }
 int getGreen() {
  return this->green;
 }
 void setBlue(int blue) {
  this->blue = blue;
 }
 int getBlue() {
  return this->blue;
 }
 void setAlpha(float alpha) {
  this->alpha = alpha;
 }
 float getAlpha() {
  return this->alpha;
 }
 void setValue(int red, int green, int blue, float alpha) {
  this->red = red;
  this->green = green;
  this->blue = blue;
  this->alpha = alpha;
 }
 Color overlayOn(Color color) {
  float r = (1 - this->alpha) * color.getRed() + this->alpha * this->getRed();
  float g = (1 - this->alpha) * color.getGreen() + this->alpha * this->getGreen();
  float b = (1 - this->alpha) * color.getBlue() + this->alpha * this->getBlue();
  return Color((int)r, (int)g, (int)b);
 }
 string toString() {
  return '(' + to_string(this->red) + ',' + to_string(this->green) + ',' + to_string(this->blue) + ',' + to_string(this->alpha) + ')';
 }
};

int* resizeBilinear(int* pixels, int w1, int h1, int w2, int h2)
{
 int* retval = new int[w2 * h2];
 int a, b, c, d, x, y, index;
 float x_ratio = ((float)(w1 - 1)) / w2;
 float y_ratio = ((float)(h1 - 1)) / h2;
 float x_diff, y_diff, blue, red, green;
 int offset = 0;
 for (int i = 0; i < h2; i++) {
  for (int j = 0; j < w2; j++) {
   x = (int)(x_ratio * j);
   y = (int)(y_ratio * i);
   x_diff = (x_ratio * j) - x;
   y_diff = (y_ratio * i) - y;
   index = (y * w1 + x);
   a = pixels[index];
   b = pixels[index + 1];
   c = pixels[index + w1];
   d = pixels[index + w1 + 1];

   // blue element
   // Yb = Ab(1-w1)(1-h1) + Bb(w1)(1-h1) + Cb(h1)(1-w1) + Db(wh)
   blue = (a & 0xff) * (1 - x_diff) * (1 - y_diff) + (b & 0xff) * (x_diff) * (1 - y_diff) +
	(c & 0xff) * (y_diff) * (1 - x_diff) + (d & 0xff) * (x_diff * y_diff);

   // green element
   // Yg = Ag(1-w1)(1-h1) + Bg(w1)(1-h1) + Cg(h1)(1-w1) + Dg(wh)
   green = ((a >> 8) & 0xff) * (1 - x_diff) * (1 - y_diff) + ((b >> 8) & 0xff) * (x_diff) * (1 - y_diff) +
	((c >> 8) & 0xff) * (y_diff) * (1 - x_diff) + ((d >> 8) & 0xff) * (x_diff * y_diff);

   // red element
   // Yr = Ar(1-w1)(1-h1) + Br(w1)(1-h1) + Cr(h1)(1-w1) + Dr(wh)
   red = ((a >> 16) & 0xff) * (1 - x_diff) * (1 - y_diff) + ((b >> 16) & 0xff) * (x_diff) * (1 - y_diff) +
	((c >> 16) & 0xff) * (y_diff) * (1 - x_diff) + ((d >> 16) & 0xff) * (x_diff * y_diff);

   retval[offset++] =
	0xff000000 | // hardcoded alpha
	((((int)red) << 16) & 0xff0000) |
	((((int)green) << 8) & 0xff00) |
	((int)blue);
  }
 }
 return retval;
}

void BicubicResizeImage(vector<unsigned char>& inputImage, vector<unsigned char>& newImage, unsigned int width, unsigned int height, int newWidth, int newHeight) {
 int* r = new int[width * height];
 int* g = new int[width * height];
 int* b = new int[width * height];
 int* a = new int[width * height];
 for (int i = 0; i < inputImage.size(); i += 4) {
  int c = int(i / 4);
  r[c] = inputImage[i];
  g[c] = inputImage[i + 1];
  b[c] = inputImage[i + 2];
  a[c] = inputImage[i + 3];
 }
 r = resizeBilinear(r, width, height, newWidth, newHeight);
 g = resizeBilinear(g, width, height, newWidth, newHeight);
 b = resizeBilinear(b, width, height, newWidth, newHeight);
 a = resizeBilinear(a, width, height, newWidth, newHeight);
 newImage = vector<unsigned char>(newWidth * newHeight * 4);
 for (int i = 0; i < newWidth * newHeight; i++) {
  newImage.push_back(r[i]);
  newImage.push_back(g[i]);
  newImage.push_back(b[i]);
  newImage.push_back(a[i]);
 }
 cout << "ana hena";
}
void WritePNG(window* pWind, image& screen, int x, int y, vector<unsigned char>& Image, unsigned int width, unsigned int height, int newWidth, int newHeight)
{
 pWind->StoreImage(screen, x, y, width, width);
 unsigned char* Arr = screen.GetArr();
 int Size = Image.size();
 long long i;
 for (i = 0; i < Size; i += 4)
 {
  if (Image[i + 3])
  {
   unsigned char r = Image[i];
   unsigned char g = Image[i + 1];
   unsigned char b = Image[i + 2];
   unsigned char a = Image[i + 3];

   Arr[i] = r;
   Arr[i + 1] = g;
   Arr[i + 2] = b;
   /*int l = int(i / 4);
   int row = y + floor(l / width);
   int column = x + l % width;
   color col = pWind->GetColor(row, column);
   Color c1 = Color(col.ucRed, col.ucGreen, col.ucBlue);
   Color c2 = Color(r, g, b, a / float(255));
   Color result = c2.overlayOn(c1);

   Arr[i] = result.getRed();
   Arr[i + 1] = result.getGreen();
   Arr[i + 2] = result.getBlue();*/
  }
 }
 screen.SetArr(Arr);
}

void DrawPNGImage(window* pWind, string r_filename, int x, int y, int newWidth, int newHeight)
{
 const char* filename = r_filename.c_str();
 vector<unsigned char> oldImageVector;
 vector<unsigned char> newImageVector(newWidth * newHeight * 4);
 vector<unsigned char> newerImageVector(newWidth * newHeight * 4);
 unsigned int width, height;
 image screen = image();
 ReadPNG(oldImageVector, width, height, filename);
 ExtendedImage oldImage = ExtendedImage(&oldImageVector, width, height);
 ExtendedImage newImage = ExtendedImage(&newImageVector, newWidth, newHeight);
 int rotation = 180;
 ExtendedImage newerImage = ExtendedImage(&newerImageVector, (rotation % 180 == 0 ? newWidth : newHeight), (rotation % 180 == 0 ? newHeight : newWidth));
 ExtendedImage::resize(&oldImage, &newImage);
 ExtendedImage::rotate(&newImage, &newerImage, rotation);
 //BicubicResizeImage(Image, newImage, width, height, newWidth, newHeight);
 WritePNG(pWind, *(&screen), x, y, *newerImage.getData(), newWidth, newHeight, newWidth, newHeight);
 pWind->DrawImage(screen, x, y);
}