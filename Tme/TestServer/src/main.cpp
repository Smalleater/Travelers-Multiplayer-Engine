#include <iostream>
#include <Windows.h>

#include "TME/NetworkEngine.h"

int main()
{
	tme::ErrorCodes result;

	result = tme::NetworkEngine::Init();
	if (result != tme::ErrorCodes::Succes)
	{
		std::cout << "Tme network engine init failur\n";
	}

	Sleep(1);

	result = tme::NetworkEngine::Shutdown();
	if (result != tme::ErrorCodes::Succes)
	{
		std::cout << "Tme network engine shutdown failur\n";
	}
}