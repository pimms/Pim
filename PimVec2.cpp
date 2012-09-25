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

	Vec2 Vec2::rotateAroundPoint(Vec2 &pt, float a)
	{
		return rotateDegrees(a) + pt;

		float r = a * (M_PI / 180.f);
		return Vec2(((x-pt.x) * cosf(r) - (y-pt.y) * sinf(r)),
				    ((y-pt.y) * cosf(r) + (x-pt.x) * sinf(r))
					);
	}
	Vec2 Vec2::rotateDegrees(float a)
	{
		float r = a * (M_PI / 180.f);
		return Vec2( x*cosf(r) - y*sinf(r),
					 y*cosf(r) + x*sinf(r)
					 );
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