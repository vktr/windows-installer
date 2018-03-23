#include "database.h"

#include "common.h"
#include "record.h"
#include "view.h"

using wi::Database;

napi_ref Database::constructor;

Database::Database(napi_env env, MSIHANDLE handle)
    : env_(env),
    wrapper_(nullptr),
    handle_(handle)
{
}

Database::~Database()
{
    MsiCloseHandle(handle_);
}

void Database::Destructor(napi_env env, void* native_obj, void* finalize_hint)
{
    delete static_cast<Database*>(native_obj);
}

napi_status Database::Init(napi_env env, napi_value exports)
{
    napi_property_descriptor properties[] =
    {
        { "commit", nullptr, Commit, nullptr, nullptr, 0, napi_default, 0 },
        { "getPrimaryKeys", nullptr, GetPrimaryKeys, nullptr, nullptr, 0, napi_default, 0 },
        { "isTablePersistent", nullptr, IsTablePersistent, nullptr, nullptr, 0, napi_default, 0 },
        { "openView", nullptr, OpenView, nullptr, nullptr, 0, napi_default, 0 },
    };

    napi_value cons;
    napi_define_class(env, "Database", NAPI_AUTO_LENGTH, New, nullptr, ARRAYSIZE(properties), properties, &cons);
    napi_create_reference(env, cons, 1, &constructor);
    napi_set_named_property(env, exports, "Database", cons);

    return napi_ok;
}

napi_value Database::New(napi_env env, napi_callback_info callback_info)
{
    napi_value target;
    napi_get_new_target(env, callback_info, &target);

    if (target == nullptr)
    {
        napi_throw_error(env, nullptr, "Not a construct (new) call");
        return nullptr;
    }

    size_t argc = 2;
    napi_value args[2];
    napi_value _this;
    napi_get_cb_info(env, callback_info, &argc, args, &_this, nullptr);

    std::string database_path;
    wi_napi_to_std_string(env, args[0], &database_path);

    LPCTSTR persist;

    napi_valuetype type;
    napi_typeof(env, args[1], &type);

    switch (type)
    {
    case napi_number:
    {
        int32_t val;
        napi_get_value_int32(env, args[1], &val);

        switch (val)
        {
        case 0: // TODO #define this
            persist = MSIDBOPEN_READONLY;
            break;
        default:
            napi_throw_type_error(env, nullptr, "Not implemented");
            return nullptr;
        }

        break;
    }
    default:
        napi_throw_type_error(env, nullptr, "Invalid type arg");
        return nullptr;
    }

    MSIHANDLE ph;
    UINT res = MsiOpenDatabase(
        database_path.c_str(),
        persist,
        &ph);

    switch (res)
    {
    case ERROR_CREATE_FAILED:
        napi_throw_type_error(env, nullptr, "The database could not be created.");
        return nullptr;

    case ERROR_INVALID_PARAMETER:
        napi_throw_type_error(env, nullptr, "One of the parameters was invalid");
        return nullptr;

    case ERROR_OPEN_FAILED:
        napi_throw_type_error(env, nullptr, "The database could not be opened as requested.");
        return nullptr;
    }

    Database* db = new Database(env, ph);
    napi_wrap(env, _this, db, Database::Destructor, nullptr, &db->wrapper_);
    return _this;
}

napi_value Database::Commit(napi_env env, napi_callback_info callback_info)
{
    napi_value _this;
    napi_get_cb_info(env, callback_info, nullptr, 0, &_this, nullptr);

    Database* db;
    napi_unwrap(env, _this, reinterpret_cast<void**>(&db));

    UINT res = MsiDatabaseCommit(db->handle_);

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

napi_value Database::IsTablePersistent(napi_env env, napi_callback_info callback_info)
{
    size_t argc = 1;
    napi_value args[1];
    napi_value _this;
    napi_get_cb_info(env, callback_info, &argc, args, &_this, nullptr);

    Database* db;
    napi_unwrap(env, _this, reinterpret_cast<void**>(&db));

    std::string table_name;
    wi_napi_to_std_string(env, args[0], &table_name);

    MSICONDITION res = MsiDatabaseIsTablePersistent(db->handle_, table_name.c_str());

    napi_value result;

    switch (res)
    {
    case MSICONDITION_ERROR:
        napi_throw_type_error(env, nullptr, "An invalid handle or invalid parameter is passed to the function.");
        return nullptr;
        
    case MSICONDITION_NONE:
        // TODO
        break;

    case MSICONDITION_FALSE:
        napi_get_boolean(env, false, &result);
        break;

    case MSICONDITION_TRUE:
        napi_get_boolean(env, true, &result);
        break;
    }

    return result;
}

napi_value Database::GetPrimaryKeys(napi_env env, napi_callback_info callback_info)
{
    size_t argc = 1;
    napi_value args[1];
    napi_value _this;
    napi_get_cb_info(env, callback_info, &argc, args, &_this, nullptr);

    Database* db;
    napi_unwrap(env, _this, reinterpret_cast<void**>(&db));

    std::string table_name;
    wi_napi_to_std_string(env, args[0], &table_name);

    MSIHANDLE ph;
    UINT res = MsiDatabaseGetPrimaryKeys(
        db->handle_,
        table_name.c_str(),
        &ph);

    switch (res)
    {
    case ERROR_INVALID_HANDLE:
        napi_throw_type_error(env, nullptr, "An invalid or inactive handle was supplied.");
        return nullptr;

    case ERROR_INVALID_PARAMETER:
        napi_throw_type_error(env, nullptr, "An invalid SQL query string was passed to the function.");
        return nullptr;

    case ERROR_INVALID_TABLE:
        napi_throw_type_error(env, nullptr, "An invalid table was passed to the function.");
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

napi_value Database::OpenView(napi_env env, napi_callback_info callback_info)
{
    size_t argc = 1;
    napi_value args[1];
    napi_value _this;
    napi_get_cb_info(env, callback_info, &argc, args, &_this, nullptr);

    Database* db;
    napi_unwrap(env, _this, reinterpret_cast<void**>(&db));

    std::string query;
    wi_napi_to_std_string(env, args[0], &query);

    MSIHANDLE ph;
    UINT res = MsiDatabaseOpenView(
        db->handle_,
        query.c_str(),
        &ph);

    switch (res)
    {
    case ERROR_BAD_QUERY_SYNTAX:
        napi_throw_type_error(env, nullptr, "An invalid SQL query string was passed to the function.");
        return nullptr;

    case ERROR_INVALID_HANDLE:
        napi_throw_type_error(env, nullptr, "An invalid or inactive handle was supplied.");
        return nullptr;
    }

    napi_value external;
    napi_create_external(env, &ph, nullptr, nullptr, &external);

    napi_value cons;
    napi_get_reference_value(env, View::constructor, &cons);

    napi_value argv[] = { external };
    napi_value instance;
    napi_new_instance(env, cons, ARRAYSIZE(argv), argv, &instance);

    return instance;
}
