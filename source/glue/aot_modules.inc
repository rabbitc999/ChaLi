/* AOT 模块登记的统一清单；由不同宏展开为独立入口及编译器选择表。
 *
 * 已全部迁移到 C 直通表的模块没有 C++ 胶水文件，这里改成直接登记该模块的入口表；
 * `std::clock` 原先与数学函数同在一个胶水文件、共用 `GLUE_NO_STD_MATH`，一并归到 math。 */
CHALI_AOT_MODULE(reference, std_reference_glue::bind(registry, error))
CHALI_AOT_MODULE(print, registry.bindModule(abi_print_records, error))
CHALI_AOT_MODULE(collection, collection_glue::bind(registry.native(), error))
CHALI_AOT_MODULE(string, registry.bindModule(abi_string_records, error))
CHALI_AOT_MODULE(big_number, big_number_glue::bind(registry.native(), error))
CHALI_AOT_MODULE(cas, std_cas_glue::bind(registry.native(), error))
CHALI_AOT_MODULE(file, file_native_glue::bind(registry.native(), error))
#ifndef GLUE_NO_STD_FILESYSTEM
CHALI_AOT_MODULE(filesystem, std_filesystem_glue::bind(registry, error))
#endif
#ifndef GLUE_NO_STD_JSON
CHALI_AOT_MODULE(json, std_json_glue::bind(registry, error))
#endif
#ifndef GLUE_NO_STD_TOML
CHALI_AOT_MODULE(toml, std_toml_glue::bind(registry, error))
#endif
#ifndef GLUE_NO_STD_BINARY
CHALI_AOT_MODULE(binary, std_binary_glue::bind(registry, error))
#endif
#ifndef GLUE_NO_STD_COMPRESS
CHALI_AOT_MODULE(compress, std_compress_glue::bind(registry, error))
#endif
#ifndef GLUE_NO_STD_MATH
CHALI_AOT_MODULE(math,
                 registry.bindModule(abi_math_records, error) && registry.bindModule(abi_clock_records, error))
#endif
#ifndef GLUE_NO_STD_RANDOM
CHALI_AOT_MODULE(random, registry.bindModule(abi_random_records, error))
#endif
#ifndef GLUE_NO_STD_REGEX
CHALI_AOT_MODULE(regex, std_regex::bind(registry, error))
#endif
#ifndef GLUE_NO_STD_SIGNAL
CHALI_AOT_MODULE(signal, std_signal_glue::bind(registry, error))
#endif
#ifndef GLUE_NO_STD_LINALG
CHALI_AOT_MODULE(linalg, std_linalg_glue::bind(registry, error))
#endif
#ifndef GLUE_NO_STD_PARSE
CHALI_AOT_MODULE(parse, std_parse_glue::bind(registry, error))
#endif
#ifndef GLUE_NO_STD_PROCESS
CHALI_AOT_MODULE(process, std_process_glue::bind(registry, error))
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
