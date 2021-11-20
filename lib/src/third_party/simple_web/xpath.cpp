#include <cstring>

#include "../../../include/third_party/simple_web/xpath.h"

std::shared_ptr<simple_web::nodes_t>
simple_web::xpath::parse(const char *p_query, const nodes_t &nodes) {
    int pos = 0;
    int length = strlen(p_query);
    int last_index = length - 1;
    auto current_nodes = std::make_shared<nodes_t>(nodes);
    bool is_in_params;

    int slash_count = 0;

    std::string text;
    std::string attribute_name_;
    std::string attribute_value_;

    std::string *write = &text;

    while (pos < length) {
        char c = p_query[pos];
        //char m1 = (pos > 0) ? p_query[pos - 1] : 0;
        //char m2 = (pos > 1) ? p_query[pos - 2] : 0;
        char p1 = (pos < length - 1) ? p_query[pos + 1] : 0;
        //char p2 = (pos < length - 2) ? p_query[pos + 2] : 0;

        if (c == '[') {
            write = &attribute_name_;
            goto cont;
        } else if (c == '=') {
            write = &attribute_value_;
            goto cont;
        } else if (c == '/' || c == ']') {
            slash_count++;
            write = &text;
            goto cont;
        }

        write->push_back(c);

        if ((pos == last_index || p1 == '/' || p1 == ']') && !attribute_name_.empty() && !attribute_name_.empty()) {
            for (int i = 0; i < current_nodes->size(); ++i) {
                auto node_ = current_nodes->operator[](i);

                if (!html::find_attributes_by_name_and_value(node_->attributes, attribute_name_, attribute_value_)
                ->empty()) {
                    goto cont2;
                }

                current_nodes->erase(current_nodes->begin() + i);
                --i;

                cont2:
                continue;
            }

            attribute_name_.clear();
            attribute_value_.clear();
            slash_count = 0;
            write = &text;
        } else if ((pos == last_index || (p1 == '/') || p1 == '[') && !text.empty()) {
            current_nodes = html::find_nodes_by_name(*current_nodes, text, slash_count > 1);

            text.clear();
            slash_count = 0;
            write = &text;
        }

        cont:
        ++pos;
    }

    return current_nodes;
}
