#pragma once

#include <Windows.h>
#include <string>
#include "singleton.h"

inline namespace MarvelDMA
{
	class Variables : public Singleton<Variables>
	{
	public:
		HANDLE process;
		uintptr_t baseAddress;
		DWORD pid;

		bool aimbotting = false;

		int height;
		int width;
	};
#define GameVars MarvelDMA::Variables::Get()

	class Offsets : public Singleton<Offsets> {
	public:

		uintptr_t uworld = 0x739E8F8;
		uintptr_t gameInstance = 0x1B8;
		uintptr_t gameState = 0x158;
		uintptr_t localPlayers = 0x38;
		uintptr_t playerController = 0x30;
		uintptr_t localPawn = 0x340;
		uintptr_t playerCameraManager = 0x350;
		uintptr_t playerCameraCache = 0x1320;
		uintptr_t playerCameraPOV = 0x10;
		uintptr_t playerArray = 0x2b0;
		uintptr_t pawnPrivate = 0x310;
		uintptr_t mesh = 0x320;
	};
#define GameOffset MarvelDMA::Offsets::Get()
	class Settings : public Singleton<Settings>
	{
	public:
		float smoothing = 1;
		int fovSize = 150;
	};
#define GameSetting MarvelDMA::Settings::Get()
}