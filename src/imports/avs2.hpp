#pragma once

#define DECLARE_AVS2_IMPORT(RETURN_TYPE, IMPORT_NAME, FN_NAME, ...) \
    extern "C" RETURN_TYPE IMPORT_NAME(__VA_ARGS__);                \
    template <class... T> RETURN_TYPE FN_NAME(T&&... args)          \
        { return IMPORT_NAME(std::forward<T>(args)...); }

namespace avs2
{
    enum class property_flag: std::uint32_t
    {
        PROP_XML    = 0x00,
        PROP_READ   = 0x01,
        PROP_WRITE  = 0x02,
        PROP_CREATE = 0x04,
        PROP_BINARY = 0x08,
        PROP_APPEND = 0x10,
    };

    constexpr property_flag operator|(property_flag lhs, property_flag rhs)
        { return static_cast<property_flag>(std::to_underlying(lhs) | std::to_underlying(rhs)); }

    struct xml_property
    {
        explicit xml_property(std::string_view src);
        ~xml_property();
        xml_property(const xml_property&) = delete;
        xml_property& operator=(const xml_property&) = delete;

        void* instance = nullptr;
        std::string_view body = {};
        std::size_t position = 0;
        std::unique_ptr<std::uint8_t[]> buffer = nullptr;
    };

    typedef std::size_t (*avs_reader_t) (std::uint32_t, void*, std::size_t);

    DECLARE_AVS2_IMPORT(void*,        XCgsqzn0000090, property_create,             property_flag, void*, std::uint32_t)
    DECLARE_AVS2_IMPORT(std::int32_t, XCgsqzn0000091, property_destroy,            void*)
    DECLARE_AVS2_IMPORT(std::int32_t, XCgsqzn000009a, property_set_flag,           void*, property_flag, property_flag)
    DECLARE_AVS2_IMPORT(std::int32_t, XCgsqzn00000b8, property_mem_write,          void*, void*, std::uint32_t)
    DECLARE_AVS2_IMPORT(std::int32_t, XCgsqzn000009f, property_query_size,         void*)
    DECLARE_AVS2_IMPORT(std::int32_t, XCgsqzn0000094, property_insert_read, void*, void*, avs_reader_t, std::uint32_t)
    DECLARE_AVS2_IMPORT(std::int32_t, XCgsqzn00000b0, property_read_query_memsize, avs_reader_t, std::int32_t, void*, void*)
}