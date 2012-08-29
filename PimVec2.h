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
}
