#pragma once
#include "./image.h"
#include "../CMUgraphicsLib/png/LoadPNG.h"
#include "..\CMUgraphicsLib\CMUgraphics.h"
using namespace std;
void ReadPNG(vector<unsigned char>& Image, unsigned int& width, unsigned int& height, const char* filename);
void WritePNG(window* pWind, image& screen, int x, int y, vector<unsigned char>& Image, unsigned int width, unsigned int height, int newWidth, int newHeight);
void DrawPNGImage(window* pWind, string r_filename, int x, int y, int newWidth, int newHeight);
