#include "preview.h"

#include "common.h"

using wi::Preview;

napi_ref Preview::constructor;

Preview::Preview(napi_env env, MSIHANDLE handle)
    : env_(env),
    wrapper_(nullptr),
    handle_(handle)
{
}

Preview::~Preview()
{
    MsiCloseHandle(handle_);
}

void Preview::Destructor(napi_env env, void* native_obj, void* finalize_hint)
{
    delete static_cast<Preview*>(native_obj);
}

napi_status Preview::Init(napi_env env, napi_value exports)
{
    napi_property_descriptor properties[] =
    {
        { "previewBillboard", nullptr, PreviewBillboard, nullptr, nullptr, 0, napi_default, 0 },
        { "previewDialog", nullptr, PreviewDialog, nullptr, nullptr, 0, napi_default, 0 },
    };

    napi_value cons;
    napi_define_class(env, "Preview", NAPI_AUTO_LENGTH, New, nullptr, ARRAYSIZE(properties), properties, &cons);
    napi_create_reference(env, cons, 1, &constructor);

    return napi_ok;
}

napi_value Preview::New(napi_env env, napi_callback_info callback_info)
{
    size_t argc = 1;
    napi_value args[1];
    napi_value _this;
    napi_get_cb_info(env, callback_info, &argc, args, &_this, nullptr);

    MSIHANDLE* ph;
    napi_get_value_external(env, args[0], reinterpret_cast<void**>(&ph));

    Preview* preview = new Preview(env, *ph);
    napi_wrap(env, _this, preview, Preview::Destructor, nullptr, &preview->wrapper_);

    return _this;
}

napi_value Preview::PreviewBillboard(napi_env env, napi_callback_info callback_info)
{
    size_t argc = 2;
    napi_value args[2];
    napi_value _this;
    napi_get_cb_info(env, callback_info, &argc, args, &_this, nullptr);

    Preview* pre;
    napi_unwrap(env, _this, reinterpret_cast<void**>(&pre));

    std::string control_name;
    wi_napi_to_std_string(env, args[0], &control_name);

    std::string billboard;
    wi_napi_to_std_string(env, args[1], &billboard);

    UINT res = MsiPreviewBillboard(
        pre->handle_,
        control_name.c_str(),
        billboard.c_str());

    switch (res)
    {
    case ERROR_FUNCTION_FAILED:
        napi_throw_type_error(env, nullptr, "The function failed.");
        break;

    case ERROR_INVALID_HANDLE:
        napi_throw_type_error(env, nullptr, "An invalid or inactive handle was supplied.");
        break;

    case ERROR_INVALID_PARAMETER:
        napi_throw_type_error(env, nullptr, "An invalid parameter was supplied.");
        break;
    }

    return nullptr;
}

napi_value Preview::PreviewDialog(napi_env env, napi_callback_info callback_info)
{
    size_t argc = 1;
    napi_value args[1];
    napi_value _this;
    napi_get_cb_info(env, callback_info, &argc, args, &_this, nullptr);

    Preview* pre;
    napi_unwrap(env, _this, reinterpret_cast<void**>(&pre));

    std::string dialog_name;
    wi_napi_to_std_string(env, args[0], &dialog_name);

    UINT res = MsiPreviewDialog(
        pre->handle_,
        dialog_name.c_str());

    switch (res)
    {
    case ERROR_FUNCTION_FAILED:
        napi_throw_type_error(env, nullptr, "The function failed.");
        break;

    case ERROR_FUNCTION_NOT_CALLED:
        napi_throw_type_error(env, nullptr, "The function was not called.");
        break;

    case ERROR_INSTALL_FAILURE:
        napi_throw_type_error(env, nullptr, "An installation failure occurred.");
        break;

    case ERROR_INSTALL_SUSPEND:
        napi_throw_type_error(env, nullptr, "The installation was suspended.");
        break;

    case ERROR_INSTALL_USEREXIT:
        napi_throw_type_error(env, nullptr, "The user exited the installation.");
        break;

    case ERROR_INVALID_HANDLE:
        napi_throw_type_error(env, nullptr, "An invalid or inactive handle was supplied.");
        break;

    case ERROR_INVALID_HANDLE_STATE:
        napi_throw_type_error(env, nullptr, "An invalid handle state was supplied.");
        break;

    case ERROR_INVALID_PARAMETER:
        napi_throw_type_error(env, nullptr, "An invalid parameter was supplied.");
        break;
    }

    return nullptr;
}

