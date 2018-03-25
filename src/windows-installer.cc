#include <node_api.h>

#include "database.h"
#include "preview.h"
#include "record.h"
#include "summary-information.h"
#include "view.h"

napi_value Init(napi_env env, napi_value exports) {
    wi::Database::Init(env, exports);
    wi::Preview::Init(env, exports);
    wi::Record::Init(env, exports);
    wi::SummaryInformation::Init(env, exports);
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

    napi_value wi_PID_CODEPAGE;
    napi_create_int32(env, PID_CODEPAGE, &wi_PID_CODEPAGE);

    napi_value wi_PID_TITLE;
    napi_create_int32(env, PIDSI_TITLE, &wi_PID_TITLE);

    napi_value wi_PID_SUBJECT;
    napi_create_int32(env, PIDSI_SUBJECT, &wi_PID_SUBJECT);

    napi_value wi_PID_AUTHOR;
    napi_create_int32(env, PIDSI_AUTHOR, &wi_PID_AUTHOR);

    napi_value wi_PID_KEYWORDS;
    napi_create_int32(env, PIDSI_KEYWORDS, &wi_PID_KEYWORDS);

    napi_value wi_PID_COMMENTS;
    napi_create_int32(env, PIDSI_COMMENTS, &wi_PID_COMMENTS);

    napi_value wi_PID_TEMPLATE;
    napi_create_int32(env, PIDSI_TEMPLATE, &wi_PID_TEMPLATE);

    napi_value wi_PID_LASTAUTHOR;
    napi_create_int32(env, PIDSI_LASTAUTHOR, &wi_PID_LASTAUTHOR);

    napi_value wi_PID_REVNUMBER;
    napi_create_int32(env, PIDSI_REVNUMBER, &wi_PID_REVNUMBER);

    napi_value wi_PID_LASTPRINTED;
    napi_create_int32(env, PIDSI_LASTPRINTED, &wi_PID_LASTPRINTED);

    napi_value wi_PID_CREATE_DTM;
    napi_create_int32(env, PIDSI_CREATE_DTM, &wi_PID_CREATE_DTM);

    napi_value wi_PID_LASTSAVE_DTM;
    napi_create_int32(env, PIDSI_LASTSAVE_DTM, &wi_PID_LASTSAVE_DTM);

    napi_value wi_PID_PAGECOUNT;
    napi_create_int32(env, PIDSI_PAGECOUNT, &wi_PID_PAGECOUNT);

    napi_value wi_PID_WORDCOUNT;
    napi_create_int32(env, PIDSI_WORDCOUNT, &wi_PID_WORDCOUNT);

    napi_value wi_PID_CHARCOUNT;
    napi_create_int32(env, PIDSI_CHARCOUNT, &wi_PID_CHARCOUNT);

    napi_value wi_PID_APPNAME;
    napi_create_int32(env, PIDSI_APPNAME, &wi_PID_APPNAME);

    napi_value wi_PID_SECURITY;
    napi_create_int32(env, PIDSI_DOC_SECURITY, &wi_PID_SECURITY);

    napi_value wi_VT_I2;
    napi_create_int32(env, VT_I2, &wi_VT_I2);

    napi_value wi_VT_LPSTR;
    napi_create_int32(env, VT_LPSTR, &wi_VT_LPSTR);

    napi_value wi_VT_FILETIME;
    napi_create_int32(env, VT_FILETIME, &wi_VT_FILETIME);

    napi_value wi_VT_I4;
    napi_create_int32(env, VT_I4, &wi_VT_I4);

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

        { "PID_CODEPAGE", nullptr, nullptr, nullptr, nullptr, wi_PID_CODEPAGE, napi_default, nullptr },
        { "PID_TITLE", nullptr, nullptr, nullptr, nullptr, wi_PID_TITLE, napi_default, nullptr },
        { "PID_SUBJECT", nullptr, nullptr, nullptr, nullptr, wi_PID_SUBJECT, napi_default, nullptr },
        { "PID_AUTHOR", nullptr, nullptr, nullptr, nullptr, wi_PID_AUTHOR, napi_default, nullptr },
        { "PID_KEYWORDS", nullptr, nullptr, nullptr, nullptr, wi_PID_KEYWORDS, napi_default, nullptr },
        { "PID_COMMENTS", nullptr, nullptr, nullptr, nullptr, wi_PID_COMMENTS, napi_default, nullptr },
        { "PID_TEMPLATE", nullptr, nullptr, nullptr, nullptr, wi_PID_TEMPLATE, napi_default, nullptr },
        { "PID_LASTAUTHOR", nullptr, nullptr, nullptr, nullptr, wi_PID_LASTAUTHOR, napi_default, nullptr },
        { "PID_REVNUMBER", nullptr, nullptr, nullptr, nullptr, wi_PID_REVNUMBER, napi_default, nullptr },
        { "PID_LASTPRINTED", nullptr, nullptr, nullptr, nullptr, wi_PID_LASTPRINTED, napi_default, nullptr },
        { "PID_CREATE_DTM", nullptr, nullptr, nullptr, nullptr, wi_PID_CREATE_DTM, napi_default, nullptr },
        { "PID_LASTSAVE_DTM", nullptr, nullptr, nullptr, nullptr, wi_PID_LASTSAVE_DTM, napi_default, nullptr },
        { "PID_PAGECOUNT", nullptr, nullptr, nullptr, nullptr, wi_PID_PAGECOUNT, napi_default, nullptr },
        { "PID_WORDCOUNT", nullptr, nullptr, nullptr, nullptr, wi_PID_WORDCOUNT, napi_default, nullptr },
        { "PID_CHARCOUNT", nullptr, nullptr, nullptr, nullptr, wi_PID_CHARCOUNT, napi_default, nullptr },
        { "PID_APPNAME", nullptr, nullptr, nullptr, nullptr, wi_PID_APPNAME, napi_default, nullptr },
        { "PID_SECURITY", nullptr, nullptr, nullptr, nullptr, wi_PID_SECURITY, napi_default, nullptr },

        { "VT_I2", nullptr, nullptr, nullptr, nullptr, wi_VT_I2, napi_default, nullptr },
        { "VT_I4", nullptr, nullptr, nullptr, nullptr, wi_VT_I4, napi_default, nullptr },
        { "VT_LPSTR", nullptr, nullptr, nullptr, nullptr, wi_VT_LPSTR, napi_default, nullptr },
        { "VT_FILETIME", nullptr, nullptr, nullptr, nullptr, wi_VT_FILETIME, napi_default, nullptr },
    };

    napi_define_properties(env, exports, ARRAYSIZE(methods), methods);

    return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)
