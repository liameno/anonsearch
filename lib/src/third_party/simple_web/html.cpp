#include <cstring>
#include <algorithm>
#include <memory>

#include "../../../include/third_party/simple_web/html.h"

#include "third_party/str/str.h"

simple_web::attribute::attribute(const std::string &name) {
    this->name = name;
}

simple_web::attribute::attribute(const std::string &name, const std::string &value) {
    this->name = name;
    this->value = value;
}

std::string simple_web::attribute::to_str() const {
    return str::format("{0}={1}", name, value);
}

simple_web::node::node() {
    is_closed = false;
    is_self_closing = false;
    is_void = false;
}

std::string simple_web::node::get_text() const {
    return str::join("", text);
}

std::string simple_web::node::to_str(const bool &is_one_string) const {
    std::string text_ = get_text();

    if (is_one_string) {
        text_ = str::replace(text_, "\n", "\\n");
    }

    std::string format = "Name: {0}\tText: {1}\tClosed: {2}\tSelfClosing: {3}\tVoid: {4}\tChildren: {5}";
    return str::format(format, name, text_, is_closed, is_self_closing, is_void, children.size());
}

simple_web::html::html() = default;
simple_web::html::~html() { }

void simple_web::html::parse(const char *p_html, int *p_pos, const node_t &node_) {
    uint length = strlen(p_html);
    std::string text;
    int pos = (p_pos != nullptr) ? *p_pos : 0;

    bool is_script = (node_ != nullptr) && (node_->name == "script" || node_->name == "style");
    bool is_comment = false;
    bool is_double_quotes = false;

    while (pos < length) {
        char c = p_html[pos];
        char p1 = (pos < length - 1) ? p_html[pos + 1] : 0;
        char p2 = (pos < length - 2) ? p_html[pos + 2] : 0;
        char p3 = (pos < length - 3) ? p_html[pos + 3] : 0;
        char m1 = (pos > 0) ? p_html[pos - 1] : 0;
        char m2 = (pos > 1) ? p_html[pos - 2] : 0;
        char m3 = (pos > 1) ? p_html[pos - 3] : 0;

        bool is_s_not_c = !is_script && !is_comment;
        bool is_not_sd = !is_script && !is_double_quotes;

        if(is_s_not_c && m1 == '\"' && m2 != '\\') {
            is_double_quotes = !is_double_quotes;
        }
        if (is_not_sd && !is_comment && c == '<' && p1 == '!' && p2 == '-' && p3 == '-') {
            is_comment = true;
        }
        if (is_not_sd && is_comment && m1 == '>' && m2 == '-' && m3 == '-') {
            is_comment = false;
        }

        if (!is_comment) {
            if (!is_double_quotes && c == '<') {
                if (p1 == '/') {
                    pos = end_of_tag(p_html, pos);

                    if (node_ != nullptr) {
                        node_->is_closed = true;
                    }

                    break;
                } else if (!is_script) {
                    auto node2 = std::make_shared<node>();
                    pos = parse_tag(node2, p_html, pos);

                    if (node_ != nullptr) {
                        node_->text.push_back(text);
                        node_->children.push_back(node2);
                    } else if (!str::contains("!doctype", node2->name, true)) {
                        nodes.push_back(node2);
                    }

                    text.clear();
                    if (!node2->is_self_closing && !node2->is_void) {
                        ++pos;
                        parse(p_html, &pos, node2);
                    }
                }
            } else {
                text.push_back(c);
            }
        }

        ++pos;
    }

    if (node_ != nullptr) {
        node_->text.push_back(text);
    }

    if (p_pos != nullptr) {
        *p_pos = pos;
    }
}

int simple_web::html::parse_tag(const node_t& node_, const char *p_html, int pos) {
    std::vector<std::string> tokens;
    int tag_end_i;

    int word_start = pos;
    bool is_tag_end = false;
    bool is_double_quotes = false;

    while (p_html[pos] != 0 && !is_tag_end) {
        char c = p_html[pos];

        if (c == '\"') {
            is_double_quotes = !is_double_quotes;
        }

        if (!is_double_quotes && str::contains(" \t\r\n<>=/", c)) {
            if (word_start < pos) {
                int start2 = word_start;
                int pos2 = pos;
                int len = pos - word_start;

                if (str::contains("\"\'", c)) {
                    start2++;

                    if (str::contains("\"\'", p_html[word_start + (len - 1)])) {
                        pos2--;
                    }
                }

                tokens.emplace_back(p_html + start2, pos2 - start2);
            }

            if (str::contains("<>=/", c)) {
                tokens.emplace_back(p_html + pos, 1);

                if (c == '>') {
                    is_tag_end = true;
                    tag_end_i = pos;
                }

                pos++;
            } else {
                while (str::contains(" \t\r\n", p_html[pos])) {
                    pos++;
                }
            }

            word_start = pos;
        } else {
            pos++;
        }
    }

    size_t tokens_size = tokens.size();
    bool is_name_found = false;

    for (int i = 0; i < tokens_size; i++) {
        std::string token = tokens[i];
        std::string m1 = (i > 0) ? tokens[i - 1] : "";
        std::string m2 = (i > 1) ? tokens[i - 2] : "";
        std::string p1 = (i < tokens_size - 1) ? tokens[i + 1] : "";
        std::string p2 = (i < tokens_size - 2) ? tokens[i + 2] : "";

        if (!str::contains("<>/", token)) {
            if (!is_name_found) {
                node_->name = token;
                is_name_found = true;
            } else {
                if (p1 == "=") {
                    if (str::starts_with(p2, "\"") || str::starts_with(p2, "\'")) {
                        str::remove_first_char(p2);
                    }
                    if (str::ends_with(p2, "\"") || str::ends_with(p2, "\'")) {
                        str::remove_last_char(p2);
                    }

                    node_->attributes.push_back(std::make_shared<attribute>(token, p2));
                    i += 2;
                } else {
                    node_->attributes.push_back(std::make_shared<attribute>(token));
                }
            }
        }

        if (token == ">") {
            if (m1 == "/") {
                node_->is_self_closing = true;
            }
        }
    }

    node_->is_void = is_void_element(node_->name);
    return tag_end_i;
}

int simple_web::html::end_of_tag(const char *p_html, int pos) {
    while (p_html[pos] != 0) {
        if (p_html[pos] == '>') {
            return pos;
        }

        pos++;
    }

    return -1;
}

bool simple_web::html::is_void_element(const std::string &name) {
    std::vector<std::string> void_names = {
            "!doctype",
            "area",
            "base",
            "br",
            "col",
            "command",
            "embed",
            "hr",
            "img",
            "input",
            "keygen",
            "link",
            "meta",
            "param",
            "source",
            "track",
            "wbr",
    };

    return str::contains(void_names, str::to_lower(name));
}

std::shared_ptr<simple_web::attributes_t>
simple_web::html::find_attributes_by_name(const attributes_t &attributes, const std::string &name_) {
    auto result = std::make_shared<attributes_t>();

    for (const auto &attribute: attributes) {
        if (attribute->name == name_) {
            result->push_back(attribute);
        }
    }

    return result;
}

std::shared_ptr<simple_web::attributes_t>
simple_web::html::find_attributes_by_value(const attributes_t &attributes, const std::string &value_, const bool &is_split_space) {
    auto result = std::make_shared<attributes_t>();

    for (const auto &attribute: attributes) {
        bool is_equal = ((is_split_space) && str::contains(str::split(attribute->value, " "), value_))
                        || attribute->value == value_;

        if (is_equal) {
            result->push_back(attribute);
        }
    }

    return result;
}

std::shared_ptr<simple_web::attributes_t>
simple_web::html::find_attributes_by_name_and_value(const attributes_t &attributes, const std::string &name_, const std::string &value_, const bool &is_split_space) {
    auto by_name = find_attributes_by_name(attributes, name_);
    auto by_value = (!by_name->empty()) ? find_attributes_by_value(*by_name, value_, is_split_space) : nullptr;

    return (!by_name->empty()) ? by_value : nullptr;
}

std::shared_ptr<simple_web::nodes_t>
simple_web::html::find_nodes_by_name(const nodes_t &nodes, const std::string &name_, const bool &is_recursive) {
    auto result = std::make_shared<nodes_t>();

    for (const auto &node_: nodes) {
        if (node_->name == name_) {
            result->push_back(node_);
        }

        auto found = find_nodes_by_name(node_->children, name_, is_recursive);
        result->insert(result->end(), found->begin(), found->end());
    }

    return result;
}

std::shared_ptr<simple_web::nodes_t>
simple_web::html::find_nodes_by_class(const nodes_t &nodes, const std::string &class_, const bool &is_recursive) {
    auto result = std::make_shared<nodes_t>();

    for (const auto &node_: nodes) {
        auto attribute = find_attribute_by_name_and_value(node_->attributes, "class", class_, true);

        if (attribute != nullptr) {
            result->push_back(node_);
        }

        auto found = find_nodes_by_class(node_->children, class_, is_recursive);
        result->insert(result->end(), found->begin(), found->end());
    }

    return result;
}

std::shared_ptr<simple_web::nodes_t>
simple_web::html::find_nodes_by_id(const nodes_t &nodes, const std::string &id_, const bool &is_recursive) {
    auto result = std::make_shared<nodes_t>();

    for (const auto &node_: nodes) {
        auto attribute = find_attribute_by_name_and_value(node_->attributes, "id", id_, true);

        if (attribute != nullptr) {
            result->push_back(node_);
        }

        auto found = find_nodes_by_id(node_->children, id_, is_recursive);
        result->insert(result->end(), found->begin(), found->end());
    }

    return result;
}

simple_web::attribute_t
simple_web::html::find_attribute_by_name(const attributes_t &attributes, const std::string &name_) {
    auto found = find_attributes_by_name(attributes, name_);
    return (!found->empty()) ? found->operator[](0) : nullptr;
}

simple_web::attribute_t
simple_web::html::find_attribute_by_value(const attributes_t &attributes, const std::string &value_,
                                          const bool &is_split_space) {
    auto found = find_attributes_by_value(attributes, value_, is_split_space);
    return (!found->empty()) ? found->operator[](0) : nullptr;
}

simple_web::attribute_t
simple_web::html::find_attribute_by_name_and_value(const attributes_t &attributes,
                                                   const std::string &name_, const std::string &value_,
                                                   const bool &is_split_space) {
    auto found = find_attributes_by_name_and_value(attributes, name_, value_, is_split_space);
    return (found != nullptr && !found->empty()) ? found->operator[](0) : nullptr;
}

simple_web::node_t
simple_web::html::find_node_by_name(const nodes_t &nodes, const std::string &name_, const bool &is_recursive) {
    auto found = find_nodes_by_name(nodes, name_, is_recursive);
    return (!found->empty()) ? found->operator[](0) : nullptr;
}

simple_web::node_t
simple_web::html::find_node_by_class(const nodes_t &nodes, const std::string &class_, const bool &is_recursive) {
    auto found = find_nodes_by_class(nodes, class_, is_recursive);
    return (!found->empty()) ? found->operator[](0) : nullptr;
}

simple_web::node_t
simple_web::html::find_node_by_id(const nodes_t &nodes, const std::string &id_, const bool &is_recursive) {
    auto found = find_nodes_by_id(nodes, id_, is_recursive);
    return (!found->empty()) ? found->operator[](0) : nullptr;
}