#pragma once
#include <functional>
#include "../printable/printable.h"
using namespace std;

template<typename Type>
class Array : Printable {
private:
 Type* data;
 int count;
 function<string(Type)> f;
 function<string(string, Type, int, Type*)> reduceToString = [this](string accumulator, Type currentValue, int currentIndex, Type* srcArray) {
  return accumulator + "\t" + this->f(currentValue) + ",\n";
 };
public:
 ~Array() {
  delete[] this->data;
 }
 Array<Type>(function<string(Type)> f = [](Type t) { return ""; }) {
  this->data = new Type[0];
  this->count = 0;
  this->f = f;
 }
 Array<Type>(Type* data, int count, function<string(Type)> f) {
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
  return Array<Type>(outArr, count, this->f);
 }
 void push(Type element)
 {
  int newCount = count + 1;
  Type* outArr = new Type[newCount];
  if (!this->count || this->count != 0) {
   this->copy(outArr);
   //delete[] this->data;
  }
  outArr[count] = element;
  this->data = outArr;
  this->count++;
 }
 void filter(function<bool(Type)> f)
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
 void forEach(function<void(Type)> f) {
  if (!this->count || this->count == 0) return;
  for (int i = 0; i < this->count; i++) {
   f(this->data[i]);
  }
 }
 template<typename MapType>
 Array<MapType>* map(function<MapType(Type)> f, function<string(MapType)> f2) {
  MapType* outArr = new MapType[this->count];
  Array<MapType>* out = new Array<MapType>(outArr, this->count, f2);
  for (int i = 0; i < this->count; i++) {
   out->getData()[i] = f(this->data[i]);
  }
  return out;
 }
 /*template<typename ReduceType>
 ReduceType reduce(ReduceType(Array<Type>::* g)(ReduceType, Type, int, Type*), Array<Type>& a, ReduceType initValue) {
  ReduceType value = initValue;
  for (int i = 0; i < this->count; i++) {
   value = (a.*g)(value, this->data[i], i, this->data);
  }
  return value;
 }*/
 template<typename ReduceType>
 ReduceType reduce(function<ReduceType(ReduceType accumulator, Type currentValue, int currentIndex, Type* srcArray)> f, ReduceType initValue) {
  ReduceType value = initValue;
  for (int i = 0; i < this->count; i++) {
   value = f(value, this->data[i], i, this->data);
  }
  return value;
 }
 string toString() {
  if (!this->count || this->count == 0) return "[]";
  return this->reduce<string>(this->reduceToString, "[\n") + "]";
 }
};
