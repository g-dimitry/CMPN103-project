#include <regex>
#include <iostream>
using namespace std;

#include "utils/printable/printable.h"
#include "utils/list/list.h"

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

class Position : Printable {
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

class Dimensions : Printable {
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

class Color : Printable {
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

class DrawableStyles : Printable {
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

class Drawable : Printable {
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
  regex_replace(back_inserter(result), objString.begin(), objString.end(), reg, "\t");
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
 a.forEach([](int element) {
  cout << "ana element" << element;
  });
 delete b;
}

template<typename Type>
class Observable {
private:
 Type value;
 Array<Observer<Type>> observers = Array<Observer<Type>>();
public:
 Observable(Type initialValue) {
  this->value = initialValue;
 }
 void subscribe(Observer<Type>* observer) {
  this->observers.push(observer);
 }
 void unsubscribe(Observer<Type>* observer) {
  this->observers.filter([](Observer<Type> element) {
   return element != observer;
   });
 }
 void setValue(Type value) {
  this->value = value;
  observers.forEach([](Observer<Type> observer) {
   observer.onChange(this->value);
   });
 }
};

template<typename Type>
class Observer {
private:
 Observable<Type> observable;
public:
 Observer(Observable<Type> observable) {
  this->observable = observable;
 }
 ~Observer() {
  observable.unsubscribe(this);
 }
 void onChange(Type value) {
 }
};

template<typename State, typename Props>
class ViewComponent {
private:
 bool isDirty;
 State state;
 Props props;
 void setState() {};
 Array<ViewComponent> children;
public:
 ViewComponent(Props props) {};
 void addChild(ViewComponent child) {};
 void render() {};
};

int main() {
 //start();
}