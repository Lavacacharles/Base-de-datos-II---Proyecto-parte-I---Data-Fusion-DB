#pragma once
#include "Record.h"
#include <utility>
#include <vector>


template<typename R>
class MethodSelector{
public:
  MethodSelector(){}
  
  virtual bool add(R record){
    cout<<"add-method"<<endl;
    return false;
  }

  virtual vector<R> load() {

    return vector<R>();
  }

  virtual void display(){
    return;
  }


  virtual void display_all(){
    return;
  }

  virtual vector<R> rangeSearch(int min, int max){
    return vector<R>();
  }

  virtual vector<R> rangeSearch(long min, long max){
    return vector<R>();
  }

  virtual vector<R> rangeSearch(char* min, char* max){
    return vector<R>();
  }

  virtual bool remove( char* key ){
    return false;
  }

  virtual bool remove( int key ){
    return false;
  }

  virtual bool remove( long key ){
    return false;
  }

  virtual pair<R, bool> search(char* key){
    return make_pair(R(), false);
  }

  virtual pair<R, bool> search(int key){
    return make_pair(R(), false);
  }
  
  virtual pair<R, bool> search(long key){
    return make_pair(R(), false);
  }
  ~MethodSelector(){}
};
