#pragma once

#include "Stdafx.h"

namespace Pim
{
	class Vec2
	{
	public:
		Vec2(float px, float py);
		Vec2(void);
		
		float angleBetween(Vec2 &other)
		{
			float dot = normalize().dot(other.normalize());
			float angle = acosf(dot) * (180.f / 3.14f);

			Vec2 diff = Vec2(x,y) - other;
			if (diff.normalize().dot(Vec2(0.f,1.f)) < 0.f)
				angle = -angle;

			return angle;
		}

		float dot(const Vec2 &other)
		{
			return x * other.x + y * other.y;
		}

		Vec2 cross(const Vec2 &other)
		{

		}

		Vec2 normalize()
		{
			return Vec2(x,y) / length();
		}

		float length()
		{
			return sqrt(x*x + y*y);
		}

		float x, y;
	
		// Operator overloading
		bool operator==(const Vec2 &other);
		bool operator!=(const Vec2 &other);

		Vec2 operator+(const Vec2 &other);
		Vec2 operator-(const Vec2 &other);
		Vec2 operator*(const Vec2 &other);
		Vec2 operator/(const Vec2 &other);

		void operator+=(const Vec2 &other);
		void operator-=(const Vec2 &other);
		void operator*=(const Vec2 &other);
		void operator/=(const Vec2 &other);

		Vec2 operator*(const float &fac);
		Vec2 operator/(float den);
		void operator*=(const float &fac);
		void operator/=(float den);
	};

	struct Rect
	{
		Rect(png_uint_32 xx, png_uint_32 yy, png_uint_32 w, png_uint_32 h) 
			: x(xx), y(yy), width(w), height(h){}

		Rect() 
			: x(0), y(0), width(0), height(0){}

		png_int_32 x, y, width, height;
	};

	struct Color
	{
		Color(float rr, float gg, float bb, float aa) : r(rr), g(gg), b(bb), a(aa) {}
		Color() : r(0.f), g(0.f), b(0.f), a(0.f) {}

		float r, g, b, a;
	};
}
