#pragma once

#include <cstring>
#include <fstream>
#include <iostream>
using namespace std;

inline void inicializarKeys(long* &keys, int M, int n){
  keys = new long[M]; 
}

inline void inicializarKeys(int* &keys, int M, int n){
  keys = new int[M]; 
}

inline void inicializarKeys(float* &keys, int M, int n){
  keys = new float[M]; 
}

inline void inicializarKeys(double* &keys, int M, int n){
  keys = new double[M]; 
}

inline void inicializarKeys(char* &keys, int M, int n){
  keys = new char[M]; 
}

inline void inicializarKeys(char** &keys, int M, int n){
  keys = new char*[M]; 
  for (int i = 0; i < M; i++) {
    keys[i] = new char[20];
  }
}

inline void escribirN(fstream &index, char *key, int n){
  index.write(key, n);
}

inline void escribirN(fstream &index, long &key, int n){
  index.write(reinterpret_cast<const char*>(&key), n);
}

inline void escribirN(fstream &index, int &key, int n){
  index.write(reinterpret_cast<const char*>(&key), n);
}

inline void escribirN(fstream &index, float &key, int n){
  index.write(reinterpret_cast<const char*>(&key), n);
}

inline void escribirN(fstream &index, double &key, int n){
  index.write(reinterpret_cast<const char*>(&key), n);
}

inline void escribirN(fstream &index, char &key, int n){
  index.write(reinterpret_cast<const char*>(&key), n);
}

inline void leerN(fstream &index, char *key, int n){
  index.read(key, n);
}

inline void leerN(fstream &index, long &key, int n){
  index.read((char*)(&key), n);
}

inline void leerN(fstream &index, int &key, int n){
  index.read((char*)(&key), n);
}

inline void leerN(fstream &index, float &key, int n){
  index.read((char*)(&key), n);
}

inline void leerN(fstream &index, double &key, int n){
  index.read((char*)(&key), n);
}

inline void leerN(fstream &index, char &key, int n){
  index.read((char*)(&key), n);
}

inline bool menor_igual(const char* str1, const char* str2) {
    return strcmp(str1, str2) <= 0;
}

inline bool menor_igual(long str1, long str2) {
    return (str1 <= str2);
}

inline bool menor_igual(int str1, int str2) {
    return (str1 <= str2);
}

inline bool menor_igual(double str1, double str2) {
    return (str1 <= str2);
}

inline bool menor_igual(float str1, float str2) {
    return (str1 <= str2);
}

inline bool menor_igual(char str1, char str2) {
    return (str1 <= str2);
}

inline bool igual_igual(const char* str1, const char* str2) {
    return strcmp(str1, str2) == 0;
}

inline bool igual_igual(int str1, int str2) {
    return (str1 == str2);
}

inline bool igual_igual(double str1, double str2) {
    return (str1 == str2);
}

inline bool igual_igual(float str1, float str2) {
    return (str1 == str2);
}

inline bool igual_igual(char str1, char str2) {
    return (str1 == str2);
}

inline bool igual_igual(long str1, long str2) {
    return (str1 == str2);
}

inline bool menor(const char* str1, const char* str2) {
    return strcmp(str1, str2) < 0;
}

inline bool menor(int str1, int str2) {
    return (str1 < str2);
}

inline bool menor(double str1, double str2) {
    return (str1 < str2);
}

inline bool menor(float str1, float str2) {
    return (str1 < str2);
}

inline bool menor(char str1, char str2) {
    return (str1 < str2);
}

inline bool menor(long str1, long str2) {
    return (str1 < str2);
}
