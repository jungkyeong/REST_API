#include "main.h"
#include "Util.h"
#include "Define.h"
#include "../lib/json/json.h"
#include <openssl/evp.h>
#include <openssl/err.h>
#include <string>
#include <cstring>

Util util;

int main() {

    

    // json file make
    Json::Value resroot;
    resroot["key"] = "value";
    resroot["dsa"] = "asd";
    Json::StreamWriterBuilder abuilder;
    resp_msg = Json::writeString(abuilder, resroot);
    std::cout << resp_msg << std::endl;


    return 0;
}