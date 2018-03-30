#ifndef MATH_FUNCTIONS_H
#define MATH_FUNCTIONS_H

Vector3D Lerp(const Vector3D& vectorA, const Vector3D& vectorB, float t, bool isEaseIn = false, bool isEaseOut = false);
Vector2D Lerp(const Vector2D& vectorA, const Vector2D& vectorB, float t, bool isEaseIn = false, bool isEaseOut = false);
float Lerp(float valueA, float valueB, float t, bool isEaseIn = false, bool isEaseOut = false);
float SinWave(float amplitude, float frequency, float phaseShift, float time);
float CosWave(float amplitude, float frequency, float phaseShift, float time);
float RandomFloat(float min, float max);
int RandomInt(int min, int max);

unsigned int GetPascalEntry(unsigned short degree, unsigned short index);
float BezierInterpolation(const std::vector<Point>& points, const float& t);

// Given an angle, returns a value between -180 and +180
float ClampAngleTo180(float angle);
#endif