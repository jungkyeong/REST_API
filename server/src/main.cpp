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

    // GET /hello → 단순 문자열 응답
    CROW_ROUTE(app, "/hello")([](){
        return "Hello, Crow Server!";
    });

    // GET /add/<a>/<b> → JSON 응답 {"result": a+b}
    CROW_ROUTE(app, "/add/<int>/<int>")([](int a, int b){
        Json::Value result;
        result["result"] = a + b;

        Json::StreamWriterBuilder writer;
        return Json::writeString(writer, result);
    });

    // POST /echo { "msg": "..." } → JSON echo
    CROW_ROUTE(app, "/echo").methods(crow::HTTPMethod::Post)([](const crow::request& req){
        Json::CharReaderBuilder reader;
        Json::Value root;
        std::string errs;

        std::istringstream s(req.body);
        if (!Json::parseFromStream(reader, s, &root, &errs)) {
            return crow::response(400, "Invalid JSON");
        }

        Json::Value reply;
        reply["echo"] = root["msg"];

        Json::StreamWriterBuilder writer;
        return crow::response(Json::writeString(writer, reply));
    });

    // server run
    app.port(1234).multithreaded().run();
}