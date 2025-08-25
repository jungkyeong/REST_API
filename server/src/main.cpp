#include "main.h"
#include "Util.h"
#include "Define.h"
#include "../../lib/json/json.h"
#include "../../lib/crow/crow.h"
#include <openssl/evp.h>
#include <openssl/err.h>
#include <string>
#include <cstring>

Util util;

int main() {
  crow::SimpleApp app;

    // [GET] Test Get
    // des: test get 
    CROW_ROUTE(app, "/get")([](const crow::request& req){
        std::string key = req.url_params.get("key") ? req.url_params.get("key") : "";
        std::string id = req.url_params.get("id") ? req.url_params.get("id") : "";

        Json::Value res_json;
        res_json["response_msg"] = "GET received with key: " + key;
        res_json["key"] = key;
        res_json["id"] = id;

        // Write Response Message
        Json::StreamWriterBuilder writer;
        std::string output = Json::writeString(writer, res_json);
        return crow::response(output);
    });

    // [POST] Test Post
    // des: test post
    CROW_ROUTE(app, "/post").methods(crow::HTTPMethod::Post)([](const crow::request& req){
        std::string key = req.url_params.get("key") ? req.url_params.get("key") : "";
        std::string id = req.url_params.get("id") ? req.url_params.get("id") : "";

        Json::Value req_json;
        Json::CharReaderBuilder reader;
        std::string errs;
        std::istringstream s(req.body);
        if (!Json::parseFromStream(reader, s, &req_json, &errs)) {
            return crow::response(400, "Invalid JSON");
        }

        Json::Value res_json;
        res_json["response_msg"] = "POST received";
        res_json["key"] = key; // url_param
        res_json["id"] = id; // url_param
        res_json["json_data"] = req_json; // json_data

        // Write Response Message
        Json::StreamWriterBuilder writer;
        std::string output = Json::writeString(writer, res_json);
        return crow::response(output);
    });

    // server run
#ifdef CROW_ENABLE_SSL
    app.ssl_file("cert/server.crt", "cert/server.key");
#endif
    app.port(1234).multithreaded().run();

}