#include <array>
#include <dirent.h>
#include <fstream>
#include <gumbo.h>
#include <iostream>
#include <string_view>
#include <sys/stat.h>
#include <sys/types.h>
#include <vector>

#include "curl.hpp"
#include "book.hpp"

using ::std::cout;
using ::std::cerr;
using ::std::endl;
using ::std::endl;
using ::std::string;
using ::std::array;
using ::std::vector;
using ::std::ifstream;
using ::std::ofstream;
using ::std::string_view;
using ::std::to_string;

constexpr static inline const unsigned short limit{1301};
constexpr static inline const unsigned short offset{18};
constexpr static inline const char * const dir{"./html/"};
constexpr static inline const array<const unsigned short, 6> as{{2, 3, 1, 1, 1, 1}};
constexpr static inline const array<const unsigned short, 5> cs{{1, 3, 1, 1, 1}};

const static string base_dir{"http://23.95.221.108/"};

static vector<Book> books{};

static inline const string get_page(const unsigned short id) noexcept {
  if (id == 1) {
    return Curl::get(base_dir);
  }

  const string url{base_dir + "page/" + to_string(id) + "/"};
  return Curl::get(url);
}

static inline const GumboVector* articles(__restrict const GumboNode * const root, const unsigned short idx = 0u) noexcept  {
  if (idx == as.size()) {
    return &(root->v.element.children);
  }

  return ::articles(static_cast<const GumboNode *>(root->v.element.children.data[as[idx]]),
                  static_cast<const unsigned short>(idx + 1));
}

static inline const GumboNode* link(__restrict const GumboNode * const root, const unsigned short idx = 0u) noexcept  {
  if (idx == cs.size()) {
    return root;
  }

  return ::link(static_cast<const GumboNode *>(root->v.element.children.data[cs[idx]]),
              static_cast<const unsigned short>(idx + 1));
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

static inline void update_model() noexcept {
  DIR* dirp = opendir(dir);

  for (struct dirent* dp = readdir(dirp); dp != nullptr; dp = readdir(dirp)) {
    cout << dp->d_name << endl;
  }
  closedir(dirp);
}

int main () {
  update_files();
  update_model();

  return EXIT_SUCCESS;
}