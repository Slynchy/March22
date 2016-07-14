#include <engine/Vectors.h>

Vec2 Vector::Sum(Vec2* _param1, Vec2* _param2)
{
	return Vec2((_param1->x() + _param2->x()),(_param1->y() + _param2->y()));	
};

Vec3 Vector::Sum(Vec3* _param1, Vec3* _param2)
{
	return Vec3((_param1->x() + _param2->x()),(_param1->y() + _param2->y()),(_param1->z() + _param2->z()));	
};

double Vector::Magnitude(Vec2* _param1)
{
	return double(sqrt( pow( _param1->x(), 2) + pow( _param1->y(), 2) ) );
};

double Vector::Magnitude(Vec3* _param1)
{
	return double(sqrt( pow( _param1->x(), 2) + pow( _param1->y(), 2) + pow( _param1->z(), 2) ) );
};

double Vector::DotProd(Vec2* _param1, Vec2* _param2)
{
	return ( (_param1->x() * _param2->x()) + (_param1->y()*_param2->y()) );
};

double Vector::DotProd(Vec3* _param1, Vec3* _param2)
{
	return ( (_param1->x() * _param2->x()) + (_param1->y()*_param2->y()) + (_param1->z()*_param2->z()) );
};

double Vector::DotProd(Vec4* _param1, Vec4* _param2)
{
	return ( (_param1->x() * _param2->x()) + (_param1->y()*_param2->y()) + (_param1->z()*_param2->z()) + (_param1->w()*_param2->w()) );
};

Vec2 Vector::Lerp(Vec2* _param1, Vec2* _param2, double _t)
{
	Vec2 tempVec = *_param1*(1-_t) + *_param2*_t;
	return tempVec;
};