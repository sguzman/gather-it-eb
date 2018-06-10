#pragma once
#include <curl/curl.h>
#include <string>

namespace Curl {
  inline static size_t callback(void * contents, const size_t size, const size_t nmemb, std::string * s) noexcept
  {
    const size_t newLength = size * nmemb;
    const size_t oldLength = s->size();
    s->resize(oldLength + newLength);

    std::copy(static_cast<char*>(contents), static_cast<char*>(contents) + newLength, s->begin() + oldLength);
    return size*nmemb;
  }

  inline static const std::string get(const std::string& url) noexcept {
    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);

    curl = curl_easy_init();
    std::string s;
    if(curl)
    {

      curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

      curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); //only for https
      curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L); //only for https
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
      curl_easy_setopt (curl, CURLOPT_VERBOSE, 0L); //remove this to disable verbose output


      /* Perform the request, res will get the return code */
      res = curl_easy_perform(curl);
      /* Check for errors */
      if(res != CURLE_OK)
      {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
      }

      /* always cleanup */
      curl_easy_cleanup(curl);
    }

    return s;
  }
}