#include <engine/Matrices.h>

Matrix Matrix::Sum ( Matrix* _mat1, Matrix* _mat2)
{
	if(_mat1->matrix_.size() != _mat2->matrix_.size() || _mat1->matrix_[0].size() != _mat2->matrix_[0].size())
	{
		// INCOMPATIBLE!!!
		throw;
	};

	Matrix result_matrix(_mat1->matrix_.size(),_mat1->matrix_[0].size());
	for ( size_t y = 0 ; y < _mat1->matrix_.size() ; y++ )
		for ( size_t x = 0 ; x < _mat1->matrix_[y].size() ; x++ )
			result_matrix.matrix_[y][x] = _mat1->matrix_[y][x] + _mat2->matrix_[y][x];

	return result_matrix;
};

Vec4* Matrix::Multiply ( Matrix* _mat1, Vec4* _vec)
{
	if(_mat1->matrix_.size() != 4 || _mat1->matrix_[0].size() != 4)
	{
		// INCOMPATIBLE!!!
		throw;
	};
	Vec4* tempVec = new Vec4();
	
	tempVec->x( (_mat1->GetValue(0,0) * _vec->x()) + (_mat1->GetValue(0,1) * _vec->y()) + (_mat1->GetValue(0,2) * _vec->z()) + (_mat1->GetValue(0,3) * _vec->w()) );
	tempVec->y( (_mat1->GetValue(1,0) * _vec->x()) + (_mat1->GetValue(1,1) * _vec->y()) + (_mat1->GetValue(1,2) * _vec->z()) + (_mat1->GetValue(1,3) * _vec->w()) );
	tempVec->z( (_mat1->GetValue(2,0) * _vec->x()) + (_mat1->GetValue(2,1) * _vec->y()) + (_mat1->GetValue(2,2) * _vec->z()) + (_mat1->GetValue(2,3) * _vec->w()) );
	tempVec->w( (_mat1->GetValue(3,0) * _vec->x()) + (_mat1->GetValue(3,1) * _vec->y()) + (_mat1->GetValue(3,2) * _vec->z()) + (_mat1->GetValue(3,3) * _vec->w()) );

	return tempVec;
};

Vec3* Matrix::Multiply ( Matrix* _mat1, Vec3* _vec)
{
	if(_mat1->matrix_.size() != 3 || _mat1->matrix_[0].size() != 3)
	{
		// INCOMPATIBLE!!!
		throw;
	};
	Vec3* tempVec = new Vec3();

	tempVec->x( (_mat1->GetValue(0,0) * _vec->x()) + (_mat1->GetValue(0,1) * _vec->y()) + (_mat1->GetValue(0,2) * _vec->z()) );
	tempVec->y( (_mat1->GetValue(1,0) * _vec->x()) + (_mat1->GetValue(1,1) * _vec->y()) + (_mat1->GetValue(1,2) * _vec->z()) );
	tempVec->z( (_mat1->GetValue(2,0) * _vec->x()) + (_mat1->GetValue(2,1) * _vec->y()) + (_mat1->GetValue(2,2) * _vec->z()) );

	return tempVec;
};

Vec2* Matrix::Rotate ( Vec2* _vector, float _angle )
{
	// Create rotation matrix
	Matrix* temp = new Matrix(3,3);
	temp->SetValue(0,0,cos(_angle));
	temp->SetValue(1,0,-sin(_angle));
	temp->SetValue(2,0,0.0);
	temp->SetValue(0,1,sin(_angle));
	temp->SetValue(1,1,cos(_angle));
	temp->SetValue(2,1,0.0);
	temp->SetValue(0,2,0.0);
	temp->SetValue(1,2,0.0);
	temp->SetValue(2,2,1.0);

	// Convert Vec2 to Vec3, and multiply
	Vec3* tempVec3 = new Vec3(_vector->x(),_vector->y(),1.0);
	tempVec3 = Matrix::Multiply(temp, tempVec3);

	// Convert back to Vec2
	Vec2* tempVec2 = new Vec2(tempVec3->x(), tempVec3->y());

	// Cleanup
	delete temp;
	delete tempVec3;

	// Return the rotated Vec2
	return tempVec2;

	/*Vec2* temp = new Vec2(_vector->x(),_vector->y());
	temp->x( (_vector->x() * cos(_angle) ) - (_vector->y() * sin(_angle) ) );
	temp->y( (_vector->y() * cos(_angle) ) + (_vector->x() * sin(_angle) ) );
	return temp;*/
};

Vec3* Matrix::RotateX ( Vec3* _vector, float _angle)
{
	// Create rotation matrix
	Matrix* temp = new Matrix(4,4);
	temp->SetValue(0,0,	1.0);
	temp->SetValue(1,0,	0.0);
	temp->SetValue(2,0,	0.0);
	temp->SetValue(3,0,	0.0);
	temp->SetValue(0,1,0.0);
	temp->SetValue(1,1,cos(_angle));
	temp->SetValue(2,1,-sin(_angle));
	temp->SetValue(3,1,0.0);
	temp->SetValue(0,2,0.0);
	temp->SetValue(1,2,sin(_angle));
	temp->SetValue(2,2,cos(_angle));
	temp->SetValue(3,2,0.0);
	temp->SetValue(0,3,0.0);
	temp->SetValue(1,3,0.0);
	temp->SetValue(2,3,0.0);
	temp->SetValue(3,3,1.0);

	// Convert Vec3 to Vec4, and multiply
	Vec4* tempVec4 = new Vec4(_vector->x(),_vector->y(),_vector->z(),1.0f);
	tempVec4 = Matrix::Multiply(temp, tempVec4);

	// Convert back to Vec3
	Vec3* tempVec3 = new Vec3(tempVec4->x(), tempVec4->y(), tempVec4->z());



	return tempVec3;
};

Vec3* Matrix::RotateY ( Vec3* _vector, float _angle)
{
	// Create rotation matrix
	Matrix* temp = new Matrix(4,4);
	temp->SetValue(0,0,	cos(_angle));
	temp->SetValue(1,0,	0.0);
	temp->SetValue(2,0,	sin(_angle));
	temp->SetValue(3,0,	0.0);
	temp->SetValue(0,1,0.0);
	temp->SetValue(1,1,1.0);
	temp->SetValue(2,1,0.0);
	temp->SetValue(3,1,0.0);
	temp->SetValue(0,2,-sin(_angle));
	temp->SetValue(1,2,0.0);
	temp->SetValue(2,2,cos(_angle));
	temp->SetValue(3,2,0.0);
	temp->SetValue(0,3,0.0);
	temp->SetValue(1,3,0.0);
	temp->SetValue(2,3,0.0);
	temp->SetValue(3,3,1.0);

	// Convert Vec3 to Vec4, and multiply
	Vec4* tempVec4 = new Vec4(_vector->x(),_vector->y(),_vector->z(),1.0f);
	tempVec4 = Matrix::Multiply(temp, tempVec4);

	// Convert back to Vec3
	Vec3* tempVec3 = new Vec3(tempVec4->x(), tempVec4->y(), tempVec4->z());



	return tempVec3;
};

Vec3* Matrix::RotateZ ( Vec3* _vector, float _angle)
{
	// Create rotation matrix
	Matrix* temp = new Matrix(4,4);
	temp->SetValue(0,0,	cos(_angle));
	temp->SetValue(1,0,	-sin(_angle));
	temp->SetValue(2,0,	0.0);
	temp->SetValue(3,0,	0.0);
	temp->SetValue(0,1,	sin(_angle));
	temp->SetValue(1,1,	cos(_angle));
	temp->SetValue(2,1,	0.0);
	temp->SetValue(3,1,	0.0);
	temp->SetValue(0,2,	0.0);
	temp->SetValue(1,2,0.0);
	temp->SetValue(2,2,1.0);
	temp->SetValue(3,2,0.0);
	temp->SetValue(0,3,0.0);
	temp->SetValue(1,3,0.0);
	temp->SetValue(2,3,0.0);
	temp->SetValue(3,3,1.0);

	// Convert Vec3 to Vec4, and multiply
	Vec4* tempVec4 = new Vec4(_vector->x(),_vector->y(),_vector->z(),1.0f);
	tempVec4 = Matrix::Multiply(temp, tempVec4);

	// Convert back to Vec3
	Vec3* tempVec3 = new Vec3(tempVec4->x(), tempVec4->y(), tempVec4->z());



	return tempVec3;
};