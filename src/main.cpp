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

/**
 * @brief Check Protocol http://, https://
 * @param ip URL
 * @return http 1, https 2, fail -1
 */
int check_protocol(std::string ip){

    int first_dot = ip.find(":");
    std::string ip_protocol = ip.substr(0, first_dot);

    if(ip_protocol == "http"){
        return HTTP_STATUS;
    }
    else if(ip_protocol == "https"){
        return HTTPS_STATUS;
    }
    return FAIL;
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
 * @brief Qgrent Server request
 * @param ip Server IP
 * @param input_msg input json message
 * @param resp_msg output json message
 * @return Success 0, Fail else
 */ 
int qserver_request(std::string ip, std::string input_msg ,std::string& resp_msg){

    resp_msg.clear();

    // Json parse
    Json::Value root;
    Json::Reader reader;
    if (!reader.parse(input_msg, root)) {
        printf("JSON parse error\n");
        return FAIL;
    }

    // request message parcsing
    std::string url_data = ip;
    std::string query;

    for (const auto& key : root.getMemberNames()) {
        std::string value = root[key].asString();
        if (!value.empty()) {
            if (!query.empty()){ // not first, add '&'
                query += "&";
            }
            query += key + "=" + value; // value add
        }
    }
    if(!query.empty()){
        url_data += "?" + query;
    }

    // Libcurl init
    CURL* curl = curl_easy_init();
    if(!curl) {
        printf(CURL_MSG_INIT_FAIL"\n");
        return FAIL;
    }

    // struct curl_slist *headers = NULL;
    // curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "https");
    // curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    // curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");

    curl_easy_setopt(curl, CURLOPT_URL, url_data.c_str());
    // curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp_msg);
    
    CURLcode res = libcurl_res_get(curl);
    if(res == CURLE_OK){
        printf("Get Message: %s \n", resp_msg.c_str());
    } else {
        printf("Get Message Fail: %d \n", res);
    }

    // clear libcurl pointer
    // curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    return res;
}

int main() {

    std::string readBuffer;

    // JSON 생성
    Json::Value resroot;
    resroot["name"] = "abc";
    resroot["id"] = "10";
    resroot["value"] = "asdasd";

    Json::StreamWriterBuilder builder;
    std::string jsonBody = Json::writeString(builder, resroot);

    qserver_request("https://127.0.0.1", jsonBody, readBuffer);
    std::cout << "Read buffer: " << readBuffer << std::endl;
    return 0;
}