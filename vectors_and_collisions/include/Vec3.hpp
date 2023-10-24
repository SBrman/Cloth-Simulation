#pragma once
#include <string>

class Vec3
{
public:
    double x, y, z;

    Vec3();
    Vec3(double x, double y, double z);

    Vec3 operator+ (const Vec3& rhs); 
    Vec3 operator- (const Vec3& rhs);
    Vec3 operator* (double rhs);

    Vec3 operator/(double rhs);

    void operator+= (const Vec3& rhs); 
    void operator-= (const Vec3& rhs);
    void operator*= (double rhs);
    void operator/= (double rhs);

    std::string toString();
    double length();
    double lengthSqr();
    void clampToLength(double maxL);
    void setToLength(double newL);
    void normalize();
    Vec3 normalized();
    double distanceTo(Vec3 rhs);
};


double dot(Vec3 a, Vec3 b);
Vec3 cross(Vec3 a, Vec3 b);

Vec3 interpolate(Vec3 a, Vec3 b, double t);
Vec3 projAB(Vec3 a, Vec3 b);
Vec3 reflection(Vec3 vector, Vec3 normal);
