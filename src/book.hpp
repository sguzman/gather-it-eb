#pragma once

#include <experimental/optional>
#include <vector>
#include <array>

using ::std::string;
using ::std::vector;
using ::std::array;
using ::std::tm;
using ::std::experimental::optional;

enum struct digits : unsigned char {
  one = 1,
  two,
  three,
  four,
  five,
  six,
  seven,
  eight,
  nine,
  ten
};

enum struct File_type : unsigned char {
  pdf = 0,
  epub,
  mobi
};

enum struct File_size : unsigned char {
  Kb = 0,
  Mb,
  Gb
};

struct Book {
  const string name;
  const string img;
  const tm date;
  const string desc;
  const string link;

  const vector<const string> categories;
  struct {
    const optional<string> publisher;
    const optional<tm> pub_date;

    const optional<array<digits, 10>> isbn_10;
    const optional<array<digits, 13>> isbn_13;

    const optional<unsigned short> pages;

    const optional<File_type> type;
    struct {
      const double size;
      const File_type format;
    } Size;
  } Details;
};