/**
  ******************************************************************************
  * @file           : RestApiRequest
  * @brief          : Class RestApiReq define use RestApiRequest function 
  ******************************************************************************
  * Made in GreenOasis  
  * https://github.com/jungkyeong
  * CopyRigth MIT License
  ******************************************************************************
  * Release History
  * branch name, working description, time
  * version_001: define add version  2025-08-25
  ******************************************************************************
  */

#include "Util.h"
#include "RestApiReq.h"
#include "Define.h"

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
CURLcode RestApiReq::libcurl_res_get(CURL* curl){

    // get response code and save data in curl pointer
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        DBG_PRINT("libcurl error: %s \n", curl_easy_strerror(res));
    } else {
        long response_code;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        DBG_PRINT("HTTP Response Code: %ld \n", response_code);
    }
    return res;
}

/**
 * @brief Check Protocol http://, https://
 * @param ip URL
 * @return http 1, https 2, fail -1
 */
int RestApiReq::check_protocol(std::string ip){

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
 * @brief Post request
 * @param ip URL
 * @param json_data input message
 * @param resp_msg output message
 * @return Success 0 Fail else
 */
int RestApiReq::post_request(std::string ip, std::string json_data, std::string& resp_msg){

    resp_msg.clear();

    int protocol_status = check_protocol(ip);

    // Libcurl init
    CURL* curl = curl_easy_init();
    if(!curl) {
        DBG_PRINT(CURL_MSG_INIT_FAIL"\n");
        return FAIL;
    }

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, ip.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp_msg);

    // if https load SSL
    if(protocol_status == HTTPS_STATUS){
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // Self Sign: 0L, CA validate 1L
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L); // Self Sign: 0L, Hostname validate 2L
    }

    // Get Data
    CURLcode res = libcurl_res_get(curl);
    if(res == CURLE_OK){
        DBG_PRINT("Request POST Success: %s \n", resp_msg.c_str());
    } else {
        DBG_PRINT("Request POST Fail: %d \n", res);
    }

    // clear libcurl pointer
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    return res;
}

/**
 * @brief Get request
 * @param ip URL
 * @param json_data input message
 * @param resp_msg output message
 * @return Success 0 Fail else
 */
int RestApiReq::get_request(std::string ip, std::string json_data, std::string& resp_msg){

    resp_msg.clear();

    int protocol_status = check_protocol(ip);
    
    // Libcurl init
    CURL* curl = curl_easy_init();
    if(!curl) {
        DBG_PRINT(CURL_MSG_INIT_FAIL"\n");
        return FAIL;
    }

    // Json Parsing check
    Json::Value json_obj;
    if(!json_data.empty()){
        Json::CharReaderBuilder reader;
        std::istringstream s(json_data);
        std::string errs;
        if(!Json::parseFromStream(reader, s, &json_obj, &errs)){
            DBG_PRINT("Json Error %s \n", errs.c_str());
            curl_easy_cleanup(curl);
            return FAIL;
        }
    }

    // URL make
    std::string full_url = ip;
    bool first_param = true;

    for(const auto& key : json_obj.getMemberNames()){
        std::string value = json_obj[key].asString();
        std::string encoded = curl_easy_escape(curl, value.c_str(), value.length());
        if(first_param){
            full_url += "?" + key + "=" + encoded;
            first_param = false;
        } else {
            full_url += "&" + key + "=" + encoded;
        }
    }

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, full_url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);  // GET setting
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp_msg);

    // if https load SSL
    if(protocol_status == HTTPS_STATUS){
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    }

    // Get Data
    CURLcode res = libcurl_res_get(curl);
    if(res == CURLE_OK){
        DBG_PRINT("Request GET Success: %s \n", resp_msg.c_str());
    } else {
        DBG_PRINT("Request GET Fail: %d \n", res);
    }

    // clear libcurl pointer
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    return res;
}

/**
 * @brief PUT request
 * @param ip URL
 * @param json_data input message
 * @param resp_msg output message
 * @return Success 0 Fail else
 */
int RestApiReq::put_request(std::string ip, std::string json_data, std::string& resp_msg){

    resp_msg.clear();

    int protocol_status = check_protocol(ip);
    
    // Libcurl init
    CURL* curl = curl_easy_init();
    if(!curl) {
        DBG_PRINT(CURL_MSG_INIT_FAIL"\n");
        return FAIL;
    }

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, ip.c_str());
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp_msg);

    if(protocol_status == HTTPS_STATUS){
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    }

   // Get Data
    CURLcode res = libcurl_res_get(curl);
    if(res == CURLE_OK){
        DBG_PRINT("Request PUT Success: %s \n", resp_msg.c_str());
    } else {
        DBG_PRINT("Request PUT Fail: %d \n", res);
    }

    // clear libcurl pointer
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    return res;
}

/**
 * @brief PATCH request
 * @param ip URL
 * @param json_data input message
 * @param resp_msg output message
 * @return Success 0 Fail else
 */
int RestApiReq::patch_request(std::string ip, std::string json_data, std::string& resp_msg){

    resp_msg.clear();

    int protocol_status = check_protocol(ip);
    
    // Libcurl init
    CURL* curl = curl_easy_init();
    if(!curl) {
        DBG_PRINT(CURL_MSG_INIT_FAIL"\n");
        return FAIL;
    }

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, ip.c_str());
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp_msg);

    if(protocol_status == HTTPS_STATUS){
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    }

   // Get Data
    CURLcode res = libcurl_res_get(curl);
    if(res == CURLE_OK){
        DBG_PRINT("Request PATCH Success: %s \n", resp_msg.c_str());
    } else {
        DBG_PRINT("Request PATCH Fail: %d \n", res);
    }

    // clear libcurl pointer
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    return res;
}

/**
 * @brief DELETE request
 * @param ip URL
 * @param json_data input message
 * @param resp_msg output message
 * @return Success 0 Fail else
 */
int RestApiReq::delete_request(std::string ip, std::string json_data, std::string& resp_msg){

    resp_msg.clear();

    int protocol_status = check_protocol(ip);
    
    // Libcurl init
    CURL* curl = curl_easy_init();
    if(!curl) {
        DBG_PRINT(CURL_MSG_INIT_FAIL"\n");
        return FAIL;
    }

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, ip.c_str());
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp_msg);

    if(!json_data.empty()){
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data.c_str());
    }

    if(protocol_status == HTTPS_STATUS){
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    }

   // Get Data
    CURLcode res = libcurl_res_get(curl);
    if(res == CURLE_OK){
        DBG_PRINT("Request DELETE Success: %s \n", resp_msg.c_str());
    } else {
        DBG_PRINT("Request DELETE Fail: %d \n", res);
    }

    // clear libcurl pointer
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    return res;
}
