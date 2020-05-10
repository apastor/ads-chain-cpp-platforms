#include <iostream>
#include <unordered_map>
#include <stdlib.h>
#include <chrono>
#include <thread>
#include <future>

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

const std::chrono::milliseconds BID_TIME = std::chrono::milliseconds(120);

const std::string tempCustody = "AUCTION";
class ttj: public cppcms::application {
  std::string ssp = "ssp.com";
  std::string dsp = "dsp.com";
  std::string dspBid = "dsp.com/bid";
  std::string dspWinning = "dsp.com/win";
  std::string adexchange = "adx.com";
  PublicKeyService * publicKeyService;
  fruit::Injector<PrivateKey, Signer> signerInjector;
  fruit::Injector<Verifier> verifierInjector;
  Poco::Logger &logger = Poco::Logger::get("adx.com");

  Signer* signer;
  Verifier* verifier;

 public:
  explicit ttj(cppcms::service &srv, PublicKeyService* publicKeyService) :
    cppcms::application(srv),
    publicKeyService(publicKeyService),
    signerInjector(getSignerComponentFromFile, "/home/debian/ssl/private/adx-domain.key"),
    verifierInjector(getVerifierComponent)
  {
    signer = signerInjector.get<Signer*>();
    verifier = verifierInjector.get<Verifier*>();
    dispatcher().assign("", &ttj::process, this);
    mapper().assign("");  // default URL

    mapper().root("/ttj");
  }
  void process() {
    auto queryString = request().query_string();
//    logger.information("Ad-request query string: %s", queryString);
    std::unique_ptr<rapidjson::Document> documentPtr = QueryString2RapidjsonDocument(queryString);
    rapidjson::Document &adInfo = *documentPtr;

    if(documentPtr->HasMember("custody-index")) {

      const char custodyIndex[2] = {adInfo["custody-index"].GetString()[0], '\0'};

      if (adInfo["ad-custody"][custodyIndex].GetString() != request().server_name()) {
        logger.information("Transaction custody not assigned to this server. Discarding");
        response().make_error_response(400);
      }

      const char prevCustodyIndex[2] = {custodyIndex[0] - 1, '\0'};
      const char nextCustodyIndex[2] = {custodyIndex[0] + 1, '\0'};
      std::string prevDomain = adInfo["ad-custody"][prevCustodyIndex].GetString();
      std::shared_ptr<PublicKey> senderPubKey = publicKeyService->getDomainPublicKey(prevDomain);
      std::string receivedSignature = std::string(adInfo["sign"][prevCustodyIndex].GetString());
      if (!verifier->verifyB64(senderPubKey.get(), receivedSignature, GetSignatureAtIndex(adInfo, prevCustodyIndex))) {
        logger.information("Signature not valid. Discarding");
        response().make_error_response(400);
      }

// temporary signature to bid request
      rapidjson::Document::AllocatorType &allocator = adInfo.GetAllocator();
      adInfo["ad-custody"].AddMember(nextCustodyIndex, tempCustody, allocator);
      std::stringstream msgstrTemp, sign_strTemp;
      msgstrTemp << adInfo["sign"][prevCustodyIndex].GetString() << ";" << tempCustody;
      sign_strTemp << "sign[" << prevCustodyIndex << "];ad-custody[" << nextCustodyIndex << "]";
      adInfo["keys-string"].AddMember(custodyIndex, sign_strTemp.str(), allocator);
      std::string adx_temp_sign = signer->signB64(msgstrTemp.str());
      adInfo["sign"].AddMember(custodyIndex, adx_temp_sign, allocator);

    }

    // send bid request[s] and wait 120ms for the bid-reponses
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    adInfo.Accept(writer);

    auto futureBidResponse = std::async(std::launch::async, &HttpPostJsonRequest, "https://" + dspBid, std::string(buffer.GetString(), buffer.GetSize()), BID_TIME.count());
    std::this_thread::sleep_for(BID_TIME);
    if (futureBidResponse.wait_for(std::chrono::seconds(0)) != std::future_status::ready) {
      response().make_error_response(cppcms::http::response::status_type::request_time_out, "NO DSP RESPONSE");
      return;
    }
    std::string bidResponse = futureBidResponse.get();

    // logger.information("BID RESPONSE");
    // logger.information(bidResponse);
    response().out() << bidResponse;


    if(documentPtr->HasMember("custody-index")) {
      const char custodyIndex[2] = {adInfo["custody-index"].GetString()[0], '\0'};
      const char prevCustodyIndex[2] = {custodyIndex[0] - 1, '\0'};
      const char nextCustodyIndex[2] = {custodyIndex[0] + 1, '\0'};
      // winning notice with final signature
      adInfo["ad-custody"][nextCustodyIndex].SetString(dsp, adInfo.GetAllocator());
      adInfo["custody-index"] = nextCustodyIndex;
      std::stringstream msgstrFinal, sign_strFinal;
      msgstrFinal << adInfo["sign"][prevCustodyIndex].GetString() << ";" << dsp;
      sign_strFinal << "sign[" << prevCustodyIndex << "];ad-custody[" << nextCustodyIndex << "]";
      adInfo["keys-string"][custodyIndex].SetString(sign_strFinal.str(), adInfo.GetAllocator());
      std::string adx_final_sign = signer->signB64(msgstrFinal.str());
      adInfo["sign"][custodyIndex].SetString(adx_final_sign, adInfo.GetAllocator());

    }
    rapidjson::StringBuffer bufferWin;
    rapidjson::Writer<rapidjson::StringBuffer> writerWin(bufferWin);
    adInfo.Accept(writerWin);
    std::string winNoticeResponse = HttpPostJsonRequest("https://" + dspWinning, std::string(bufferWin.GetString(), bufferWin.GetSize()), 1000);
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
        .mount(cppcms::applications_factory<ttj, PublicKeyService*>(publicKeyService));
    app.run();
  } catch(std::exception const &e) {
    std::cerr << e.what() << std::endl;
  }
}
