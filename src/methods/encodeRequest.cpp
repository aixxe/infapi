#include "encodeRequest.hpp"
#include "../imports/eacnet.hpp"

using namespace eacapi;

void methods::encodeRequest::handle(server::router_t* router)
{
    router->http_post("/request", [] (auto session, auto params)
    {
        try
        {
            auto request = json_dto::from_json<encodeRequest::request>(session->body());

            auto base = eacnet::dummy_request();
            auto property = avs2::xml_property(request.body);

            base->set_request_property(property.instance);

            if (!eacnet::makeRequestStringAndSignature(*base))
                throw std::runtime_error("makeRequestStringAndSignature failed");

            session->create_response()
                .set_body(json_dto::to_json(encodeRequest::response {
                    .clock     = eacnet::getClock(),
                    .request   = base->get_request(),
                    .signature = base->get_signature(),
                })).done();
        }
        catch (const std::exception& error)
        {
            std::cerr << "Error: " << error.what() << "." << std::endl;
            return restinio::request_rejected();
        }

        return restinio::request_accepted();
    });
}
