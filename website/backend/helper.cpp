#include <fstream>
#include "helper.h"
#include <anonsearch/third_party/str/str.h>

namespace helper {
    std::string get_file_content(const std::string &path) {
        std::ifstream ifstream_(path);
        ifstream_.seekg(0, std::ios::end);

        std::string buffer;
        buffer.resize(ifstream_.tellg());
        ifstream_.seekg(0);
        ifstream_.read(const_cast<char *>(buffer.data()), buffer.size());

        return buffer;
    }
    std::string url_to_https(std::string url) {
        if (str::starts_with(url, "https")) {
            return url;
        }

        if (str::starts_with(url, "http")) {
            url.insert(4, "s");
        } else if (str::starts_with(url, "//")) {
            url.insert(0, "https:");
        } else if (str::starts_with(url, "/")) {
            url.insert(0, "https:/");
        } else {
            url.insert(0, "https://");
        }

        return url;
    }
    std::string check_on_empty_icon_url(const std::string &url) {
        if (url.size() < 5) {
            return "/assets/error_icon.png";
        }

        return url_to_https(url);
    }
}