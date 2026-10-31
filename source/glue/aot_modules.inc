/* AOT 模块登记的统一清单；由不同宏展开为独立入口及编译器选择表。
 *
 * 已全部迁移到 C 直通表的模块没有 C++ 胶水文件，这里改成直接登记该模块的入口表；
 * `std::clock` 原先与数学函数同在一个胶水文件、共用 `GLUE_NO_STD_MATH`，一并归到 math。 */
CHALI_AOT_MODULE(print, registry.bindModule(abi_print_records, error))
CHALI_AOT_MODULE(collection, collection_glue::bind(registry.native(), error))
CHALI_AOT_MODULE(string, registry.bindModule(abi_string_records, error))
CHALI_AOT_MODULE(big_number, big_number_glue::bind(registry.native(), error))
CHALI_AOT_MODULE(cas, std_cas_glue::bind(registry.native(), error))
CHALI_AOT_MODULE(file, file_native_glue::bind(registry.native(), error))
#ifndef GLUE_NO_STD_FILESYSTEM
CHALI_AOT_MODULE(filesystem, registry.bindModule(abi_filesystem_records, error))
#endif
#ifndef GLUE_NO_STD_JSON
CHALI_AOT_MODULE(json, registry.bindModule(abi_serialization_records, error) && registry.bindModule(abi_json_records, error))
#endif
#ifndef GLUE_NO_STD_TOML
CHALI_AOT_MODULE(toml, registry.bindModule(abi_toml_records, error))
#endif
#ifndef GLUE_NO_STD_COMPRESS
CHALI_AOT_MODULE(compress, registry.bindModule(abi_compress_records, error))
#endif
#ifndef GLUE_NO_STD_MATH
CHALI_AOT_MODULE(math,
                 registry.bindModule(abi_math_records, error) && registry.bindModule(abi_clock_records, error))
#endif
#ifndef GLUE_NO_STD_RANDOM
CHALI_AOT_MODULE(random, registry.bindModule(abi_random_records, error))
#endif
#ifndef GLUE_NO_STD_REGEX
CHALI_AOT_MODULE(regex, registry.bindModule(abi_regex_records, error))
#endif
#ifndef GLUE_NO_STD_LINALG
CHALI_AOT_MODULE(linalg, std_linalg_glue::bind(registry, error))
#endif
#ifndef GLUE_NO_STD_PARSE
CHALI_AOT_MODULE(parse, registry.bindModule(abi_parse_records, error))
#endif
#ifndef GLUE_NO_STD_PROCESS
CHALI_AOT_MODULE(process, registry.bindModule(abi_process_records, error))
#endif
#ifndef GLUE_NO_STD_SQL
CHALI_AOT_MODULE(sql, std_sql_glue::bind(registry, error))
#endif
#ifndef GLUE_NO_STD_IMAGE
CHALI_AOT_MODULE(image, std_image_glue::bind(registry, error))
#endif
#ifndef GLUE_NO_STD_NET
CHALI_AOT_MODULE(net, std_net_glue::bind(registry, error))
#endif
CHALI_AOT_MODULE(demo, bindChaLiGlue(registry, error))
