#pragma once

namespace eacnet
{
    class EacnetWinHttpRequest
    {
        public:
            explicit EacnetWinHttpRequest(std::vector<std::uint8_t> body);
        private:
            std::vector<std::uint8_t> _data = {};
    };

    struct EacnetRequestBase
    {
        ~EacnetRequestBase()
        {
            auto vftbl = *reinterpret_cast<void***>(this);
            auto destructor = reinterpret_cast<void* (*) (void*, bool)>(vftbl[0]);

            set_http_request(nullptr);
            set_request_property(nullptr);

            destructor(this, true);
        }

        const char* get_request();
        const char* get_signature();
        void set_request_property(void* property);
        void* get_response_property();
        void set_http_request(EacnetWinHttpRequest* request);
    };

    class dummy_request
    {
        public:
            dummy_request();
            ~dummy_request();
            dummy_request(const dummy_request&) = delete;
            dummy_request& operator=(const dummy_request&) = delete;

            EacnetRequestBase* operator->() { return _request; }
            EacnetRequestBase* operator*() { return _request; }
        private:
            EacnetRequestBase* _request = nullptr;
    };

    extern std::uint64_t (*getClock) ();
    extern void* (*initializeClock) (std::uint64_t);
    extern bool (*decodeResponseBody) (EacnetRequestBase*);
    extern EacnetRequestBase* (*createRequestObject) (void*, std::int32_t);
    extern bool (*makeRequestStringAndSignature) (void*);

    void resolve_functions();
}