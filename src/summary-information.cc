#include "summary-information.h"

#include "common.h"

using wi::SummaryInformation;

napi_ref SummaryInformation::constructor;

SummaryInformation::SummaryInformation(napi_env env, MSIHANDLE handle)
    : env_(env),
    wrapper_(nullptr),
    handle_(handle)
{
}

SummaryInformation::~SummaryInformation()
{
    MsiCloseHandle(handle_);
}

void SummaryInformation::Destructor(napi_env env, void* native_obj, void* finalize_hint)
{
    delete static_cast<SummaryInformation*>(native_obj);
}

napi_status SummaryInformation::Init(napi_env env, napi_value exports)
{
    napi_property_descriptor properties[] =
    {
        { "getProperty", nullptr, GetProperty, nullptr, nullptr, 0, napi_default, 0 },
        { "getPropertyCount", nullptr, GetPropertyCount, nullptr, nullptr, 0, napi_default, 0 },
        { "persist", nullptr, Persist, nullptr, nullptr, 0, napi_default, 0 },
        { "setProperty", nullptr, SetProperty, nullptr, nullptr, 0, napi_default, 0 },
    };

    napi_value cons;
    napi_define_class(env, "SummaryInformation", NAPI_AUTO_LENGTH, New, nullptr, ARRAYSIZE(properties), properties, &cons);
    napi_create_reference(env, cons, 1, &constructor);
    napi_set_named_property(env, exports, "SummaryInformation", cons);

    return napi_ok;
}

napi_value SummaryInformation::New(napi_env env, napi_callback_info callback_info)
{
    size_t argc = 2;
    napi_value args[2];
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

        SummaryInformation* si = new SummaryInformation(env, *ph);
        napi_wrap(env, _this, si, SummaryInformation::Destructor, nullptr, &si->wrapper_);

        break;
    }
    case napi_string:
    {
        std::string database_path;
        wi_napi_to_std_string(env, args[0], &database_path);

        int32_t update_count;
        napi_get_value_int32(env, args[1], &update_count);

        MSIHANDLE ph;

        UINT res = MsiGetSummaryInformation(
            0,
            database_path.c_str(),
            static_cast<UINT>(update_count),
            &ph);

        switch (res)
        {
        case ERROR_INSTALL_PACKAGE_INVALID:
            napi_throw_type_error(env, nullptr, "The installation package is invalid.");
            return nullptr;

        case ERROR_INVALID_HANDLE:
            napi_throw_type_error(env, nullptr, "An invalid or inactive handle was supplied.");
            return nullptr;

        case ERROR_INVALID_PARAMETER:
            napi_throw_type_error(env, nullptr, "An invalid parameter was passed to the function.");
            return nullptr;
        }

        SummaryInformation* si = new SummaryInformation(env, ph);
        napi_wrap(env, _this, si, SummaryInformation::Destructor, nullptr, &si->wrapper_);

        break;
    }
    default:
        napi_throw_type_error(env, nullptr, "Wrong argument type");
        return nullptr;
    }

    return _this;
}

napi_value SummaryInformation::GetProperty(napi_env env, napi_callback_info callback_info)
{
    size_t argc = 1;
    napi_value args[1];
    napi_value _this;
    napi_get_cb_info(env, callback_info, &argc, args, &_this, nullptr);

    SummaryInformation* si;
    napi_unwrap(env, _this, reinterpret_cast<void**>(&si));

    UINT uiProperty = 0;
    napi_get_value_uint32(env, args[0], &uiProperty);

    UINT puiDataType = 0;
    INT piValue = 0;
    FILETIME pftValue;
    DWORD pcchValueBuf = 0;
    UINT res = MsiSummaryInfoGetProperty(
        si->handle_,
        uiProperty,
        &puiDataType,
        &piValue,
        &pftValue,
        "",
        &pcchValueBuf);

    std::string szValueBuf;

    switch (res)
    {
    case ERROR_INVALID_HANDLE:
        napi_throw_type_error(env, nullptr, "An invalid or inactive handle was supplied.");
        return nullptr;
        break;

    case ERROR_INVALID_PARAMETER:
        napi_throw_type_error(env, nullptr, "An invalid parameter was passed to the function.");
        return nullptr;
        break;

    case ERROR_MORE_DATA:
    {
        pcchValueBuf += 1;
        szValueBuf = std::string(pcchValueBuf, '\0');
        MsiSummaryInfoGetProperty(
            si->handle_,
            uiProperty,
            &puiDataType,
            &piValue,
            &pftValue,
            &szValueBuf[0],
            &pcchValueBuf);
        break;
    }

    case ERROR_UNKNOWN_PROPERTY:
        return nullptr;
    }

    napi_value value;

    switch (puiDataType)
    {
    case VT_EMPTY:
        napi_get_undefined(env, &value);
        break;

    case VT_FILETIME:
    {
        ULARGE_INTEGER ui;
        ui.LowPart = pftValue.dwLowDateTime;
        ui.HighPart = pftValue.dwHighDateTime;
        napi_create_int64(env, ui.QuadPart, &value);
        break;
    }

    case VT_I2:
    case VT_I4:
        napi_create_int32(env, piValue, &value);
        break;

    case VT_LPSTR:
        napi_create_string_utf8(env, szValueBuf.c_str(), pcchValueBuf, &value);
        break;

    default:
    {
        std::string err = "Unknown result type: ";
        err += std::to_string(puiDataType);
        napi_throw_type_error(env, nullptr, err.c_str());
        return nullptr;
    }
    }

    return value;
}

napi_value SummaryInformation::GetPropertyCount(napi_env env, napi_callback_info callback_info)
{
    napi_value _this;
    napi_get_cb_info(env, callback_info, 0, nullptr, &_this, nullptr);

    SummaryInformation* si;
    napi_unwrap(env, _this, reinterpret_cast<void**>(&si));

    UINT property_count = 0;
    UINT res = MsiSummaryInfoGetPropertyCount(si->handle_, &property_count);

    switch (res)
    {
    case ERROR_INVALID_HANDLE:
        napi_throw_type_error(env, nullptr, "An invalid or inactive handle was supplied.");
        break;
    }

    napi_value result;
    napi_create_uint32(env, static_cast<uint32_t>(property_count), &result);

    return result;
}

napi_value SummaryInformation::Persist(napi_env env, napi_callback_info callback_info)
{
    napi_value _this;
    napi_get_cb_info(env, callback_info, 0, nullptr, &_this, nullptr);

    SummaryInformation* si;
    napi_unwrap(env, _this, reinterpret_cast<void**>(&si));

    UINT res = MsiSummaryInfoPersist(si->handle_);

    switch (res)
    {
    case ERROR_FUNCTION_FAILED:
        napi_throw_type_error(env, nullptr, "The function failed.");
        break;

    case ERROR_INVALID_HANDLE:
        napi_throw_type_error(env, nullptr, "An invalid or inactive handle was supplied.");
        break;
    }

    return nullptr;
}

napi_value SummaryInformation::SetProperty(napi_env env, napi_callback_info callback_info)
{
    size_t argc = 2;
    napi_value args[2];
    napi_value _this;
    napi_get_cb_info(env, callback_info, &argc, args, &_this, nullptr);

    SummaryInformation* si;
    napi_unwrap(env, _this, reinterpret_cast<void**>(&si));

    int32_t property_id;
    napi_get_value_int32(env, args[0], &property_id);

    UINT uiDataType = VT_UNKNOWN;
    INT iValue = 0;
    FILETIME pftValue;
    std::string szValue = "";

    switch (property_id)
    {
    case PID_CODEPAGE:
    {
        int32_t value;
        napi_get_value_int32(env, args[1], &value);
        iValue = value;
        uiDataType = VT_I2;
        break;
    }
    case PIDSI_TITLE:
    case PIDSI_SUBJECT:
    case PIDSI_AUTHOR:
    case PIDSI_KEYWORDS:
    case PIDSI_COMMENTS:
    case PIDSI_TEMPLATE:
    case PIDSI_LASTAUTHOR:
    case PIDSI_REVNUMBER:
    case PIDSI_APPNAME:
    {
        std::string value;
        wi_napi_to_std_string(env, args[1], &value);
        szValue = value;
        uiDataType = VT_LPSTR;
        break;
    }
    case PIDSI_LASTPRINTED:
    case PIDSI_CREATE_DTM:
    case PIDSI_LASTSAVE_DTM:
    {
        int64_t value;
        napi_get_value_int64(env, args[1], &value);

        ULARGE_INTEGER ui;
        ui.QuadPart = value;

        FILETIME ft;
        ft.dwHighDateTime = ui.HighPart;
        ft.dwLowDateTime = ui.LowPart;

        pftValue = ft;
        uiDataType = VT_FILETIME;

        break;
    }
    case PIDSI_PAGECOUNT:
    case PIDSI_WORDCOUNT:
    case PIDSI_CHARCOUNT:
    case PIDSI_DOC_SECURITY:
    {
        int32_t value;
        napi_get_value_int32(env, args[1], &value);
        iValue = value;
        uiDataType = VT_I4;
        break;
    }
    default:
        napi_throw_type_error(env, nullptr, "Unknown property.");
        return nullptr;
    }

    UINT res = MsiSummaryInfoSetProperty(
            si->handle_,
            static_cast<UINT>(property_id),
            uiDataType,
            iValue,
            &pftValue,
            szValue.c_str());

    switch (res)
    {
    case ERROR_DATATYPE_MISMATCH:
        napi_throw_type_error(env, nullptr, "The data types were mismatched.");
        break;

    case ERROR_FUNCTION_FAILED:
        napi_throw_type_error(env, nullptr, "The function failed.");
        break;

    case ERROR_INVALID_HANDLE:
        napi_throw_type_error(env, nullptr, "An invalid or inactive handle was supplied.");
        break;

    case ERROR_INVALID_PARAMETER:
        napi_throw_type_error(env, nullptr, "An invalid parameter was passed to the function.");
        break;

    case ERROR_UNKNOWN_PROPERTY:
        napi_throw_type_error(env, nullptr, "The summary information property is unknown.");
        break;

    case ERROR_UNSUPPORTED_TYPE:
        napi_throw_type_error(env, nullptr, "The type is unsupported.");
        break;
    }

    return nullptr;
}
