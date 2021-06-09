#include "shared.hpp"

DWORD WINAPI on_attach(LPVOID lp_param)
{
	try
	{
		hooks::init();
	}
	catch (const std::exception& ex)
	{
		MessageBoxA(nullptr, ex.what(), "ERROR", MB_OK | MB_ICONERROR | MB_TOPMOST);
		FreeLibraryAndExitThread(static_cast<HMODULE>(lp_param), EXIT_FAILURE);
	}

	while (!(GetAsyncKeyState(VK_END) & 0x8000))
		Sleep(50);

    FreeLibraryAndExitThread(static_cast<HMODULE>(lp_param), EXIT_SUCCESS);
}

BOOL WINAPI on_detach()
{
	hooks::destroy();
	return TRUE;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpParam)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hModule);
		if (HANDLE thread = CreateThread(nullptr, NULL, on_attach, hModule, NULL, nullptr))
		{
			CloseHandle(thread);
			return TRUE;
		}
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		return on_detach();
	}

	return FALSE;
}