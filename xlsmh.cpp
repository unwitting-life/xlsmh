// xlsmh.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#define CPPHTTPLIB_HTTPLIB_SUPPORT
#include <iostream>
#include "../cpp-utils/utils.hpp"

#define HOST _T("https://www.xlsmh.com")
#define PADDING_LENGTH (75)
#define EasyLazyload _T("EasyLazyload.js")
#define jquery_3_0_0 _T("jquery-3.0.0.min.js")
#define JPEG _T(".jpg")
#define ID_TITLE "ID_TITLE"
#define ID_PREVIOUS "ID_PREVIOUS"
#define ID_PREVIOUS_TITLE "ID_PREVIOUS_TITLE"
#define ID_NEXT "ID_NEXT"
#define ID_NEXT_TITLE "ID_NEXT_TITLE"
#define ID_IMAGES "ID_IMAGE"
#define PREFIX "<li class=\"\">"
#define SUFFIX "</li>"
#define LF "\n"

auto reloadCatalogMode = false;

int main() {
    auto idx = 0;
    auto catalog = std::vector<string_t>();
    catalog.push_back(_T("xinlingdeshengyin"));
    catalog.push_back(_T("shishijilingdishejishi"));
    catalog.push_back(_T("zuikuaigengxindexinlingdeshengyin"));
    for (auto& e : catalog) {
        utils::io::directory::mkdir(e);
        auto html = utils::httplib::Get(utils::strings::format(HOST _T("/manhua/%s/"), e.c_str()));
        if (!html.empty()) {
            if (!reloadCatalogMode) {
                for (auto& i : utils::strings::sortA(utils::strings::removeEmptyElementA(utils::strings::__findA(html, PREFIX, SUFFIX)), true,
                     [](std::string s) ->std::string {
                         s = utils::strings::truncateA(s, "html\">", "<i>");
                         auto num = std::string();
                         for (auto i = 0; i < s.size(); i++) {
                             if (s[i] >= '0' && s[i] <= '9') {
                                 num += s[i];
                             } else if (!num.empty()) {
                                 break;
                             }
                         }
                         return utils::strings::formatA("%04d", utils::strings::atoiA(num));
                     })) {
                    auto li = utils::strings::t2t(i);
                    auto href = utils::strings::trim(utils::strings::truncate(li, _T("<a  href=\""), _T("\">")));
                    auto imageHtml = utils::httplib::Get(utils::strings::format(HOST _T("%s"), href.c_str()));
                    auto title = utils::strings::t2t(utils::strings::truncateA(imageHtml, "<h1>", "</h1>"));
                    auto chapterImages = utils::strings::t2t(utils::strings::truncateA(imageHtml, "var chapterImages = [", "]"));
                    auto images = utils::strings::split(chapterImages, _T(","));
                    auto index = 0;
                    title = utils::strings::replace(title, _T("<span id=\"qTcms_picID\">"), _T(""));
                    title = utils::strings::replace(title, _T("<b id=\"page\" class=\"curPage\">"), _T(""));
                    title = utils::strings::replace(title, _T("</b>"), _T(""));
                    title = utils::strings::replace(title, _T("</span>"), _T(""));
                    title = utils::strings::format(_T("[%04d]%s"), idx++, title.c_str());
                    auto folder = utils::io::path::combine(e, title);
                    for (auto& image : images) {
                        utils::io::directory::mkdir(folder);
                        auto uri = utils::strings::replace(utils::strings::replace(image, _T("\\/"), _T("/")), _T("\""), _T(""));
                        auto format = utils::strings::format(_T("%%0%dd%s"),
                                                             utils::strings::itoa(images.size()).size(),
                                                             utils::io::path::GetFileExtensionName(uri).c_str());
                        auto jpeg = utils::io::path::combine(folder, utils::strings::format(format, index++));
                        if (utils::io::file::size(jpeg) == utils::io::file::nsize) {
                            auto file = utils::httplib::Get(uri, string_t(HOST));
                            if (file.empty()) {
                                utils::console::println_red(utils::strings::format(_T("%s -> %s"),
                                                            utils::strings::paddingRight(uri, utils::strings::SPACE, PADDING_LENGTH).c_str(), jpeg.c_str()));
                            } else {
                                utils::io::file::write(jpeg, file);
                                utils::console::println_green(utils::strings::format(_T("%s -> %s"),
                                                              utils::strings::paddingRight(uri, utils::strings::SPACE, PADDING_LENGTH).c_str(), jpeg.c_str()));
                            }
                        } else {
                            utils::console::println_gray(utils::strings::format(_T("%s -> %s"),
                                                         utils::strings::paddingRight(uri, utils::strings::SPACE, PADDING_LENGTH).c_str(), jpeg.c_str()));
                        }
                    }
                }
            }

            auto template_dir = string_t(_T("."));
            template_dir = utils::io::path::combine(template_dir, _T(".."));
            template_dir = utils::io::path::combine(template_dir, _T(".."));
            template_dir = utils::io::path::combine(template_dir, _T("template"));

            auto file = utils::io::path::combine(template_dir, _T("index.html"));
            auto _template = utils::io::file::read(file);
            if (!_template.empty()) {
                auto directories = utils::strings::sort(utils::io::directory::GetDirectories(e));
                for (auto i = 0; i < directories.size(); i++) {
                    auto title = utils::strings::truncate(directories[i].substr(e.size() + 1), _T("]"), _T(""));
                    auto previous = i > 0 ? utils::strings::format(_T("../../%s/index.html"), directories[static_cast<std::vector<std::wstring, std::allocator<std::wstring>>::size_type>(i) - 1].c_str()) : string_t();
                    auto previousTitle = utils::strings::truncate(utils::io::path::GetFileName(utils::io::path::GetDirectoryName(previous)), _T("]"), _T(""));
                    auto next = i < directories.size() - 1 ? utils::strings::format(_T("../../%s/index.html"), directories[static_cast<std::vector<std::wstring, std::allocator<std::wstring>>::size_type>(i) + 1].c_str()) : string_t();
                    auto nextTitle = utils::strings::truncate(utils::io::path::GetFileName(utils::io::path::GetDirectoryName(next)), _T("]"), _T(""));
                    auto images = string_t();
                    for (auto& file : utils::io::directory::GetFiles(directories[i])) {
                        if (utils::strings::equalsIgnoreCase(JPEG, utils::io::path::GetFileExtensionName(file))) {
                            if (!images.empty()) {
                                images += _T("\n");
                            }
                            images += utils::strings::format(_T("        <img alt=\"%s\" data-lazy-src=\"%s\">"),
                                                             e.c_str(),
                                                             utils::io::path::GetFileName(file).c_str());
                        }
                    }
                    auto html = std::string(_template);
                    html = utils::strings::replaceA(html, ID_TITLE, utils::strings::t2utf8(title));
                    html = utils::strings::replaceA(html, ID_PREVIOUS, utils::strings::replaceA(utils::strings::t2utf8(previous), PATH_SEPARATOR_A, "/"));
                    html = utils::strings::replaceA(html, ID_PREVIOUS_TITLE, utils::strings::replaceA(utils::strings::t2utf8(previousTitle), PATH_SEPARATOR_A, "/"));
                    html = utils::strings::replaceA(html, ID_NEXT, utils::strings::replaceA(utils::strings::t2utf8(next), PATH_SEPARATOR_A, "/"));
                    html = utils::strings::replaceA(html, ID_NEXT_TITLE, utils::strings::replaceA(utils::strings::t2utf8(nextTitle), PATH_SEPARATOR_A, "/"));
                    html = utils::strings::replaceA(html, ID_IMAGES, utils::strings::t2utf8(images));
                    auto file = utils::io::path::combine(directories[i], _T("index.html"));
                    utils::io::file::write(file, html);
                    utils::io::file::copy(utils::io::path::combine(template_dir, EasyLazyload),
                                          (utils::io::path::combine(directories[i], EasyLazyload)));
                    utils::io::file::copy(utils::io::path::combine(template_dir, jquery_3_0_0),
                                          (utils::io::path::combine(directories[i], jquery_3_0_0)));
                    utils::console::println_green(utils::strings::format(_T("%s"), file.c_str()));
                }
            }
        }
    }
    std::cout << "Hello World!\n";
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
