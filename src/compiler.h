#ifndef COMPILER_H
#define COMPILER_H
#include <string>
#include <sstream>
#include "common.h"

stringstream ilbuf;

void exAssign(const operatorNode &opr);
void exBin(const operatorNode &opr);
#endif
