#pragma once

#include <Windows.h>
#include <cstdint>
#include "Globals.h"
#include <TlHelp32.h>
#include <tchar.h>
#include <iostream>
#include <d3d9types.h>

#define M_PI 3.14159265359

template <typename T>
static T Read(uintptr_t address) {
	T buffer;
	if (!ReadProcessMemory(GameVars.process, (LPCVOID)address, &buffer, sizeof(T), nullptr)) {
		//std::cout << "Failed to read memory at address 0x" << std::hex << address << "\n";
		return T{};
	}
	return buffer;
}

template <typename T>
static void Write(uintptr_t address, T value) {
	if (!WriteProcessMemory(GameVars.process, (LPVOID)address, &value, sizeof(T), nullptr)) {
		std::cout << "Failed to write memory at address 0x" << std::hex << address << "\n";
	}
}

inline double GetCrossDistance(double x1, double y1, double x2, double y2) {
	double dx = x2 - x1;
	double dy = y2 - y1;
	return sqrt(dx * dx + dy * dy);
}

inline DWORD GetProcessIdByName(const wchar_t* processName) {
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE) {
		return 0;
	}
	if (Process32First(hSnapshot, &pe32)) {
		do {
			if (wcscmp(pe32.szExeFile, processName) == 0) {
				CloseHandle(hSnapshot);
				return pe32.th32ProcessID;
			}
		} while (Process32Next(hSnapshot, &pe32));
	}

	CloseHandle(hSnapshot);
	return 0;
}

inline uintptr_t GetModuleBaseAddress(const TCHAR* lpszModuleName, uintptr_t pID) {
	uintptr_t dwModuleBaseAddress = 0;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pID);
	MODULEENTRY32 ModuleEntry32 = { 0 };
	ModuleEntry32.dwSize = sizeof(MODULEENTRY32);

	if (Module32First(hSnapshot, &ModuleEntry32))
	{
		do {
			if (_tcscmp(ModuleEntry32.szModule, lpszModuleName) == 0)
			{
				dwModuleBaseAddress = (uintptr_t)ModuleEntry32.modBaseAddr;
				break;
			}
		} while (Module32Next(hSnapshot, &ModuleEntry32));


	}
	CloseHandle(hSnapshot);
	return dwModuleBaseAddress;
}

template <typename T>
bool isDefault(const T& obj) {
	T defaultObj{}; // Create a named default-initialized instance
	return std::memcmp(&obj, &defaultObj, sizeof(T)) == 0;
}

template <class T>
class TArray
{
public:
	int Num() const
	{
		return count;
	}

	int getIdentifier()
	{
		return data + count * max;
	}

	bool isValid() const
	{
		if (count > max)
			return false;
		if (count < 0)
			return false;
		if (!data)
			return false;
		return true;
	}

	uint64_t getAddress() const
	{
		return data;
	}

	T operator [](size_t idx) const
	{
		return Read<T>(data + sizeof(T) * idx);
	}

protected:
	uintptr_t data;
	uint32_t count;
	uint32_t max;
};

struct Vector2 {
	float x = 0.0f;
	float y = 0.0f;

	Vector2() = default;
	Vector2(float x, float y) : x(x), y(y) {}
};

class Vector3
{
public:
	Vector3() : x(0.f), y(0.f), z(0.f)
	{

	}

	Vector3(double _x, double _y, double _z) : x(_x), y(_y), z(_z)
	{

	}
	~Vector3()
	{

	}

	double x;
	double y;
	double z;

	bool operator==(const Vector3& other) const {
		return x == other.x && y == other.y && z == other.z;
	}

	inline double Dot(Vector3 v)
	{
		return x * v.x + y * v.y + z * v.z;
	}

	inline double Distance(Vector3 v)
	{
		return double(sqrtf(powf(v.x - x, 2.0) + powf(v.y - y, 2.0) + powf(v.z - z, 2.0)));
	}

	inline double Length() {
		return sqrt(x * x + y * y + z * z);
	}

	inline void Normalize() {
		double len = Length();
		if (len > 0.0) {
			x /= len;
			y /= len;
			z /= len;
		}
	}

	inline double Size() const {
		return sqrt(x * x + y * y + z * z);
	}

	Vector3 operator+(Vector3 v)
	{
		return Vector3(x + v.x, y + v.y, z + v.z);
	}

	Vector3 operator-(Vector3 v)
	{
		return Vector3(x - v.x, y - v.y, z - v.z);
	}

	Vector3 operator*(double flNum) { return Vector3(x * flNum, y * flNum, z * flNum); }
};

const float Const_RadToUnrRot = 180.0f / 3.141592653589793f; // Constant to convert radians to Unreal rotations

class FRotator {
public:
	float Pitch, Yaw, Roll;

	FRotator() : Pitch(0), Yaw(0), Roll(0) {}

	// Constructor for creating an FRotator from pitch, yaw, and roll
	FRotator(float pitch, float yaw, float roll)
		: Pitch(pitch), Yaw(yaw), Roll(roll) {
	}

	// Method to convert a direction vector into a rotator (like Unreal's VectorToRotation)
	static FRotator VectorToRotation(const Vector3& vVector) {
		FRotator rRotation;

		// Yaw calculation (rotation around the Z axis)
		rRotation.Yaw = std::atan2(vVector.y, vVector.x) * Const_RadToUnrRot;

		// Pitch calculation (rotation around the X axis)
		rRotation.Pitch = std::atan2(vVector.z, std::sqrt(vVector.x * vVector.x + vVector.y * vVector.y)) * Const_RadToUnrRot;

		// Roll is often not needed unless there's a specific requirement
		rRotation.Roll = 0.0f;

		return rRotation;
	}
};

struct FQuat
{
	double x;
	double y;
	double z;
	double w;
};


struct FTransform
{
	FQuat rot;
	Vector3 translation;
	char pad[4];
	Vector3 scale;
	char pad1[4];

	D3DMATRIX ToMatrixWithScale()
	{
		D3DMATRIX m;
		m._41 = translation.x;
		m._42 = translation.y;
		m._43 = translation.z;

		float x2 = rot.x + rot.x;
		float y2 = rot.y + rot.y;
		float z2 = rot.z + rot.z;

		float xx2 = rot.x * x2;
		float yy2 = rot.y * y2;
		float zz2 = rot.z * z2;
		m._11 = (1.0f - (yy2 + zz2)) * scale.x;
		m._22 = (1.0f - (xx2 + zz2)) * scale.y;
		m._33 = (1.0f - (xx2 + yy2)) * scale.z;

		float yz2 = rot.y * z2;
		float wx2 = rot.w * x2;
		m._32 = (yz2 - wx2) * scale.z;
		m._23 = (yz2 + wx2) * scale.y;

		float xy2 = rot.x * y2;
		float wz2 = rot.w * z2;
		m._21 = (xy2 - wz2) * scale.y;
		m._12 = (xy2 + wz2) * scale.x;

		float xz2 = rot.x * z2;
		float wy2 = rot.w * y2;
		m._31 = (xz2 + wy2) * scale.z;
		m._13 = (xz2 - wy2) * scale.x;

		m._14 = 0.0f;
		m._24 = 0.0f;
		m._34 = 0.0f;
		m._44 = 1.0f;

		return m;
	}
};

inline void FindComponentToWorld(uintptr_t actor_mesh)
{
	for (int i = 150; i < 800; i++) // usually somehwere around 0x1C0
	{
		std::cout << i << "\n";
		FTransform res = Read<FTransform>(actor_mesh + i);
		if (res.translation.x > 0 && res.translation.x < 20 && res.translation.y > 0 && res.translation.y < 20 && res.translation.z > 0 && res.translation.z < 20) {
			std::cout << " - C2WTranslation: " << res.translation.x << ", " << res.translation.y << ", " << res.translation.z << "\n";

			std::cout << "Found Match: " << i << "\n";
			system("PAUSE");
		}
	}
}

struct Camera
{
	Vector3 Location;
	Vector3 Rotation;
	float FieldOfView;
};

inline D3DMATRIX MatrixMultiplication(D3DMATRIX pM1, D3DMATRIX pM2)
{
	D3DMATRIX pOut;
	pOut._11 = pM1._11 * pM2._11 + pM1._12 * pM2._21 + pM1._13 * pM2._31 + pM1._14 * pM2._41;
	pOut._12 = pM1._11 * pM2._12 + pM1._12 * pM2._22 + pM1._13 * pM2._32 + pM1._14 * pM2._42;
	pOut._13 = pM1._11 * pM2._13 + pM1._12 * pM2._23 + pM1._13 * pM2._33 + pM1._14 * pM2._43;
	pOut._14 = pM1._11 * pM2._14 + pM1._12 * pM2._24 + pM1._13 * pM2._34 + pM1._14 * pM2._44;
	pOut._21 = pM1._21 * pM2._11 + pM1._22 * pM2._21 + pM1._23 * pM2._31 + pM1._24 * pM2._41;
	pOut._22 = pM1._21 * pM2._12 + pM1._22 * pM2._22 + pM1._23 * pM2._32 + pM1._24 * pM2._42;
	pOut._23 = pM1._21 * pM2._13 + pM1._22 * pM2._23 + pM1._23 * pM2._33 + pM1._24 * pM2._43;
	pOut._24 = pM1._21 * pM2._14 + pM1._22 * pM2._24 + pM1._23 * pM2._34 + pM1._24 * pM2._44;
	pOut._31 = pM1._31 * pM2._11 + pM1._32 * pM2._21 + pM1._33 * pM2._31 + pM1._34 * pM2._41;
	pOut._32 = pM1._31 * pM2._12 + pM1._32 * pM2._22 + pM1._33 * pM2._32 + pM1._34 * pM2._42;
	pOut._33 = pM1._31 * pM2._13 + pM1._32 * pM2._23 + pM1._33 * pM2._33 + pM1._34 * pM2._43;
	pOut._34 = pM1._31 * pM2._14 + pM1._32 * pM2._24 + pM1._33 * pM2._34 + pM1._34 * pM2._44;
	pOut._41 = pM1._41 * pM2._11 + pM1._42 * pM2._21 + pM1._43 * pM2._31 + pM1._44 * pM2._41;
	pOut._42 = pM1._41 * pM2._12 + pM1._42 * pM2._22 + pM1._43 * pM2._32 + pM1._44 * pM2._42;
	pOut._43 = pM1._41 * pM2._13 + pM1._42 * pM2._23 + pM1._43 * pM2._33 + pM1._44 * pM2._43;
	pOut._44 = pM1._41 * pM2._14 + pM1._42 * pM2._24 + pM1._43 * pM2._34 + pM1._44 * pM2._44;

	return pOut;
}
inline D3DMATRIX Matrix(Vector3 rot, Vector3 origin) {
	float radPitch = (rot.x * float(M_PI) / 180.f);
	float radYaw = (rot.y * float(M_PI) / 180.f);
	float radRoll = (rot.z * float(M_PI) / 180.f);

	float SP = sinf(radPitch);
	float CP = cosf(radPitch);
	float SY = sinf(radYaw);
	float CY = cosf(radYaw);
	float SR = sinf(radRoll);
	float CR = cosf(radRoll);

	D3DMATRIX matrix;
	matrix.m[0][0] = CP * CY;
	matrix.m[0][1] = CP * SY;
	matrix.m[0][2] = SP;
	matrix.m[0][3] = 0.f;

	matrix.m[1][0] = SR * SP * CY - CR * SY;
	matrix.m[1][1] = SR * SP * SY + CR * CY;
	matrix.m[1][2] = -SR * CP;
	matrix.m[1][3] = 0.f;

	matrix.m[2][0] = -(CR * SP * CY + SR * SY);
	matrix.m[2][1] = CY * SR - CR * SP * SY;
	matrix.m[2][2] = CR * CP;
	matrix.m[2][3] = 0.f;

	matrix.m[3][0] = origin.x;
	matrix.m[3][1] = origin.y;
	matrix.m[3][2] = origin.z;
	matrix.m[3][3] = 1.f;

	return matrix;
}

inline Vector3 fTransformToVector3(FTransform fTransform, FTransform C2W) {
	D3DMATRIX matrix = MatrixMultiplication(fTransform.ToMatrixWithScale(), C2W.ToMatrixWithScale());
	return Vector3(matrix._41, matrix._42, matrix._43);
}

inline Vector2 doMatrix(Vector3 position, Camera camera) {

	D3DMATRIX tempMatrix = Matrix(camera.Rotation, Vector3(0, 0, 0));

	Vector3 vAxisX = Vector3(tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2]);
	Vector3 vAxisY = Vector3(tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2]);
	Vector3 vAxisZ = Vector3(tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2]);

	Vector3 vDelta = position - camera.Location;

	Vector3 vTransformed = Vector3(vDelta.Dot(vAxisY), vDelta.Dot(vAxisZ), vDelta.Dot(vAxisX));

	if (vTransformed.z < 1.f)
		vTransformed.z = 1.f;

	return Vector2((GameVars.width / 2.0f) + vTransformed.x * (((GameVars.width / 2.0f) / tanf(camera.FieldOfView * (float)M_PI / 360.f))) / vTransformed.z, (GameVars.height / 2.0f) - vTransformed.y * (((GameVars.height / 2.0f) / tanf(camera.FieldOfView * (float)M_PI / 360.f))) / vTransformed.z);
}

inline Vector2 doMatrix(FTransform Bone, FTransform C2W, Camera camera) {
	D3DMATRIX matrix = MatrixMultiplication(Bone.ToMatrixWithScale(), C2W.ToMatrixWithScale());
	Vector3 BoneV = Vector3(matrix._41, matrix._42, matrix._43);

	D3DMATRIX tempMatrix = Matrix(camera.Rotation, Vector3(0, 0, 0));

	Vector3 vAxisX = Vector3(tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2]);
	Vector3 vAxisY = Vector3(tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2]);
	Vector3 vAxisZ = Vector3(tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2]);

	Vector3 vDelta = BoneV - camera.Location;

	Vector3 vTransformed = Vector3(vDelta.Dot(vAxisY), vDelta.Dot(vAxisZ), vDelta.Dot(vAxisX));

	if (vTransformed.z < 1.f)
		vTransformed.z = 1.f;

	camera.FieldOfView = 100.f;

	return Vector2((GameVars.width / 2.0f) + vTransformed.x * (((GameVars.width / 2.0f) / tanf(camera.FieldOfView * (float)M_PI / 360.f))) / vTransformed.z, (GameVars.height / 2.0f) - vTransformed.y * (((GameVars.width / 2.0f) / tanf(camera.FieldOfView * (float)M_PI / 360.f))) / vTransformed.z);
}