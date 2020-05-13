#include <stdlib.h>
#include <iostream>
#include <thread>
#include <sys/sysinfo.h>

#include <cppcms/application.h>
#include <cppcms/service.h>
#include <cppcms/http_request.h>
#include <cppcms/http_response.h>
#include <cppcms/url_dispatcher.h>
#include <cppcms/url_mapper.h>
#include <cppcms/applications_pool.h>

#include "Poco/Logger.h"
#include "Poco/ConsoleChannel.h"
#include "Poco/AutoPtr.h"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "adschain/adschain.h"
#include "adschain/tools/unixtime_generator.h"
//#include "adschain/tools/Stopwatch.h"

#include "./content.h"

using namespace adschain;

class mostachio : public cppcms::application {
  std::string sspserver = "ssp.com";
  boost::uuids::unixtime_generator uuid_gen;
  fruit::Injector<PrivateKey, Signer> injector;
  Signer* signer;
  Poco::Logger &logger = Poco::Logger::get("publisher.com");
//  TimeLogger& timeLogger;
 public:
  explicit mostachio(cppcms::service &srv) :
      cppcms::application(srv), uuid_gen(0x0943, 0x345670ABCDEF), injector(getSignerComponentFromFile, "/home/debian/ssl/private/publisher-domain.key") {
//  explicit mostachio(cppcms::service &srv, TimeLogger& timeLogger) :
//      cppcms::application(srv), uuid_gen(0x0943, 0x345670ABCDEF), injector(getSignerComponentFromFile, "/home/debian/ssl/private/publisher-domain.key"), timeLogger(timeLogger) {

//    timeLogger.getLogger().information("# mostachio constructor");

    signer = injector.get<Signer*>();

    dispatcher().assign("/about", &mostachio::about, this);
    mapper().assign("about", "about");

    dispatcher().assign("/contact", &mostachio::contact, this);
    mapper().assign("contact", "contact");

    dispatcher().assign("/blog", &mostachio::blog, this);
    mapper().assign("blog", "blog");

    dispatcher().assign("/blog-post", &mostachio::blog_post, this);
    mapper().assign("blog-post", "blog-post");

    dispatcher().assign("/gallery", &mostachio::gallery, this);
    mapper().assign("gallery", "gallery");

    dispatcher().assign("/gallery-post", &mostachio::gallery_post, this);
    mapper().assign("gallery-post", "gallery-post");

    dispatcher().assign("/", &mostachio::index, this);
    mapper().assign("");  // default URL

    mapper().root("/");
  }

  void index() {
//    AutoStopwatch sw(timeLogger.getLogger());

    auto domain = request().server_name();
    auto client_ip = request().remote_addr();
    auto queryString = request().query_string();

    std::stringstream adtag_list_300x250;
    const bool plain_rtb = queryString.find("PLAIN-RTB") != std::string::npos;
    if (plain_rtb) {
      uuid_gen.generate();
    }
//    sw.setSignatureState(not plain_rtb);

    std::string test_id = "";
    const size_t test_id_key = queryString.find("TEST-ID");
    if (test_id_key != std::string::npos) {
      try {
        const size_t test_id_val_pos0 = queryString.find("=", test_id_key)+1;
        const size_t test_id_val_pos1 = queryString.find("&", test_id_val_pos0);
        test_id = queryString.substr(test_id_val_pos0, test_id_val_pos1);
      }
      catch (const std::exception& e) {
        response().make_error_response(400);
        logger.error("query string NUMBER-OF-ADS format error");
      }
    }
//    sw.setTestID(test_id.c_str());

    auto ad_size = "300x250";
    int n_ads = 1;
    const size_t n_ads_key = queryString.find("NUMBER-OF-ADS");
    if (n_ads_key != std::string::npos) {
      try {
        const size_t n_ads_val_pos0 = queryString.find("=", n_ads_key)+1;
        const size_t n_ads_val_pos1 = queryString.find("&", n_ads_val_pos0);
        n_ads = std::stoi(queryString.substr(n_ads_val_pos0, n_ads_val_pos1));
      }
      catch (const std::exception& e) {
        response().make_error_response(400);
        logger.error("query string NUMBER-OF-ADS format error");
      }
    }
//    sw.setNumberOfAds(n_ads);

    for (int idx = 0; idx < n_ads; ++idx) {
      adtag_list_300x250 << " <li> \n";
      adtag_list_300x250 << " <script src=\"https://" << sspserver << "/ttj"
                    << "?ad-size=" << ad_size;
      auto uuid = uuid_gen.generate();
      if (! plain_rtb) {
        std::stringstream msgstr;
        msgstr << domain << ";" << sspserver << ";"
             << uuid << ";" << client_ip << ";" << ad_size;
        auto sign_string = "custody[0];custody[1];tuuid;user-ip;ad-size";

        std::string dom_sign = signer->signB64(msgstr.str().c_str());

        adtag_list_300x250 << "&tuuid=" << uuid
                      << "&user-ip=" << client_ip
                      << "&custody[0]=" << domain
                      << "&custody[1]=" << sspserver
                      << "&custody-index=" << "1"
                      << "&signature[0]=" << dom_sign
                      << "&keys-string[0]=" << sign_string;
      } else {
//        adtag_list_300x250 << "&tuuid=" << uuid;
        adtag_list_300x250 << "&id=012534" << idx;
      }
      adtag_list_300x250 << "\" type=\"text/javascript\"></script> \n";
      adtag_list_300x250 << " </li>\n";
    }

//    logger.information(adtag_list_300x250.str());
    content::index c(adtag_list_300x250.str());
    render("index", c);
  }

  void about() {
    content::master c("About");
    render("about", c);
  }

  void blog() {
    content::master c("Blog");
    render("blog", c);
  }

  void blog_post() {
    content::master c("Post - Blog ");
    render("blog_post", c);
  }

  void gallery() {
    content::master c("Gallery");
    render("gallery", c);
  }

  void gallery_post() {
    content::master c("Post - Gallery");
    render("gallery_post", c);
  }

  void contact() {
    content::master c("Contact");
    render("contact", c);
  }
};

int main(int argc, char ** argv) {
//  ==> Time measurement and logging code commented by default
//  const char * timesLog = argv[argc-1];// : "publisher-times-log.txt";
//  TimeLogger timeLogger(timesLog);
  Poco::AutoPtr<Poco::ConsoleChannel> pCons(new Poco::ConsoleChannel);
  Poco::Logger::root().setChannel(pCons);
  Poco::Logger &logger = Poco::Logger::get("publisher.com");
  logger.information("hello");
//  logger.information("Logging times of index page to: %s", std::string(timesLog));
//  struct sysinfo info;
//  sysinfo(&info);
//  timeLogger.getLogger().information("## %u cpus - %.2f gb ram", std::thread::hardware_concurrency(), info.totalram/1073741824.0);

  try {
    cppcms::service app(argc, argv);
    app.applications_pool().
        mount(cppcms::applications_factory<mostachio>());
//        mount(cppcms::applications_factory<mostachio, TimeLogger&>(timeLogger));

    app.run();
  }
  catch(std::exception const &e) {
    std::cerr << e.what() << std::endl;
  }
}
