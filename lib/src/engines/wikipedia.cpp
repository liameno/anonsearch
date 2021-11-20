#include "../../include/engines/wikipedia.h"

#include "../../include/third_party/json.hpp"
#include "third_party/str/str.h"

namespace anonsearch {
    wikipedia::wikipedia() {
        about = std::make_shared<engine::about_t>();

        about->name = "wikipedia";
        about->url = "https://wikipedia.org/";
        about->type = additional;
        about->has_search = true;
        about->has_suggestions = true;
    }

    std::shared_ptr<engine::about_t> wikipedia::get_about() {
        return about;
    }

    std::shared_ptr<engine::search_result_t> wikipedia::search(const std::string &query, const int &start_index, const std::shared_ptr<options_t> &options) {
        const auto http_client_ = std::make_shared<http::client>();
        const auto html_parser = std::make_shared<simple_web::html>();

        const auto suggestions_ = suggestions(query, options);
        const auto query2 = (suggestions_ != nullptr && !suggestions_->empty()) ? suggestions_->operator[](0) : query;

        const std::string url = str::format("https://www.wikipedia.org/wiki/{0}", str::replace(query2, " ", "_"));
        const std::string response = http_client_->get(url, options).response;

        if (response.size() < 50) { return nullptr; }
        html_parser->parse(response.c_str());

        const auto tag_div = simple_web::html::find_node_by_class(html_parser->nodes, "mw-parser-output");
        if (tag_div == nullptr) { return nullptr; }
        const auto tags_p = simple_web::html::find_nodes_by_name(tag_div->children, "p", false);
        const auto tag_h1 = simple_web::html::find_node_by_id(html_parser->nodes, "firstHeading");
        const auto tag_a = simple_web::html::find_node_by_class(tag_div->children, "image");

        if (tags_p == nullptr || tag_h1 == nullptr /*|| tag_a == nullptr*/) {
            return nullptr;
        }

        auto result = std::make_shared<search_result_t>();
        result->reserve(1);

        search_result search_result_;

        search_result_.url = url;
        search_result_.title = engine::get_text_from_node(*tag_h1);

        if (tag_a != nullptr) {
            auto attributes = tag_a->children[0]->attributes;
            search_result_.icon_url = simple_web::html::find_attribute_by_name(attributes, "src")->value;
        }

        for (const auto &tag_p : *tags_p) {
            if (!tag_p->attributes.empty()) {
                continue;
            }

            search_result_.description = engine::get_text_from_node(*tag_p);
            break;
        }
        result->push_back(search_result_);

        return result;
    }

    std::shared_ptr<engine::suggestions_result_t> wikipedia::suggestions(const std::string &query, const std::shared_ptr<options_t> &options) {
        const auto http_client_ = std::make_shared<http::client>();

        const std::string format = "https://www.wikipedia.org/w/api.php?action=opensearch&format=json&formatversion=2&search={0}&namespace=0&limit=10";
        const std::string url = str::format(format, query);
        const std::string response = http_client_->get(url, options).response;

        if (response.size() < 5) { return nullptr; }
        auto json = nlohmann::json::parse(response);

        if (json == nullptr || json.size() < 2) {
            return nullptr;
        }

        auto result = std::make_shared<suggestions_result_t>();
        result->reserve(json.size());

        for (const auto &item: json[1]) {
            result->push_back(item);
        }

        return result;
    }
}