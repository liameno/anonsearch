#include "third_party/httplib.h"

#include <anonsearch/anonsearch.h>
#include <anonsearch/third_party/str/str.h>

#include "config.h"
#include "helper.h"

using namespace anonsearch;

void home_page(const httplib::Request &request, httplib::Response &response) {
    std::string page_src = helper::get_file_content("../../frontend/src/index.html");
    const std::string engine_checkbox_src_format = "<span>"
                                                   "\t<input class=\"checkbox\" id=\"{0}\" name=\"{0}\" type=\"checkbox\">"
                                                   "\t<label for=\"{0}\">{0}</label>"
                                                   "</span>\n";
    const std::string engine_checkbox_checked_src_format = "<span>"
                                                           "\t<input class=\"checkbox\" id=\"{0}\" name=\"{0}\" type=\"checkbox\" checked=\"checked\">"
                                                           "\t<label for=\"{0}\">{0}</label>"
                                                           "</span>\n";

    std::string engine_checkboxes_src;
    const auto size = config::engine_names.size();

    for (int i = 0; i < size; ++i) {
        const auto &name = config::engine_names[i];

        if (i < 2) {
            engine_checkboxes_src.append(str::format(engine_checkbox_checked_src_format, name));
        } else {
            engine_checkboxes_src.append(str::format(engine_checkbox_src_format, name));
        }
    }

    page_src = str::replace(page_src, "{VAR_ENGINE_CHECKBOXES}", engine_checkboxes_src);

    response.status = 200;
    response.set_content(page_src, "text/html");
}

void search_page(const httplib::Request &request, httplib::Response &response) {
    std::string page_src = helper::get_file_content("../../frontend/src/search.html");
    const std::string query = request.get_param_value("q");
    auto s = request.get_param_value("s");
    const int start_index = (!s.empty()) ? std::stoi(s) : 0;
    const auto engine_names_size = config::engine_names.size();
    std::vector<std::shared_ptr<engine>> engines_enabled;
    engines_enabled.reserve(config::engines.size());

    for (int i = 0; i < engine_names_size; ++i) {
        if (request.has_param(config::engine_names[i].c_str())) {
            engines_enabled.push_back(config::engines[i]);
            break;
        }
    }

    const auto useragent = config::get_random_useragent();
    const auto options = std::make_shared<http::options>(config::connect_timeout_s, useragent);

    const std::string left_result_src_format = "<div class=\"left_result\">"
                                               "\t<img class=\"icon\" src=\"{0}\" alt=\"\">"
                                               "\t<a class=\"title\" href=\"{2}\">{1}</a>"
                                               "\t<div class=\"url\">{2}</div>"
                                               "\t<div class=\"description\">{3}</div>"
                                               "</div>\n";
    const std::string right_result_src_format = "<div class=\"right_container\">"
                                                "\t<div class=\"right_result\">"
                                                "\t\t<a class=\"title\" href=\"{2}\">{1}</a>"
                                                "\t\t<div class=\"url\">{2}</div>"
                                                "\t\t<div class=\"description\">{3}</div>"
                                                "\t</div>"
                                                "</div>\n";

    std::string left_results_src;
    std::string right_results_src;

    std::vector<std::shared_ptr<std::thread>> threads;
    threads.reserve(config::engines.size());

    for (const auto &engine: engines_enabled) {
        threads.push_back(std::make_shared<std::thread>([&]() {
            const auto search_results = engine->search(query, start_index, options);

            if (search_results == nullptr) {
                return;
            }

            for (const auto &result: *search_results) {
                const std::string *format;
                std::string *src;

                auto type = engine->get_about()->type;

                if (type == engine::classic) {
                    format = &left_result_src_format;
                    src = &left_results_src;
                } else /*if (type == engine::additional)*/ {
                    format = &right_result_src_format;
                    src = &right_results_src;
                }

                const std::string icon_url = helper::check_on_empty_icon_url(result.icon_url);
                std::string url;

                if (config::is_change_corps_to_free) {
                    url = helper::url_to_https(config::change_corp_to_free(result.url));
                } else {
                    url = helper::url_to_https(result.url);
                }

                src->append(str::format(*format, icon_url, result.title, url, result.description));
            }
        }));
    }
    for (const auto &thread: threads) {
        thread->join();
    }

    page_src = str::replace(page_src, "{VAR_LEFTCONTAINER}", left_results_src);
    page_src = str::replace(page_src, "{VAR_RIGHTCONTAINERS}", right_results_src);
    page_src = str::replace(page_src, "{VAR_QUERY}", query);

    response.status = 200;
    response.set_content(page_src, "text/html");
}

void not_found_page(const httplib::Request &request, httplib::Response &response) {
    response.status = 301;
    response.set_redirect("/home");
}

int main() {
    using namespace httplib;
    config::perform();

    Server server;
    server.set_mount_point("/", "../../frontend/");

    server.Get("/home", home_page);
    server.Get("/search", search_page);
    server.Get(".*", not_found_page);

    server.listen("localhost", 1234);
}