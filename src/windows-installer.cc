#include <node_api.h>

#include "database.h"
#include "record.h"
#include "view.h"

napi_value Init(napi_env env, napi_value exports) {
    wi::Database::Init(env, exports);
    wi::Record::Init(env, exports);
    wi::View::Init(env, exports);

    /* Constants */
    napi_value wi_MSIDBOPEN_READONLY;
    napi_create_int32(env, 0, &wi_MSIDBOPEN_READONLY);

    napi_property_descriptor methods[] =
    {
        { "MSIDBOPEN_READONLY", nullptr, nullptr, nullptr, nullptr, wi_MSIDBOPEN_READONLY, napi_default, nullptr }
    };

    napi_define_properties(env, exports, ARRAYSIZE(methods), methods);

    return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)
