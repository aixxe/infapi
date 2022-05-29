#include "methods/decodeResponse.hpp"
#include "methods/encodeRequest.hpp"
#include "methods/setEacnetClock.hpp"

using namespace eacapi;

using traits_t = restinio::traits_t
    <restinio::asio_timer_manager_t, restinio::single_threaded_ostream_logger_t, server::router_t>;

auto get_request_handler()
{
    auto router = std::make_unique<server::router_t>();

    router->http_get("/", [] (auto session, auto params)
        { return session->create_response().done(); });

    methods::decodeResponse::handle(router.get());
    methods::encodeRequest::handle(router.get());
    methods::setEacnetClock::handle(router.get());

    return router;
}

void server::listen()
{
    restinio::run(restinio::on_this_thread<traits_t>()
        .port(8080)
        .address("0.0.0.0")
        .request_handler(get_request_handler()));
}