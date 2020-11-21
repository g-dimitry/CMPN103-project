#include <iostream>
using namespace std;

#define UNICODE

#include "../Project/utils/list/list.h"
#include "CMUgraphicsLib/CMUgraphics.h"

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
};

class Collider {
};

class View {
};

class GameObject {
private:
 int id;
 Vector2D position = Vector2D(0, 0);
 int zIndex = 0;
 int rotation = 0;
 GameObject* parent;
 Array<GameObject*> children = Array<GameObject*>([](GameObject* gO) {return ""; });
 static int gameObjectCount;
public:
 static int getGameObjectId() {
  gameObjectCount++;
  return gameObjectCount;
 }
 GameObject(GameObject* parent) {
  this->id = GameObject::getGameObjectId();
  this->parent = parent;
 }
};
int GameObject::gameObjectCount = 0;

class OrthogonalCamera : GameObject {
private:
 int width;
 int height;
};

class Dummy : GameObject {
};

int main() {
 window* wind = new window(1920, 1080);
 wind->ChangeTitle("Testing");
 //wind->SetBuffering(true);
 wind->SetPen(BLACK);
 wind->SetBrush(BLACK);
 wind->DrawCircle(200, 200, 100);
 //wind->SetBuffering(false);
}