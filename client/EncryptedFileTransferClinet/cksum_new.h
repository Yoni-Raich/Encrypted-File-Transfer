#pragma once
// cksum_new.h
#ifndef CKSUM_NEW_H
#define CKSUM_NEW_H

#include <iostream>
#include <fstream>
#include <ostream>
#include <cstdio>
#include <vector>
#include <iterator>
#include <filesystem>
#include <string>
#include <cstdint>
// Declaration of the CRC table
extern const uint_fast32_t crctab[8][256];

// Macro to convert a number to unsigned
#define UNSIGNED(n) (n & 0xffffffff)

// Function declarations
unsigned long memcrc(char* b, size_t n);
unsigned long readfile(std::string fname);

#endif // CKSUM_NEW_H