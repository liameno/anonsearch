#ifndef BACKEND_CONFIG_H
#define BACKEND_CONFIG_H

#include <string>
#include <vector>
#include <thread>
#include <mutex>

#include <anonsearch/anonsearch.h>

namespace config {
    using namespace anonsearch;
    struct corp_to_free_filter {
        std::string corp;
        std::string free;

        corp_to_free_filter() = default;
        corp_to_free_filter(const std::string &corp, const std::string &free) {
            this->corp = corp;
            this->free = free;
        }
    };

    bool is_use_custom_useragents = true;
    bool is_use_custom_proxies = true;
    bool is_change_corps_to_free = true;
    int connect_timeout_s = 5;

    std::vector<corp_to_free_filter> change_corps_to_free_filters;
    std::vector<std::shared_ptr<engine>> engines;
    std::vector<std::string> engine_names;
    std::vector<std::string> useragents;
    std::vector<std::shared_ptr<http::proxy>> proxies;

    std::mutex useragents_mutex;
    std::mutex proxies_mutex;

    void perform() {
        change_corps_to_free_filters = {
                //{"wikipedia.org",   "wikiless.org"}, //en.wikiless.org -> error
                {"youtube.com",     "yewtu.be"},
                {"youtu.be",        "yewtu.be"},
                {"youtube.de",      "yewtu.be"},
                {"reddit.com",      "libredd.it"},
        };

        engines = {
                std::make_shared<duckduckgo>(),
                std::make_shared<brave>(),
                std::make_shared<mojeek>(),
                std::make_shared<yahoo>(),
                std::make_shared<wikipedia>(),
                std::make_shared<answers>(),
                std::make_shared<ip_api>(),
        };
        engine_names = {
                "duckduckgo",
                "brave",
                "mojeek",
                "yahoo",
                "wikipedia",
                "answers",
                "ip_api",
        };

        if (is_use_custom_useragents) {
            useragents = {
                    "Mozilla/5.0 (X11; U; Linux x86_64) Gecko/20012419 Firefox/92.0",
                    "Mozilla/5.0 (X11; openSUSE Leap; Linux x86_64; rv:666) Gecko/20100101 Firefox/666",
                    "Mozilla/5.0 (X11; Linux x86_64; rv:68.0) Gecko/20100101 Firefox/68.0 Waterfox/56.6.2021.10",
                    "Mozilla/5.0 (X11; FreeBSD amd64; rv:94.0) Gecko/20100101 Firefox/94.0",
                    "Mozilla/5.0 (X11; Ubuntu; Linux aarch64; rv:93.0) Gecko/20100101 Firefox/93.0",
                    "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:54.0) Gecko/20100101 Firefox/54.0",
                    "Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:94.0) Gecko/20100101 Firefox/94.0",
                    "Mozilla/5.0 (Windows NT 10.0; WOW64; rv:56.0) Gecko/20100101 Firefox/93.0",
            };
        }
        if (is_use_custom_proxies) {
            proxies = {
                    std::make_shared<http::proxy>("127.0.0.1:9050"),
            };
        }
    }

    int get_random_int() {
        return rand();
    }

    std::string change_corp_to_free(const std::string &corp) {
        for (const auto &corp_to_free_filter: change_corps_to_free_filters) {
            if (str::contains(corp, corp_to_free_filter.corp)) {
                return str::replace(corp, corp_to_free_filter.corp, corp_to_free_filter.free);
            }
        }

        return corp;
    }

    std::string get_random_useragent() {
        useragents_mutex.lock();
        auto useragent = useragents.operator[](get_random_int() % proxies.size());
        useragents_mutex.unlock();

        return useragent;
    }

    std::shared_ptr<http::proxy> get_random_proxy() {
        proxies_mutex.lock();
        auto proxy = proxies.operator[](get_random_int() % proxies.size());
        proxies_mutex.unlock();

        return proxy;
    }
}

#endif