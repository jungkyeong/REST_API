#include "../../lib/json/json.h"
#include "../../lib/curl/curl.h"
#include "RestApiReq.h"
#include <openssl/evp.h>
#include <openssl/err.h>
#include <iostream>
#include <string>
#include <sstream>

RestApiReq restapireq;

int main() {

    std::string response;

    // make json
    Json::Value root;
    root["key"] = "value";
    root["id"] = "asdasd";
    Json::StreamWriterBuilder writer;
    std::string jsonBody = Json::writeString(writer, root);

    // request
    restapireq.get_request("https://127.0.0.1:1234/get", jsonBody, response);
    std::cout << "Server Response: " << response << std::endl;

    restapireq.post_request("https://127.0.0.1:1234/post", jsonBody, response);
    std::cout << "Server Response: " << response << std::endl;

    return 0;
}