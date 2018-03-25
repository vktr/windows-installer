#include "database.h"

#include "common.h"
#include "record.h"
#include "summary-information.h"
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
        { "applyTransform", nullptr, ApplyTransform, nullptr, nullptr, 0, napi_default, 0 },
        { "commit", nullptr, Commit, nullptr, nullptr, 0, napi_default, 0 },
        { "export", nullptr, Export, nullptr, nullptr, 0, napi_default, 0 },
        { "generateTransform", nullptr, GenerateTransform, nullptr, nullptr, 0, napi_default, 0 },
        { "getPrimaryKeys", nullptr, GetPrimaryKeys, nullptr, nullptr, 0, napi_default, 0 },
        { "getState", nullptr, GetState, nullptr, nullptr, 0, napi_default, 0 },
        { "getSummaryInformation", nullptr, GetSummaryInformation, nullptr, nullptr, 0, napi_default, 0 },
        { "import", nullptr, Import, nullptr, nullptr, 0, napi_default, 0 },
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

napi_value Database::ApplyTransform(napi_env env, napi_callback_info callback_info)
{
    size_t argc = 2;
    napi_value args[2];
    napi_value _this;
    napi_get_cb_info(env, callback_info, &argc, args, &_this, nullptr);

    Database* db;
    napi_unwrap(env, _this, reinterpret_cast<void**>(&db));

    std::string transform_file;
    wi_napi_to_std_string(env, args[0], &transform_file);

    int32_t error_conditions;
    napi_get_value_int32(env, args[1], &error_conditions);

    UINT res = MsiDatabaseApplyTransform(
        db->handle_,
        transform_file.c_str(),
        error_conditions);

    switch (res)
    {
    case ERROR_INVALID_HANDLE:
        napi_throw_type_error(env, nullptr, "An invalid or inactive handle was supplied.");
        break;

    case ERROR_INVALID_PARAMETER:
        napi_throw_type_error(env, nullptr, "An invalid parameter was passed to the function.");
        break;

    case ERROR_INSTALL_TRANSFORM_FAILURE:
        napi_throw_type_error(env, nullptr, "The transform could not be applied.");
        break;

    case ERROR_OPEN_FAILED:
        napi_throw_type_error(env, nullptr, "The transform storage file could not be opened.");
        break;
    }

    return nullptr;
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

napi_value Database::Export(napi_env env, napi_callback_info callback_info)
{
    size_t argc = 3;
    napi_value args[3];
    napi_value _this;
    napi_get_cb_info(env, callback_info, &argc, args, &_this, nullptr);

    Database* db;
    napi_unwrap(env, _this, reinterpret_cast<void**>(&db));

    std::string table_name;
    wi_napi_to_std_string(env, args[0], &table_name);

    std::string folder_path;
    wi_napi_to_std_string(env, args[1], &folder_path);

    std::string file_name;
    wi_napi_to_std_string(env, args[2], &file_name);

    UINT res = MsiDatabaseExport(
        db->handle_,
        table_name.c_str(),
        folder_path.c_str(),
        file_name.c_str());

    switch (res)
    {
    case ERROR_BAD_PATHNAME:
        napi_throw_type_error(env, nullptr, "An invalid path is passed to the function.");
        break;

    case ERROR_FUNCTION_FAILED:
        napi_throw_type_error(env, nullptr, "The function fails.");
        break;

    case ERROR_INVALID_HANDLE:
        napi_throw_type_error(env, nullptr, "An invalid or inactive handle is supplied.");
        break;

    case ERROR_INVALID_PARAMETER:
        napi_throw_type_error(env, nullptr, "An invalid parameter is passed to the function.");
        break;
    }

    return nullptr;
}

napi_value Database::GenerateTransform(napi_env env, napi_callback_info callback_info)
{
    size_t argc = 2;
    napi_value args[2];
    napi_value _this;
    napi_get_cb_info(env, callback_info, &argc, args, &_this, nullptr);

    Database* db;
    napi_unwrap(env, _this, reinterpret_cast<void**>(&db));

    Database* db_reference;
    napi_unwrap(env, args[0], reinterpret_cast<void**>(&db_reference));

    if (argc == 1) // No transform file, returns true or false if the databases are equal or not
    {
        UINT res = MsiDatabaseGenerateTransform(
            db->handle_,
            db_reference->handle_,
            NULL,
            0, // This is a reserved argument and must be set to 0.
            0); // This is a reserved argument and must be set to 0.

        napi_value result;

        switch (res)
        {
        case ERROR_NO_DATA:
            napi_get_boolean(env, true, &result);
            break;

        case ERROR_CREATE_FAILED:
            napi_throw_type_error(env, nullptr, "The storage for the transform file could not be created.");
            return nullptr;

        case ERROR_INSTALL_TRANSFORM_FAILURE:
            napi_throw_type_error(env, nullptr, "The transform could not be generated.");
            return nullptr;

        case ERROR_INVALID_HANDLE:
            napi_throw_type_error(env, nullptr, "An invalid or inactive handle was supplied.");
            return nullptr;

        case ERROR_INVALID_PARAMETER:
            napi_throw_type_error(env, nullptr, "An invalid parameter was passed to the function.");
            return nullptr;

        // case NOERROR:
        case ERROR_SUCCESS:
            napi_get_boolean(env, true, &result);
            break;
        }

        return result;
    }

    // Assume two args
    std::string transform_file;
    wi_napi_to_std_string(env, args[1], &transform_file);

    UINT res = MsiDatabaseGenerateTransform(
        db->handle_,
        db_reference->handle_,
        transform_file.c_str(),
        0, // This is a reserved argument and must be set to 0.
        0); // This is a reserved argument and must be set to 0.

    switch (res)
    {
    case ERROR_CREATE_FAILED:
        napi_throw_type_error(env, nullptr, "The storage for the transform file could not be created.");
        return nullptr;

    case ERROR_INSTALL_TRANSFORM_FAILURE:
        napi_throw_type_error(env, nullptr, "The transform could not be generated.");
        return nullptr;

    case ERROR_INVALID_HANDLE:
        napi_throw_type_error(env, nullptr, "An invalid or inactive handle was supplied.");
        return nullptr;

    case ERROR_INVALID_PARAMETER:
        napi_throw_type_error(env, nullptr, "An invalid parameter was passed to the function.");
        return nullptr;
    }

    return nullptr;
}

napi_value Database::Import(napi_env env, napi_callback_info callback_info)
{
    size_t argc = 2;
    napi_value args[2];
    napi_value _this;
    napi_get_cb_info(env, callback_info, &argc, args, &_this, nullptr);

    Database* db;
    napi_unwrap(env, _this, reinterpret_cast<void**>(&db));

    std::string folder_path;
    wi_napi_to_std_string(env, args[0], &folder_path);

    std::string file_name;
    wi_napi_to_std_string(env, args[0], &file_name);

    UINT res = MsiDatabaseImport(
        db->handle_,
        folder_path.c_str(),
        file_name.c_str());

    switch (res)
    {
    case ERROR_BAD_PATHNAME:
        napi_throw_type_error(env, nullptr, "An invalid path was passed to the function.");
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

napi_value Database::GetState(napi_env env, napi_callback_info callback_info)
{
    napi_value _this;
    napi_get_cb_info(env, callback_info, 0, nullptr, &_this, nullptr);

    Database* db;
    napi_unwrap(env, _this, reinterpret_cast<void**>(&db));

    MSIDBSTATE state = MsiGetDatabaseState(db->handle_);

    switch (state)
    {
    case MSIDBSTATE_ERROR:
        napi_throw_type_error(env, nullptr, "An invalid database handle was passed to the function.");
        return nullptr;
    }

    napi_value result;
    napi_create_int32(env, state, &result);

    return result;
}

napi_value Database::GetSummaryInformation(napi_env env, napi_callback_info callback_info)
{
    size_t argc = 1;
    napi_value args[1];
    napi_value _this;
    napi_get_cb_info(env, callback_info, &argc, args, &_this, nullptr);

    Database* db;
    napi_unwrap(env, _this, reinterpret_cast<void**>(&db));

    int32_t update_count;
    napi_get_value_int32(env, args[0], &update_count);

    MSIHANDLE ph;

    UINT res = MsiGetSummaryInformation(
        db->handle_,
        0,
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

    napi_value external;
    napi_create_external(env, &ph, nullptr, nullptr, &external);

    napi_value cons;
    napi_get_reference_value(env, SummaryInformation::constructor, &cons);

    napi_value argv[] = { external };
    napi_value instance;
    napi_new_instance(env, cons, ARRAYSIZE(argv), argv, &instance);

    return instance;
}

napi_value Database::Merge(napi_env env, napi_callback_info callback_info)
{
    size_t argc = 2;
    napi_value args[2];
    napi_value _this;
    napi_get_cb_info(env, callback_info, &argc, args, &_this, nullptr);

    Database* db;
    napi_unwrap(env, _this, reinterpret_cast<void**>(&db));

    Database* db_merge;
    napi_unwrap(env, args[0], reinterpret_cast<void**>(&db));

    std::string table_name;
    wi_napi_to_std_string(env, args[1], &table_name);

    UINT res = MsiDatabaseMerge(
        db->handle_,
        db_merge->handle_,
        table_name.c_str());

    switch (res)
    {
    case ERROR_FUNCTION_FAILED:
        napi_throw_type_error(env, nullptr, "Row merge conflicts were reported.");
        break;

    case ERROR_INVALID_HANDLE:
        napi_throw_type_error(env, nullptr, "An invalid or inactive handle was supplied.");
        break;

    case ERROR_INVALID_TABLE:
        napi_throw_type_error(env, nullptr, "An invalid table was supplied.");
        break;

    case ERROR_DATATYPE_MISMATCH:
        napi_throw_type_error(env, nullptr, "Schema difference between the two databases.");
        break;
    }

    return nullptr;
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
