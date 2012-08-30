#pragma once

class PVec2
{
public:
	PVec2(float px, float py);
	PVec2(void);

	float x, y;
	
	// Operator overloading
	bool operator==(const PVec2 &other);
	bool operator!=(const PVec2 &other);

	PVec2 operator+(const PVec2 &other);
	PVec2 operator-(const PVec2 &other);
	PVec2 operator*(const PVec2 &other);
	PVec2 operator/(const PVec2 &other);

	void operator+=(const PVec2 &other);
	void operator-=(const PVec2 &other);
	void operator*=(const PVec2 &other);
	void operator/=(const PVec2 &other);

	PVec2 operator*(const float &fac);
	PVec2 operator/(float den);
	void operator*=(const float &fac);
	void operator/=(float den);
};

