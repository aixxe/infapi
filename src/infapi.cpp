#include <MinHook.h>
#include "server.hpp"
#include "imports/eacnet.hpp"

static void* (*original_boot_fn) (void*, void*, void*, void*, void*, void*) = nullptr;

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID)
{
	if (reason != DLL_PROCESS_ATTACH)
        return TRUE;

    DisableThreadLibraryCalls(module);

    AttachConsole(ATTACH_PARENT_PROCESS);

    freopen_s(reinterpret_cast<FILE**>(stderr), "CONOUT$", "w", stderr);
    freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);

    MH_Initialize();

    MH_CreateHookApiEx(L"avs2-core.dll", "XCgsqzn0000129", reinterpret_cast<void*>(+[]
		(void* a1, void* a2, void* a3, void* a4, void* a5, void* a6)
	{
        original_boot_fn(a1, a2, a3, a4, a5, a6);

        eacnet::resolve_functions();
        eacapi::server::listen();

        ExitProcess(EXIT_SUCCESS);
	}), reinterpret_cast<LPVOID*>(&original_boot_fn), nullptr);

    MH_EnableHook(MH_ALL_HOOKS);

    return TRUE;
}