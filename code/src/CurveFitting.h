#ifndef _CURVEFITTING_H_
#define _CURVEFITTING_H_
#include <arduino.h>
//********** 曲线拟合程序 **********
//曲线拟合算法来至https://blog.csdn.net/m0_37362454/article/details/82456616 by欧阳小俊
void curveFitting();

void polyfit(int n, int x[], int y[], int poly_n, float p[]);

void gaussSolve(int n, float A[], float x[], float b[]);
#endif