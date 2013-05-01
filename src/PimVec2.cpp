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
		A*factor + B*(1-factor)
	The factor should be a value between 0 and 1
	=====================
	*/
	Vec2 Vec2::Interpolate(Vec2 source, Vec2 dest, float factor) {
		return Vec2( 
			source.x * (1.f-factor) + dest.x * factor,
			source.y * (1.f-factor) + dest.y * factor
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
	Vec2::SingedAngleBetween
	=====================
	*/
	float Vec2::SignedAngleBetween(const Vec2 &other) const {
		float perpDot = x*other.y - y*other.x;
		float theta = atan2f(perpDot, Dot(other));

		return theta * RADTODEG;
	}

	/*
	=====================
	Vec2::AngleBetween
	=====================
	*/
	float Vec2::AngleBetween(const Vec2 &other) const {
		float dot = Dot(other);

		float len = Length() * other.Length();
		if (len == 0.f) len = 0.000001f;

		float angle = acosf(dot / len) * (180.f / M_PI);

		return angle;
	}

	/*
	=====================
	Vec2::Angle
	=====================
	*/
	float Vec2::Angle() const {
		//return atan2f(y, x) * (180.f / M_PI);
		float a = AngleBetween(Vec2(1.f, 0.f));
		
		if (Dot(Vec2(0.f, 1.f)) < 0.f) {
			a = (180.f - a);
			a += 180.f;
		}

		return a;
	}

	/*
	=====================
	Vec2::Dot
	=====================
	*/
	float Vec2::Dot(const Vec2 &other) const {
		return x * other.x + y * other.y;
	}

	/*
	=====================
	Vec2::Cross
	=====================
	*/
	float Vec2::Cross(const Vec2 &other) const {
		return x*other.y - y*other.x;
	}

	/*
	=====================
	Vec2::Normalize
	=====================
	*/
	Vec2 Vec2::Normalize() const {
		return Vec2(x,y) / Length();
	}

	/*
	=====================
	Vec2::Length
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
	Vec2::operator==
	=====================
	*/
	bool Vec2::operator==(const Vec2 &other) const  {
		return x == other.x && y == other.y;
	}

	/*
	=====================
	Vec2::operator!=
	=====================
	*/
	bool Vec2::operator!=(const Vec2 &other) const  {
		return x != other.x || y != other.y;
	}

	/*
	=====================
	Vec2::operator+
	=====================
	*/
	Vec2 Vec2::operator+(const Vec2 &other) const  {
		return Vec2(x+other.x, y+other.y);
	}

	/*
	=====================
	Vec2::operator-
	=====================
	*/
	Vec2 Vec2::operator-(const Vec2 &other) const  {
		return Vec2(x-other.x, y-other.y);
	}

	/*
	=====================
	Vec2::operator*
	=====================
	*/
	Vec2 Vec2::operator*(const Vec2 &other) const {
		return Vec2(x*other.x, y*other.y);
	}

	/*
	=====================
	Vec2::operator/
	=====================
	*/
	Vec2 Vec2::operator/(const Vec2 &other) const {
		return Vec2(x/(other.x==0.f?0.000001f:other.x),
					y/(other.y==0.f?0.000001f:other.y));
	}

	/*
	=====================
	Vec2::operator+=
	=====================
	*/
	void Vec2::operator+=(const Vec2 &other) {
		x += other.x;
		y += other.y;
	}

	/*
	=====================
	Vec2::operator-=
	=====================
	*/
	void Vec2::operator-=(const Vec2 &other) {
		x -= other.x;
		y -= other.y;
	}

	/*
	=====================
	Vec2::operator*=
	=====================
	*/
	void Vec2::operator*=(const Vec2 &other) {
		x *= other.x;
		y *= other.y;
	}

	/*
	=====================
	Vec2::operator/=
	=====================
	*/
	void Vec2::operator/=(const Vec2 &other) {
		x /= other.x==0.f?0.000001f:other.x;
		y /= other.y==0.f?0.000001f:other.y;
	}

	/*
	=====================
	Vec2::operator*
	=====================
	*/
	Vec2 Vec2::operator*(const float &fac) const {
		return Vec2(x*fac, y*fac);
	}

	/*
	=====================
	Vec2::operator/
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
	Vec2::operator*=
	=====================
	*/
	void Vec2::operator*=(const float &fac) {
		x *= fac;
		y *= fac;
	}

	/*
	=====================
	Vec2::operator/=
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
			source.r * (1.f-factor) + dest.r * factor,
			source.g * (1.f-factor) + dest.g * factor,
			source.b * (1.f-factor) + dest.b * factor,
			source.a * (1.f-factor) + dest.a * factor
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