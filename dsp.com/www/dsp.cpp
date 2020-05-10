#include <iostream>
#include <unordered_map>
#include <stdlib.h>

#include <cppcms/application.h>
#include <cppcms/service.h>

#include <cppcms/http_request.h>
#include <cppcms/http_response.h>
#include <cppcms/url_dispatcher.h>
#include <cppcms/url_mapper.h>
#include <cppcms/applications_pool.h>
#include <cppcms/session_interface.h>


#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "Poco/Logger.h"
#include "Poco/ConsoleChannel.h"
#include "Poco/AutoPtr.h"

#include "adschain/adschain.h"

//#define RAPIDJSON_HAS_STDSTRING 1
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

using namespace adschain;



class dsp: public cppcms::application {
  std::string adserver = "adserver.com";
  std::string adPath = "https://" + adserver + "/mostachio-ad.html";
  PublicKeyService* publicKeyService;
  fruit::Injector<Verifier> verifierInjector;

  Signer* signer;
  Verifier* verifier;
  Poco::Logger &logger = Poco::Logger::get("adserver.com");

public:
  explicit dsp(cppcms::service &srv, PublicKeyService* publicKeyService) : 
    cppcms::application(srv),
    publicKeyService(publicKeyService),
    verifierInjector(getVerifierComponent)
  {
    verifier = verifierInjector.get<Verifier*>();

    dispatcher().assign("/",&dsp::process,this);
    mapper().assign(""); // default URL

    dispatcher().assign("/bid", &dsp::bid, this);
    mapper().assign("bid", "bid");

    dispatcher().assign("/win", &dsp::win, this);
    mapper().assign("win", "win");

    mapper().root("/");
  }

  void process(){
    logger.information(request().path_info());
    response().out() << "<h1>dsp.com home page default response</h1>\n";
  }

  void bid(){
    rapidjson::Document bidRequest;
   // response().set_content_header("application/json");

    auto post_data = request().raw_post_data();

    if (post_data.second > 0) {
      std::string dataString = std::string(
          reinterpret_cast<char const *>(post_data.first), post_data.second);
      if (bidRequest.Parse(dataString.c_str()).HasParseError())
        logger.error("Invalid JSON");
    } else {
      logger.error("No data in POST");
    }
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    bidRequest.Accept(writer);
   logger.information("BID REQUEST json post data: %s", std::string(buffer.GetString(), buffer.GetSize()));

    if(bidRequest.HasMember("custody-index")) {
      const char custodyIndex[2] = {bidRequest["custody-index"].GetString()[0], '\0'};
      const char nextCustodyIndex[2] = {custodyIndex[0] + 1, '\0'};
      if (bidRequest["ad-custody"][nextCustodyIndex].GetString() != std::string("AUCTION")) {
        logger.information("[BID] Transaction custody not assigned to this server. Discarding");
        response().make_error_response(400);
      }
      std::string prevDomain = bidRequest["ad-custody"][custodyIndex].GetString();
      std::shared_ptr<PublicKey> senderPubKey = publicKeyService->getDomainPublicKey(prevDomain);
      if (senderPubKey == nullptr) logger.information("no ADX publicKey");
      std::string receivedSignature = std::string(bidRequest["sign"][custodyIndex].GetString());
      if (!verifier->verifyB64(senderPubKey.get(), receivedSignature, GetSignatureAtIndex(bidRequest, custodyIndex))) {
        logger.information("[BID] Signature not valid. Discarding");
        response().make_error_response(400);
      }
    }
      response().out() << adPath;
    }

  void win() {
    rapidjson::Document winNotice;
    //response().set_content_header("application/json");

    auto post_data = request().raw_post_data();

    if (post_data.second > 0) {
      std::string dataString = std::string(
          reinterpret_cast<char const *>(post_data.first), post_data.second);
      if (winNotice.Parse(dataString.c_str()).HasParseError())
        logger.error("Invalid JSON");
    } else {
      logger.error("No data in POST");
    }
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    winNotice.Accept(writer);
   logger.information("WIN NOTICE json post data: %s", std::string(buffer.GetString(), buffer.GetSize()));

    if (winNotice.HasMember("custody-index")) {
      const char custodyIndex[2] = {winNotice["custody-index"].GetString()[0], '\0'};
      if (winNotice["ad-custody"][custodyIndex].GetString() != request().server_name()) {
        logger.information("[WIN] Transaction custody not assigned to this server. Discarding");
        response().make_error_response(400);
      }
      const char prevCustodyIndex[2] = {custodyIndex[0] - 1, '\0'};
      std::string prevDomain = winNotice["ad-custody"][prevCustodyIndex].GetString();
      std::shared_ptr<PublicKey> senderPubKey = publicKeyService->getDomainPublicKey(prevDomain);
      std::string receivedSignature = std::string(winNotice["sign"][prevCustodyIndex].GetString());
      if (!verifier->verifyB64(senderPubKey.get(),
                               receivedSignature,
                               GetSignatureAtIndex(winNotice, prevCustodyIndex))) {
        logger.information("[WIN] Signature not valid. Discarding");
        response().make_error_response(400);
      }
    }
  }
};

int main(int argc,char ** argv){
  Poco::AutoPtr<Poco::ConsoleChannel> pCons(new Poco::ConsoleChannel);
  Poco::Logger::root().setChannel(pCons);

  fruit::Injector<PublicKeyService> injector(getPublicKeyServiceComponent, "/etc/ssl/certs/");
  PublicKeyService *publicKeyService(injector);

  try {
    cppcms::service app(argc, argv);
    app.applications_pool()
        .mount(cppcms::applications_factory<dsp, PublicKeyService*>(publicKeyService));
    app.run();
  } catch(std::exception const &e) {
    std::cerr << e.what() << std::endl;
  }
}
