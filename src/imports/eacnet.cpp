#include <memory_signature.hpp>
#include "eacnet.hpp"

decltype(eacnet::getClock) eacnet::getClock = nullptr;
decltype(eacnet::initializeClock) eacnet::initializeClock = nullptr;
decltype(eacnet::decodeResponseBody) eacnet::decodeResponseBody = nullptr;
decltype(eacnet::createRequestObject) eacnet::createRequestObject = nullptr;
decltype(eacnet::makeRequestStringAndSignature) eacnet::makeRequestStringAndSignature = nullptr;

eacnet::EacnetWinHttpRequest::EacnetWinHttpRequest(std::vector<std::uint8_t> body):
    _data(std::move(body))
{
    *reinterpret_cast<std::uint8_t**>(std::uintptr_t(this) + 0xD8) = _data.data();
    *reinterpret_cast<std::uint8_t**>(std::uintptr_t(this) + 0xE0) = _data.data() + _data.size();
}

const char* eacnet::EacnetRequestBase::get_request()
{
    return *reinterpret_cast<const char**>(std::uintptr_t(this) + 0x88);
}

const char* eacnet::EacnetRequestBase::get_signature()
{
    return *reinterpret_cast<const char**>(std::uintptr_t(this) + 0xA8);
}

void eacnet::EacnetRequestBase::set_request_property(void* property)
{
    *reinterpret_cast<void**>(std::uintptr_t(this) + 0x50) = property;
}

void* eacnet::EacnetRequestBase::get_response_property()
{
    return *reinterpret_cast<void**>(std::uintptr_t(this) + 0xD0);
}

void eacnet::EacnetRequestBase::set_http_request(eacnet::EacnetWinHttpRequest* request)
{
    *reinterpret_cast<void**>(std::uintptr_t(this) + 0x20) = request;
}

inline std::uint8_t* resolve_address(std::uint8_t* base, std::int32_t offset, std::int32_t size)
{
    return (base + *reinterpret_cast<std::int32_t*>(base + offset) + size);
}

void eacnet::resolve_functions()
{
    auto const module = GetModuleHandle(nullptr);

	auto const dos_header = reinterpret_cast<PIMAGE_DOS_HEADER>(module);
	auto const nt_headers = reinterpret_cast<PIMAGE_NT_HEADERS>(module + dos_header->e_lfanew);

    auto const start = reinterpret_cast<std::uint8_t*>(module);
	auto const end = (start + nt_headers->OptionalHeader.SizeOfImage);

    {
		auto const addr = jm::memory_signature("E8 ? ? ? ? FF 43 10").find(start, end);

		if (addr == end)
			throw std::runtime_error("failed to find eacnet::getClock");

		eacnet::getClock = reinterpret_cast<decltype(eacnet::getClock)>(
			resolve_address(addr, 1, 5)
		);
	}

	{
		auto const addr = jm::memory_signature("48 8B 4C ? ? E8 ? ? ? ? E8").find(start, end);

		if (addr == end)
			throw std::runtime_error("failed to find eacnet::initializeClock");

		eacnet::initializeClock = reinterpret_cast<decltype(eacnet::initializeClock)>(
			resolve_address(addr + 5, 1, 5)
		);
	}

	{
		auto const addr = jm::memory_signature("48 89 5C 24 20 57 48 83 EC 30 48 8B 41 20").find(start, end);

		if (addr == end)
			throw std::runtime_error("failed to find eacnet::decodeResponseBody");

		eacnet::decodeResponseBody = reinterpret_cast<decltype(eacnet::decodeResponseBody)>(addr);
	}

	{
		auto const addr = jm::memory_signature("E8 ? ? ? ? 48 8B D8 48 85 C0 75 19").find(start, end);

		if (addr == end)
			throw std::runtime_error("failed to find eacnet::createRequestObject");

		eacnet::createRequestObject = reinterpret_cast<decltype(eacnet::createRequestObject)>(
			resolve_address(addr, 1, 5)
		);
	}

	{
		auto const addr = jm::memory_signature("40 53 48 83 EC 20 48 8B D9 E8 ? ? ? ? 84 C0 75 34").find(start, end);

		if (addr == end)
			throw std::runtime_error("failed to find eacnet::makeRequestStringAndSignature");

		eacnet::makeRequestStringAndSignature = reinterpret_cast<decltype(eacnet::makeRequestStringAndSignature)>(
			resolve_address(addr + 9, 1, 5)
		);
	}
}

eacnet::dummy_request::dummy_request():
    _request(eacnet::createRequestObject(nullptr, 0)) {}

eacnet::dummy_request::~dummy_request() { _request->~EacnetRequestBase(); }