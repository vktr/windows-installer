#ifndef WI_COMMON_H
#define WI_COMMON_H

#include <node_api.h>
#include <string>

inline napi_status wi_napi_to_std_string(napi_env env, napi_value value, std::string* str)
{
    size_t len;
    napi_get_value_string_utf8(env, value, nullptr, 0, &len);

    *str = std::string(len + 1, '\0');
    napi_get_value_string_utf8(env, value, &(*str)[0], len + 1, nullptr);

    return napi_ok;
}

#endif
