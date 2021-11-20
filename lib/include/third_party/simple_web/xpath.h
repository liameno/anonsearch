#ifndef SIMPLE_WEB_XPATH_H
#define SIMPLE_WEB_XPATH_H

#include "html.h"

namespace simple_web {
    class xpath {
    public:
        static std::shared_ptr<nodes_t> parse(const char *p_query, const nodes_t &nodes);
    };
}

#endif