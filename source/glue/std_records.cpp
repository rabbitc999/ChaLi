#include "std_native_record.hpp"
#include "std_records.hpp"
#include "std_reference.hpp"
#include "std_collection.hpp"
#include "../chali_glue.hpp"
#include "std_big_number.hpp"
#include "std_cas.hpp"
#include "std_file.hpp"
#include "abi/chali_standard.h"

namespace chali_standard_records
{
/**
 * @brief 为独立桌面宿主登记标准不透明记录及 C 入口。
 * @param registry 宿主拥有的胶水注册表。
 * @param error 注册失败的诊断输出。
 * @return 所有模块绑定成功时返回 true。
 */
bool bind(chali_glue::Registry &registry, Memory::String *error)
{
    const bool ok = std_reference_glue::bind(registry, error) && collection_glue::bind(registry.native(), error) && big_number_glue::bind(registry.native(), error) && std_cas_glue::bind(registry.native(), error) && file_native_glue::bind(registry.native(), error) && registry.bindModule(abi_process_records, error);
    /* `std::lang` 的标准错误流输出已全部是 C 直通入口，没有 C++ 胶水可调。 */
    registry.bindDirectEntries();
    return ok;
}
}  // namespace chali_standard_records
