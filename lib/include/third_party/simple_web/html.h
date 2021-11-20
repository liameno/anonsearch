#ifndef SIMPLE_WEB_HTML_H
#define SIMPLE_WEB_HTML_H

#include <string>
#include <vector>
#include <memory>

namespace simple_web {
    struct attribute {
        std::string name;
        std::string value;

        attribute(const std::string &name);
        attribute(const std::string &name, const std::string &value);

        std::string to_str() const;
    };
    struct node {
        std::string name;
        std::vector<std::string> text;
        std::vector<std::shared_ptr<node>> children;
        std::vector<std::shared_ptr<attribute>> attributes;

        bool is_closed;
        bool is_self_closing;
        bool is_void;

        node();

        std::string get_text() const;
        std::string to_str(const bool &is_one_string = true) const;
    };

    typedef std::shared_ptr<node> node_t;
    typedef std::shared_ptr<attribute> attribute_t;

    typedef std::vector<node_t> nodes_t;
    typedef std::vector<attribute_t> attributes_t;
}

namespace simple_web {
    class html {
    public:
        nodes_t nodes;

    public:
        html();
        ~html();

        void parse(const char *p_html, int *p_pos = nullptr, const node_t& node_ = nullptr);
        static int parse_tag(const node_t& node_, const char *p_html, int pos);
        static int end_of_tag(const char *p_html, int pos);

        static bool is_void_element(const std::string &name);

        static std::shared_ptr<attributes_t>
        find_attributes_by_name(const attributes_t &attributes, const std::string &name_);
        static std::shared_ptr<attributes_t>
        find_attributes_by_value(const attributes_t &attributes, const std::string &value_, const bool &is_split_space = false);
        static std::shared_ptr<attributes_t>
        find_attributes_by_name_and_value
        (const attributes_t &attributes, const std::string &name_, const std::string &value_, const bool &is_split_space = false);

        static std::shared_ptr<nodes_t>
        find_nodes_by_name(const nodes_t &nodes, const std::string &name_, const bool &is_recursive = true);
        static std::shared_ptr<nodes_t>
        find_nodes_by_class(const nodes_t &nodes, const std::string &class_, const bool &is_recursive = true);
        static std::shared_ptr<nodes_t>
        find_nodes_by_id(const nodes_t &nodes, const std::string &id_, const bool &is_recursive = true);

        static attribute_t
        find_attribute_by_name(const attributes_t &attributes, const std::string &name_);
        static attribute_t
        find_attribute_by_value(const attributes_t &attributes, const std::string &value_, const bool &is_split_space = false);
        static attribute_t
        find_attribute_by_name_and_value(const attributes_t &attributes, const std::string &name_, const std::string &value_, const bool &is_split_space = false);

        static node_t
        find_node_by_name(const nodes_t &nodes, const std::string &name_, const bool &is_recursive = true);
        static node_t
        find_node_by_class(const nodes_t &nodes, const std::string &class_, const bool &is_recursive = true);
        static node_t
        find_node_by_id(const nodes_t &nodes, const std::string &id_, const bool &is_recursive = true);
    };
}

#endif