#include "../../include/engines/yahoo.h"

#include "../../include/third_party/json.hpp"

namespace anonsearch {
    yahoo::yahoo() {
        about = std::make_shared<engine::about_t>();

        about->name = "yahoo";
        about->url = "https://search.yahoo.com/";
        about->type = classic;
        about->has_search = false;
        about->has_suggestions = true;
    }

    std::shared_ptr<engine::about_t> yahoo::get_about() {
        return about;
    }

    std::shared_ptr<engine::search_result_t> yahoo::search(const std::string &query, const int &start_index, const std::shared_ptr<options_t> &options) {
        return nullptr;
    }

    std::shared_ptr<engine::suggestions_result_t> yahoo::suggestions(const std::string &query, const std::shared_ptr<options_t> &options) {
        const auto http_client = std::make_shared<http::client>();

        const std::string url = "https://search.yahoo.com/sugg/gossip/gossip?command=" + query;
        const std::string response = http_client->get(url, options).response;

        if (response.size() < 5) { return nullptr; }
        const std::string json_array = response.substr(9, response.length() - (3 + 9));
        const auto json = nlohmann::json::parse(json_array);

        if (json == nullptr || json.size() < 2) {
            return nullptr;
        }

        const auto array = json[1];
        auto result = std::make_shared<suggestions_result_t>();
        result->reserve(array.size());

        for (const auto &item: array) {
            result->push_back(item);
        }

        return result;
    }
}