#ifndef ANONSEARCH_WIKIPEDIA_H
#define ANONSEARCH_WIKIPEDIA_H

#include "../engine.h"

namespace anonsearch {
    class wikipedia : public engine {
    private:
        std::shared_ptr<about_t> about;
    public:
        wikipedia();

        std::shared_ptr<about_t> get_about() override;
        std::shared_ptr<search_result_t> search(const std::string &query, const int &start_index, const std::shared_ptr<options_t> &options) override;
        std::shared_ptr<suggestions_result_t> suggestions(const std::string &query, const std::shared_ptr<options_t> &options) override;
    };
}

#endif