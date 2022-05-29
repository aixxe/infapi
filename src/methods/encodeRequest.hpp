#pragma once

#include "../server.hpp"

namespace eacapi::methods::encodeRequest
{
    struct request
    {
        template <typename Json_Io> void json_io(Json_Io& io)
            { io & json_dto::mandatory("body", body); }

        std::string body = {};
    };

    struct response
    {
        template <typename Json_Io> void json_io(Json_Io& io)
            { io & json_dto::mandatory("clock", clock)
                 & json_dto::mandatory("request", request)
                 & json_dto::mandatory("signature", signature); }

        std::uint64_t clock = 0;
        std::string request = {};
        std::string signature = {};
    };

    void handle(server::router_t* router);
}