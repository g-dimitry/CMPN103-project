#include <stdio.h>
#include <string>
#include <iostream>
#include <regex>
#include <functional>
#include <cstdio>
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
using namespace std;
using namespace std::placeholders;

#ifdef _DEBUG
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
// allocations to be of _CLIENT_BLOCK type
#else
#define DBG_NEW new
#endif

enum AnchorPosition {
 TOP_LEFT,
 TOP_CENTER,
 TOP_RIGHT,
 MIDDLE_LEFT,
 MIDDLE_CENTER,
 MIDDLE_RIGHT,
 BOTTOM_LEFT,
 BOTTOM_CENTER,
 BOTTOM_RIGHT,
};

class Printable {
public:
 virtual string toString() { return ""; };
};

class Position: Printable {
private:
 int verticalOffset = 0;
 int horizontalOffset = 0;
public:
 Position(int verticalOffset = 0, int horizontalOffset = 0) {
  this->verticalOffset = verticalOffset;
  this->horizontalOffset = horizontalOffset;
 }
 void setVerticalOffset(int verticalOffset) {
  this->verticalOffset = verticalOffset;
 }
 int getVerticalOffset() {
  return this->verticalOffset;
 }
 void setHorizontalOffset(int horizontalOffset) {
  this->horizontalOffset = horizontalOffset;
 }
 int getHorizontalOffset() {
  return this->horizontalOffset;
 }
 void setValue(int verticalOffset, int horizontalOffset) {
  this->verticalOffset = verticalOffset;
  this->horizontalOffset = horizontalOffset;
 }
 string toString() {
  return '(' + to_string(this->horizontalOffset) + ',' + to_string(this->verticalOffset) + ')';
 }
};

class Dimensions: Printable {
private:
 int height = 0;
 int width = 0;
public:
 Dimensions(int height = 0, int width = 0) {
  this->height = height;
  this->width = width;
 }
 void setheight(int height) {
  this->height = height;
 }
 int getheight() {
  return this->height;
 }
 void setwidth(int width) {
  this->width = width;
 }
 int getwidth() {
  return this->width;
 }
 void setValue(int height, int width) {
  this->height = height;
  this->width = width;
 }
 string toString() {
  return '(' + to_string(this->width) + ',' + to_string(this->height) + ')';
 }
};

class Color: Printable {
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

class DrawableStyles: Printable {
private:
 Position position = Position();
 int zIndex = 0;
 Dimensions dimensions = Dimensions();
 string imagePath = "";
 Color backgroundColor = Color();
public:
 DrawableStyles(
  Position position = Position(),
  int zIndex = 0,
  Dimensions dimensions = Dimensions(),
  string imagePath = "",
  Color backgroundColor = Color()
 ) {
  this->setPosition(position);
  this->setZIndex(zIndex);
  this->setDimensions(dimensions);
  this->setImagePath(imagePath);
  this->setBackgroundColor(backgroundColor);
 }
 DrawableStyles setPosition(Position position) {
  this->position = position;
  return *this;
 }
 Position getPosition() {
  return this->position;
 }
 DrawableStyles setZIndex(int zIndex) {
  this->zIndex = zIndex;
  return *this;
 }
 Position getZIndex() {
  return this->zIndex;
 }
 DrawableStyles setDimensions(Dimensions dimensions) {
  this->dimensions = dimensions;
  return *this;
 }
 Dimensions getDimensions() {
  return this->dimensions;
 }
 DrawableStyles setImagePath(string imagePath) {
  this->imagePath = imagePath;
  return *this;
 }
 string getImagePath() {
  return this->imagePath;
 }
 DrawableStyles setBackgroundColor(Color backgroundColor) {
  this->backgroundColor = backgroundColor;
  return *this;
 }
 Color getBackgroundColor() {
  return this->backgroundColor;
 }
 string toString() {
  string result = "{\n";
  result += "\tposition: " + this->position.toString() + ",\n";
  result += "\tzIndex: " + to_string(this->zIndex) + ",\n";
  result += "\tdimensions: " + this->dimensions.toString() + ",\n";
  result += "\timagePath: " + this->imagePath + ",\n";
  result += "\tbackgroundColor: " + this->backgroundColor.toString() + ",\n";
  result += "}";
  return result;
 }
};

class Drawable: Printable {
private:
 DrawableStyles styles = DrawableStyles();
public:
 Drawable(DrawableStyles styles = DrawableStyles()) {
  setStyles(styles);
 }
 void setStyles(DrawableStyles styles) {
  this->styles = styles;
 }
 DrawableStyles getStyles() {
  return this->styles;
 }
 string indentNestedObject(string objString) {
  regex reg("^");
  string result;
  regex_replace(std::back_inserter(result), objString.begin(), objString.end(), reg, "\t");
  return result;
 }
 string toString() {
  string result = "{\n";
  result += "\tstyles: " + indentNestedObject(this->styles.toString()) + ",\n";
  result += "}\n";
  return result;
 }
 virtual void draw() {};
};

template<typename Type>
class Array: Printable {
private:
 Type* data;
 int count;
 string(*f)(Type element);
 string reduceToString(string accumulator, Type currentValue, int currentIndex, Type* srcArray) {
  return accumulator + "\t" + this->f(currentValue) + ",\n";
 };
public:
 ~Array() {
  delete[] this->data;
 }
 Array<Type>(string(*f)(Type element)) {
  this->data = new Type;
  this->count = 0;
  this->f = f;
 }
 Array<Type>(Type* data, int count, string(*f)(Type element)) {
  this->data = data; 
  this->count = count;
  this->f = f;
 }
 Type* getData() {
  return this->data;
 }
 int getCount() {
  return this->count;
 }
 void copy(Type* dest) {
  for (int i = 0; i < count; i++) {
   dest[i] = this->data[i];
  }
 }
 Array<Type> clone() {
  Type* outArr = new Type[count];
  this->copy(outArr);
  return Array<Type>(outArr, count);
 }
 void push(Type element)
 {
  int newCount = count + 1;
  Type* outArr = new Type[newCount];
  this->copy(outArr);
  outArr[count] = element;
  delete[] this->data;
  this->data = outArr;
  this->count++;
 }
 void filter(bool (*f)(Type))
 {
  Array<Type>* out = new Array<Type>(this->f);
  for (int i = 0; i < count; i++) {
   if (f(this->data[i])) {
	out->push(this->data[i]);
   }
  }
  delete[] this->data;
  this->count = out->count;
  this->data = new Type[out->getCount()];
  out->copy(this->data);
  delete out;
 }
 void forEach(void (*f)(Type)) {
  for (int i = 0; i < this->count; i++) {
   f(this->data[i]);
  }
 }
 template<typename MapType>
 Array<MapType>* map(MapType (*f)(Type), string(*f2)(MapType element)) {
  MapType* outArr = new MapType[this->count];
  Array<MapType>* out = new Array<MapType>(outArr, this->count, f2);
  for (int i = 0; i < this->count; i++) {
   out->getData()[i] = f(this->data[i]);
  }
  return out;
 }
 template<typename ReduceType>
 ReduceType reduce(ReduceType(Array<Type>::*g)(ReduceType, Type, int, Type*),Array<Type>& a , ReduceType initValue) {
  ReduceType value = initValue;
  for (int i = 0; i < this->count; i++) {
   value = (a.*g)(value, this->data[i], i, this->data);
  }
  return value;
 }
 template<typename ReduceType>
 ReduceType reduce(ReduceType (*f)(ReduceType accumulator, Type currentValue, int currentIndex, Type* srcArray), ReduceType initValue) {
  ReduceType value = initValue;
  for (int i = 0; i < this->count; i++) {
   value = f(value, this->data[i], i, this->data);
  }
  return value;
 }
 string toString() {
  return this->reduce<string>(&Array<Type>::reduceToString, *this, "[\n") + "]";
 }
};

void start() {
 Drawable drawable = Drawable();
 cout << drawable.toString();
 Color c1 = Color(255, 0, 0, 0.5);
 Color c2 = Color(255, 255, 255, 1);
 Color c3 = c1.overlayOn(c2);
 cout << c3.toString();
 Array<int> a = Array<int>(to_string);
 a.push(1);
 a.push(2);
 a.push(3);
 a.push(4);
 a.filter([](int element) {
  return element % 2 == 0;
  });
 Array<string>* b = a.map<string>([](int element) -> string {
  return "abc: " + to_string(element);
  }, [](string a) -> string {return a; });
 cout << a.toString();
 cout << b->toString();
 cout << a.reduce<int>([](int accumulator, int currentValue, int currentIndex, int* srcArray) {
  return accumulator + currentValue;
  }, 0);
 delete b;
}

int main() {
 start();
 _CrtDumpMemoryLeaks();
}
