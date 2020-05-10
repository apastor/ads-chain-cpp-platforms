#line 1 "master.tmpl"
#include "content.h"
#line 2 "master.tmpl"
namespace my_skin {
#line 3 "master.tmpl"
struct  master :public cppcms::base_view
#line 3 "master.tmpl"
{
#line 3 "master.tmpl"
  content::master &content;
#line 3 "master.tmpl"
  master(std::ostream &_s,content::master &_content): cppcms::base_view(_s),content(_content),_domain_id(0)
#line 3 "master.tmpl"
  {
#line 3 "master.tmpl"
    _domain_id=booster::locale::ios_info::get(_s).domain_id();
#line 3 "master.tmpl"
  }
#line 5 "master.tmpl"
  virtual void title() {
#line 5 "master.tmpl"
    cppcms::translation_domain_scope _trs(out(),_domain_id);

#line 5 "master.tmpl"
    out()<<cppcms::filters::escape(content.title);
#line 5 "master.tmpl"
  } // end of template title
#line 6 "master.tmpl"
  virtual void page_content() {
#line 6 "master.tmpl"
    cppcms::translation_domain_scope _trs(out(),_domain_id);

#line 6 "master.tmpl"
    out()<<"Override  Me";
#line 6 "master.tmpl"
  } // end of template page_content
#line 7 "master.tmpl"
  virtual void render() {
#line 7 "master.tmpl"
    cppcms::translation_domain_scope _trs(out(),_domain_id);

#line 15 "master.tmpl"
    out()<<"\n"
           "\n"
           "<!-- <!doctype html> -->\n"
           "<!-- Website Template by freewebsitetemplates.com -->\n"
           "<html>\n"
           "<head>\n"
           "\t<meta charset=\"UTF-8\">\n"
           "\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
           "\t<title>";
#line 15 "master.tmpl"
    title();
#line 22 "master.tmpl"
    out()<<"</title>\n"
           "\t<link rel=\"stylesheet\" type=\"text/css\" href=\"css/style.css\">\n"
           "\t<link rel=\"stylesheet\" type=\"text/css\" href=\"css/mobile.css\" media=\"screen and (max-width : 568px)\">\n"
           "\t<script type=\"text/javascript\" src=\"js/mobile.js\"></script>\n"
           "</head>\n"
           "<body>\n"
           "\t<div id=\"header\">\n"
           "\t\t<a href='";
#line 22 "master.tmpl"
    content.app().mapper().map(out(),"");
#line 27 "master.tmpl"
    out()<<"' class=\"logo\">\n"
           "\t\t\t<img src=\"images/logo.jpg\" alt=\"\">\n"
           "\t\t</a>\n"
           "\t\t<ul id=\"navigation\">\n"
           "\t\t\t<li class=\"selected\">\n"
           "\t\t\t\t<a href='";
#line 27 "master.tmpl"
    content.app().mapper().map(out(),"");
#line 30 "master.tmpl"
    out()<<"'>home</a>\n"
           "\t\t\t</li>\n"
           "\t\t\t<li>\n"
           "\t\t\t\t<a href='";
#line 30 "master.tmpl"
    content.app().mapper().map(out(),"/about");
#line 33 "master.tmpl"
    out()<<"'>about</a>\n"
           "\t\t\t</li>\n"
           "\t\t\t<li>\n"
           "\t\t\t\t<a href='";
#line 33 "master.tmpl"
    content.app().mapper().map(out(),"/gallery");
#line 36 "master.tmpl"
    out()<<"'>gallery</a>\n"
           "\t\t\t</li>\n"
           "\t\t\t<li>\n"
           "\t\t\t\t<a href='";
#line 36 "master.tmpl"
    content.app().mapper().map(out(),"/blog");
#line 39 "master.tmpl"
    out()<<"'>blog</a>\n"
           "\t\t\t</li>\n"
           "\t\t\t<li>\n"
           "\t\t\t\t<a href='";
#line 39 "master.tmpl"
    content.app().mapper().map(out(),"/contact");
#line 44 "master.tmpl"
    out()<<"'>contact</a>\n"
           "\t\t\t</li>\n"
           "\t\t</ul>\n"
           "\t</div>\n"
           "\t<div id=\"body\">\n"
           "";
#line 44 "master.tmpl"
    page_content();
#line 70 "master.tmpl"
    out()<<"\n"
           "\t</div>\n"
           "\t<div id=\"footer\">\n"
           "\t\t<div>\n"
           "\t\t\t<p>&copy; 2023 by Mustacchio. All rights reserved.</p>\n"
           "\t\t\t<!--\n"
           "\t\t\t<ul>\n"
           "\t\t\t\t<li>\n"
           "\t\t\t\t\t<a href=\"http://freewebsitetemplates.com/go/twitter/\" id=\"twitter\">twitter</a>\n"
           "\t\t\t\t</li>\n"
           "\t\t\t\t<li>\n"
           "\t\t\t\t\t<a href=\"http://freewebsitetemplates.com/go/facebook/\" id=\"facebook\">facebook</a>\n"
           "\t\t\t\t</li>\n"
           "\t\t\t\t<li>\n"
           "\t\t\t\t\t<a href=\"http://freewebsitetemplates.com/go/googleplus/\" id=\"googleplus\">googleplus</a>\n"
           "\t\t\t\t</li>\n"
           "\t\t\t\t<li>\n"
           "\t\t\t\t\t<a href=\"http://pinterest.com/fwtemplates/\" id=\"pinterest\">pinterest</a>\n"
           "\t\t\t\t</li>\n"
           "\t\t\t</ul>\n"
           "\t\t\t-->\n"
           "\t\t</div>\n"
           "\t</div>\n"
           "</body>\n"
           "</html>\n"
           "\n"
           "";
#line 70 "master.tmpl"
  } // end of template render
#line 71 "master.tmpl"
 private:
#line 71 "master.tmpl"
  int _domain_id;
#line 71 "master.tmpl"
}; // end of class master
#line 72 "master.tmpl"
} // end of namespace my_skin
#line 1 "./gallery.tmpl"
#include "content.h"
#line 2 "./gallery.tmpl"
namespace my_skin {
#line 3 "./gallery.tmpl"
struct  gallery :public master
#line 3 "./gallery.tmpl"
{
#line 3 "./gallery.tmpl"
  content::master &content;
#line 3 "./gallery.tmpl"
  gallery(std::ostream &_s,content::master &_content): master(_s,_content),content(_content),_domain_id(0)
#line 3 "./gallery.tmpl"
  {
#line 3 "./gallery.tmpl"
    _domain_id=booster::locale::ios_info::get(_s).domain_id();
#line 3 "./gallery.tmpl"
  }
#line 4 "./gallery.tmpl"
  virtual void page_content() {
#line 4 "./gallery.tmpl"
    cppcms::translation_domain_scope _trs(out(),_domain_id);

#line 8 "./gallery.tmpl"
    out()<<"\n"
           "<h1><span>gallery</span></h1>\n"
           "<ul class=\"gallery\">\n"
           "<li>\n"
           "\t<a href='";
#line 8 "./gallery.tmpl"
    content.app().mapper().map(out(),"/gallery-post");
#line 13 "./gallery.tmpl"
    out()<<"'>\n"
           "\t\t<img src=\"images/mustache1.jpg\" alt=\"\">\n"
           "\t</a>\n"
           "</li>\n"
           "<li>\n"
           "\t<a href='";
#line 13 "./gallery.tmpl"
    content.app().mapper().map(out(),"/gallery-post");
#line 18 "./gallery.tmpl"
    out()<<"'>\n"
           "\t\t<img src=\"images/mustache2.jpg\" alt=\"\">\n"
           "\t</a>\n"
           "</li>\n"
           "<li>\n"
           "\t<a href='";
#line 18 "./gallery.tmpl"
    content.app().mapper().map(out(),"/gallery-post");
#line 23 "./gallery.tmpl"
    out()<<"'>\n"
           "\t\t<img src=\"images/mustache3.jpg\" alt=\"\">\n"
           "\t</a>\n"
           "</li>\n"
           "<li>\n"
           "\t<a href='";
#line 23 "./gallery.tmpl"
    content.app().mapper().map(out(),"/gallery-post");
#line 28 "./gallery.tmpl"
    out()<<"'>\n"
           "\t\t<img src=\"images/mustache4.jpg\" alt=\"\">\n"
           "\t</a>\n"
           "</li>\n"
           "<li>\n"
           "\t<a href='";
#line 28 "./gallery.tmpl"
    content.app().mapper().map(out(),"/gallery-post");
#line 33 "./gallery.tmpl"
    out()<<"'>\n"
           "\t\t<img src=\"images/mustache5.jpg\" alt=\"\">\n"
           "\t</a>\n"
           "</li>\n"
           "<li>\n"
           "\t<a href='";
#line 33 "./gallery.tmpl"
    content.app().mapper().map(out(),"/gallery-post");
#line 38 "./gallery.tmpl"
    out()<<"'>\n"
           "\t\t<img src=\"images/mustache6.jpg\" alt=\"\">\n"
           "\t</a>\n"
           "</li>\n"
           "<li>\n"
           "\t<a href='";
#line 38 "./gallery.tmpl"
    content.app().mapper().map(out(),"/gallery-post");
#line 43 "./gallery.tmpl"
    out()<<"'>\n"
           "\t\t<img src=\"images/mustache7.jpg\" alt=\"\">\n"
           "\t</a>\n"
           "</li>\n"
           "<li>\n"
           "\t<a href='";
#line 43 "./gallery.tmpl"
    content.app().mapper().map(out(),"/gallery-post");
#line 48 "./gallery.tmpl"
    out()<<"'>\n"
           "\t\t<img src=\"images/mustache8.jpg\" alt=\"\">\n"
           "\t</a>\n"
           "</li>\n"
           "<li>\n"
           "\t<a href='";
#line 48 "./gallery.tmpl"
    content.app().mapper().map(out(),"/gallery-post");
#line 53 "./gallery.tmpl"
    out()<<"'>\n"
           "\t\t<img src=\"images/mustache9.jpg\" alt=\"\">\n"
           "\t</a>\n"
           "</li>\n"
           "</ul>\n"
           "";
#line 53 "./gallery.tmpl"
  } // end of template page_content
#line 54 "./gallery.tmpl"
 private:
#line 54 "./gallery.tmpl"
  int _domain_id;
#line 54 "./gallery.tmpl"
}; // end of class gallery
#line 55 "./gallery.tmpl"
} // end of namespace my_skin
#line 2 "master.tmpl"
namespace my_skin {
#line 72 "master.tmpl"
} // end of namespace my_skin
#line 2 "./gallery.tmpl"
namespace my_skin {
#line 55 "./gallery.tmpl"
} // end of namespace my_skin
#line 56 "./gallery.tmpl"
namespace {
#line 56 "./gallery.tmpl"
cppcms::views::generator my_generator;
#line 56 "./gallery.tmpl"
struct loader {
#line 56 "./gallery.tmpl"
  loader() {
#line 56 "./gallery.tmpl"
    my_generator.name("my_skin");
#line 56 "./gallery.tmpl"
    my_generator.add_view<my_skin::master,content::master>("master",true);
#line 56 "./gallery.tmpl"
    my_generator.add_view<my_skin::gallery,content::master>("gallery",true);
#line 56 "./gallery.tmpl"
    cppcms::views::pool::instance().add(my_generator);
#line 56 "./gallery.tmpl"
  }
#line 56 "./gallery.tmpl"
  ~loader() {  cppcms::views::pool::instance().remove(my_generator); }
#line 56 "./gallery.tmpl"
} a_loader;
#line 56 "./gallery.tmpl"
} // anon 
