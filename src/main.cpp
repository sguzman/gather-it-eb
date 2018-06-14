#include <dirent.h>
#include <fstream>
#include <gumbo.h>
#include <iomanip>
#include <iostream>
#include <locale>
#include <string_view>
#include <sys/stat.h>
#include <sys/types.h>
#include <vector>
#include <sstream>

#include "curl.hpp"
#include "book.hpp"

using ::std::cout;
using ::std::cerr;
using ::std::endl;
using ::std::endl;
using ::std::string;
using ::std::vector;
using ::std::ifstream;
using ::std::ofstream;
using ::std::string_view;
using ::std::to_string;
using ::std::istreambuf_iterator;
using ::std::get_time;
using ::std::locale;
using ::std::tm;
using ::std::istringstream;
using ::std::put_time;

constexpr static inline const unsigned short limit{1302};
constexpr static inline const unsigned short offset{18};
constexpr static inline const char* const dir{"./html/"};

const static inline string base_dir{"http://23.95.221.108/"};

static vector<Book> books{};

static inline const string get_page(const unsigned short id) noexcept {
  if (id == 1) {
    return Curl::get(base_dir);
  }

  const string url{base_dir + "page/" + to_string(id) + "/"};
  return Curl::get(url);
}

static inline const GumboNode* get_node(__restrict const GumboNode* const node, const unsigned short n) noexcept {
  return reinterpret_cast<const GumboNode *>(node->v.element.children.data[n]);
}

static inline const GumboVector* articles(__restrict const GumboNode* const node) noexcept  {
  constexpr static const unsigned short articles[6] {2, 3, 1, 1, 1, 1};

  const auto n1 = ::get_node(node, articles[0]);
  const auto n2 = ::get_node(n1, articles[1]);
  const auto n3 = ::get_node(n2, articles[2]);
  const auto n4 = ::get_node(n3, articles[3]);
  const auto n5 = ::get_node(n4, articles[4]);
  const auto n6 = ::get_node(n5, articles[5]);

  return &n6->v.element.children;
}

static inline const GumboNode* link(__restrict const GumboNode* const node) noexcept  {
  constexpr static const unsigned short link[5] {1, 3, 1, 1, 1};

  const auto n1 = ::get_node(node, link[0]);
  const auto n2 = ::get_node(n1, link[1]);
  const auto n3 = ::get_node(n2, link[2]);
  const auto n4 = ::get_node(n3, link[3]);
  const auto n5 = ::get_node(n4, link[4]);

  return n5;
}

static inline bool exists(string_view filename) noexcept {
  struct stat buffer{};
  return stat(filename.data(), &buffer) == 0;
}

static inline const vector<const string> rake(const unsigned short n) noexcept {
  const string body{::get_page(n)};
  GumboOutput* output{::gumbo_parse(body.c_str())};
  const auto arts = ::articles(output->root);

  vector<const string> book_links{};
  {
    for (unsigned int i{}, count{}; i < arts->length && count < 10; ++i) {

      if (const auto* p = static_cast<GumboNode *>(arts->data[i]); p->type == GUMBO_NODE_ELEMENT && p->v.element.tag == GUMBO_TAG_ARTICLE) {
        ++count;
        const GumboNode* l{::link(p)};
        const string str{gumbo_get_attribute(&l->v.element.attributes, "href")->value};
        const string sub_str{str.substr(offset, str.length() - (offset + 1))};
        book_links.push_back(sub_str);
      }
    }
  }

  gumbo_destroy_output(&kGumboDefaultOptions, output);
  return book_links;
}

static inline void update_files() noexcept {
  for (unsigned short i{1}; i <= limit; ++i) {
    const vector<const string>& book_links{::rake(i)};
    for (const auto &iter : book_links) {

      if (const string filepath{dir + iter + ".html"}; !exists(filepath)) {
        cout << "Downloading " << filepath << endl;
        const string link{base_dir + iter + "/"};
        const string html{Curl::get(link)};

        ofstream out_file{filepath};
        out_file << html;
        out_file.close();
      } else {
        return;
      }
    }
  }
}

static inline const string title(__restrict const GumboNode* const node) noexcept {
  constexpr static const unsigned short title[7] {2, 3, 1, 1, 1, 3, 0};

  const auto n1 = ::get_node(node, title[0]);
  const auto n2 = ::get_node(n1, title[1]);
  const auto n3 = ::get_node(n2, title[2]);
  const auto n4 = ::get_node(n3, title[3]);
  const auto n5 = ::get_node(n4, title[4]);
  const auto n6 = ::get_node(n5, title[5]);
  const auto n7 = ::get_node(n6, title[6]);

  const string str{n7->v.text.text};

  return str;
}

static inline const tm date(__restrict const GumboNode* const node) noexcept {
  constexpr static const unsigned short date[11] {2, 3, 1, 3, 1, 1, 1, 1, 1, 1, 0};

  const auto n1 = ::get_node(node, date[0]);
  const auto n2 = ::get_node(n1, date[1]);
  const auto n3 = ::get_node(n2, date[2]);
  const auto n4 = ::get_node(n3, date[3]);
  const auto n5 = ::get_node(n4, date[4]);
  const auto n6 = ::get_node(n5, date[5]);
  const auto n7 = ::get_node(n6, date[6]);
  const auto n8 = ::get_node(n7, date[7]);
  const auto n9 = ::get_node(n8, date[8]);
  const auto n10 = ::get_node(n9, date[9]);
  const auto n11 = ::get_node(n10, date[10]);

  const string str{n11->v.text.text};

  tm t{};
  istringstream ss{str};
  ss >> get_time(&t, "%b %d, %Y");
  if (ss.fail()) {
    cerr << "Parse failed" << endl;
  }

  return t;
}

static inline const string img(__restrict const GumboNode* const node) noexcept {
  constexpr static const unsigned short img[11] {2, 3, 1, 3, 1, 1, 1, 1, 3, 1, 0};

  const auto n1 = ::get_node(node, img[0]);
  const auto n2 = ::get_node(n1, img[1]);
  const auto n3 = ::get_node(n2, img[2]);
  const auto n4 = ::get_node(n3, img[3]);
  const auto n5 = ::get_node(n4, img[4]);
  const auto n6 = ::get_node(n5, img[5]);
  const auto n7 = ::get_node(n6, img[6]);
  const auto n8 = ::get_node(n7, img[7]);
  const auto n9 = ::get_node(n8, img[8]);
  const auto n10 = ::get_node(n9, img[9]);
  const auto n11 = ::get_node(n10, img[10]);

  const string str{::gumbo_get_attribute(&n11->v.element.attributes, "src")->value};
  const string path{base_dir + str.substr(offset)};

  return path;
}

static inline void update_model() noexcept {
  DIR* dirp{opendir(dir)};

  readdir(dirp);
  readdir(dirp);
  for (struct dirent* dp = readdir(dirp); dp != nullptr; dp = readdir(dirp)) {
    const string name{dp->d_name};
    const string path{dir + name};
    ifstream input{path};
    const string content{istreambuf_iterator<char>(input), istreambuf_iterator<char>()};

    GumboOutput* output{::gumbo_parse(content.c_str())};

    const string ts{::title(output->root)};
    cout << ts << endl;

    const tm ds{::date(output->root)};
    cout << put_time(&ds, "%c") << endl;

    const string is{::img(output->root)};
    cout << is << endl;

    auto node_1 = ::get_node(output->root, 2);
    cout << ::gumbo_get_attribute(&node_1->v.element.attributes, "class")->value << endl;
    auto node_2 = ::get_node(node_1, 3);
    cout << ::gumbo_get_attribute(&node_2->v.element.attributes, "class")->value << endl;
    auto node_3 = ::get_node(node_2, 1);
    cout << ::gumbo_get_attribute(&node_3->v.element.attributes, "class")->value << endl;
    auto node_4 = ::get_node(node_3, 3);
    cout << ::gumbo_get_attribute(&node_4->v.element.attributes, "class")->value << endl;
    auto node_5 = ::get_node(node_4, 1);
    cout << ::gumbo_get_attribute(&node_5->v.element.attributes, "class")->value << endl;
    auto node_6 = ::get_node(node_5, 1);
    cout << ::gumbo_get_attribute(&node_6->v.element.attributes, "class")->value << endl;
    auto node_7 = ::get_node(node_6, 1);
    cout << ::gumbo_get_attribute(&node_7->v.element.attributes, "class")->value << endl;
    auto node_8 = ::get_node(node_7, 1);
    cout << ::gumbo_get_attribute(&node_8->v.element.attributes, "class")->value << endl;
    auto node_9 = ::get_node(node_8, 3);
    cout << ::gumbo_get_attribute(&node_9->v.element.attributes, "class")->value << endl;
    auto node_10 = ::get_node(node_9, 4);
    auto node_11 = ::get_node(node_10, 0);


    gumbo_destroy_output(&kGumboDefaultOptions, output);
  }

  closedir(dirp);
}

int main () {
  std::locale::global(std::locale("en_US.utf-8"));

  update_files();
  update_model();

  return EXIT_SUCCESS;
}