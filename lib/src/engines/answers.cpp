#include "../../include/engines/answers.h"

#include "../../include/third_party/json.hpp"

namespace anonsearch {
    answers::answers() {
        about = std::make_shared<engine::about_t>();

        about->name = "answers";
        about->url = "https://www.answers.com/";
        about->type = additional;
        about->has_search = true;
        about->has_suggestions = true;
    }

    std::shared_ptr<engine::about_t> answers::get_about() {
        return about;
    }

    std::shared_ptr<engine::search_result_t> answers::search(const std::string &query, const int &start_index, const std::shared_ptr<options_t> &options) {
        const auto http_client = std::make_shared<http::client>();
        const auto html_parser = std::make_shared<simple_web::html>();

        std::string url = "https://www.answers.com/search?q=" + query;
        std::string response = http_client->get(url, options).response;

        if (response.size() < 50) { return nullptr; }
        html_parser->parse(response.c_str());

        auto result = std::make_shared<search_result_t>();
        result->reserve(1);

        const auto best_answer = simple_web::html::find_node_by_id(html_parser->nodes, "best-answer");
        if (best_answer == nullptr)return nullptr;
        const auto highlight_text = simple_web::html::find_node_by_id(best_answer->children, "highlightText");
        const auto tags_a = simple_web::html::find_nodes_by_name(best_answer->children, "a");
        if (tags_a == nullptr) return nullptr;
        const auto tag_a = simple_web::html::find_node_by_class(*tags_a, "text-primaryColor");

        if (highlight_text == nullptr || tag_a == nullptr) {
            return nullptr;
        }

        search_result search_result_;

        search_result_.url = simple_web::html::find_attribute_by_name(tag_a->attributes, "href")->value;
        search_result_.title = tag_a->get_text();
        search_result_.description = engine::get_text_from_node(*highlight_text);
        //search_result_.icon_url = "https://www.answers.com/favicon.ico";

        result->push_back(search_result_);
        return result;
    }

    std::shared_ptr<engine::suggestions_result_t> answers::suggestions(const std::string &query, const std::shared_ptr<options_t> &options) {
        const auto http_client = std::make_shared<http::client>();

        const std::string url = "https://www.answers.com/question/search?q=" + query;
        const std::string response = http_client->get(url, options).response;

        if (response.size() < 50) { return nullptr; }
        const auto json = nlohmann::json::parse(response);

        if (json == nullptr || json.size() < 2) {
            return nullptr;
        }

        const auto data = json["data"];
        auto result = std::make_shared<suggestions_result_t>();
        result->reserve(data.size());

        for (const auto &item: data) {
            const std::string title = item["title"];
            result->push_back(title);
        }

        return result;
    }
}