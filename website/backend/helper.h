#ifndef BACKEND_HELPER_H
#define BACKEND_HELPER_H

#include <string>

namespace helper {
    std::string get_file_content(const std::string &path = "index.html");
    std::string url_to_https(std::string url);
    std::string check_on_empty_icon_url(const std::string &url);
}

#endif