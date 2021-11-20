#include "../../include/engines/mojeek.h"

#include "third_party/str/str.h"

namespace anonsearch {
    mojeek::mojeek() {
        about = std::make_shared<engine::about_t>();

        about->name = "mojeek";
        about->url = "https://www.mojeek.com/";
        about->type = classic;
        about->has_search = true;
        about->has_suggestions = false;
    }

    std::shared_ptr<engine::about_t> mojeek::get_about() {
        return about;
    }

    std::shared_ptr<engine::search_result_t> mojeek::search(const std::string &query, const int &start_index, const std::shared_ptr<options_t> &options) {
        const auto http_client = std::make_shared<http::client>();
        const auto html_parser = std::make_shared<simple_web::html>();

        const std::string url = str::format("https://www.mojeek.com/search?q={0}&s={1}", query, start_index);
        const std::string response = http_client->get(url, options).response;

        if (response.size() < 50) { return nullptr; }
        html_parser->parse(response.c_str());

        const auto results = simple_web::html::find_node_by_class(html_parser->nodes, "results-standard");
        if (results == nullptr) { return nullptr; }
        const auto lis = simple_web::html::find_nodes_by_name(results->children, "li", false);

        if (lis == nullptr) {
            return nullptr;
        }

        auto result = std::make_shared<search_result_t>();
        result->reserve(lis->size());

        for (const auto &li: *lis) {
            const auto tag_a = simple_web::html::find_node_by_class(li->children, "ob");
            const auto tag_p = simple_web::html::find_node_by_class(li->children, "s");

            if (tag_a == nullptr || tag_p == nullptr) {
                continue;
            }

            search_result search_result_;

            search_result_.url = simple_web::html::find_attribute_by_name(tag_a->attributes, "href")->value;
            search_result_.title = engine::get_text_from_node(*tag_a);
            search_result_.description = engine::get_text_from_node(*tag_p);
            //search_result_.icon_url = "https://www.mojeek.com/favicon.ico";

            result->push_back(search_result_);
        }

        return result;
    }

    std::shared_ptr<engine::suggestions_result_t> mojeek::suggestions(const std::string &query, const std::shared_ptr<options_t> &options) {
        return nullptr;
    }
}