#include <iostream>
#include "Globals.h"
#include <Windows.h>
#include "Util.h"
#include "Game.h"

int main()
{
	GameVars.pid = GetProcessIdByName(L"Beached-Win64-Shipping.exe");

	if (GameVars.pid == 0) {
		std::cout << "Beached not found\n";
		exit(1);
	}

	GameVars.process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, GameVars.pid);
	GameVars.baseAddress = GetModuleBaseAddress(TEXT("Beached-Win64-Shipping.exe"), GameVars.pid);
	
	Game::start();

}