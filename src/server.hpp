#pragma once

#include <restinio/all.hpp>
#include <json_dto/pub.hpp>

namespace eacapi::server
{
    using router_t = restinio::router::express_router_t<>;

    void listen();
}