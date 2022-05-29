#include "avs2.hpp"

static std::size_t xml_property_read_cb(std::uint32_t src, void* dst, std::size_t nbytes)
{
    auto property = reinterpret_cast<avs2::xml_property*>(TlsGetValue(src));
    auto start = (property->body.data() + property->position);

    if (property->position + nbytes > property->body.size())
        nbytes = (property->body.size() - property->position);

    if (nbytes == 0)
        return 0;

    std::memcpy(dst, start, nbytes);
    property->position += nbytes;

    return nbytes;
};

avs2::xml_property::xml_property(std::string_view src):
    body(src)
{
    auto const index = TlsAlloc();
    TlsSetValue(index, this);

    auto const size = property_read_query_memsize(xml_property_read_cb, index, nullptr, nullptr);
    auto const flags = (property_flag::PROP_XML | property_flag::PROP_READ | property_flag::PROP_WRITE |
                        property_flag::PROP_CREATE | property_flag::PROP_APPEND);

    buffer = std::make_unique<std::uint8_t[]>(size);
    instance = property_create(flags, buffer.get(), size);
    position = 0;

    property_insert_read(instance, nullptr, xml_property_read_cb, index);
    property_set_flag(instance, property_flag::PROP_BINARY, property_flag::PROP_XML);

    TlsFree(index);
}

avs2::xml_property::~xml_property()
{
    property_destroy(instance);
}