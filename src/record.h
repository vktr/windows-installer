#ifndef WI_RECORD_H
#define WI_RECORD_H

#include <node_api.h>

#include <Windows.h>
#include <Msi.h>
#include <MsiQuery.h>

namespace wi
{
    class Record
    {
    public:
        static napi_status Init(napi_env env, napi_value exports);
        static napi_value New(napi_env env, napi_callback_info callback_info);

        static napi_ref constructor;

    private:
        static void Destructor(napi_env env, void* native_obj, void* finalize_hint);

        // Class functions
        static napi_value ClearData(napi_env env, napi_callback_info callback_info);
        static napi_value DataSize(napi_env env, napi_callback_info callback_info);
        static napi_value GetFieldCount(napi_env env, napi_callback_info callback_info);
        static napi_value GetInteger(napi_env env, napi_callback_info callback_info);
        static napi_value GetString(napi_env env, napi_callback_info callback_info);
        static napi_value IsNull(napi_env env, napi_callback_info callback_info);
        static napi_value ReadStream(napi_env env, napi_callback_info callback_info);
        static napi_value SetInteger(napi_env env, napi_callback_info callback_info);
        static napi_value SetStream(napi_env env, napi_callback_info callback_info);
        static napi_value SetString(napi_env env, napi_callback_info callback_info);

        Record(napi_env env, MSIHANDLE handle);
        ~Record();

        napi_env env_;
        napi_ref wrapper_;

        MSIHANDLE handle_;
    };
}

#endif
