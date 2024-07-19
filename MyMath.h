#pragma once
#include<cmath>
#include<assert.h>
struct Vector2 {
	float x;
	float y;
};

struct Vector3 {
	float x;
	float y;
	float z;
};

struct Matrix4x4 {
	float m[4][4];
};

class MyMath
{
public:
	Vector3 Add(const Vector3& v1, const Vector3& v2);
	Vector3 Subtract(const Vector3& v1, const Vector3& v2);
	Vector3 MultiplyFV(float f, const Vector3& v);
	Vector3 MultiplyVV(const Vector3& v1, const Vector3& v2);

	Matrix4x4 Inverse(const Matrix4x4& m);

	Matrix4x4 Multiply(Matrix4x4 m1, Matrix4x4 m2);

	Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);

	Matrix4x4 MakePerspectiveFovMatrix(float forY, float aspectRatio, float nearClip, float farClip);

	Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);

};

