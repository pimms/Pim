#pragma once

#include "PimInternal.h"

namespace Pim
{
	class Vec2
	{
	public:
		Vec2(float px, float py);
		Vec2(void);

		// Create a unit vector pointing in degree-angle 'a'
		static Vec2 unitD(float a);

		// Create a unit vector pointing in radian-angle 'a'
		static Vec2 unitR(float a);

		// [DOES NOT MODIFY THIS VECTOR]
		// Returns this vector, rotated around pt, a degrees
		Vec2 rotateAroundPoint(Vec2 &pt, float a);	

		// [DOES NOT MODIFY THIS VECTOR]
		// Same as rotateAroundPoint, but defaults to (0,0)
		Vec2 rotateDegrees(float a);				
		
		// Returns the difference in the range 0.0 to 360.0
		float angleBetween360(const Vec2 &other);

		// Returns the difference in the range -180.0 to 180.0
		float angleBetween(Vec2 &other);

		// Returns the dot product between this vector and the passed vector
		float dot(const Vec2 &other);

		// Returns the cross product between this vector and the passed vector
		float cross(const Vec2 &other);

		// Returns the unit length vector without altering any current values.
		Vec2 normalize();

		// Returns the length of this vector
		float length();

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

		bool contains(Pim::Vec2 &vec);

		png_int_32 x, y, width, height;

		// Operator overloading
		bool operator==(const Rect &other)
		{
			return x == other.x 
				&& y == other.y
				&& width == other.width
				&& height == other.height;
		}
	};

	struct Color
	{
		Color(float rr, float gg, float bb, float aa) : r(rr), g(gg), b(bb), a(aa) {}
		Color() : r(1.f), g(1.f), b(1.f), a(1.f) {}

		float r, g, b, a;
	};
}
