#include "../../lib/json/json.h"
#include "../../lib/curl/curl.h"
#include <openssl/evp.h>
#include <openssl/err.h>
#include <iostream>
#include <string>
#include <sstream>

/**
 * @brief CURLOPT_WRITEFUNCTION
 * @param contents buffer pointer
 * @param size data element size
 * @param nmemb element count
 * @param data response data
 * @return byte size
 */
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* data) {
    size_t totalSize = size * nmemb;
    data->append((char*)contents, totalSize);
    return totalSize;
}

/**
 * @brief libcurl Status and data save
 * @param curl curl struct
 * @return libcurl status value
 */
CURLcode libcurl_res_get(CURL* curl){

    // get response code and save data in curl pointer
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        printf("libcurl error: %s \n", curl_easy_strerror(res));
    } else {
        long response_code;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        printf("HTTP Response Code: %ld \n", response_code);
    }
    return res;
}

/**
 * @brief server request
 * @param url IP
 * @param json_body JSON 요청 문자열
 * @param resp_msg 응답 저장
 * @return Success 0 FAIL else
 */
int server_request(const std::string& url, const std::string& json_body, std::string& resp_msg) {
    resp_msg.clear();
    CURL* curl = curl_easy_init();
    if(!curl) {
        std::cerr << "CURL init failed\n";
        return CURLE_FAILED_INIT;
    }

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_body.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp_msg);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

    CURLcode res = curl_easy_perform(curl);
    if(res != CURLE_OK) {
        std::cerr << "CURL error: " << curl_easy_strerror(res) << "\n";
        return -1;
    }

    long response_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    std::cout << "HTTP Response Code: " << response_code << "\n";

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    return 0;
}

int hello_request(){
    CURL* curl = curl_easy_init();
    if(!curl) return 1;

    std::string response;
    curl_easy_setopt(curl, CURLOPT_URL, "http://127.0.0.1:1234/hello");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    if(res != CURLE_OK) {
        std::cerr << "CURL error: " << curl_easy_strerror(res) << "\n";
    } else {
        std::cout << "Server Response: " << response << "\n";
    }

    curl_easy_cleanup(curl);
    return 0;
}

int param_request(){

   CURL* curl = curl_easy_init();
    if(!curl) return 1;

    std::string response;
    int a = 3;
    int b = 5;
    std::string url = "http://127.0.0.1:1234/add/" + std::to_string(a) + "/" + std::to_string(b);

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    if(res != CURLE_OK) {
        std::cerr << "CURL error: " << curl_easy_strerror(res) << "\n";
    } else {
        std::cout << "Raw Response: " << response << "\n";

        // JSON 파싱
        Json::CharReaderBuilder reader;
        Json::Value root;
        std::istringstream s(response);
        if(Json::parseFromStream(reader, s, &root, nullptr)) {
            std::cout << "Result: " << root["result"].asInt() << "\n";
        }
    }

    curl_easy_cleanup(curl);
    return 0;
}

int main() {
    std::string response;

    // make json
    Json::Value root;
    root["name"] = "abc";
    root["msg"] = "10";
    root["value"] = "asdasd";

    Json::StreamWriterBuilder writer;
    std::string jsonBody = Json::writeString(writer, root);

    // server request
    std::string url = "http://127.0.0.1:1234/echo";
    //server_request(url, jsonBody, response);

    hello_request();
    param_request();

    std::cout << "Server Response: " << response << std::endl;
    return 0;
}