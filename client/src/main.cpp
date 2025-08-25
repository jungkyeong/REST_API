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
    restapireq.patch_request("https://127.0.0.1:1234/patch", jsonBody, response);
    std::cout << "Serve Response: " << response << std::endl;

    restapireq.put_request("https://127.0.0.1:1234/put", jsonBody, response);
    std::cout << "Server Response: " << response << std::endl;

    restapireq.delete_request("https://127.0.0.1:1234/delete", jsonBody, response);
    std::cout << "Server Response: " << response << std::endl;

    return 0;
}