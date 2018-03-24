#include "record.h"

#include "common.h"
#include "view.h"

using wi::Record;

napi_ref Record::constructor;

Record::Record(napi_env env, MSIHANDLE handle)
    : env_(env),
    wrapper_(nullptr),
    handle_(handle)
{
}

Record::~Record()
{
    MsiCloseHandle(handle_);
}

void Record::Destructor(napi_env env, void* native_obj, void* finalize_hint)
{
    delete static_cast<Record*>(native_obj);
}

napi_status Record::Init(napi_env env, napi_value exports)
{
    napi_property_descriptor properties[] =
    {
        { "clearData", nullptr, ClearData, nullptr, nullptr, 0, napi_default, 0 },
        { "dataSize", nullptr, DataSize, nullptr, nullptr, 0, napi_default, 0 },
        { "getFieldCount", nullptr, GetFieldCount, nullptr, nullptr, 0, napi_default, 0 },
        { "getInteger", nullptr, GetInteger, nullptr, nullptr, 0, napi_default, 0 },
        { "getString", nullptr, GetString, nullptr, nullptr, 0, napi_default, 0 }
    };

    napi_value cons;
    napi_define_class(env, "Record", NAPI_AUTO_LENGTH, New, nullptr, ARRAYSIZE(properties), properties, &cons);
    napi_create_reference(env, cons, 1, &constructor);
    napi_set_named_property(env, exports, "Record", cons);

    return napi_ok;
}

napi_value Record::New(napi_env env, napi_callback_info callback_info)
{
    size_t argc = 1;
    napi_value args[1];
    napi_value _this;
    napi_get_cb_info(env, callback_info, &argc, args, &_this, nullptr);

    napi_valuetype type;
    napi_typeof(env, args[0], &type);

    switch (type)
    {
    case napi_external:
    {
        MSIHANDLE* ph;
        napi_get_value_external(env, args[0], reinterpret_cast<void**>(&ph));

        Record* rec = new Record(env, *ph);
        napi_wrap(env, _this, rec, Record::Destructor, nullptr, &rec->wrapper_);

        break;
    }
    case napi_number:
    {
        int32_t num;
        napi_get_value_int32(env, args[0], &num);

        MSIHANDLE ph = MsiCreateRecord(static_cast<UINT>(num));
        Record* rec = new Record(env, ph);
        napi_wrap(env, _this, rec, Record::Destructor, nullptr, &rec->wrapper_);

        break;
    }
    default:
        napi_throw_type_error(env, nullptr, "Wrong argument type");
        return nullptr;
    }

    return _this;
}

napi_value Record::ClearData(napi_env env, napi_callback_info callback_info)
{
    napi_value _this;
    napi_get_cb_info(env, callback_info, nullptr, 0, &_this, nullptr);

    Record* rec;
    napi_unwrap(env, _this, reinterpret_cast<void**>(&rec));

    unsigned int res = MsiRecordClearData(rec->handle_);

    switch (res)
    {
    case ERROR_INVALID_HANDLE:
        napi_throw_type_error(env, nullptr, "The handle is inactive or does not represent a record.");
        break;
    }

    return nullptr;
}

napi_value Record::DataSize(napi_env env, napi_callback_info callback_info)
{
    size_t argc = 1;
    napi_value args[1];
    napi_value _this;
    napi_get_cb_info(env, callback_info, &argc, args, &_this, nullptr);

    Record* rec;
    napi_unwrap(env, _this, reinterpret_cast<void**>(&rec));

    int32_t field;
    napi_get_value_int32(env, args[0], &field);

    unsigned int res = MsiRecordDataSize(rec->handle_, static_cast<UINT>(field));

    napi_value result;
    napi_create_uint32(env, res, &result);

    return result;
}

napi_value Record::GetFieldCount(napi_env env, napi_callback_info callback_info)
{
    napi_value _this;
    napi_get_cb_info(env, callback_info, nullptr, 0, &_this, nullptr);

    Record* rec;
    napi_unwrap(env, _this, reinterpret_cast<void**>(&rec));

    unsigned int count = MsiRecordGetFieldCount(rec->handle_);

    napi_value res;
    napi_create_uint32(env, count, &res);

    return res;
}

napi_value Record::GetInteger(napi_env env, napi_callback_info callback_info)
{
    size_t argc = 1;
    napi_value args[1];
    napi_value _this;
    napi_get_cb_info(env, callback_info, &argc, args, &_this, nullptr);

    Record* rec;
    napi_unwrap(env, _this, reinterpret_cast<void**>(&rec));

    int32_t field;
    napi_get_value_int32(env, args[0], &field);

    int res = MsiRecordGetInteger(rec->handle_, static_cast<UINT>(field));

    switch (res)
    {
    case MSI_NULL_INTEGER:
        napi_throw_type_error(env, nullptr, "The field is null or if the field is a string that cannot be converted to an integer.");
        return nullptr;
    }

    napi_value result;
    napi_create_int32(env, res, &result);

    return result;
}

napi_value Record::GetString(napi_env env, napi_callback_info callback_info)
{
    size_t argc = 1;
    napi_value args[1];
    napi_value _this;
    napi_get_cb_info(env, callback_info, &argc, args, &_this, nullptr);

    Record* rec;
    napi_unwrap(env, _this, reinterpret_cast<void**>(&rec));

    int32_t field;
    napi_get_value_int32(env, args[0], &field);

    LPTSTR temp = "";
    DWORD pcchValueBuf = 0;
    MsiRecordGetString(rec->handle_, static_cast<UINT>(field), temp, &pcchValueBuf);

    pcchValueBuf += 1;

    std::string val(static_cast<size_t>(pcchValueBuf), '\0');
    MsiRecordGetString(rec->handle_, static_cast<UINT>(field), &val[0], &pcchValueBuf);

    napi_value result;
    napi_create_string_utf8(env, val.c_str(), static_cast<size_t>(pcchValueBuf), &result);

    return result;
}

