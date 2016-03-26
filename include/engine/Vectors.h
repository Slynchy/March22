/*
	Vectors.h
	A small vector library made for a university assignment.

	By Sam Lynch, Amduat Games
*/

#pragma once

#include "Matrices.h"
#include <string>
#include <ostream>

class Vec2;
class Vec3;
class Vec4;

class Vector
{
private:

public:
	static Vec2 Sum(Vec2*, Vec2*);
	static Vec3 Sum(Vec3*, Vec3*);
	static double Magnitude(Vec2*);
	static double Magnitude(Vec3*);
	static double DotProd(Vec2*, Vec2*);
	static double DotProd(Vec3*, Vec3*);
	static double DotProd(Vec4*, Vec4*);
	static Vec2 Lerp(Vec2*,Vec2*, double);
};

class Vec2 : public Vector
{
private:
	double x_;
	double y_;
public:
	Vec2()
	{
		Vec2::x_ = 0.0;
		Vec2::y_ = 0.0;
	};
	Vec2(double _x)
	{
		Vec2::x_ = _x;
		Vec2::y_ = 0.0;
	};
	Vec2(double _x,double _y)
	{
		Vec2::x_ = _x;
		Vec2::y_ = _y;
	};

	inline double x() { return x_; }
	inline double y() { return y_; }
	inline void x(double _x) { x_ = _x; }
	inline void y(double _y) { y_ = _y; }
	inline void Vec2::operator/=(const double _scalar)
	{
		(*this).x_ /= _scalar;
		(*this).y_ /= _scalar;
		return;
	}
	inline Vec2 Vec2::operator/(const double _scalar)
	{
		Vec2 newVec;
		newVec.x_ = this->x_ / _scalar;
		newVec.y_ = this->y_ / _scalar;
		return newVec;
	}
	inline void Vec2::operator*=(const double _scalar)
	{
		(*this).x_ *= _scalar;
		(*this).y_ *= _scalar;
		return;
	}
	inline Vec2 Vec2::operator*(const double _scalar)
	{
		Vec2 newVec;
		newVec.x_ = this->x_ * _scalar;
		newVec.y_ = this->y_ * _scalar;
		return newVec;
	}
/*	inline void Vec2::operator*=(const Vec2& _vec)
	{
		(*this).x_ *= _vec.x_;
		(*this).y_ *= _vec.y_;
		return;
	}
	inline Vec2 Vec2::operator*(const Vec2& _vec)
	{
		Vec2 newVec;
		newVec.x_ = this->x_ * _vec.x_;
		newVec.y_ = this->y_ * _vec.y_;
		return newVec;
	}*/
	inline void Vec2::operator+=(const Vec2& _vec)
	{
		(*this).x_ += _vec.x_;
		(*this).y_ += _vec.y_;
		return;
	}
	inline Vec2 Vec2::operator+(const Vec2& _vec)
	{
		Vec2 newVec;
		newVec.x_ = this->x_ + _vec.x_;
		newVec.y_ = this->y_ + _vec.y_;
		return newVec;
	}
	inline void Vec2::operator-=(const Vec2& _vec)
	{
		(*this).x_ -= _vec.x_;
		(*this).y_ -= _vec.y_;
		return;
	}
	inline Vec2 Vec2::operator-(const Vec2& _vec)
	{
		Vec2 newVec;
		newVec.x_ = this->x_ - _vec.x_;
		newVec.y_ = this->y_ - _vec.y_;
		return newVec;
	}
	friend std::ostream &operator<<( std::ostream &out, const Vec2 &vec )
    { 
        out << "(" << vec.x_ << ", " << vec.y_ << ")";
        return out;            
    }
};

class Vec3 : public Vector
{
private:
	double x_;
	double y_;
	double z_;
public:
	Vec3()
	{
		Vec3::x_ = 0.0;
		Vec3::y_ = 0.0;
		Vec3::z_ = 0.0;
	};
	Vec3(double _x)
	{
		Vec3::x_ = _x;
		Vec3::y_ = 0.0;
		Vec3::z_ = 0.0;
	};
	Vec3(double _x,double _y)
	{
		Vec3::x_ = _x;
		Vec3::y_ = _y;
		Vec3::z_ = 0.0;
	};
	Vec3(double _x,double _y,double _z)
	{
		Vec3::x_ = _x;
		Vec3::y_ = _y;
		Vec3::z_ = _z;
	};
	inline void Vec3::operator/=(const double _scalar)
	{
		(*this).x_ /= _scalar;
		(*this).y_ /= _scalar;
		(*this).z_ /= _scalar;
		return;
	}
	inline Vec3 Vec3::operator/(const double _scalar)
	{
		Vec3 newVec;
		newVec.x_ = this->x_ / _scalar;
		newVec.y_ = this->y_ / _scalar;
		newVec.z_ = this->z_ / _scalar;
		return newVec;
	}
	inline void Vec3::operator*=(const double _scalar)
	{
		(*this).x_ *= _scalar;
		(*this).y_ *= _scalar;
		(*this).z_ *= _scalar;
		return;
	}
	inline Vec3 Vec3::operator*(const double _scalar)
	{
		Vec3 newVec;
		newVec.x_ = this->x_ * _scalar;
		newVec.y_ = this->y_ * _scalar;
		newVec.z_ = this->z_ * _scalar;
		return newVec;
	}
	inline void Vec3::operator+=(const Vec3& _vec)
	{
		(*this).x_ += _vec.x_;
		(*this).y_ += _vec.y_;
		(*this).z_ += _vec.z_;
		return;
	}
	inline Vec3 Vec3::operator+(const Vec3& _vec)
	{
		Vec3 newVec;
		newVec.x_ = this->x_ + _vec.x_;
		newVec.y_ = this->y_ + _vec.y_;
		newVec.z_ = this->z_ + _vec.z_;
		return newVec;
	}
	inline void Vec3::operator-=(const Vec3& _vec)
	{
		(*this).x_ -= _vec.x_;
		(*this).y_ -= _vec.y_;
		(*this).z_ -= _vec.z_;
		return;
	}
	inline Vec3 Vec3::operator-(const Vec3& _vec)
	{
		Vec3 newVec;
		newVec.x_ = this->x_ - _vec.x_;
		newVec.y_ = this->y_ - _vec.y_;
		newVec.z_ = this->z_ - _vec.z_;
		return newVec;
	}
	friend std::ostream &operator<<( std::ostream &out, const Vec3 &vec )
    { 
        out << "(" << vec.x_ << ", " << vec.y_ << ", " << vec.z_ << ")";
        return out;            
    }

	inline double x() { return x_; }
	inline double y() { return y_; }
	inline double z() { return z_; }
	inline void x(double _x) { x_ = _x; }
	inline void y(double _y) { y_ = _y; }
	inline void z(double _z) { z_ = _z; }
};

class Vec4 : public Vector
{
private:
	double x_;
	double y_;
	double z_;
	double w_;
public:
	Vec4()
	{
		Vec4::x_ = 0.0;
		Vec4::y_ = 0.0;
		Vec4::z_ = 0.0;
		Vec4::w_ = 0.0;
	};
	Vec4(double _x)
	{
		Vec4::x_ = _x;
		Vec4::y_ = 0.0;
		Vec4::z_ = 0.0;
		Vec4::w_ = 0.0;
	};
	Vec4(double _x,double _y)
	{
		Vec4::x_ = _x;
		Vec4::y_ = _y;
		Vec4::z_ = 0.0;
		Vec4::w_ = 0.0;
	};
	Vec4(double _x,double _y,double _z)
	{
		Vec4::x_ = _x;
		Vec4::y_ = _y;
		Vec4::z_ = _z;
		Vec4::w_ = 0.0;
	};
	Vec4(double _x,double _y,double _z,double _w)
	{
		Vec4::x_ = _x;
		Vec4::y_ = _y;
		Vec4::z_ = _z;
		Vec4::w_ = _w;
	};
	inline Vec4 Vec4::operator+(const Vec4& _vec)
	{
		Vec4 newVec;
		newVec.x_ = this->x_ + _vec.x_;
		newVec.y_ = this->y_ + _vec.y_;
		newVec.z_ = this->z_ + _vec.z_;
		newVec.w_ = this->w_ + _vec.w_;
		return newVec;
	}
	inline void Vec4::operator+=(const Vec4& _vec)
	{
		(*this).x_ += _vec.x_;
		(*this).y_ += _vec.y_;
		(*this).z_ += _vec.z_;
		(*this).w_ += _vec.w_;
		return;
	}
	inline Vec4 Vec4::operator-(const Vec4& _vec)
	{
		Vec4 newVec;
		newVec.x_ = this->x_ - _vec.x_;
		newVec.y_ = this->y_ - _vec.y_;
		newVec.z_ = this->z_ - _vec.z_;
		newVec.w_ = this->w_ - _vec.w_;
		return newVec;
	}
	inline void Vec4::operator-=(const Vec4& _vec)
	{
		(*this).x_ -= _vec.x_;
		(*this).y_ -= _vec.y_;
		(*this).z_ -= _vec.z_;
		(*this).w_ -= _vec.w_;
		return;
	}
	inline Vec4 Vec4::operator*(const double _scalar)
	{
		Vec4 newVec;
		newVec.x_ = this->x_ * _scalar;
		newVec.y_ = this->y_ * _scalar;
		newVec.z_ = this->z_ * _scalar;
		newVec.w_ = this->w_ * _scalar;
		return newVec;
	}
	inline void Vec4::operator*=(const double _scalar)
	{
		(*this).x_ *= _scalar;
		(*this).y_ *= _scalar;
		(*this).z_ *= _scalar;
		(*this).w_ *= _scalar;
		return;
	}
	inline Vec4 Vec4::operator/(const double _scalar)
	{
		Vec4 newVec;
		newVec.x_ = this->x_ / _scalar;
		newVec.y_ = this->y_ / _scalar;
		newVec.z_ = this->z_ / _scalar;
		newVec.w_ = this->w_ / _scalar;
		return newVec;
	}
	inline void Vec4::operator/=(const double _scalar)
	{
		(*this).x_ /= _scalar;
		(*this).y_ /= _scalar;
		(*this).z_ /= _scalar;
		(*this).w_ /= _scalar;
		return;
	}
	friend std::ostream &operator<<( std::ostream &out, const Vec4 &vec )
    { 
        out << "(" << vec.x_ << ", " << vec.y_ << ", " << vec.z_ << ", " << vec.w_ << ")";
        return out;            
    }

	inline double x() { return x_; }
	inline double y() { return y_; }
	inline double z() { return z_; }
	inline double w() { return w_; }
	inline void x(double _x) { x_ = _x; }
	inline void y(double _y) { y_ = _y; }
	inline void z(double _z) { z_ = _z; }
	inline void w(double _w) { w_ = _w; }
};