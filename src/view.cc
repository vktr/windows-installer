#include "view.h"

#include "common.h"
#include "record.h"

using wi::View;

napi_ref View::constructor;

View::View(napi_env env, MSIHANDLE handle)
    : env_(env),
    wrapper_(nullptr),
    handle_(handle)
{
}

View::~View()
{
    MsiCloseHandle(handle_);
}

void View::Destructor(napi_env env, void* native_obj, void* finalize_hint)
{
    delete static_cast<View*>(native_obj);
}

napi_status View::Init(napi_env env, napi_value exports)
{
    napi_property_descriptor properties[] =
    {
        { "close", nullptr, Close, nullptr, nullptr, 0, napi_default, 0 },
        { "execute", nullptr, Execute, nullptr, nullptr, 0, napi_default, 0 },
        { "fetch", nullptr, Fetch, nullptr, nullptr, 0, napi_default, 0 },
        { "getColumnInfo", nullptr, GetColumnInfo, nullptr, nullptr, 0, napi_default, 0 },
        { "getError", nullptr, GetError, nullptr, nullptr, 0, napi_default, 0 },
        { "modify", nullptr, Modify, nullptr, nullptr, 0, napi_default, 0 },
    };

    napi_value cons;
    napi_define_class(env, "View", NAPI_AUTO_LENGTH, New, nullptr, ARRAYSIZE(properties), properties, &cons);
    napi_create_reference(env, cons, 1, &constructor);

    return napi_ok;
}

napi_value View::New(napi_env env, napi_callback_info callback_info)
{
    size_t argc = 1;
    napi_value args[1];
    napi_value _this;
    napi_get_cb_info(env, callback_info, &argc, args, &_this, nullptr);

    MSIHANDLE* ph;
    napi_get_value_external(env, args[0], reinterpret_cast<void**>(&ph));

    View* view = new View(env, *ph);
    napi_wrap(env, _this, view, View::Destructor, nullptr, &view->wrapper_);

    return _this;
}

napi_value View::Close(napi_env env, napi_callback_info callback_info)
{
    napi_value _this;
    napi_get_cb_info(env, callback_info, nullptr, 0, &_this, nullptr);

    View* v;
    napi_unwrap(env, _this, reinterpret_cast<void**>(&v));

    UINT res = MsiViewClose(v->handle_);

    switch (res)
    {
    case ERROR_FUNCTION_FAILED:
        napi_throw_type_error(env, nullptr, "The function failed.");
        break;
    case ERROR_INVALID_HANDLE:
        napi_throw_type_error(env, nullptr, "An invalid or inactive handle was supplied.");
        break;
    case ERROR_INVALID_HANDLE_STATE:
        napi_throw_type_error(env, nullptr, "The handle is in an invalid state.");
        break;
    }

    return nullptr;
}

napi_value View::Execute(napi_env env, napi_callback_info callback_info)
{
    size_t argc = 1;
    napi_value args[1];
    napi_value _this;
    napi_get_cb_info(env, callback_info, &argc, args, &_this, nullptr);

    // TODO(record)
    MSIHANDLE hRecord = NULL;

    if (argc > 0)
    {
        // DO STUFF
    }

    View* v;
    napi_unwrap(env, _this, reinterpret_cast<void**>(&v));

    UINT res = MsiViewExecute(v->handle_, 0);

    switch (res)
    {
    case ERROR_FUNCTION_FAILED:
        napi_throw_type_error(env, nullptr, "A view could not be executed.");
        break;
    case ERROR_INVALID_HANDLE:
        napi_throw_type_error(env, nullptr, "An invalid or inactive handle was supplied.");
        break;
    }

    return nullptr;
}

napi_value View::Fetch(napi_env env, napi_callback_info callback_info)
{
    napi_value _this;
    napi_get_cb_info(env, callback_info, nullptr, 0, &_this, nullptr);

    View* v;
    napi_unwrap(env, _this, reinterpret_cast<void**>(&v));

    MSIHANDLE ph;
    UINT res = MsiViewFetch(v->handle_, &ph);

    switch (res)
    {
    case ERROR_FUNCTION_FAILED:
        napi_throw_type_error(env, nullptr, "An error occurred during fetching.");
        return nullptr;

    case ERROR_INVALID_HANDLE:
        napi_throw_type_error(env, nullptr, "An invalid or inactive handle was supplied.");
        return nullptr;

    case ERROR_INVALID_HANDLE_STATE:
        napi_throw_type_error(env, nullptr, "The handle was in an invalid state.");
        return nullptr;

    case ERROR_NO_MORE_ITEMS:
        return nullptr;
    }

    napi_value external;
    napi_create_external(env, &ph, nullptr, nullptr, &external);

    napi_value cons;
    napi_get_reference_value(env, Record::constructor, &cons);

    napi_value argv[] = { external };
    napi_value instance;
    napi_new_instance(env, cons, ARRAYSIZE(argv), argv, &instance);

    return instance;
}

napi_value View::GetColumnInfo(napi_env env, napi_callback_info callback_info)
{
    size_t argc = 1;
    napi_value args[1];
    napi_value _this;
    napi_get_cb_info(env, callback_info, &argc, args, &_this, nullptr);

    int32_t val;
    napi_get_value_int32(env, args[0], &val);

    View* v;
    napi_unwrap(env, _this, reinterpret_cast<void**>(&v));

    MSIHANDLE ph;
    UINT res = MsiViewGetColumnInfo(v->handle_, static_cast<MSICOLINFO>(val), &ph);

    switch (res)
    {
    case ERROR_INVALID_HANDLE:
        napi_throw_type_error(env, nullptr, "An invalid or inactive handle was supplied.");
        return nullptr;

    case ERROR_INVALID_HANDLE_STATE:
        napi_throw_type_error(env, nullptr, "The view is not in an active state.");
        return nullptr;

    case ERROR_INVALID_PARAMETER:
        napi_throw_type_error(env, nullptr, "An invalid MSICOLINFO column information enumeration value was passed to the function.");
        return nullptr;
    }

    napi_value external;
    napi_create_external(env, &ph, nullptr, nullptr, &external);

    napi_value cons;
    napi_get_reference_value(env, Record::constructor, &cons);

    napi_value argv[] = { external };
    napi_value instance;
    napi_new_instance(env, cons, ARRAYSIZE(argv), argv, &instance);

    return instance;
}

napi_value View::GetError(napi_env env, napi_callback_info callback_info)
{
    napi_value _this;
    napi_get_cb_info(env, callback_info, nullptr, 0, &_this, nullptr);

    View* v;
    napi_unwrap(env, _this, reinterpret_cast<void**>(&v));

    DWORD size;
    MsiViewGetError(v->handle_, "", &size);

    std::string col_name(size, '\0');
    MSIDBERROR err = MsiViewGetError(v->handle_, &col_name[0], &size);

    napi_value code;
    napi_create_int32(env, err, &code);

    return code;
}

napi_value View::Modify(napi_env env, napi_callback_info callback_info)
{
    size_t argc = 2;
    napi_value args[2];
    napi_value _this;
    napi_get_cb_info(env, callback_info, &argc, args, &_this, nullptr);

    // TODO
    //_In_ MSIMODIFY eModifyMode,
    // _In_ MSIHANDLE hRecord

    MSIMODIFY eModifyMode;
    MSIHANDLE hRecord = NULL;

    View* v;
    napi_unwrap(env, _this, reinterpret_cast<void**>(&v));

    UINT res = MsiViewModify(v->handle_, eModifyMode, hRecord);

    switch (res) // TODO
    {
    case ERROR_FUNCTION_FAILED:
        napi_throw_type_error(env, nullptr, "A view could not be executed.");
        break;
    case ERROR_INVALID_HANDLE:
        napi_throw_type_error(env, nullptr, "An invalid or inactive handle was supplied.");
        break;
    }

    return nullptr;
}
