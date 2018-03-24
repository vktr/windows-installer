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

    napi_value wi_MSITRANSFORM_ERROR_ADDEXISTINGROW;
    napi_create_int32(env, MSITRANSFORM_ERROR_ADDEXISTINGROW, &wi_MSITRANSFORM_ERROR_ADDEXISTINGROW);

    napi_value wi_MSITRANSFORM_ERROR_DELMISSINGROW;
    napi_create_int32(env, MSITRANSFORM_ERROR_DELMISSINGROW, &wi_MSITRANSFORM_ERROR_DELMISSINGROW);

    napi_value wi_MSITRANSFORM_ERROR_ADDEXISTINGTABLE;
    napi_create_int32(env, MSITRANSFORM_ERROR_ADDEXISTINGTABLE, &wi_MSITRANSFORM_ERROR_ADDEXISTINGTABLE);

    napi_value wi_MSITRANSFORM_ERROR_DELMISSINGTABLE;
    napi_create_int32(env, MSITRANSFORM_ERROR_DELMISSINGTABLE, &wi_MSITRANSFORM_ERROR_DELMISSINGTABLE);

    napi_value wi_MSITRANSFORM_ERROR_UPDATEMISSINGROW;
    napi_create_int32(env, MSITRANSFORM_ERROR_UPDATEMISSINGROW, &wi_MSITRANSFORM_ERROR_UPDATEMISSINGROW);

    napi_value wi_MSITRANSFORM_ERROR_CHANGECODEPAGE;
    napi_create_int32(env, MSITRANSFORM_ERROR_CHANGECODEPAGE, &wi_MSITRANSFORM_ERROR_CHANGECODEPAGE);

    napi_value wi_MSITRANSFORM_ERROR_VIEWTRANSFORM;
    napi_create_int32(env, MSITRANSFORM_ERROR_VIEWTRANSFORM, &wi_MSITRANSFORM_ERROR_VIEWTRANSFORM);

    napi_value wi_MSIDBSTATE_READ;
    napi_create_int32(env, MSIDBSTATE_READ, &wi_MSIDBSTATE_READ);

    napi_value wi_MSIDBSTATE_WRITE;
    napi_create_int32(env, MSIDBSTATE_WRITE, &wi_MSIDBSTATE_WRITE);

    napi_property_descriptor methods[] =
    {
        { "MSIDBOPEN_READONLY", nullptr, nullptr, nullptr, nullptr, wi_MSIDBOPEN_READONLY, napi_default, nullptr },

        { "MSITRANSFORM_ERROR_ADDEXISTINGROW", nullptr, nullptr, nullptr, nullptr, wi_MSITRANSFORM_ERROR_ADDEXISTINGROW, napi_default, nullptr },
        { "MSITRANSFORM_ERROR_DELMISSINGROW", nullptr, nullptr, nullptr, nullptr, wi_MSITRANSFORM_ERROR_DELMISSINGROW, napi_default, nullptr },
        { "MSITRANSFORM_ERROR_ADDEXISTINGTABLE", nullptr, nullptr, nullptr, nullptr, wi_MSITRANSFORM_ERROR_ADDEXISTINGTABLE, napi_default, nullptr },
        { "MSITRANSFORM_ERROR_DELMISSINGTABLE", nullptr, nullptr, nullptr, nullptr, wi_MSITRANSFORM_ERROR_DELMISSINGTABLE, napi_default, nullptr },
        { "MSITRANSFORM_ERROR_UPDATEMISSINGROW", nullptr, nullptr, nullptr, nullptr, wi_MSITRANSFORM_ERROR_UPDATEMISSINGROW, napi_default, nullptr },
        { "MSITRANSFORM_ERROR_CHANGECODEPAGE", nullptr, nullptr, nullptr, nullptr, wi_MSITRANSFORM_ERROR_CHANGECODEPAGE, napi_default, nullptr },
        { "MSITRANSFORM_ERROR_VIEWTRANSFORM", nullptr, nullptr, nullptr, nullptr, wi_MSITRANSFORM_ERROR_VIEWTRANSFORM, napi_default, nullptr },

        { "MSIDBSTATE_READ", nullptr, nullptr, nullptr, nullptr, wi_MSIDBSTATE_READ, napi_default, nullptr },
        { "MSIDBSTATE_WRITE", nullptr, nullptr, nullptr, nullptr, wi_MSIDBSTATE_WRITE, napi_default, nullptr },
    };

    napi_define_properties(env, exports, ARRAYSIZE(methods), methods);

    return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)
