#ifndef ANONSEARCH_BRAVE_H
#define ANONSEARCH_BRAVE_H

#include "../engine.h"

namespace anonsearch {
    class brave : public engine {
    private:
        std::shared_ptr<about_t> about;
    public:
        brave();

        std::shared_ptr<about_t> get_about() override;
        std::shared_ptr<search_result_t> search(const std::string &query, const int &start_index, const std::shared_ptr<options_t> &options) override;
        std::shared_ptr<suggestions_result_t> suggestions(const std::string &query, const std::shared_ptr<options_t> &options) override;
    };
}

#endif