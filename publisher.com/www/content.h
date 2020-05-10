#pragma once

#include <cppcms/view.h>
#include <string>

namespace content  {

class master : public cppcms::base_content {
 public:
  std::string title;
  explicit master() : title("Mustache Enthusiast"){ };
  explicit master(const std::string& _title) : title(_title + " - Mustache Enthusiast"){ };
};

class index : public master {
 public:
  std::string adtag_list_300x250;
  explicit index(const std::string& adtag_list_300x250) :
      master(),
      adtag_list_300x250(adtag_list_300x250){ };
};
}

