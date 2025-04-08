#include "Aimbot.h"
#include "Util.h"

void Aimbot::aimbot(Vector3 cameraPos, Vector3 targetPos, uintptr_t playerCameraManager) {
	//mouse_event(0x0001, (UINT) Angles.x, (UINT) Angles.y, NULL, NULL);

	Vector3 VectorPos = targetPos - cameraPos;

	float distance = sqrtf(VectorPos.x * VectorPos.x + VectorPos.y * VectorPos.y + VectorPos.z * VectorPos.z);
	float x, y;
	x = -((acosf(VectorPos.z / distance) * (float)(180.0f / M_PI)) - 90.f);
	y = atan2f(VectorPos.y, VectorPos.x) * (float)(180.0f / M_PI);

	// Write back the new angles
	Write<float>(playerCameraManager + 0x2394, x);
	Write<float>(playerCameraManager + 0x2398, x);

	Write<float>(playerCameraManager + 0x239c, y);
	Write<float>(playerCameraManager + 0x23a0, y);

}