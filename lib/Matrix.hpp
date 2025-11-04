#pragma once
#include<bits/stdc++.h>
struct Matrix {
    std::vector< std::vector<double> > data;
    int rows, cols;
    Matrix(int r, int c) : rows(r), cols(c) {
        data.resize(r, std::vector<double>(c, 0.0));
    }
    std::vector<double>& operator[](int i) {
        return data[i];
    }
    Matrix operator*(const Matrix& other) {
        if (cols != other.rows) {
            throw std::invalid_argument("Matrix dimensions do not match for multiplication.");
        }
        Matrix result(rows, other.cols);
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < other.cols; ++j) {
                for (int k = 0; k < cols; ++k) {
                    result[i][j] += data[i][k] * other.data[k][j];
                }
            }
        }
        return result;
    } 
    Matrix operator/(const double& div) {
        Matrix tmp = *this;
        for(int i = 0; i < rows; i ++) {
            for(int j = 0; j < cols; j ++) {
                tmp[i][j] /= div;
            }
        }
        return tmp;
    }
    void operator/=(const double& div) {
        for(int i = 0; i < rows; i ++) {
            for(int j = 0; j < cols; j ++) {
                data[i][j] /= div;
            }
        }
    }
    void Softmax(){
        for(int i = 0; i < rows; i ++) {
            double sum = 0;
            for(int j = 0; j < cols; j ++) {
                sum += exp(data[i][j]);
            }
            for(int j = 0; j < cols; j ++) {
                data[i][j] = (double)(exp(data[i][j])/sum);
            }
        }
    }
    Matrix operator+(const Matrix& b) {
        if(rows != b.rows || cols != b.cols) {
            throw std::invalid_argument("Matrix dimensions do not match for addition.");
        }
        Matrix tmp = *this;
        for(int i = 0; i < rows; i ++) {
            for(int j = 0; j < cols; j ++) {
                tmp[i][j] += b.data[i][j];
            }
        }
        return tmp;
    }
};