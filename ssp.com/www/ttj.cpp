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
//#include "adschain/tools/Stopwatch.h"

//#define RAPIDJSON_HAS_STDSTRING 1
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"

using namespace adschain;


class ttj : public cppcms::application {
  std::string adexchange = "adx.com";
  PublicKeyService * publicKeyService;
  fruit::Injector<PrivateKey, Signer> signerInjector;
  fruit::Injector<Verifier> verifierInjector;
  Poco::Logger &logger = Poco::Logger::get("ssp.com");
//  TimeLogger& timeLogger;

  Signer* signer;
  Verifier* verifier;

 public:
  explicit ttj(cppcms::service &srv, PublicKeyService* publicKeyService) :
//  explicit ttj(cppcms::service &srv, PublicKeyService* publicKeyService, TimeLogger& timeLogger) :
    cppcms::application(srv),
    publicKeyService(publicKeyService),
    signerInjector(getSignerComponentFromFile, "/home/debian/ssl/private/ssp-domain.key"),
    verifierInjector(getVerifierComponent)
//    ,timeLogger(timeLogger)
  {
    signer = signerInjector.get<Signer*>();
    verifier = verifierInjector.get<Verifier*>();

    dispatcher().assign("", &ttj::process, this);
    mapper().assign("");  // default URL

    mapper().root("/ttj");
  }
  void process() {
    auto queryString = request().query_string();
    auto client_ip = request().remote_addr();
    auto domain = request().server_name();

//    logger.information("Ad-tag query string: %s", queryString);
    std::unique_ptr<rapidjson::Document> documentPtr = QueryString2RapidjsonDocument(queryString);
    rapidjson::Document &adInfo = *documentPtr;
    if(documentPtr->HasMember("custody-index")) {
//      Stopwatch sw(timeLogger.getLogger());
      const char custodyIndex[2] = {adInfo["custody-index"].GetString()[0], '\0'};
//    logger.information("custody index: %s", std::string(custodyIndex));
//    logger.information("ad custody: %s", (std::string) adInfo["custody"][custodyIndex].GetString());

      if (adInfo["custody"][custodyIndex].GetString() != request().server_name()) {
        logger.information("Transaction custody not assigned to this server. Discarding");
        response().make_error_response(400);
      }

      const char prevCustodyIndex[2] = {custodyIndex[0] - 1, '\0'};
      const char nextCustodyIndex[2] = {custodyIndex[0] + 1, '\0'};
      //    logger.information("Previous custody index %s", std::string(prevCustodyIndex));
      //    logger.information("Next custody index %s", std::string(nextCustodyIndex));
      std::string prevDomain = adInfo["custody"][prevCustodyIndex].GetString();
      std::shared_ptr<PublicKey> senderPubKey = publicKeyService->getDomainPublicKey(prevDomain);
//      sw.stop("public-key-service");
      std::string receivedSignature = std::string(adInfo["signature"][prevCustodyIndex].GetString());
//      logger.information("receivedSignature: %s", receivedSignature);

      if (!verifier->verifyB64(senderPubKey.get(), receivedSignature, GetSignatureAtIndex(adInfo, prevCustodyIndex))) {
        logger.information("Signature not valid. Discarding");
        response().make_error_response(400);
      }
//      sw.stop("verification");
      rapidjson::Document::AllocatorType &allocator = adInfo.GetAllocator();
      adInfo["custody"].AddMember(nextCustodyIndex, adexchange, allocator);
      adInfo["custody-index"] = nextCustodyIndex;
      std::stringstream msgstr, sign_str;
      msgstr << adInfo["signature"][prevCustodyIndex].GetString() << ";" << adexchange;
      sign_str << "signature[" << prevCustodyIndex << "];custody[" << nextCustodyIndex << "]";

      adInfo["keys-string"].AddMember(custodyIndex, sign_str.str(), allocator);
      std::string ssp_sign = signer->signB64(msgstr.str());
      adInfo["signature"].AddMember(custodyIndex, ssp_sign, adInfo.GetAllocator());
//      sw.stop("signing");
    }
//     logger.information(RapidJsonDocument2QueryString(adInfo));
  try {
    std::string adxResponse = HttpGetRequest("https://" + adexchange + "/ttj", RapidJsonDocument2QueryString(adInfo));
//    std::string adxResponse("times-test-response");
    // logger.information("RESPONSE FROM AD-REQUEST GET METHOD: " + adxResponse);

    response().out()
        << "document.write('<iframe frameborder=\"0\" width=\"300\" height=\"250\" marginheight=\"0\" marginwidth=\"0\" target=\"_blank\" scrolling=\"no\" "
        << "src=\""
        << adxResponse
        << "\"></iframe>');";
  } catch (const std::exception& e) {
    response().make_error_response(400);
    logger.error("HttpGetRequest exception %s", std::string(e.what()));
  }
}
};

int main(int argc, char ** argv) {
//  ==> Time measurement and logging code commented by default
//  const char * timesLog = argv[argc-1];// : "publisher-times-log.txt";
//  TimeLogger timeLogger(timesLog);
  Poco::AutoPtr<Poco::ConsoleChannel> pCons(new Poco::ConsoleChannel);
  Poco::Logger::root().setChannel(pCons);
  Poco::Logger &logger = Poco::Logger::get("publisher.com");
//  logger.information("Logging times of index page to: %s", std::string(timesLog));

  fruit::Injector<PublicKeyService> injector(getPublicKeyServiceComponent, "/etc/ssl/certs/");
  PublicKeyService *publicKeyService(injector);

  try {
    cppcms::service app(argc, argv);
    app.applications_pool()
        .mount(cppcms::applications_factory<ttj, PublicKeyService*>(publicKeyService));
//    .mount(cppcms::applications_factory<ttj, PublicKeyService*, TimeLogger&>(publicKeyService, timeLogger));
    app.run();
  } catch(std::exception const &e) {
    std::cerr << e.what() << std::endl;
  }
}
