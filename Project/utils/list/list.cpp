#include <stdio.h>
#include <string>
#include <iostream>
#include <regex>
#include <functional>
#include <cstdio>
using namespace std;
using namespace std::placeholders;
#include "list.h"

template<typename Type>
string Array<Type>::reduceToString(string accumulator, Type currentValue, int currentIndex, Type* srcArray) {
 return accumulator + "\t" + this->f(currentValue) + ",\n";
};

template<typename Type>
Array<Type>::~Array() {
 delete[] this->data;
};

template<typename Type>
Array<Type>::Array(string(*f)(Type element)) {
 this->data = new Type;
 this->count = 0;
 this->f = f;
};

template<typename Type>
Array<Type>::Array(Type* data, int count, string(*f)(Type element)) {
 this->data = data;
 this->count = count;
 this->f = f;
};

template<typename Type>
Type* Array<Type>::getData() {
 return this->data;
};

template<typename Type>
int Array<Type>::getCount() {
 return this->count;
};

template<typename Type>
void Array<Type>::copy(Type* dest) {
 for (int i = 0; i < count; i++) {
  dest[i] = this->data[i];
 }
};

template<typename Type>
Array<Type> Array<Type>::clone() {
 Type* outArr = new Type[count];
 this->copy(outArr);
 return Array<Type>(outArr, count);
};

template<typename Type>
void Array<Type>::push(Type element)
{
 int newCount = count + 1;
 Type* outArr = new Type[newCount];
 this->copy(outArr);
 outArr[count] = element;
 delete[] this->data;
 this->data = outArr;
 this->count++;
};

template<typename Type>
void Array<Type>::filter(bool (*f)(Type))
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
};

template<typename Type>
void Array<Type>::forEach(void (*f)(Type)) {
 for (int i = 0; i < this->count; i++) {
  f(this->data[i]);
 }
};

template<typename Type>
template<typename MapType>
Array<MapType>* Array<Type>::map(MapType(*f)(Type), string(*f2)(MapType element)) {
 MapType* outArr = new MapType[this->count];
 Array<MapType>* out = new Array<MapType>(outArr, this->count, f2);
 for (int i = 0; i < this->count; i++) {
  out->getData()[i] = f(this->data[i]);
 }
 return out;
};

template<typename Type>
template<typename ReduceType>
ReduceType Array<Type>::reduce(ReduceType(Array<Type>::* g)(ReduceType, Type, int, Type*), Array<Type>& a, ReduceType initValue) {
 ReduceType value = initValue;
 for (int i = 0; i < this->count; i++) {
  value = (a.*g)(value, this->data[i], i, this->data);
 }
 return value;
};

template<typename Type>
template<typename ReduceType>
ReduceType Array<Type>::reduce(ReduceType(*f)(ReduceType accumulator, Type currentValue, int currentIndex, Type* srcArray), ReduceType initValue) {
 ReduceType value = initValue;
 for (int i = 0; i < this->count; i++) {
  value = f(value, this->data[i], i, this->data);
 }
 return value;
};

template<typename Type>
string Array<Type>::toString() {
 return this->reduce<string>(&Array<Type>::reduceToString, *this, "[\n") + "]";
};