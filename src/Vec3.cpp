#include <cmath>
#include "Vec3.hpp"

Vec3::Vec3(): x(0), y(0), z(0) { }

Vec3::Vec3(double x, double y, double z) { 
    this->x = x; 
    this->y = y; 
    this->z = z;
}

Vec3 Vec3::operator+ (const Vec3& rhs) {
	return Vec3(x + rhs.x, y + rhs.y, z + rhs.z);
}

Vec3 Vec3::operator- (const Vec3& rhs) {
	return Vec3(x - rhs.x, y - rhs.y, z - rhs.z);
}

Vec3 Vec3::operator* (double rhs) {
	return Vec3(x * rhs, y * rhs, z * rhs);
}

Vec3 Vec3::operator/ (double rhs) {
	return Vec3(x / rhs, y / rhs, z / rhs);
}

void Vec3::operator+= (const Vec3& rhs) {
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;
}

void Vec3::operator-= (const Vec3& rhs) {
	x -= rhs.x;
	y -= rhs.y;
	z -= rhs.z;
}

void Vec3::operator*= (double rhs) {
	x *= rhs;
	y *= rhs;
	z *= rhs;
}

void Vec3::operator/= (double rhs) {
	x /= rhs;
	y /= rhs;
	z /= rhs;
}

std::string Vec3::toString() {
    return "Vector(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")";
}

double Vec3::length() {
    return std::sqrt(x * x + y * y + z * z);
}

double Vec3::lengthSqr() {
    return x * x + y * y + z * z;
}

void Vec3::clampToLength(double maxL) {
    double magnitude = std::sqrt(x * x + y * y + z * z);
    if (magnitude > maxL) {
        x *= maxL / magnitude;
        y *= maxL / magnitude;
        z *= maxL / magnitude;
    }
}

void Vec3::setToLength(double newL) {
    double magnitude = std::sqrt(x * x + y * y + z * z);
    x *= newL / magnitude;
    y *= newL / magnitude;
    z *= newL / magnitude;
}

void Vec3::normalize() {
    double magnitude = std::sqrt(x * x + y * y + z * z);
    x /= magnitude;
    y /= magnitude;
    z /= magnitude;
}

Vec3 Vec3::normalized() {
    double magnitude = std::sqrt(x * x + y * y + z * z);
    if (magnitude > 1000000 || magnitude <= 0) return Vec3{ 0.0, 0.0, 0.0 };
    return Vec3(x / magnitude, y / magnitude, z / magnitude);
}

double Vec3::distanceTo(Vec3 rhs) {
    double dx = rhs.x - x;
    double dy = rhs.y - y;
    double dz = rhs.z - z;
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}

Vec3 interpolate(Vec3 a, Vec3 b, double t) {
    return a + ((b - a) * t);
}

double dot(Vec3 a, Vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vec3 cross(Vec3 a, Vec3 b) {
    return Vec3{a.y * b.z - b.y * a.z, a.z * b.x - b.z * a.x, a.x * b.y - b.x * a.y};
}

Vec3 projAB(Vec3 a, Vec3 b) {
    return b * (a.x * b.x + a.y * b.y + a.z * b.z);
}

Vec3 reflection(Vec3 vector, Vec3 normal) {
    return vector - normal.normalized() * dot(vector, normal.normalized()) * 2;
}