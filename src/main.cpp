#include "main.h"
#include "Util.h"
#include "Define.h"
#include "../lib/json/json.h"
#include "../lib/curl/curl.h"
#include <openssl/evp.h>
#include <openssl/err.h>
#include <string>
#include <cstring>

Util util;

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

int main() {

    // JSON 생성
    Json::Value resroot;
    resroot["key"] = "value";
    resroot["dsa"] = "asd";

    Json::StreamWriterBuilder builder;
    std::string jsonBody = Json::writeString(builder, resroot);

    std::cout << "[Send JSON] " << jsonBody << std::endl;

    CURL* curl = curl_easy_init();
    if(curl){
        printf("curl success \n");
        CURLcode res;
        std::string readBuffer;
        struct curl_slist* headers = NULL;

        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:1234/api");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonBody.c_str());

        // 응답 콜백 설정
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        // 요청 수행
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "libcurl error: " << curl_easy_strerror(res) << std::endl;
        } else {
            std::cout << "[Response] " << readBuffer << std::endl;
        }

        // 정리
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

    } else {
        printf("curl fail \n");
    }





    return 0;
}