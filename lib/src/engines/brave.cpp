#include "../../include/engines/brave.h"

#include "../../include/third_party/json.hpp"

namespace anonsearch {
    brave::brave() {
        about = std::make_shared<engine::about_t>();

        about->name = "brave";
        about->url = "https://search.brave.com/";
        about->type = classic;
        about->has_search = true;
        about->has_suggestions = true;
    }

    std::shared_ptr<engine::about_t> brave::get_about() {
        return about;
    }

    std::shared_ptr<engine::search_result_t> brave::search(const std::string &query, const int &start_index, const std::shared_ptr<options_t> &options) {
        const auto http_client_ = std::make_shared<http::client>();
        const auto html_parser = std::make_shared<simple_web::html>();

        const std::string url = "https://search.brave.com/search?q=" + query;
        const std::string response = http_client_->get(url, options).response;

        if (response.size() < 50) { return nullptr; }
        html_parser->parse(response.c_str());

        auto results = simple_web::html::find_node_by_id(html_parser->nodes, "results");
        if (results == nullptr) { return nullptr; }
        auto divs = simple_web::html::find_nodes_by_class(results->children, "snippet");

        if (divs == nullptr) {
            return nullptr;
        }

        auto result = std::make_shared<search_result_t>();
        result->reserve(divs->size());

        for (const auto &div: *divs) {
            const auto tag_a = simple_web::html::find_node_by_class(div->children, "result-header");
            const auto tag_span = simple_web::html::find_node_by_class(div->children, "snippet-title");
            const auto tag_img = simple_web::html::find_node_by_class(div->children, "favicon");
            const auto tag_p = simple_web::html::find_node_by_class(div->children, "snippet-description");

            if (tag_a == nullptr || tag_span == nullptr || tag_img == nullptr || tag_p == nullptr) {
                continue;
            }

            search_result search_result_;

            search_result_.url = simple_web::html::find_attribute_by_name(tag_a->attributes, "href")->value;
            search_result_.title = tag_span->get_text();
            search_result_.description = engine::get_text_from_node(*tag_p);

            const auto src = simple_web::html::find_attribute_by_name(tag_img->attributes, "src");

            if (src != nullptr) {
                search_result_.icon_url = src->value;
            }

            result->push_back(search_result_);
        }

        return result;
    }

    std::shared_ptr<engine::suggestions_result_t> brave::suggestions(const std::string &query, const std::shared_ptr<options_t> &options) {
        const auto http_client_ = std::make_shared<http::client>();
        const std::string response = http_client_->get("https://search.brave.com/api/suggest?q=" + query, options).response;

        if (response.size() < 5) { return nullptr; }
        const auto json = nlohmann::json::parse(response);

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