#include <iostream>
#include <sstream>
#include <cmath>
#include <windows.h>
#include <regex>

#include "./Utilities/ExtendedImage/ExtendedImage.h"
#include "./OCL/OCL.h"

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

Vector2D rotatePoint(Vector2D point, float angle, Vector2D p)
{
 float cx = point.getX();
 float cy = point.getY();

 float s = sin(angle);
 float c = cos(angle);

 // translate point back to origin:
 p.setX(p.getX() - cx);
 p.setY(p.getY() - cy);

 // rotate point
 float xnew = p.getX() * c - p.getY() * s;
 float ynew = p.getX() * s + p.getY() * c;

 // translate point back:
 p.setX(xnew + cx);
 p.setY(ynew + cy);
 return p;
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
 virtual void draw();
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
 void render();
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
 static int gameObjectCount;
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
void Shape::draw() {};

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
 return rotatePoint(this->shapeStart + this->parent->getAbsolutePosition(), this->parent->getRotation(), this->parent->getAbsolutePosition());
}
Vector2D Shape::getAbsoluteShapeEnd() {
 return rotatePoint(this->shapeEnd + this->parent->getAbsolutePosition(), this->parent->getRotation(), this->parent->getAbsolutePosition());
}
Vector2D Shape::getAbsoluteColliderStart() {
 return rotatePoint(this->colliderStart + this->parent->getAbsolutePosition(), this->parent->getRotation(), this->parent->getAbsolutePosition());
}
Vector2D Shape::getAbsoluteColliderEnd() {
 return rotatePoint(this->colliderEnd + this->parent->getAbsolutePosition(), this->parent->getRotation(), this->parent->getAbsolutePosition());
}

class ImageShape : public Shape {
private:
 string imagePath;
 string extension;
 int width = 4 * gridUnitSize;
 int height = 4 * gridUnitSize;
public:
 ImageShape(GameObject* parent, string imageFolderPath, string extension) : Shape(parent) {
  this->imagePath = imageFolderPath;
  this->extension = extension;
 }
 void draw() {
  GameObject* parent = this->getParent();
  Vector2D absolutePosition = parent->getAbsolutePosition();
  //cout << parent->getName() << this->getAbsoluteShapeStart().toString() << "\n";
  wind->DrawPNG(this->imagePath + "." + this->extension, this->getAbsoluteShapeStart().getX(), this->getAbsoluteShapeStart().getY(), this->width, this->height);
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
 //cout << "a";
}
void GameObject::render() {
 //cout << "render " << this->name << "\n";
 //cout << this->shapes->toString() << "\n";
 //cout << this->children->toString() << "\n";
 this->shapes->forEach([](Shape* shape) {
  shape->draw();
  });
 this->children->forEach([](GameObject* gameObject) {
  gameObject->render();
  });
}
void GameObject::onCreate() {}
void GameObject::update() {
 //cout << "hena ?";
 this->position = this->position + Vector2D(100, 100);
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
 GameObject* rootGameObject;
 OrthogonalCamera* camera;
public:
 Scene(GameObject* rootGameObject, OrthogonalCamera* camera) {
  this->rootGameObject = rootGameObject;
  this->camera = camera;
 }
 void Start() {
  this->rootGameObject->update();
  this->rootGameObject->render();
 }
};

int main()
{
 OCL::initializeOCL();
 int horizontal, vertical;
 GetDesktopResolution(horizontal, vertical);
 wind = new window(horizontal, vertical);
 wind->ChangeTitle("Testing");
 SetCursor(NULL);
 GameObject rootGameObject = GameObject("Root Game Object");
 OrthogonalCamera camera = OrthogonalCamera("Camera", &rootGameObject, initialCameraWidth, initialCameraWidth * vertical / horizontal);
 GameObject imagesContainer1 = GameObject("Image Container 1", &rootGameObject);
 imagesContainer1.setPosition(Vector2D(160));
 GameObject image1 = GameObject("Image 1", &imagesContainer1);
 Array<Shape*> shapes1 = Array<Shape*>([](Shape* shape) { return shape->toString(); }); // TO-DO
 ImageShape imageShape1 = ImageShape(&image1, "./image", "png");
 shapes1.push(&imageShape1);
 image1.setShapes(&shapes1);
 imagesContainer1.addChild(&image1);
 GameObject imagesContainer2 = GameObject("Image Container 2", &rootGameObject);
 imagesContainer2.setPosition(Vector2D(0, 0));
 GameObject image2 = GameObject("Image 2", &imagesContainer2);
 Array<Shape*> shapes2 = Array<Shape*>([](Shape* shape) { return shape->toString(); });
 ImageShape imageShape2 = ImageShape(&image2, "./Assets/Images/2AND", "png");
 shapes2.push(&imageShape2);
 image2.setShapes(&shapes2);
 imagesContainer2.addChild(&image2);
 //cout << imagesContainer1.toString();
 rootGameObject.addChild(&imagesContainer1);
 rootGameObject.addChild(&imagesContainer2);
 Scene scene = Scene(&rootGameObject, &camera);
 //cout << "bla" << rootGameObject.toString();
 while (1) {
  //cout << rootGameObject.toString();
  wind->SetBuffering(true);
  scene.Start();
  wind->UpdateBuffer();
  wind->SetBuffering(true);
 }
 return 0;
}