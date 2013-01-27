#include "PimInternal.h"
#include "PimVec2.h"

namespace Pim {

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
	STATIC Vec2::UnitDegree
	=====================
	*/
	Vec2 Vec2::UnitDegree(const float a) {
		return UnitRadian(a * (float(M_PI)/180.f));
	}

	/*
	=====================
	STATIC Vec2::UnitRadian
	=====================
	*/
	Vec2 Vec2::UnitRadian(const float a) {
		return Vec2(cosf(a), sinf(a));
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
		float a = (atan2f(other.y, other.x) - atan2f(y, x)) * (180.f / M_PI);

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
}