#include <windows.h>
#include <spdlog/spdlog.h>

int main(int argc, char* argv[])
{
    auto const args = std::vector<std::string_view> { argv, argv + argc };

    if (std::any_of(args.begin(), args.end(), [] (auto i) { return i == "-v" || i == "--verbose"; }))
        spdlog::set_level(spdlog::level::debug);

    auto cmdline = std::string {};

    auto s_info = STARTUPINFO { .cb = sizeof(STARTUPINFO) };
    auto p_info = PROCESS_INFORMATION {};

    std::memset(&s_info, 0, sizeof(s_info));
    std::memset(&p_info, 0, sizeof(p_info));

    spdlog::debug("creating host process 'bm2dx.exe' in suspended state...");

    auto result = CreateProcessA("bm2dx.exe", cmdline.data(), nullptr, nullptr, FALSE,
                                 CREATE_SUSPENDED, nullptr, nullptr, &s_info, &p_info);

    if (result == FALSE)
    {
        spdlog::error("CreateProcessA failed (GLE={})", GetLastError());
        return EXIT_FAILURE;
    }

    spdlog::debug("host process created with pid {}", p_info.dwProcessId);

    auto const job = CreateJobObject(nullptr, nullptr);

    if (job == nullptr)
    {
        spdlog::error("CreateJobObject failed (GLE={})", GetLastError());
        return EXIT_FAILURE;
    }

    auto j_info = JOBOBJECT_EXTENDED_LIMIT_INFORMATION {
        .BasicLimitInformation = { .LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE }
    };

    result = SetInformationJobObject(job, JobObjectExtendedLimitInformation, &j_info, sizeof(j_info));

    if (result == FALSE)
    {
        spdlog::error("SetInformationJobObject failed (GLE={})", GetLastError());
        return EXIT_FAILURE;
    }

    result = AssignProcessToJobObject(job, p_info.hProcess);

    if (result == FALSE)
    {
        spdlog::error("AssignProcessToJobObject failed (GLE={})", GetLastError());
        return EXIT_FAILURE;
    }

    auto const dll = std::string_view { "infapi.dll" };
    auto const memory = VirtualAllocEx(p_info.hProcess, nullptr, dll.size(), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

    spdlog::debug("allocated library filename memory at {}", memory);

    if (memory == nullptr)
    {
        spdlog::error("VirtualAllocEx failed (GLE={})", GetLastError());
        return EXIT_FAILURE;
    }

    auto bytes_written = std::size_t { 0 };
    result = WriteProcessMemory(p_info.hProcess, memory, dll.data(), dll.size(), &bytes_written);

    if (result == FALSE)
    {
        spdlog::error("WriteProcessMemory failed (GLE={})", GetLastError());
        return EXIT_FAILURE;
    }

    if (bytes_written != dll.size())
    {
        spdlog::error("WriteProcessMemory failed (expected {} bytes, wrote {} bytes)", dll.size(), bytes_written);
        return EXIT_FAILURE;
    }

    auto const kernel32 = GetModuleHandleA("kernel32.dll");

    if (kernel32 == nullptr)
    {
        spdlog::error("GetModuleHandleA failed (GLE={})", GetLastError());
        return EXIT_FAILURE;
    }

    auto const entrypoint = LPTHREAD_START_ROUTINE(GetProcAddress(kernel32, "LoadLibraryA"));

    if (entrypoint == nullptr)
    {
        spdlog::error("GetProcAddress failed (GLE={})", GetLastError());
        return EXIT_FAILURE;
    }

    auto thread = CreateRemoteThread(p_info.hProcess, nullptr, 0, entrypoint, memory, 0, nullptr);

    if (thread == nullptr)
    {
        spdlog::error("CreateRemoteThread failed (GLE={})", GetLastError());
        return EXIT_FAILURE;
    }

    if (ResumeThread(p_info.hThread) == DWORD(-1))
    {
        spdlog::error("ResumeThread failed (GLE={})", GetLastError());
        return EXIT_FAILURE;
    }

    spdlog::debug("resumed thread successfully");

    WaitForSingleObject(p_info.hProcess, INFINITE);

    return EXIT_SUCCESS;
}