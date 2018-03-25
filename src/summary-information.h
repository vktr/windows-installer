#ifndef WI_SUMMARYINFORMATION_H
#define WI_SUMMARYINFORMATION_H

#include <node_api.h>

#include <Windows.h>
#include <Msi.h>
#include <MsiQuery.h>

namespace wi
{
    class SummaryInformation
    {
    public:
        static napi_status Init(napi_env env, napi_value exports);
        static napi_value New(napi_env env, napi_callback_info callback_info);

        static napi_ref constructor;

    private:
        static void Destructor(napi_env env, void* native_obj, void* finalize_hint);

        // Class functions
        static napi_value GetProperty(napi_env env, napi_callback_info callback_info);
        static napi_value GetPropertyCount(napi_env env, napi_callback_info callback_info);
        static napi_value Persist(napi_env env, napi_callback_info callback_info);
        static napi_value SetProperty(napi_env env, napi_callback_info callback_info);


        SummaryInformation(napi_env env, MSIHANDLE handle);
        ~SummaryInformation();

        napi_env env_;
        napi_ref wrapper_;

        MSIHANDLE handle_;
    };
}

#endif
