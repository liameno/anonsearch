#include "../../include/engines/ip_api.h"

#include "../../include/third_party/json.hpp"

namespace anonsearch {
    ip_api::ip_api() {
        about = std::make_shared<engine::about_t>();

        about->name = "ip_api";
        about->url = "https://ip-api.com/";
        about->type = additional;
        about->has_search = true;
        about->has_suggestions = false;
    }

    std::shared_ptr<engine::about_t> ip_api::get_about() {
        return about;
    }

    std::shared_ptr<engine::search_result_t> ip_api::search(const std::string &query, const int &start_index, const std::shared_ptr<options_t> &options) {
        const auto http_client = std::make_shared<http::client>();

        const std::string url = "http://ip-api.com/json/" + query;
        const std::string response = http_client->get(url, options).response;

        if (response.size() < 5) { return nullptr; }
        const auto json = nlohmann::json::parse(response);

        if (json == nullptr || json.size() < 5) {
            return nullptr;
        }

        auto result = std::make_shared<search_result_t>();
        result->reserve(1);

        search_result search_result_;

        search_result_.url = url;
        search_result_.title = json["org"];
        search_result_.description = response;

        result->push_back(search_result_);
        return result;
    }

    std::shared_ptr<engine::suggestions_result_t> ip_api::suggestions(const std::string &query, const std::shared_ptr<options_t> &options) {
        return nullptr;
    }
}