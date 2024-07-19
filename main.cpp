#include <Novice.h>
#include "MyMath.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include<imgui.h>

const char kWindowTitle[] = "LE2C_07_オオサキ_ハルキ_タイトル";

struct ConicalPendulum {
	Vector3 anchor;
	float length;
	float halfApexAngle;
	float angle;
	float angularVelocity;
};




MyMath* myMath = new MyMath();

Vector3 operator+(const Vector3& v1, const Vector3& v2) { return myMath->Add(v1, v2); }
Vector3 operator-(const Vector3& v1, const Vector3& v2) { return myMath->Subtract(v1, v2); }
Vector3 operator*(float s, const Vector3& v) { return myMath->MultiplyFV(s, v); }
Vector3 operator*(const Vector3& v1, const Vector3& v2) { return myMath->MultiplyVV(v1, v2); }
Vector3 operator*(const Vector3& v, float s) { return s * v; }
Vector3 operator/(const Vector3& v, float s) { return myMath->MultiplyFV(1.0f / s, v); }


Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix) {
	Vector3 result{};

	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + 1.0f * matrix.m[3][0];
	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + 1.0f * matrix.m[3][1];
	result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + 1.0f * matrix.m[3][2];
	float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + 1.0f * matrix.m[3][3];
	assert(w != 0.0f);
	result.x /= w;
	result.y /= w;
	result.z /= w;

	return result;
}

void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {
	const float kGridHandleWidth = 2.0f;
	const uint32_t kSubdivision = 10;
	const float kGridEvery = (kGridHandleWidth * 2.0f) / float(kSubdivision);

	for (uint32_t xIndex = 0; xIndex <= kSubdivision; ++xIndex) {
		float x = -kGridHandleWidth + (xIndex * kGridEvery);
		unsigned int color = 0xAAAAAAFF;
		Vector3 start{ x,0.0f,-kGridHandleWidth };
		Vector3 end{ x,0.0f,kGridHandleWidth };

		Vector3 startScreen = Transform(Transform(start, viewProjectionMatrix), viewportMatrix);
		Vector3 endScreen = Transform(Transform(end, viewProjectionMatrix), viewportMatrix);

		if (x == 0.0f) {
			color = BLACK;
		}
		Novice::DrawLine((int)startScreen.x, (int)startScreen.y, (int)endScreen.x, (int)endScreen.y, color);
	}
	for (uint32_t zIndex = 0; zIndex <= kSubdivision; ++zIndex) {
		float z = -kGridHandleWidth + (zIndex * kGridEvery);
		unsigned int color = 0xAAAAAAFF;
		Vector3 start{ -kGridHandleWidth,0.0f,z };
		Vector3 end{ kGridHandleWidth,0.0f,z };

		Vector3 startScreen = Transform(Transform(start, viewProjectionMatrix), viewportMatrix);
		Vector3 endScreen = Transform(Transform(end, viewProjectionMatrix), viewportMatrix);

		if (z == 0.0f) {
			color = BLACK;
		}
		Novice::DrawLine((int)startScreen.x, (int)startScreen.y, (int)endScreen.x, (int)endScreen.y, color);

	}
}

struct Sphere {
	Vector3 center;
	float radius;
};


void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	const uint32_t kSubdivision = 30;
	const float kLonEvery = float(M_PI) / 8.0f;
	const float kLatEvery = float(M_PI) / 8.0f;

	float pi = float(M_PI);

	Vector3 pointAB[kSubdivision] = {};
	Vector3 pointAC[kSubdivision] = {};

	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		float lat = -pi / 2.0f + kLatEvery * latIndex;//緯度 シ－タ

		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {

			float lon = lonIndex * kLonEvery;//経度　ファイ

			Vector3 a{ sphere.center.x + sphere.radius * std::cos(lat) * std::cos(lon), sphere.center.y + sphere.radius * std::sin(lat),sphere.center.z + sphere.radius * std::cos(lat) * std::sin(lon) };
			Vector3 b{ sphere.center.x + sphere.radius * std::cos(lat + lat) * std::cos(lon), sphere.center.y + sphere.radius * std::sin(lat + lat),sphere.center.z + sphere.radius * std::cos(lat + lat) * std::sin(lon) };
			Vector3 c{ sphere.center.x + sphere.radius * std::cos(lat) * std::cos(lon + lon), sphere.center.y + sphere.radius * std::sin(lat), sphere.center.z + sphere.radius * std::cos(lat) * std::sin(lon + lon) };


			Vector3 aScreen = Transform(Transform(a, viewProjectionMatrix), viewportMatrix);
			Vector3 bScreen = Transform(Transform(b, viewProjectionMatrix), viewportMatrix);
			Vector3 cScreen = Transform(Transform(c, viewProjectionMatrix), viewportMatrix);


			if (pointAB[latIndex].x != 0 && pointAB[lonIndex].x != 0) {
				Novice::DrawLine((int)aScreen.x, (int)aScreen.y, (int)pointAB[latIndex].x, (int)pointAB[latIndex].y, color);
				Novice::DrawLine((int)aScreen.x, (int)aScreen.y, (int)pointAC[lonIndex].x, (int)pointAC[lonIndex].y, color);
			}

			pointAB[latIndex] = aScreen;
			pointAC[lonIndex] = aScreen;
		}
	}
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

	ConicalPendulum conicalpendulum;
	conicalpendulum.anchor = { 0.0f,1.0f,0.0f }; //アンカーポイント
	conicalpendulum.length = 0.8f; //紐の長さ
	conicalpendulum.halfApexAngle = 0.7f; //円錐の頂角の半分
	conicalpendulum.angle = 0.0f; //角度
	conicalpendulum.angularVelocity = 0.0f; //　角速度



	Vector3 p = { 0.0f,0.0f,0.0f };
	float radiusBall = 0.06f;


	float deltaTime = 1.0f / 60.0f;

	Vector3 cameraPosition = { 0.0f ,0.0f,-14.0f };
	Vector3 cameraTranslate = { 0.0f,0.0f,-6.49f };
	Vector3 cameraRotate = { -0.26f,0.0f,0.0f };

	bool start = false;


	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///

		Matrix4x4 worldMatrix = myMath->MakeAffineMatrix({ 1.0f,1.0f,1.0f }, cameraRotate, cameraTranslate);
		Matrix4x4 cameraMatrix = myMath->MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, cameraPosition);
		Matrix4x4 viewMatrix = myMath->Inverse(cameraMatrix);
		Matrix4x4 projectionMatrix = myMath->MakePerspectiveFovMatrix(0.45f, float(1280.0f) / float(720.0f), 0.1f, 100.0f);
		Matrix4x4 WorldViewProjectionMatrix = myMath->Multiply(worldMatrix, myMath->Multiply(viewMatrix, projectionMatrix));
		Matrix4x4 viewportMatrix = myMath->MakeViewportMatrix(0, 0, float(1280.0f), float(720.0f), 0.0f, 1.0f);


		DrawGrid(WorldViewProjectionMatrix, viewportMatrix);

		Sphere sphere = {
			p,
			radiusBall
		};

		if (start) {
			conicalpendulum.angularVelocity = std::sqrt(9.8f / (conicalpendulum.length * std::cos(conicalpendulum.halfApexAngle)));
			conicalpendulum.angle += conicalpendulum.angularVelocity * deltaTime;
		}
			float radius = std::sin(conicalpendulum.halfApexAngle) * conicalpendulum.length;
			float height = std::cos(conicalpendulum.halfApexAngle) * conicalpendulum.length;
			p.x = conicalpendulum.anchor.x + std::sin(conicalpendulum.angle) * radius;
			p.y = conicalpendulum.anchor.y - height;
			p.z = conicalpendulum.anchor.z - std::cos(conicalpendulum.angle) * radius;


		Vector3 startLine = Transform(Transform(conicalpendulum.anchor, WorldViewProjectionMatrix), viewportMatrix);
		Vector3 endLine = Transform(Transform(sphere.center, WorldViewProjectionMatrix), viewportMatrix);

		Novice::DrawLine((int)startLine.x, (int)startLine.y, (int)endLine.x, (int)endLine.y, WHITE);



		DrawSphere(sphere, WorldViewProjectionMatrix, viewportMatrix, BLUE);




		ImGui::Begin("window");
		ImGui::DragFloat3("CameraTranslate", &cameraTranslate.x, 0.01f);
		ImGui::DragFloat3("CameraRotate", &cameraRotate.x, 0.01f);

		ImGui::Checkbox("start", &start);

		ImGui::End();


		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}
