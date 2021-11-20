#include "../../include/engines/duckduckgo.h"

#include "../../include/third_party/json.hpp"
#include "third_party/str/str.h"

namespace anonsearch {
    duckduckgo::duckduckgo() {
        about = std::make_shared<engine::about_t>();

        about->name = "duckduckgo";
        about->url = "https://html.duckduckgo.com/";
        about->type = classic;
        about->has_search = true;
        about->has_suggestions = true;
    }

    std::shared_ptr<engine::about_t> duckduckgo::get_about() {
        return about;
    }

    std::shared_ptr<engine::search_result_t> duckduckgo::search(const std::string &query, const int &start_index, const std::shared_ptr<options_t> &options) {
        const auto http_client_ = std::make_shared<http::client>();
        const auto html_parser = std::make_shared<simple_web::html>();

        const std::string url = "https://html.duckduckgo.com/html/";
        const std::string post_data = str::format("q={0}&s={1}", query, start_index);
        const std::string response = http_client_->post(url, post_data, options).response;

        if (response.size() < 50) { return nullptr; }
        html_parser->parse(response.c_str());

        const auto results = simple_web::html::find_node_by_class(html_parser->nodes, "results");
        if (results == nullptr) { return nullptr; }
        const auto divs = simple_web::html::find_nodes_by_class(results->children, "result", false);

        if (divs == nullptr) {
            return nullptr;
        }

        auto result = std::make_shared<search_result_t>();
        result->reserve(divs->size());

        for (const auto &div: *divs) {
            const auto tag_a = simple_web::html::find_node_by_class(div->children, "result__a");
            const auto tag_img = simple_web::html::find_node_by_class(div->children, "result__icon__img");
            const auto tag_a2 = simple_web::html::find_node_by_class(div->children, "result__snippet");

            search_result search_result_;

            if (tag_a == nullptr || tag_img == nullptr || tag_a2 == nullptr) {
                continue;
            }

            search_result_.url = simple_web::html::find_attribute_by_name(tag_a->attributes, "href")->value;
            search_result_.title = tag_a->get_text();
            search_result_.description = engine::get_text_from_node(*tag_a2);

            const auto src = simple_web::html::find_attribute_by_name(tag_img->attributes, "src");

            if (src != nullptr) {
                search_result_.icon_url = src->value;
            }

            result->push_back(search_result_);
        }

        return result;
    }

    std::shared_ptr<engine::suggestions_result_t> duckduckgo::suggestions(const std::string &query, const std::shared_ptr<options_t> &options) {
        const auto http_client_ = std::make_shared<http::client>();

        const std::string url = "https://duckduckgo.com/ac/?q=" + query;
        const std::string response = http_client_->get(url, options).response;

        if (response.size() < 5) { return nullptr; }
        const auto json = nlohmann::json::parse(response);

        if (json == nullptr || json.size() < 2) {
            return nullptr;
        }

        auto result = std::make_shared<suggestions_result_t>();
        result->reserve(json.size());

        for (const auto &item: json) {
            result->push_back(item["phrase"]);
        }

        return result;
    }
}