#ifndef WI_VIEW_H
#define WI_VIEW_H

#include <node_api.h>

#include <Windows.h>
#include <Msi.h>
#include <MsiQuery.h>

namespace wi
{
    class View
    {
    public:
        static napi_status Init(napi_env env, napi_value exports);
        static napi_value New(napi_env env, napi_callback_info callback_info);

        static napi_ref constructor;

    private:
        static void Destructor(napi_env env, void* native_obj, void* finalize_hint);

        // Class functions
        static napi_value Close(napi_env env, napi_callback_info callback_info);
        static napi_value Execute(napi_env env, napi_callback_info callback_info);
        static napi_value Fetch(napi_env env, napi_callback_info callback_info);
        static napi_value GetColumnInfo(napi_env env, napi_callback_info callback_info);
        static napi_value GetError(napi_env env, napi_callback_info callback_info);
        static napi_value Modify(napi_env env, napi_callback_info callback_info);

        View(napi_env env, MSIHANDLE handle);
        ~View();

        napi_env env_;
        napi_ref wrapper_;

        MSIHANDLE handle_;
    };
}

#endif
