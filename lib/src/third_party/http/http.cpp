#include "third_party/http/http.h"
#include "third_party/str/str.h"

namespace http {
    size_t write_function(void *ptr, size_t size, size_t nmemb, std::string *data) {
        data->append((char *) ptr, size * nmemb);
        return size * nmemb;
    }

    void proxy::set_full(const std::string &full) {
        if (full.empty()) {
            this->full = ip + ":" + port;
        } else {
            this->full = full;
        }
    }

    proxy::proxy(const std::string &ip, const std::string &port, const http::proxy_type &type) {
        this->ip = ip;
        this->port = port;
        this->type = type;
        set_full();
    }

    proxy::proxy(const std::string &full, const proxy_type &type) {
        auto split = str::split(full, ":");

        if (split.size() > 1) {
            this->ip = split[0];
            this->port = split[1];
        }

        this->type = type;
        set_full(full);
    }

    std::string proxy::to_curl_format() const {
        std::string result;

        if (full.empty()) {
            return {};
        }

        switch (type) {
            case proxy_type::http:
                result.append("http");
                break;
            case proxy_type::https:
                result.append("https");
                break;
            case proxy_type::socks4:
                result.append("socks4");
                break;
            case proxy_type::socks5:
                result.append("socks5");
                break;
        }

        result.append("://");
        result.append(full);
        return result;
    }

    header::header(const std::string &name, const std::string &value) {
        set_name(name);
        set_value(value);
        set_full();
    }

    void header::set_name(const std::string &name) {
        this->name = name;
        set_full();
    }

    void header::set_value(const std::string &value) {
        this->value = value;
        set_full();
    }

    void header::set_full(const std::string &full) {
        if (full.empty()) {
            this->full = name + ": " + value;
        } else {
            this->full = full;
        }
    }

    options::options(const int &timeout_s, const std::string &useragent,
                     const std::shared_ptr<http::proxy> &proxy_, const bool &is_fake_browser) {
        this->headers = std::make_shared<std::vector<header>>();
        this->timeout_s = timeout_s;
        this->useragent = useragent;
        this->proxy = proxy_;

        if (is_fake_browser) {
            headers->emplace_back("Accept","text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8");
            headers->emplace_back("Accept-Language", "en-US,en;q=0.5");
            //headers->emplace_back("Accept-Encoding", "gzip, deflate, br");
            headers->emplace_back("Connection", "keep-alive");
            //headers->emplace_back("Referer", "");
            //headers->emplace_back("Cookie", "");
            headers->emplace_back("DNT", "1");                          //-> do not track
            headers->emplace_back("Sec-GPC", "1");                      //-> do not sell or share
            //headers->emplace_back("X-Requested-With", "XMLHttpRequest");
            headers->emplace_back("Upgrade-Insecure-Requests","1");    //-> the server can now redirect to a secure version of the site.
        }
    }

    client::client() {
        curl = curl_easy_init();
    }

    client::~client() {
        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }

    void client::set_default_settings() {
        curl_easy_reset(curl);                                                      //reset options
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_function);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);                  //follow HTTP 3xx redirects
    }

    curl_slist client::headers_to_curl_struct(const std::shared_ptr<std::vector<header>> &headers) {
        struct curl_slist *curl_headers = nullptr;

        for (auto &header_: *headers) {
            if (header_.value.empty()) {
                continue;
            }

            curl_headers = curl_slist_append(curl_headers, header_.full.c_str());
        }

        if (curl_headers == nullptr) {
            curl_headers = curl_slist_append(curl_headers, "");
        }

        return *curl_headers;
    }

    client::result client::get(const std::string &url, const std::shared_ptr<options> &options_) {
        set_default_settings();
        result result_;

        if (options_ != nullptr) {
            if (options_->proxy != nullptr) {
                curl_easy_setopt(curl, CURLOPT_PROXY, options_->proxy->to_curl_format().c_str());
            }
            if (!options_->useragent.empty()) {
                curl_easy_setopt(curl, CURLOPT_USERAGENT, options_->useragent.c_str());
            }
            if (options_->timeout_s > 0) {
                curl_easy_setopt(curl, CURLOPT_TIMEOUT, options_->timeout_s);
            }

            struct curl_slist curl_headers = headers_to_curl_struct(options_->headers);
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, &curl_headers);
        }

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result_.response);

        curl_easy_perform(curl);
        //curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &result_.code); BUG
        return result_;
    }

    client::result client::post(const std::string &url, const std::string &data, const std::shared_ptr<options> &options_) {
        set_default_settings();
        result result_;

        if (options_ != nullptr) {
            if (options_->proxy != nullptr) {
                curl_easy_setopt(curl, CURLOPT_PROXY, options_->proxy->to_curl_format().c_str());
            }
            if (!options_->useragent.empty()) {
                curl_easy_setopt(curl, CURLOPT_USERAGENT, options_->useragent.c_str());
            }
            if (options_->timeout_s > 0) {
                curl_easy_setopt(curl, CURLOPT_TIMEOUT, options_->timeout_s);
            }

            struct curl_slist curl_headers = headers_to_curl_struct(options_->headers);
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, &curl_headers);
        }

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result_.response);

        curl_easy_perform(curl);
        //curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &result_.code); BUG
        return result_;
    }
}