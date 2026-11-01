/**
 * @file chali_generator_assets.cpp
 * @brief 从实际前端库解压资产，逐项核对受控 ABI 目录与单一集合清单。
 */
#include "chali_stdlib.hpp"
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
/**
 * @brief 规范化不影响源码语义的 Windows 行尾。
 * @param text 原始源码。
 * @return LF 行尾的源码。
 */
std::string normalized(std::string text) {
    for(std::size_t at=0;(at=text.find("\r\n",at))!=std::string::npos;)text.erase(at,1);
    return text;
}
/**
 * @brief 核对真实压缩资产中的全部 ABI 文件，不将磁盘源读取冒充资产读取。
 * @param argc 参数个数。
 * @param argv 显式传入用于比对的仓库根。
 * @return 所有资产完整且无旧胶水时为零。
 */
int main(int argc,char **argv) {
    assert(argc==2);
    std::map<std::string,std::string> assets;
    for(auto *entry=chali_module::embedded::generatorAssetTable();entry&&entry->path;++entry)
        assert(assets.emplace(entry->path,entry->text).second);
    std::size_t count=0;
    for(const auto &entry:std::filesystem::directory_iterator(std::filesystem::u8path(argv[1])/"source/abi")) {
        const auto extension=entry.path().extension().string();
        if(!entry.is_regular_file() || (extension!=".c"&&extension!=".cpp"&&extension!=".h"&&extension!=".hpp"&&extension!=".inc"))continue;
        const auto name="abi/"+entry.path().filename().u8string();
        std::ifstream file(entry.path(),std::ios::binary);assert(file);std::ostringstream text;text<<file.rdbuf();
        assert(assets.count(name)&&normalized(assets.at(name))==normalized(text.str()));++count;
    }
    assert(!assets.count("chali_glue.hpp"));
    for (const auto *name : { "chali_digest.hpp", "chali_digest.cpp", "chali_library.hpp", "chali_library_manifest.cpp", "chali_library_package.cpp", "chali_library_import.cpp", "chali_library_build.cpp", "chali_aot_library.hpp", "chali_library_tools.cpp" })
    {
        std::ifstream file(std::filesystem::u8path(argv[1])/"source"/name,std::ios::binary);
        assert(file);std::ostringstream text;text<<file.rdbuf();
        assert(assets.count(name)&&normalized(assets.at(name))==normalized(text.str()));
    }
    assert(count>20&&assets.count("abi/collection_operations.inc")&&assets.count("abi/abi_big_number.cpp")&&assets.count("abi/abi_cas.cpp"));
    for(auto *name:{"std_native_record.hpp","std_native_record.cpp","std_big_number.hpp","std_big_number.cpp","std_cas.hpp","std_cas.cpp","std_demo.cpp","std_demo.hpp","std_registry.hpp","std_glue.cpp","std_glue.hpp","std_records.cpp","std_records.hpp","std_binding.hpp"})
        assert(!assets.count(std::string("glue/")+name));
    std::cout<<count<<" ABI assets decoded from actual compiler library match sources; retired glue absent\n";
}
