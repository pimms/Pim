#include "PimInternal.h"
#include "PimVec2.h"

namespace Pim {
	/*
	=====================
	Vec2::Vec2
	=====================
	*/
	Vec2::Vec2(const float px, const float py) {
		x	= px;
		y	= py;
	}

	/*
	=====================
	Vec2::Vec2
	=====================
	*/
	Vec2::Vec2(void) {
		x	= 0.f;
		y	= 0.f;
	}

	/*
	=====================
	Vec2::UnitDegree
	=====================
	*/
	Vec2 Vec2::UnitDegree(const float a) {
		return UnitRadian(a * (float(M_PI)/180.f));
	}

	/*
	=====================
	Vec2::UnitRadian
	=====================
	*/
	Vec2 Vec2::UnitRadian(const float a) {
		return Vec2(cosf(a), sinf(a));
	}

	/*
	=====================
	STATIC Vec2::Interpolate

	Given two vectors A and B, the method returns:
		A - ((A-B) * factor)
	The factor should be a value between 0 and 1
	=====================
	*/
	Vec2 Vec2::Interpolate(Vec2 source, Vec2 dest, float factor) {
		return Vec2( 
			source.x - ((source.x-dest.x) * factor),
			source.y - ((source.y-dest.y) * factor)
			);
	}

	/*
	=====================
	Vec2::RotateAroundPoint
	=====================
	*/
	Vec2 Vec2::RotateAroundPoint(const Vec2 &pt, const float a) const {
		return RotateDegrees(a) + pt;
	}

	/*
	=====================
	Vec2::RotateDegrees
	=====================
	*/
	Vec2 Vec2::RotateDegrees(const float a) const {
		float r = a * (float(M_PI) / 180.f);
		return Vec2( x*cosf(r) - y*sinf(r),
					 y*cosf(r) + x*sinf(r)
				   );
	}

	/*
	=====================
	Vec2::AngleBetween360
	=====================
	*/
	float Vec2::AngleBetween360(const Vec2 &other) const {
		float a = (atan2f(y, x) - atan2f(other.y, other.x)) * (180.f / M_PI);

		if (a < 0.f) {
			a += 360.f;
		}

		return a;
	}

	/*
	=====================
	Vec2::AngleBetween
	=====================
	*/
	float Vec2::AngleBetween(const Vec2 &other) const {
		float dot = Normalize().Dot(other.Normalize());
		float angle = acosf(dot) * (180.f / M_PI);

		Vec2 diff = Vec2(x,y) - other;
		if (diff.Dot(Vec2(0.f,1.f)) < 0.f) {
			angle = -angle;
		}

		return angle;
	}

	/*
	=====================
	Vec2::Angle
	=====================
	*/
	float Vec2::Angle() const {
		return atan2f(y, x) * (180.f / M_PI);
	}

	/*
	=====================
	Vec2::Vec2
	=====================
	*/
	float Vec2::Dot(const Vec2 &other) const {
		return x * other.x + y * other.y;
	}

	/*
	=====================
	Vec2::Vec2
	=====================
	*/
	float Vec2::Cross(const Vec2 &other) const {
		return x*other.y - y*other.x;
	}

	/*
	=====================
	Vec2::Vec2
	=====================
	*/
	Vec2 Vec2::Normalize() const {
		return Vec2(x,y) / Length();
	}

	/*
	=====================
	Vec2::Vec2
	=====================
	*/
	float Vec2::Length() const {
		return sqrt(x*x + y*y);
	}

	/*
	=====================
	Vec2::GLVertex
	=====================
	*/
	void Vec2::GLVertex() const {
		glVertex2f(x, y);
	}

	/*
	=====================
	Vec2::Vec2
	=====================
	*/
	bool Vec2::operator==(const Vec2 &other) const  {
		return x == other.x && y == other.y;
	}

	/*
	=====================
	Vec2::Vec2
	=====================
	*/
	bool Vec2::operator!=(const Vec2 &other) const  {
		return x != other.x || y != other.y;
	}

	/*
	=====================
	Vec2::Vec2
	=====================
	*/
	Vec2 Vec2::operator+(const Vec2 &other) const  {
		return Vec2(x+other.x, y+other.y);
	}

	/*
	=====================
	Vec2::Vec2
	=====================
	*/
	Vec2 Vec2::operator-(const Vec2 &other) const  {
		return Vec2(x-other.x, y-other.y);
	}

	/*
	=====================
	Vec2::Vec2
	=====================
	*/
	Vec2 Vec2::operator*(const Vec2 &other) const {
		return Vec2(x*other.x, y*other.y);
	}

	/*
	=====================
	Vec2::Vec2
	=====================
	*/
	Vec2 Vec2::operator/(const Vec2 &other) const {
		return Vec2(x/(other.x==0.f?0.000001f:other.x),
					y/(other.y==0.f?0.000001f:other.y));
	}

	/*
	=====================
	Vec2::Vec2
	=====================
	*/
	void Vec2::operator+=(const Vec2 &other) {
		x += other.x;
		y += other.y;
	}

	/*
	=====================
	Vec2::Vec2
	=====================
	*/
	void Vec2::operator-=(const Vec2 &other) {
		x -= other.x;
		y -= other.y;
	}

	/*
	=====================
	Vec2::Vec2
	=====================
	*/
	void Vec2::operator*=(const Vec2 &other) {
		x *= other.x;
		y *= other.y;
	}

	/*
	=====================
	Vec2::Vec2
	=====================
	*/
	void Vec2::operator/=(const Vec2 &other) {
		x /= other.x==0.f?0.000001f:other.x;
		y /= other.y==0.f?0.000001f:other.y;
	}

	/*
	=====================
	Vec2::Vec2
	=====================
	*/
	Vec2 Vec2::operator*(const float &fac) const {
		return Vec2(x*fac, y*fac);
	}

	/*
	=====================
	Vec2::Vec2
	=====================
	*/
	Vec2 Vec2::operator/(float den) const {
		if (den == 0.f) {
			den = 0.000001f;
		}

		return Vec2(x/den, y/den);
	}

	/*
	=====================
	Vec2::Vec2
	=====================
	*/
	void Vec2::operator*=(const float &fac) {
		x *= fac;
		y *= fac;
	}

	/*
	=====================
	Vec2::Vec2
	=====================
	*/
	void Vec2::operator/=(float den) {
		if (den == 0.f) den = 0.000001f;
		x /= den;
		y /= den;
	}


	/*
	=====================
	Color::Interpolate

	Given two colors A and B and a factor F, the returned color is:
	A - ((A-B) * F)

	Color source and Color dest should be valid colors, and factor should be between 0 and 1.
	=====================
	*/
	Color Color::Interpolate(const Color &source, const Color &dest, float factor) {
		return Color(
			source.r - (source.r-dest.r) * factor,
			source.g - (source.g-dest.g) * factor,
			source.b - (source.b-dest.b) * factor,
			source.a - (source.a-dest.a) * factor
			);
	}

	/*
	=====================
	Color::Color
	=====================
	*/
	Color::Color(const float rr, const float gg, const float bb, const float aa) {
		r	= rr;
		g	= gg;
		b	= bb;
		a	= aa;
	}
	
	/*
	=====================
	Color::Color
	=====================
	*/
	Color::Color() {
		r	= 0.f;
		g	= 0.f;
		b	= 0.f;
		a	= 0.f;
	}
}