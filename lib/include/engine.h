#ifndef ANONSEARCH_ENGINE_H
#define ANONSEARCH_ENGINE_H

#include <string>
#include <vector>

#include "third_party/simple_web/html.h"
#include "third_party/http/http.h"

namespace anonsearch {
    class engine {
    public:
        enum type_engine {
            classic,                //duckduckgo, ...
            additional,             //wikipedia, ...
        };

        struct search_result {
            std::string title;
            std::string url;
            std::string icon_url;
            std::string description;

            std::string to_str() const;
        };
        struct about {
            std::string name;
            std::string url;
            type_engine type;
            bool has_search;
            bool has_suggestions;

            std::string to_str() const;
        };
    public:
        typedef about about_t;
        typedef http::options options_t;
        typedef std::vector<search_result> search_result_t;
        typedef std::vector<std::string> suggestions_result_t;
    public:     //Interface
        virtual std::shared_ptr<about_t> get_about() = 0;
        virtual std::shared_ptr<search_result_t> search(const std::string &query, const int &start_index, const std::shared_ptr<options_t> &options) = 0;
        virtual std::shared_ptr<suggestions_result_t> suggestions(const std::string &query, const std::shared_ptr<options_t> &options) = 0;
    public:     //Help Functions
        static std::string get_text_from_node(simple_web::node node_);
    };
}

#endif
