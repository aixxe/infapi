#include "setEacnetClock.hpp"
#include "../imports/eacnet.hpp"

using namespace eacapi;

void methods::setEacnetClock::handle(server::router_t* router)
{
    router->http_post("/clock", [] (auto session, auto params)
    {
        try
        {
            auto request = json_dto::from_json<setEacnetClock::request>(session->body());
            eacnet::initializeClock(request.value);

            session->create_response()
                .set_body(json_dto::to_json(setEacnetClock::response{})).done();
        }
        catch (const std::exception& error)
        {
            std::cerr << "Error: " << error.what() << "." << std::endl;
            return restinio::request_rejected();
        }

        return restinio::request_accepted();
    });
}
