#pragma once

namespace Pim
{
	class Vec2
	{
	public:
		Vec2(float px, float py);
		Vec2(void);

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
		Rect(float xx, float yy, float w, float h) : x(xx), y(yy), width(w), height(h){}
		Rect() : x(0.f), y(0.f), width(0.f), height(0.f){}

		float x, y, width, height;
	};

	struct Color
	{
		Color(float rr, float gg, float bb, float aa) : r(rr), g(gg), b(bb), a(aa) {}
		Color() : r(0.f), g(0.f), b(0.f), a(0.f) {}

		float r, g, b, a;
	};
}
