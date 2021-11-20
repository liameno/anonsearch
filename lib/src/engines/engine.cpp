#include "../../include/engine.h"

#include "third_party/str/str_impl.h"

namespace anonsearch {
    std::string engine::get_text_from_node(simple_web::node node_) {
        std::string result;

        if (!node_.text.empty()) {
            result.append(node_.text[0]);
            node_.text[0].clear();
        }

        for (int i = 0; i < node_.children.size(); ++i) {
            const auto child = node_.children[i];

            result.append(child->get_text());
            result.append(node_.text[i + 1]);
            node_.text[i + 1].clear();
        }

        result.append(node_.get_text());
        return result;
    }

    std::string engine::search_result::to_str() const {
        const std::string format = "Title: {0}\tUrl: {1}\tIconUrl: {2}\tDescription: {3}";
        return str::format(format, title, url, icon_url, description);
    }

    std::string engine::about::to_str() const {
        const std::string format = "Name: {0}\tUrl: {1}\tType: {2}\tHasSearch: {3}\tHasSuggestions: {4}";
        return str::format(format, name, url, type, has_search, has_suggestions);
    }
}