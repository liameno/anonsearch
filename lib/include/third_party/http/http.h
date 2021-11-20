#ifndef HTTP_HTTP_H
#define HTTP_HTTP_H

#include <iostream>
#include <memory>
#include <vector>
#include <curl/curl.h>
#include <memory>

namespace http {
    size_t write_function(void *ptr, size_t size, size_t nmemb, std::string *data);

    enum class proxy_type {
        http,
        https,
        socks4,
        socks5,
    };

    struct proxy {
        std::string ip;
        std::string port;
        std::string full;
        proxy_type type;

        void set_full(const std::string &full = {});

        std::string to_curl_format() const;

        proxy(const std::string &ip, const std::string &port, const proxy_type &type = proxy_type::http);
        proxy(const std::string &full, const proxy_type &type = proxy_type::http);
    };

    struct header {
        std::string name;
        std::string value;
        std::string full;

        void set_name(const std::string &name);
        void set_value(const std::string &value);
        void set_full(const std::string &full = {});

        header(const std::string &name, const std::string &value = {});
    };

    struct options {
        int timeout_s;
        std::string useragent;
        std::shared_ptr<http::proxy> proxy;
        std::shared_ptr<std::vector<header>> headers;

        options(const int &timeout_s = 0, const std::string &useragent = {},
                const std::shared_ptr<http::proxy> &proxy_ = nullptr,
                const bool &is_fake_browser = true);
    };

    class client {
    public:
        struct result {
            std::string response;
            /*long code;*/
        };
    private:
        CURL *curl;

        void set_default_settings();
        static curl_slist headers_to_curl_struct(const std::shared_ptr<std::vector<header>> &headers);
    public:
        client();
        ~client();

        result get(const std::string &url, const std::shared_ptr<options> &options_ = nullptr);
        result post(const std::string &url, const std::string &data, const std::shared_ptr<options> &options_ = nullptr);
    };
}

#endif