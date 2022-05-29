#pragma once

#include "../server.hpp"

namespace eacapi::methods::decodeResponse
{
    struct request
    {
        template <typename Json_Io> void json_io(Json_Io& io)
            { io & json_dto::mandatory("body", body); }

        std::string body = {};
    };

    using response = request;

    void handle(server::router_t* router);
}