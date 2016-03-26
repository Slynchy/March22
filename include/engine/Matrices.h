/*
	Matrices.h
	A small matrix library made for a university assignment.

	By Sam Lynch, Amduat Games
*/

#pragma once

#include "Vectors.h"
#include <vector>
#include <iostream>

// Forward declarations
class Vec2;
class Vec3;
class Vec4;

class Matrix
{
private:
	int x_;
	int y_;
	std::vector<std::vector<double>> matrix_;
public:
	Matrix(){}
	Matrix(int _x, int _y)
	{
		matrix_.resize(_y);
		for (size_t i=0; i < matrix_.size(); i++) {
			matrix_[i].resize(_x);
		};
	}

	inline double GetValue(int _x, int _y)
	{
		return matrix_[_y][_x];
	};

	inline void SetValue(int _y, int _x, double value)
	{
		matrix_[_y][_x] = value;
	};
	friend std::ostream &operator<<( std::ostream &out, const Matrix &matrix )
    { 
		for(size_t y = 0; y < matrix.matrix_.size(); y++)
		{
			for(size_t x = 0; x < matrix.matrix_.size(); x++)
			{
				out << matrix.matrix_[y][x] << ',';
			};
			out << '\n';
		};
        return out;            
    }

	static Matrix Matrix::Sum ( Matrix* _mat1, Matrix* _mat2);
	
	static Vec3* Matrix::Multiply( Matrix* _mat1, Vec3* _vec);
	static Vec4* Matrix::Multiply ( Matrix* _mat1, Vec4* _vec);
	static Vec2* Matrix::Rotate ( Vec2* _vector, float _angle );
	static Vec3* Matrix::RotateX ( Vec3* _vector, float _angle);
	static Vec3* Matrix::RotateY ( Vec3* _vector, float _angle);
	static Vec3* Matrix::RotateZ ( Vec3* _vector, float _angle);

//	static 
};