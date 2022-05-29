#include <cppcodec/base64_default_rfc4648.hpp>
#include "decodeResponse.hpp"
#include "../imports/eacnet.hpp"

using namespace eacapi;

void methods::decodeResponse::handle(server::router_t* router)
{
    router->http_post("/response", [] (auto session, auto params)
    {
        try
        {
            auto request = json_dto::from_json<decodeResponse::request>(session->body());
            auto body = cppcodec::base64_rfc4648::decode(request.body);

            auto base = eacnet::dummy_request();
            auto http = eacnet::EacnetWinHttpRequest(body);
            base->set_http_request(&http);

            if (!eacnet::decodeResponseBody(*base))
                throw std::runtime_error("decodeResponseBody failed");

            auto property = base->get_response_property();
            avs2::property_set_flag(property, avs2::property_flag::PROP_XML, avs2::property_flag::PROP_BINARY);

            auto size = avs2::property_query_size(property);

            if (size < 0)
                throw std::runtime_error("property_query_size failed");

            auto decoded = std::string(size, 0);

            if (avs2::property_mem_write(property, decoded.data(), decoded.size()) < 0)
                throw std::runtime_error("property_mem_write failed");

            session->create_response()
                .set_body(json_dto::to_json(decodeResponse::response {
                    .body = std::move(decoded)
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
