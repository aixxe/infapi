#pragma once

#include "../server.hpp"

namespace eacapi::methods::setEacnetClock {
    struct request {
        template <typename Json_Io> void json_io(Json_Io &io)
            { io & json_dto::mandatory("value", value); }

        std::uint64_t value = 0;
    };

    struct response {
        template <typename Json_Io> void json_io(Json_Io &io) {}
    };

    void handle(server::router_t *router);
}