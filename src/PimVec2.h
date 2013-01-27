#pragma once

#include "PimInternal.h"

namespace Pim {
	class Vec2 {
	public:
		float				x;
		float				y;

							Vec2(const float px, const float py);
							Vec2(void);
		static Vec2			UnitDegree(const float angle);
		static Vec2			UnitRadian(const float angle);
		Vec2				RotateAroundPoint(const Vec2 &pt, const float a) const;
		Vec2				RotateDegrees(const float a) const;
		float				AngleBetween360(const Vec2 &other) const;	// Return range [0-360]
		float				AngleBetween(const Vec2 &other) const;		// Return range [-180-180]
		float				Dot(const Vec2 &other) const;
		float				Cross(const Vec2 &other) const;
		Vec2				Normalize() const;
		float				Length() const;
		bool				operator==(const Vec2 &other) const;
		bool				operator!=(const Vec2 &other) const;
		Vec2				operator+(const Vec2 &other) const;
		Vec2				operator-(const Vec2 &other) const;
		Vec2				operator*(const Vec2 &other) const;
		Vec2				operator/(const Vec2 &other) const;
		void				operator+=(const Vec2 &other);
		void				operator-=(const Vec2 &other);
		void				operator*=(const Vec2 &other);
		void				operator/=(const Vec2 &other);
		Vec2				operator*(const float &fac) const;
		Vec2				operator/(float den) const;
		void				operator*=(const float &fac);
		void				operator/=(float den);
	};


	struct Color {
		float				r;
		float				g;
		float				b;
		float				a;

							Color(const float rr, const float gg, 
								  const float bb, const float aa);
							Color();
	};


	/*
	===============================
	Class Rect_t

	The class is defined in the header for the sake of templating.
	By default, all rects used internally is <int>.
	===============================
	*/
	template <typename T>
	struct Rect_t {
		T 			x;
		T			y;
		T			width;
		T			height;

		
		Rect_t(T xpos, T ypos, T w, T h) {
			x		= xpos;
			y		= ypos;
			width	= w;
			height	= h;
		}

		Rect_t() {
			x		= T(0);
			y		= T(0);
			width	= T(0);
			height	= T(0);
		}

		template <typename Y>
		Rect_t(const Rect_t<Y> &other) {
			x		= T(other.x);
			y		= T(other.y);
			width	= T(other.width);
			height	= T(other.height);
		}

		template <typename Y>
		Rect_t(Y xpos, Y ypos, Y w, Y h) {
			x		= T(xpos);
			y		= T(ypos);
			width	= T(w);
			height	= T(h);
		}

		bool Contains(const Vec2 &vec) {
			return		vec.x > float(x) 
				&&		vec.x < float(x + width)
				&&		vec.y > float(y) 
				&&		vec.y < float(y + height);
		}

		template <typename Y>
		bool operator==(const Rect_t<Y> &other) {
			return		x		== T(other.x) 
				&&		y		== T(other.y) 
				&&		width	== T(other.width)
				&&		height	== T(other.height);
		}

		template <typename Y>
		bool operator!=(const Rect_t<Y> &other) {
			return		x		!= T(other.x) 
				||		y		!= T(other.y) 
				||		width	!= T(other.width)
				||		height	!= T(other.height);
		}

		template <typename Y>
		Rect_t<T> operator=(const Rect_t<Y> &other) {
			return Rect_t<T>(
				T(other.x), 
				T(other.y), 
				T(other.width),
				T(other.height)
			);
		}
	};

	typedef Rect_t<int>		Rect;		// Rect_t<int> is used internally by default
	typedef Rect_t<float>	Rectf;

	/*
	struct Rect {
		int			x;
		int			y;
		int			width;
		int			height;

		Rect(int xx, int yy, int ww, int hh) {
			x		= xx;
			y		= yy;
			width	= ww;
			height	= hh;
		}

		Rect() {
			x		= 0;
			y		= 0;
			width	= 0;
			height	= 0;
		}

		bool Contains(const Vec2 &vec) {
			return		vec.x > float(x) 
				&&		vec.x < float(x + width)
				&&		vec.y > float(y) 
				&&		vec.y < float(y + height);
		}

		bool operator==(const Rect &o) {
			return	x == o.x 
				&&  y == o.y
				&&  width == o.width 
				&& height == o.height;
		}

		bool operator!=(const Rect &o) {
			return	x != o.x
				&&  y != o.y
				&& width != o.width
				&& height != o.height;
		}
	};
	*/
}
