#pragma once

#include <iostream>
#include <vector>
#include <cstring>
#include "functions.h"

using namespace std;

struct Record{
  int key;
  //char key[20];
  char nombre[20];
  char producto[20];
  char marca[20];
  float precio;
  int cantidad;
  
  Record(int key_, char nombre_[20], char producto_[20], char marca_[20], float precio_, int cantidad_){
  // Record(char key_[20], char nombre_[20], char producto_[20], char marca_[20], float precio_, int cantidad_){
    //strncpy(key, key_, sizeof(key));
    this->key = key_;
    strncpy(nombre, nombre_, sizeof(nombre));
    strncpy(producto, producto_, sizeof(producto));
    strncpy(marca, marca_, sizeof(marca));
    this->precio = precio_;
    this->cantidad = cantidad_;
  }

  Record(){};

  void print(){
    // cout << key << "\t" << nombre << "\t" << producto << "\t" << marca << "\t" << precio << "\t" << cantidad << endl;
    cout << key << "\t" << nombre << "\t" << producto << "\t" << endl;

  }
};

// Nombre,Género,Profesión,Edad,Sueldo
 

struct Record2{

  char key[20]; // Nombre
  char genero[20];
  char profesion[20];
  int edad;
  float sueldo;
  
  
  Record2(char key_[20], char genero_[20], char profesion_[20], int edad_, float sueldo_ ){
    strncpy(key, key_, sizeof(key));
    strncpy(genero, genero_, sizeof(genero));
    strncpy(profesion, profesion_, sizeof(profesion));
    this->edad = edad_;
    this->sueldo = sueldo_;
  }

  Record2(){};

  void print(){
    cout << key << "\t" << genero << "\t" << profesion << "\t" << endl;
  }

};
