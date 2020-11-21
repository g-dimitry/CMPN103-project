#include <iostream>
#include <functional>
using namespace std;
#include "../Project/utils/list/list.h";

template<typename Type>
class Observable;

template<typename Type>
class Observer;

template<typename Type>
class Observable {
private:
 Type value;
 Array<Observer<Type>*> observers = Array<Observer<Type>*>([](Observer<Type>* a) {return ""; });
public:
 Observable(Type initialValue) {
  this->value = initialValue;
 }
 void subscribe(Observer<Type>* observer) {
  this->observers.push(observer);
 }
 void unsubscribe(Observer<Type>* observer) {
  function<bool(Observer<Type>*)> callback = [=](Observer<Type>* element) -> bool {
   return element != observer;
  };
  this->observers.filter(callback);
 }
 void setValue(Type value) {
  this->value = value;
  function<void(Observer<Type>*)> callback = [this](Observer<Type>* observer) {
   observer->notify(this->value);
  };
  observers.forEach(callback);
 }
 Type getValue() {
  return this->value;
 }
};

template<typename Type>
class Observer {
private:
 Observable<Type>* observable;
 void(*onChange)(Type);
public:
 Observer(Observable<Type>* observable, void(*onChange)(Type)) {
  this->observable = observable;
  this->observable->subscribe(this);
  this->onChange = onChange;
 }
 ~Observer() {
  this->observable->unsubscribe(this);
 }
 void notify(Type value) {
  this->onChange(value);
 }
 void getValue() {
  return this->observable.getValue();
 }
};

template<typename Props, typename State >
class ViewComponent {
private:
 Observer<Props> props;
 Observable<State> state;
 void componendDidMount() {}
 void componentWillUnmount() {}
 void componentDidUpdate(Observable<Props> previousProps, Observable<State> previousState) {}
public:
 ViewComponent(Observer<Props> props) {
  this->props = props;
 }
 void render() {}
};



class GameObject {

};

int main() {
 Observable<int> observable = Observable<int>(0);
 Observer<int> observer1 = Observer<int>(&observable, [](int value) { cout << "observer 1: " + to_string(value) + "\n"; });
 Observer<int> observer2 = Observer<int>(&observable, [](int value) { cout << "observer 2: " + to_string(value) + "\n"; });
 observable.setValue(2);
};