#include <iostream>
#include <sstream>
#include <cmath>
#include <windows.h>
#include <regex>

#include "./Utilities/ExtendedImage/ExtendedImage.h"
#include "./OCL/OCL.h"
#include "./Assets/Assets.h";

#include "./Utilities/List/List.h"
#include "CMUgraphicsLib/CMUgraphics.h"

window* wind;

int gridUnitSize = 32;
int gridSize = 240 * gridUnitSize;
int initialCameraWidth = gridUnitSize * 40;

void GetDesktopResolution(int& horizontal, int& vertical)
{
 RECT desktop;
 const HWND hDesktop = GetDesktopWindow();
 GetWindowRect(hDesktop, &desktop);
 horizontal = desktop.right;
 vertical = desktop.bottom;
}

float getVW() {
 int horizontal, vertical;
 GetDesktopResolution(horizontal, vertical);
 return float(horizontal) / 100;
}

float getVH() {
 int horizontal, vertical;
 GetDesktopResolution(horizontal, vertical);
 return float(vertical) / 100;
}

class Vector2D {
private:
 float x = 0;
 float y = 0;
public:
 Vector2D(float x = 0, float y = 0) {
  this->x = x;
  this->y = y;
 }
 Vector2D operator+(Vector2D v) {
  return this->add(v);
 }
 Vector2D operator*(float v) {
  return Vector2D(this->x * v, this->y * v);
 }
 Vector2D operator-(Vector2D v) {
  return this->add(v * -1);
 }
 Vector2D operator/(float v) {
  return *this * 1 / v;
 }
 void setX(float x) {
  this->x = x;
 }
 float getX() {
  return this->x;
 }
 void setY(float y) {
  this->y = y;
 }
 float getY() {
  return this->y;
 }
 static Vector2D add(Vector2D v1, Vector2D v2) {
  return Vector2D(v1.getX() + v2.getX(), v1.getY() + v2.getY());
 }
 static float scalarProduct(Vector2D v1, Vector2D v2) {
  return v1.getX() * v2.getX() + v1.getY() * v2.getY();
 }
 Vector2D add(Vector2D v) {
  return Vector2D(this->getX() + v.getX(), this->getY() + v.getY());
 }
 float scalarProduct(Vector2D v) {
  return this->getX() * v.getX() + this->getY() * v.getY();
 }
 float magnitude() {
  return sqrtf(Vector2D::scalarProduct(*this, *this));
 }
 Vector2D normalize() {
  return Vector2D(this->getX() / this->magnitude(), this->getY() / this->magnitude());
 }
 string toString() {
  return "(" + to_string(this->x) + "," + to_string(this->y) + ")";
 }
};

Vector2D rotatePoint(Vector2D p, float angle, Vector2D point)
{
 float pi = 2 * acos(0.0);

 float cx = point.getX();
 float cy = point.getY();

 float s = sin(angle * pi / 180);
 float c = cos(angle * pi / 180);

 // translate point back to origin:
 p.setX(p.getX() - cx);
 p.setY(p.getY() - cy);

 // rotate point
 float xnew = p.getX() * c - p.getY() * s;
 float ynew = p.getX() * s + p.getY() * c;

 // translate point back:
 p.setX(round(xnew + cx));
 p.setY(round(ynew + cy));
 return p;
}

void rotateRect(Vector2D start, Vector2D end, int rotation, Vector2D origin, Vector2D& startOut, Vector2D& endOut) {
 Vector2D rotatedEnd = rotatePoint(end, rotation, origin);
 int startX = min(start.getX(), rotatedEnd.getX());
 int endX = max(start.getX(), rotatedEnd.getX());
 int startY = min(start.getY(), rotatedEnd.getY());
 int endY = max(start.getY(), rotatedEnd.getY());
 startOut = Vector2D(startX, startY);
 endOut = Vector2D(endX, endY);
}

class GameObject;

class Shape {
private:
 GameObject* parent;
 Vector2D shapeStart;
 Vector2D shapeEnd;
 Vector2D colliderStart;
 Vector2D colliderEnd;
public:
 Shape(GameObject* parent = nullptr);
 GameObject* getParent();
 void setShapeStart(Vector2D shapeStart);
 void setShapeEnd(Vector2D shapeEnd);
 void setColliderStart(Vector2D colliderStart);
 void setColliderEnd(Vector2D colliderEnd);
 Vector2D getShapeStart();
 Vector2D getShapeEnd();
 Vector2D getColliderStart();
 Vector2D getColliderEnd();
 Vector2D getAbsoluteShapeStart();
 Vector2D getAbsoluteShapeEnd();
 Vector2D getAbsoluteColliderStart();
 Vector2D getAbsoluteColliderEnd();
 virtual string toString();
 virtual void toMatrixForm(vector<float>* v);
};

class GameObject {
private:
 int id;
 Vector2D position = Vector2D(0, 0);
 int zIndex = 0;
 int rotation = 0;
 GameObject* parent;
 Array<Shape*>* shapes = new Array<Shape*>([](Shape* shape) { return shape->toString(); });
 Array<GameObject*>* children = new Array<GameObject*>([](GameObject* gameObject) { return gameObject->toString(); });
 static int gameObjectCount;
 string name;
public:
 static int getGameObjectId();
 GameObject(string name, GameObject* parent = nullptr);
 GameObject* getParent();
 string getName();
 Vector2D getAbsolutePosition();
 Vector2D getScreenPos();
 int getRotation();
 void setRotation(int rotation);
 Vector2D getPosition();
 void setPosition(Vector2D position);
 void setChildren(Array<GameObject*>* children);
 Array<GameObject*>* getChildren();
 void setShapes(Array<Shape*>* shapes);
 Array<Shape*>* getShapes();
 void addChild(GameObject* child);
 virtual void onCreate();
 void update();
 virtual void onDestroy();
 virtual void onPress();
 virtual void onHoverStart();
 virtual void onHoverEnd();
 virtual void onHover();
 virtual void onDragStart();
 virtual void onDrag();
 virtual void onDragEnd();
 string toString();
 void shapesMatrix(vector<float>* v) {
  auto func = [&](Shape* shape) {
   vector<float> v2;
   shape->toMatrixForm(&v2);
   v->insert(v->end(), v2.begin(), v2.end());
  };
  this->shapes->forEach(func);
  this->children->forEach([&](GameObject* child) {
   vector<float> v2;
   child->shapesMatrix(&v2);
   v->insert(v->end(), v2.begin(), v2.end());
   });
 }
};

Shape::Shape(GameObject* parent) {
 this->parent = parent;
}
string Shape::toString() {
 string result = "{\n";
 result += "\tshapeStart: " + this->shapeStart.toString() + ",\n";
 result += "\tshapeEnd: " + this->shapeEnd.toString() + ",\n";
 result += "\tcolliderStart: " + this->colliderStart.toString() + ",\n";
 result += "\tcolliderEnd: " + this->colliderEnd.toString() + ",\n";
 result += "}";
 return result;
}
string indentNestedObject(string objString) {
 regex reg("^");
 string result;
 regex_replace(back_inserter(result), objString.begin(), objString.end(), reg, "\t");
 return result;
}
string GameObject::toString() {
 string result = "{\n";
 result += "\tid: " + to_string(this->id) + ",\n";
 result += "\tname: " + this->name + ",\n";
 result += "\tposition: " + this->position.toString() + ",\n";
 result += "\tzIndex: " + to_string(this->zIndex) + ",\n";
 result += "\trotation: " + to_string(this->rotation) + ",\n";
 result += "\tshapes: " + indentNestedObject(this->shapes->toString()) + ",\n";
 result += "\tchildren: " + indentNestedObject(this->children->toString()) + ",\n";
 result += "}";
 return result;
}
GameObject* Shape::getParent() {
 return this->parent;
}

void Shape::setShapeStart(Vector2D shapeStart) {
 this->shapeStart = shapeStart;
};
void Shape::setShapeEnd(Vector2D shapeEnd) {
 this->shapeEnd = shapeEnd;
};
void Shape::setColliderStart(Vector2D colliderStart) {
 this->colliderStart = colliderStart;
};
void Shape::setColliderEnd(Vector2D colliderEnd) {
 this->colliderEnd = colliderEnd;
};
Vector2D Shape::getShapeStart() {
 return this->shapeStart;
};
Vector2D Shape::getShapeEnd() {
 return this->shapeEnd;
};
Vector2D Shape::getColliderStart() {
 return this->colliderStart;
};
Vector2D Shape::getColliderEnd() {
 return this->colliderEnd;
};

Vector2D Shape::getAbsoluteShapeStart() {
 Vector2D start, end;
 rotateRect(
  this->shapeStart + this->parent->getAbsolutePosition(),
  this->shapeEnd + this->parent->getAbsolutePosition(),
  this->parent->getRotation(),
  this->parent->getAbsolutePosition(),
  start,
  end);
 return start;
}
Vector2D Shape::getAbsoluteShapeEnd() {
 Vector2D start, end;
 rotateRect(
  this->shapeStart + this->parent->getAbsolutePosition(),
  this->shapeEnd + this->parent->getAbsolutePosition(),
  this->parent->getRotation(),
  this->parent->getAbsolutePosition(),
  start,
  end);
 return end;
}
Vector2D Shape::getAbsoluteColliderStart() {
 return this->colliderStart + this->parent->getAbsolutePosition();
}
Vector2D Shape::getAbsoluteColliderEnd() {
 return this->colliderEnd + this->parent->getAbsolutePosition();
}

void Shape::toMatrixForm(vector<float>* v) {}

class ImageShape : public Shape {
private:
 IMAGES image;
public:
 ImageShape(GameObject* parent, IMAGES image) : Shape(parent) {
  this->image = image;
 }
 string toString() override {
  string result = "{\n";
  result += "\tshapeStart: " + this->getShapeStart().toString() + ",\n";
  result += "\tshapeEnd: " + this->getShapeEnd().toString() + ",\n";
  result += "\tcolliderStart: " + this->getColliderStart().toString() + ",\n";
  result += "\tcolliderEnd: " + this->getColliderEnd().toString() + ",\n";
  result += "}";
  return result;
 }
 void toMatrixForm(vector<float>* v) override {
  *v = vector<float>({
   0,
   this->getAbsoluteShapeStart().getX(),
   this->getAbsoluteShapeStart().getY(),
   this->getAbsoluteShapeEnd().getX(),
   this->getAbsoluteShapeEnd().getY(),
   float(this->getParent()->getRotation()),
   float(Assets::getImageWidth(this->image)),
   float(Assets::getImageHeight(this->image)),
   float(Assets::getImageStart(this->image)),
   float(Assets::getImageEnd(this->image)),
   255,
   255,
   255,
   255,
   });
 }
};

int GameObject::getGameObjectId() {
 GameObject::gameObjectCount++;
 return GameObject::gameObjectCount;
}
GameObject::GameObject(string name, GameObject* parent) {
 this->id = GameObject::getGameObjectId();
 this->parent = parent;
 this->name = name;
}
GameObject* GameObject::getParent() {
 return this->parent;
}
string GameObject::getName() {
 return this->name;
}
Vector2D GameObject::getAbsolutePosition() {
 if (this->parent) {
  return this->parent->getAbsolutePosition() + this->position;
 }
 return this->position;
}
Vector2D GameObject::getScreenPos() {
 return Vector2D();
}
int GameObject::getRotation() {
 return this->rotation;
}
void GameObject::setRotation(int rotation) {
 this->rotation = rotation;
}
Vector2D GameObject::getPosition() {
 return this->position;
}
void GameObject::setPosition(Vector2D position) {
 this->position = position;
}
void GameObject::setChildren(Array<GameObject*>* children) {
 this->children = children;
}
Array<GameObject*>* GameObject::getChildren() {
 return this->children;
}
void GameObject::setShapes(Array<Shape*>* shapes) {
 this->shapes = shapes;
}
Array<Shape*>* GameObject::getShapes() {
 return this->shapes;
}
void GameObject::addChild(GameObject* child) {
 this->children->push(child);
}
void GameObject::onCreate() {}
void GameObject::update() {
 //this->position = this->position + Vector2D(100, 100);
}
void GameObject::onDestroy() {}
void GameObject::onPress() {}
void GameObject::onHoverStart() {}
void GameObject::onHoverEnd() {}
void GameObject::onHover() {}
void GameObject::onDragStart() {}
void GameObject::onDrag() {}
void GameObject::onDragEnd() {}
int GameObject::gameObjectCount = 0;

class OrthogonalCamera : public GameObject {
private:
 int width;
 int height;
public:
 OrthogonalCamera(string name, GameObject* parent, int width, int height) : GameObject(name, parent) {
  this->width = width;
  this->height = height;
 }
};

class AND_2 : public GameObject {
private:
 ImageShape gateImage = ImageShape(this, IMAGES::AND_2);
public:
 AND_2(string name, GameObject* parent) : GameObject(name, parent) {
  this->gateImage.setShapeStart(Vector2D(0, 0));
  this->gateImage.setShapeEnd(Vector2D(320, 160));
  this->getShapes()->push(&(this->gateImage));
 }
};

class RootGameObject : public GameObject {
 GameObject* andGate1 = new AND_2("Gate 1", this);
 GameObject* andGate2 = new AND_2("Gate 2", this);
 GameObject* andGate3 = new AND_2("Gate 3", this);
 GameObject* andGate4 = new AND_2("Gate 4", this);
 OrthogonalCamera* camera = new OrthogonalCamera("Main Camera", this, initialCameraWidth, initialCameraWidth * 9 / 16);
public:
 RootGameObject() : GameObject("Root Game Object", nullptr) {
  this->andGate1->setPosition(Vector2D(320, 320));
  this->andGate1->setRotation(0);
  this->andGate2->setPosition(Vector2D(320, 320));
  this->andGate2->setRotation(90);
  this->andGate3->setPosition(Vector2D(320, 320));
  this->andGate3->setRotation(180);
  this->andGate4->setPosition(Vector2D(320, 320));
  this->andGate4->setRotation(270);
  this->getChildren()->push(this->andGate1);
  this->getChildren()->push(this->andGate2);
  this->getChildren()->push(this->andGate3);
  this->getChildren()->push(this->andGate4);
  this->getChildren()->push(this->camera);
 }
 OrthogonalCamera* getCamera() {
  return this->camera;
 }
};

enum JUSTIFY_CONTENT {
 START,
 CENTER,
 END
};
class HorizontalList : GameObject {
private:
 int elementMinWidth;
 int elementMaxWidth;
 int elementAspectRatio;
 JUSTIFY_CONTENT justifyContent;
 JUSTIFY_CONTENT alignItems;
};

class Dummy : GameObject {
};

class Scene {
private:
 RootGameObject rootGameObject = RootGameObject();
 OrthogonalCamera* camera;
public:
 Scene() {
  this->camera = this->rootGameObject.getCamera();
 }
 void Start() {
  vector<float> v;
  this->rootGameObject.shapesMatrix(&v);
  vector<unsigned char> out;
  OCL::ocl.renderShapes(&v, Assets::textureBuffer, 1280, 720, &out);
  //lodepng::encode("test.png", out.data(), 1280, 720, LodePNGColorType::LCT_RGB);
  BITMAPINFO* bmiImage = (LPBITMAPINFO) new BYTE[sizeof(BITMAPINFOHEADER)];
  memset(bmiImage, 0, sizeof(BITMAPINFOHEADER));
  bmiImage->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bmiImage->bmiHeader.biPlanes = 1;
  bmiImage->bmiHeader.biBitCount = 24;
  bmiImage->bmiHeader.biCompression = BI_RGB;
  bmiImage->bmiHeader.biSizeImage = 0;
  bmiImage->bmiHeader.biClrUsed = 0;
  bmiImage->bmiHeader.biClrImportant = 0;
  bmiImage->bmiHeader.biWidth = 1280;
  bmiImage->bmiHeader.biHeight = -720;
  SetDIBitsToDevice(wind->getDCActive(), 0, 0, 1280, 720, 0, 0, 0, 720, out.data(), bmiImage, DIB_RGB_COLORS);
  this->rootGameObject.update();
 }
};

int main()
{
 OCL::initializeOCL();
 Assets::preloadImages();
 int horizontal, vertical;
 GetDesktopResolution(horizontal, vertical);
 wind = new window(1280, 720);
 wind->ChangeTitle("Testing");
 SetCursor(NULL);
 Scene a;
 while (1) {
  a.Start();
 }
 return 0;
}