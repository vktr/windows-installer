#ifndef WI_DATABASE_H
#define WI_DATABASE_H

#include <node_api.h>

#include <Windows.h>
#include <Msi.h>
#include <MsiQuery.h>

namespace wi
{
    class Database
    {
    public:
        static napi_status Init(napi_env env, napi_value exports);
        static napi_value New(napi_env env, napi_callback_info callback_info);

        static napi_ref constructor;

    private:
        static void Destructor(napi_env env, void* native_obj, void* finalize_hint);

        // Class functions
        static napi_value ApplyTransform(napi_env env, napi_callback_info callback_info);
        static napi_value Commit(napi_env env, napi_callback_info callback_info);
        static napi_value Export(napi_env env, napi_callback_info callback_info);
        static napi_value GenerateTransform(napi_env env, napi_callback_info callback_info);
        static napi_value GetPrimaryKeys(napi_env env, napi_callback_info callback_info);
        static napi_value GetState(napi_env env, napi_callback_info callback_info);
        static napi_value GetSummaryInformation(napi_env env, napi_callback_info callback_info);
        static napi_value Import(napi_env env, napi_callback_info callback_info);
        static napi_value IsTablePersistent(napi_env env, napi_callback_info callback_info);
        static napi_value Merge(napi_env env, napi_callback_info callback_info);
        static napi_value OpenView(napi_env env, napi_callback_info callback_info);

        Database(napi_env env, MSIHANDLE handle);
        ~Database();

        napi_env env_;
        napi_ref wrapper_;

        MSIHANDLE handle_;
    };
}

#endif
