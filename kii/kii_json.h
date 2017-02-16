/**
 * @file kii_json.h
 * @brief This is a file defining Kii JSON APIs.
 */
#ifndef KII_JSON_H
#define KII_JSON_H

#include <jsmn.h>

#ifdef __cplusplus
extern "C" {
#endif

/** JSON token data to parse JSON string. */
typedef jsmntok_t kii_json_token_t;

/** Resource used by KII JSON library. Fields of this struct
 * determines max number of tokens inside json can be parsed.
 */
typedef struct kii_json_resource_t {

    /** Array to set kii_json_t#tokens. */
    kii_json_token_t *tokens;

    /** Size of kii_json_resource_t#tokens */
    size_t tokens_num;

} kii_json_resource_t;

/** Resource allocator for kii_json_resource_t.
 *
 * When parsing target JSON string requires more tokens provided by
 * kii_json_resource_t, This function is called. You need to resize
 * kii_json_resource_t#tokens and set new size to
 * kii_json_resource_t#tokens_num which is greater than or equal to
 * required_size.
 *
 * If you allocate new resource,
 * Application may need to free old resource since the library never free the
 * given resource.
 *
 * \param[inout] resource kii json resources used by KII JSON library.
 * \param[in] required_size newly required token size.
 * \return 1: success to allocate resource, 0: fail to allocate resourceresize.
 */
typedef int
    (*KII_JSON_RESOURCE_CB)(
        kii_json_resource_t* resource,
        size_t required_size);

/** object manages context of kii json apis.
 * Do not share this object and fields inside this object among the
 * thread/tasks.
 * library won't sync the request if the multiple threads/tasks share the
 * resource.
 */
typedef struct kii_json_t {

    /** Resource used by KII JSON library.
     *
     * This field is optional. If KII_JSON_FIXED_TOKEN_NUM macro is
     * defined, KII JSON library allocate resources in stack memory
     * with the size specified number by the macro.
     * In this case, re-allocation won't happens.
     * This would be convenient if your environment has enough size of stack
     * memory to work with the targetting JSON data structure.
     *
     * If KII_JSON_FIXED_TOKEN_NUM macro is not present,
     * you can allocate resource anywhere you want to put.
     * Library never allocate/free the resource.
     * Allocation and free must be done by application.
     * In case of running out given resource and need more resouces to parse
     * given JSON string,
     * library calls resource_cb to ask application to allocate more resources.
     */
    kii_json_resource_t* resource;

    /** Callback asks for allocate more tokens to parse JSON.
     *
     * Called when the target JSON string can not be parsed with given resource.
     * Library won't try to allocate resource when given resource is not enough
     * to parse JSON string when this field is NULL.
     *
     * If KII_JSON_FIXED_TOKEN_NUM is defined, this callback won't be called.
     */
    KII_JSON_RESOURCE_CB resource_cb;

    /** Error string. If error occurs in kii_json library, then error
     * message is set to this fields. If NULL, no error message is
     * set.
     */
    char* error_string_buff;

    /** Size of error_string_buff. */
    size_t error_string_length;
} kii_json_t;

/** Boolean type */
typedef enum kii_json_boolean_t {
    KII_JSON_FALSE,
    KII_JSON_TRUE
} kii_json_boolean_t;

/** Return value of kii_json_read_object(kii_json_t*, const char*,
 * size_t, kii_json_field_t*) */
typedef enum kii_json_parse_result_t {

    /** JSON string is successfully parsed and all kii_json_field_t
     * variables are successfully set. i.e., all kii_json_field_t type
     * fields are KII_JSON_FIELD_PARSE_SUCCESS.
     */
    KII_JSON_PARSE_SUCCESS,

    /** JSON string is successfully parsed but some kii_json_field_t
     * variables are failed. i.e., some kii_json_field_t type fields
     * are not KII_JSON_FIELD_PARSE_SUCCESS.
     */
    KII_JSON_PARSE_PARTIAL_SUCCESS,

    /** JSON string is successfully parsed but type of root object
     * type is unmatched to using function.
     *
     * kii_json_read_object(kii_json_t*, const char*, size_t,
     * kii_json_field_t*) requires JSON object if JSON array is
     * passed, then this error is returned.
     */
    KII_JSON_PARSE_ROOT_TYPE_ERROR,

    /** kii_json_t#tokens is not enough to parse JSON string.*/
    KII_JSON_PARSE_SHORTAGE_TOKENS,

    /** JSON string is failed to parse. Passed string is not JSON string. */
    KII_JSON_PARSE_INVALID_INPUT
} kii_json_parse_result_t;

/** Field parsing result. Assigned to kii_json_field_t#result. */
typedef enum kii_json_field_parse_result_t {
    /** Field parsing is success. */
    KII_JSON_FIELD_PARSE_SUCCESS,

    /** Type of field specified kii_json_field_t#type is unmathced.*/
    KII_JSON_FIELD_PARSE_TYPE_UNMATCHED,

    /** Field specified by kii_json_field_t#name is not found. */
    KII_JSON_FIELD_PARSE_NOT_FOUND,

    /** Coping string to kii_json_field_t#field_copy#string is failed.
     * kii_json_field_t#field_copy_buff_size may shorter than actual
     * length.
     */
    KII_JSON_FIELD_PARSE_COPY_FAILED,

    /** Coping int, long or double value to
     * kii_json_field_t#field_copy#int_value,
     * kii_json_field_t#field_copy#long_value or
     * kii_json_field_t#field_copy#double_value is failed. value is
     * overflowed.
     */
    KII_JSON_FIELD_PARSE_COPY_OVERFLOW,

    /** Coping int, long or double value to
     * kii_json_field_t#field_copy#int_value,
     * kii_json_field_t#field_copy#long_value or
     * kii_json_field_t#field_copy#double_value is failed. value is
     * underflowed.
     */
    KII_JSON_FIELD_PARSE_COPY_UNDERFLOW
} kii_json_field_parse_result_t;

/** Type of parsed JSON field. This value is assigned to
 * kii_json_field_t#type. */
typedef enum kii_json_field_type_t {

    /** This value denotes any JSON types. If this value is set to
     * kii_json_field_t#type, then kii_json_read_object(kii_json_t*,
     * const char*, size_t, kii_json_field_t*) ignore type checking.
     */
    KII_JSON_FIELD_TYPE_ANY,

    /** This values denotes an signed interger value. Maximum is
     * INT_MAX and Minimum is INT_MIN. */
    KII_JSON_FIELD_TYPE_INTEGER,

    /** This values denotes an signed interger value. Maximum is
     * LONG_MAX and Minimum is LONG_MIN. */
    KII_JSON_FIELD_TYPE_LONG,

    /** This value denotes an double value. */
    KII_JSON_FIELD_TYPE_DOUBLE,

    /** This value denotes kii_json_boolean_t value. */
    KII_JSON_FIELD_TYPE_BOOLEAN,

    /** This value denotes denotes NULL value. */
    KII_JSON_FIELD_TYPE_NULL,

    /** This value denotes JSON string. */
    KII_JSON_FIELD_TYPE_STRING,

    /** This value denotes JSON object. */
    KII_JSON_FIELD_TYPE_OBJECT,

    /** This value denotes JSON array. */
    KII_JSON_FIELD_TYPE_ARRAY
} kii_json_field_type_t;

/** JSON parsed field data.
 *
 * Input of kii_json_read_object(kii_json_t*, const char*, size_t,
 * kii_json_field_t*).
 *
 * Array of kii_json_field_t is passed to
 * kii_json_read_object(kii_json_t*, const char*, size_t,
 * kii_json_field_t*).
 *

 * End point of the array is specified by kii_json_field_t#name and
 * kii_json_field_t#path. If both of kii_json_field_t#name and
 * kii_json_field_t#path are NULL, kii_json_read_object(kii_json_t*,
 * const char*, size_t, kii_json_field_t*) consider that it is the end
 * point of the passed array.
 */
typedef struct kii_json_field_t {

    /** Parsing target key name. Input of
     * kii_json_read_object(kii_json_t*, const char*, size_t,
     * kii_json_field_t*).
     *
     * This can point an only field in root json object.
     * \deprecated{You should use kii_json_field_t#path}
     */
    const char* name;

    /** Parsing target path. Input of
     * kii_json_read_object(kii_json_t*, const char*, size_t,
     * kii_json_field_t*).
     *
     * This can point any field or element of array. BNF like notation
     * of path is following:
     *
     * \code
     * path ::= '/' identifier subpath
     * subpath ::= '/' identifier subpath | ''
     * identifier ::= field | index
     * index ::= '[' 0 ']' | '[' [1-9][0-9]+ ']'
     * field ::= [char | escaped]+
     * char ::= any ascii characters expect '/', '[', ']' and '\'
     * escaped = "\\/" | "\\[" | "\\]" | "\\"
     * \endcode
     *
     * If you want to get first element of color array in following
     * json example:
     *
     * \code
     * {
     *     "ligtht" : {
     *         "color" : [ 0, 128, 255]
     *      }
     * }
     * \endcode
     *
     * \code
     * path ="/ligtht/color/[0]";
     * \endcode
     *
     * In rare cases, like following:
     * \code
     * {
     *     "[]/\\" : "rare"
     * }
     * \endcode
     *
     * You can specify "[]/\\" as following:
     *
     * \code
     * path ="/\\[\\]\\/\\";
     * \endcode
     */
    const char* path;

    /** Field parse result. Output of
     * kii_json_read_object(kii_json_t*, const char*, size_t,
     * kii_json_field_t*).
     */
    kii_json_field_parse_result_t result;

    /** Parsed target value type. Input and Output of
     * kii_json_read_object(kii_json_t*, const char*, size_t,
     * kii_json_field_t*). Inputted value is expected value type and
     * outputted value is actual value type.
     *
     * If type is set as
     * kii_json_field_type_t#KII_JSON_FIELD_TYPE_ANY, then
     * kii_json_read_object(kii_json_t*, const char*, size_t,
     * kii_json_field_t*) ignore type checking.
     *
     * If actual type is not matched expected type:
     *   - kii_json_read_object(kii_json_t*, const char*, size_t,
     *     kii_json_field_t*) set actual type.
     *   - if expected type is not
     *     kii_json_field_type_t#KII_JSON_FIELD_TYPE_ANY, then
     *     kii_json_field_t#result becomes
     *     kii_json_parse_result_t#KII_JSON_FIELD_PARSE_TYPE_UNMATCHED.
     *   - if expected type is
     *     kii_json_field_type_t#KII_JSON_FIELD_TYPE_ANY, then
     *     kii_json_field_t#result become
     *     kii_json_parse_result_t#KII_JSON_FIELD_PARSE_SUCCESS.
     */
    kii_json_field_type_t type;

    /** Start point of this field in given buffer. Output of
     * kii_json_read_object(kii_json_t*, const char*, size_t,
     * kii_json_field_t*).
     */
    size_t start;

    /** End point of this field in given buffer. Output of
     * kii_json_read_object(kii_json_t*, const char*, size_t,
     * kii_json_field_t*).
     */
    size_t end;

    /** Buffer to copy field value. if NULL, no copy is
     * generated. Using value is determined by value of
     * kii_json_field_t#type. If kii_json_field_t#type is
     * kii_json_field_type_t#KII_JSON_FIELD_TYPE_NULL, no copy is
     * generated.
     */
    union {

        /** This value is used if kii_json_field_t#type is
         * kii_json_field_type_t#KII_JSON_FIELD_TYPE_STRING,
         * kii_json_field_type_t#KII_JSON_FIELD_TYPE_OBJECT or
         * kii_json_field_type_t#KII_JSON_FIELD_TYPE_ARRAY.
         */
        char* string;

        /** This value is used if kii_json_field_t#type is
         * kii_json_field_type_t#KII_JSON_FIELD_TYPE_INTEGER. If
         * parsing target is overflowed, then this value is
         * INT_MAX. If parsing is underflowed, then this value is
         * INT_MIN.
         */
        int int_value;

        /** This value is used if kii_json_field_t#type is
         * kii_json_field_type_t#KII_JSON_FIELD_TYPE_LONG. If parsing
         * target is overflowed, then this value is LONG_MAX. If parsing
         * is underflowed, then this value is LONG_MIN.
         */
        long long_value;

        /** This value is used if kii_json_field_t#type is
         * kii_json_field_type_t#KII_JSON_FIELD_TYPE_DOUBLE. If
         * parsing target is overflowed, then this value is plus or
         * minus HUGE_VAL. If parsing is underflowed, then this value is
         * 0.
         */
        double double_value;

        /** This value is used if kii_json_field_t#type is
         * kii_json_field_type_t#KII_JSON_FIELD_TYPE_BOOLEAN.
         */
        kii_json_boolean_t boolean_value;
    } field_copy;

    /** Length of field_copy#string. ignored if field_copy#string is
     * null or kii_json_field_t#type is not
     * kii_json_field_type_t#KII_JSON_FIELD_TYPE_STRING,
     * kii_json_field_type_t#KII_JSON_FIELD_TYPE_OBJECT and
     * kii_json_field_type_t#KII_JSON_FIELD_TYPE_ARRAY.
     */
    size_t field_copy_buff_size;

} kii_json_field_t;

/** parse JSON string as JSON object.
 *  \param [in] kii json instance.
 *  \param [in] pointer of JSON string.
 *  \param [in] length of JSON string.
 *  \param [inout] field of kii JSON parser.
 *  \return parse JSON result.
 */
kii_json_parse_result_t kii_json_read_object(
        kii_json_t* kii_json,
        const char* json_string,
        size_t json_string_len,
        kii_json_field_t* fields);

#ifdef __cplusplus
}
#endif

#endif
