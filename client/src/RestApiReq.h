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

#ifndef __RESTAPIREQ_HPP
#define __RESTAPIREQ_HPP

#include <iostream>
#include "../../lib/curl/curl.h"
#include "../../lib/json/json.h"

// Debug print
#ifdef DEBUG
#define DBG_PRINT(fmt, ...) printf("[DEBUG] " fmt, ##__VA_ARGS__)
#else
#define DBG_PRINT(fmt, ...)
#endif

class RestApiReq {
private:

    /**
     * @brief libcurl Status and data save
     * @param curl curl struct
     * @return libcurl status value
     */
    CURLcode libcurl_res_get(CURL* curl);

    /**
     * @brief Check Protocol http://, https://
     * @param ip URL
     * @return http 1, https 2, fail -1
     */
    int check_protocol(std::string ip);

public:

    /**
     * @brief Post request
     * @param ip URL
     * @param json_data input message
     * @param resp_msg output message
     * @return Success 0 Fail else
     */
    int post_request(std::string ip, std::string json_data, std::string& resp_msg);

    /**
     * @brief Get request
     * @param ip URL
     * @param json_data input message
     * @param resp_msg output message
     * @return Success 0 Fail else
     */
    int get_request(std::string ip, std::string json_data, std::string& resp_msg);

    /**
     * @brief Patch request
     * @param ip URL
     * @param json_data input message
     * @param resp_msg output message
     * @return Success 0 Fail else
     */
    int patch_request(std::string ip, std::string json_data, std::string& resp_msg);

    /**
     * @brief Put request
     * @param ip URL
     * @param json_data input message
     * @param resp_msg output message
     * @return Success 0 Fail else
     */
    int put_request(std::string ip, std::string json_data, std::string& resp_msg);

    /**
     * @brief Delete request
     * @param ip URL
     * @param json_data input message
     * @param resp_msg output message
     * @return Success 0 Fail else
     */
    int delete_request(std::string ip, std::string json_data, std::string& resp_msg);


};

#endif /* __RESTAPIREQ_HPP */