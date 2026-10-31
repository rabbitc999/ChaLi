/* 模块名与 C 入口表提供函数的统一清单。每项可包含多张入口表。
 * 标准库集中注册排除 demo，AOT 选择保留 demo；裁剪宏沿用既有约定。 */
CHALI_AOT_MODULE(print, abi_print_records)
CHALI_AOT_MODULE(collection, abi_collection_records)
CHALI_AOT_MODULE(string, abi_string_records)
CHALI_AOT_MODULE(big_number, abi_big_number_records)
CHALI_AOT_MODULE(cas, abi_cas_records)
CHALI_AOT_MODULE(file, abi_file_records)
#ifndef GLUE_NO_STD_FILESYSTEM
CHALI_AOT_MODULE(filesystem, abi_filesystem_records)
#endif
#ifndef GLUE_NO_STD_JSON
CHALI_AOT_MODULE(json, abi_serialization_records, abi_json_records)
#endif
#ifndef GLUE_NO_STD_TOML
CHALI_AOT_MODULE(toml, abi_toml_records)
#endif
#ifndef GLUE_NO_STD_COMPRESS
CHALI_AOT_MODULE(compress, abi_compress_records)
#endif
#ifndef GLUE_NO_STD_MATH
CHALI_AOT_MODULE(math,
                 abi_math_records, abi_clock_records)
#endif
#ifndef GLUE_NO_STD_RANDOM
CHALI_AOT_MODULE(random, abi_random_records)
#endif
#ifndef GLUE_NO_STD_REGEX
CHALI_AOT_MODULE(regex, abi_regex_records)
#endif
#ifndef GLUE_NO_STD_LINALG
CHALI_AOT_MODULE(linalg, abi_scalar_records, abi_linalg_records)
#endif
#ifndef GLUE_NO_STD_PARSE
CHALI_AOT_MODULE(parse, abi_parse_records)
#endif
#ifndef GLUE_NO_STD_PROCESS
CHALI_AOT_MODULE(process, abi_process_records)
#endif
#ifndef GLUE_NO_STD_SQL
CHALI_AOT_MODULE(sql, abi_sql_records, abi_sql_ipage_records)
#endif
#ifndef GLUE_NO_STD_IMAGE
CHALI_AOT_MODULE(image, abi_image_records)
#endif
#ifndef GLUE_NO_STD_NET
CHALI_AOT_MODULE(net, abi_net_records)
#endif
#ifndef CHALI_STANDARD_ONLY
CHALI_AOT_MODULE(demo, abi_demo_records)
#endif
