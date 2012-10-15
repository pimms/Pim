#include "StdAfx.h"
#include "PimVec2.h"

namespace Pim
{

	Vec2::Vec2(float px, float py)
	{
		x=px; y=py;
	}
	Vec2::Vec2(void)
	{
		x=0; y=0;
	}

	Vec2 Vec2::unitD(float a)
	{
		return unitR(a * (M_PI/180.f));
	}
	Vec2 Vec2::unitR(float a)
	{
		return Vec2(cosf(a), sinf(a));
	}

	Vec2 Vec2::rotateAroundPoint(Vec2 &pt, float a)
	{
		return rotateDegrees(a) + pt;
	}
	Vec2 Vec2::rotateDegrees(float a)
	{
		float r = a * (M_PI / 180.f);
		return Vec2( x*cosf(r) - y*sinf(r),
					 y*cosf(r) + x*sinf(r)
					 );
	}

	float Vec2::angleBetween360(const Vec2 &other)
	{
		float a = (atan2f(other.y, other.x) - atan2f(y, x)) * (180.f / (float)M_PI);
		if (a < 0.f)
			a += 360.f;

		return a;
	}
	float Vec2::angleBetween(Vec2 &other)
	{
		float dot = normalize().dot(other.normalize());
		float angle = acosf(dot) * (180.f / 3.14f);

		Vec2 diff = Vec2(x,y) - other;
		if (diff.dot(Vec2(0.f,1.f)) < 0.f)
			angle = -angle;

		return angle;
	}

	float Vec2::dot(const Vec2 &other)
	{
		return x * other.x + y * other.y;
	}

	Vec2 Vec2::normalize()
	{
		return Vec2(x,y) / length();
	}

	float Vec2::length()
	{
		return sqrt(x*x + y*y);
	}

	// Operator overloading
	bool Vec2::operator==(const Vec2 &other)
	{
		return x == other.x && y == other.y;
	}
	bool Vec2::operator!=(const Vec2 &other)
	{
		return x != other.x || y != other.y;
	}

	Vec2 Vec2::operator+(const Vec2 &other)
	{
		return Vec2(x+other.x, y+other.y);
	}
	Vec2 Vec2::operator-(const Vec2 &other)
	{
		return Vec2(x-other.x, y-other.y);
	}
	Vec2 Vec2::operator*(const Vec2 &other)
	{
		return Vec2(x*other.x, y*other.y);
	}
	Vec2 Vec2::operator/(const Vec2 &other)
	{
		return Vec2(x/(other.x==0.f?0.000001f:other.x), 
					 y/(other.y==0.f?0.000001f:other.y));
	}

	void Vec2::operator+=(const Vec2 &other)
	{
		x += other.x;
		y += other.y;
	}
	void Vec2::operator-=(const Vec2 &other)
	{
		x -= other.x;
		y -= other.y;
	}
	void Vec2::operator*=(const Vec2 &other)
	{
		x *= other.x;
		y *= other.y;
	}
	void Vec2::operator/=(const Vec2 &other)
	{
		x /= other.x==0.f?0.000001f:other.x;
		y /= other.y==0.f?0.000001f:other.y;
	}

	Vec2 Vec2::operator*(const float &fac)
	{
		return Vec2(x*fac, y*fac);
	}
	Vec2 Vec2::operator/(float den)
	{
		if (den == 0.f) den = 0.000001f;
		return Vec2(x/den, y/den);
	}
	void Vec2::operator*=(const float &fac)
	{
		x *= fac;
		y *= fac;
	}
	void Vec2::operator/=(float den)
	{
		if (den == 0.f) den = 0.000001f;
		x /= den;
		y /= den;
	}

}