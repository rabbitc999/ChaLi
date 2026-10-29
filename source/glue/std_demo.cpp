#include "../chali_glue.hpp"
#include "std_demo.hpp"
#include <cstdint>
#include <optional>

namespace
{
/** @brief 演示宿主类型的名义身份标记。 */
struct DemoCounterTag;
/** @brief 演示宿主类型的名义身份标记。 */
struct DemoSignalTag;
/** @brief 演示宿主类型的名义身份标记。 */
struct GlueDemoPointTag;
/** @brief 演示宿主类型的名义身份标记。 */
struct GlueDemoSignalTag;
/** @brief 演示宿主类型的名义身份标记。 */
struct GlueDemoRectTag;
/** @brief 演示宿主类型的名义身份标记。 */
struct GlueDemoCubeTag;
/** @brief 演示宿主类型的名义身份标记。 */
struct GlueDemoBufferTag;
/** @brief 演示宿主类型的名义身份标记。 */
struct GluePointerBoxTag;
using DemoCounter = chali_glue::Handle<DemoCounterTag>;
using DemoSignal = chali_glue::Enum<DemoSignalTag>;
}  // namespace

/**
 * @brief 注册编译器与 AOT 共同使用的演示宿主接口及类型声明。
 * @param registry 接收演示绑定的独立注册表。
 * @param error 接收绑定诊断；演示使用固定且互不重复的服务名。
 * @return 全部服务注册成功时返回真，首个失败时返回假。
 */
bool bindChaLiGlue(chali_glue::Registry &registry, Memory::String *error)
{
    if (error) error->clear();
    if (!registry.bindTypedImplementation<DemoCounter, std::uint64_t>(
            "demo_counter_from_uint", [](std::uint64_t value) { return DemoCounter{ value }; }, error)) return false;
    if (!registry.bindTypedImplementation<std::uint64_t, DemoCounter>(
            "demo_counter_to_uint", [](DemoCounter value) { return value.value; }, error)) return false;
    if (!registry.bindTyped<DemoCounter, DemoCounter, DemoCounter>(
            "demo_cpp_add", [](DemoCounter left, DemoCounter right) {
                return DemoCounter{ left.value + right.value };
            },
            error)) return false;
    if (!registry.bindTypedGeneric<std::int32_t, std::int32_t>(
            "demo_cpp_identity", { "int32" }, [](std::int32_t value) { return value; }, error)) return false;
    if (!registry.bindTyped<std::int32_t, Memory::String>(
            "demo_cpp_text_size",
            [](const Memory::String &text) { return static_cast<std::int32_t>(text.size()); }, error)) return false;
    if (!registry.bindTyped<std::int64_t, std::int64_t, chali_glue::Variadic<std::int64_t>>(
            "demo_cpp_variadic_sum",
            [](std::int64_t base, const chali_glue::Variadic<std::int64_t> &values) {
                for (const std::int64_t value : values) base += value;
                return base;
            },
            error)) return false;
    if (!registry.bindTyped<std::int64_t, chali_glue::Function<std::int64_t(std::int64_t)>,
                            std::int64_t>(
            "demo_cpp_apply",
            [](const chali_glue::Function<std::int64_t(std::int64_t)> &function,
               std::int64_t value) { return function(value); },
            error)) return false;
    if (!registry.bindTypedGeneric<DemoSignal, DemoSignal>(
            "demo_cpp_identity", { "program::Signal" }, [](DemoSignal value) { return value; }, error)) return false;
    if (!registry.bindTypedGeneric<std::int32_t, DemoSignal>(
            "demo_cpp_enum_tag", { "program::Signal" }, [](DemoSignal value) { return value.tag(); }, error)) return false;
    if (!registry.bindTypedGeneric<std::int64_t, DemoSignal>(
            "demo_cpp_enum_payload", { "program::Signal" },
            [](DemoSignal value) { return value.payload<std::int64_t>(1u); }, error)) return false;

    /* ---- 胶水层声明的 CHALI 类型（结构体 / 枚举 / 接口 + 宿主实现）----
     *
     * 这些定义不写在 `.chl` 里：`registry.declarations()` 生成声明文本，编译前追加到
     * 合并源码末尾（见本文件 `main` 里的 `declarationSource()`），样例直接使用即可。
     * 多方法接口 `GlueDemoInspect` 的方法按 `<impl 符号>.<方法名>` 查宿主符号。 */
    chali_glue::Declarations &declarations = registry.declarations();
    declarations.structType("GlueDemoPoint", { { "int64", "x" }, { "int64", "y" } });
    declarations.enumType("GlueDemoSignal", { { "Idle", {} }, { "Tone", { { "int64", "hz" } } } });
    declarations.interfaceType("GlueDemoInspect",
                               { { "int64", "area", {} },
                                 { "int64", "scaled", { { "int64", "factor" } } } });
    declarations.implementation("GlueDemoPoint", "GlueDemoInspect", "glue_demo_inspect");
    declarations.interfaceType("GlueDemoCoded", { { "int64", "code", {} } });
    declarations.implementation("GlueDemoSignal", "GlueDemoCoded", "glue_demo_signal");
    if (!registry.bindTypedMethod<std::int64_t, chali_glue::Record<GlueDemoPointTag>>(
            "glue_demo_inspect", "area",
            [](chali_glue::Record<GlueDemoPointTag> self) {
                return self.field<std::int64_t>(0u) * self.field<std::int64_t>(1u);
            },
            error)) return false;
    if (!registry.bindTypedMethod<std::int64_t, chali_glue::Record<GlueDemoPointTag>, std::int64_t>(
            "glue_demo_inspect", "scaled",
            [](chali_glue::Record<GlueDemoPointTag> self, std::int64_t factor) {
                return self.field<std::int64_t>(0u) * self.field<std::int64_t>(1u) * factor;
            },
            error)) return false;
    if (!registry.bindTyped<std::int64_t, chali_glue::Enum<GlueDemoSignalTag>>(
            "glue_demo_signal",
            [](chali_glue::Enum<GlueDemoSignalTag> self) {
                return static_cast<std::int64_t>(self.tag());
            },
            error)) return false;

    /* ---- 胶水层声明**接口继承**：`GlueDemoMeasured : GlueDemoInspect`（外加 unit()），
     * `GlueDemoTagged` 用 `interfaceInherit` 只继承、不加方法。结构体实现派生接口时，
     * 继承来的 area / scaled 与自己的 unit 都要有宿主符号（多方法 impl 仍按
     * `<impl 符号>.<方法名>` 查）。样例见 `example/samples/glue_interface_inherit.chl`。 ---- */
    declarations.interfaceType("GlueDemoMeasured", { { "int64", "unit", {} } },
                               { "GlueDemoInspect" });
    declarations.interfaceInherit("GlueDemoTagged", { "GlueDemoMeasured" });
    declarations.structType("GlueDemoCube", { { "int64", "edge" }, { "int64", "unitScale" } });
    declarations.implementation("GlueDemoCube", "GlueDemoTagged", "glue_demo_cube");
    if (!registry.bindTypedMethod<std::int64_t, chali_glue::Record<GlueDemoCubeTag>>(
            "glue_demo_cube", "area",
            [](chali_glue::Record<GlueDemoCubeTag> self) {
                const std::int64_t edge = self.field<std::int64_t>(0u);
                return edge * edge;
            },
            error)) return false;
    if (!registry.bindTypedMethod<std::int64_t, chali_glue::Record<GlueDemoCubeTag>, std::int64_t>(
            "glue_demo_cube", "scaled",
            [](chali_glue::Record<GlueDemoCubeTag> self, std::int64_t factor) {
                const std::int64_t edge = self.field<std::int64_t>(0u);
                return edge * edge * factor;
            },
            error)) return false;
    if (!registry.bindTypedMethod<std::int64_t, chali_glue::Record<GlueDemoCubeTag>>(
            "glue_demo_cube", "unit",
            [](chali_glue::Record<GlueDemoCubeTag> self) {
                return self.field<std::int64_t>(1u);
            },
            error)) return false;

    /* ---- 胶水层声明**系统接口继承**：`GlueDemoBuffered` 继承 `Index<int64,int64>` /
     * `Multiply<int64,int64>` / `ExplicitConvert<int64>`，再带一个自己的 `size()`。
     * 一个宿主 impl 要绑 4 条方法（多方法接口按 `<impl 符号>.<方法名>` 查），于是 CHALI 侧
     * `buffer[3]` / `buffer * 4` / `cast<int64>(buffer)` 都落到宿主符号上。
     * 样例见 `example/samples/glue_system_interfaces.chl`。 ---- */
    declarations.structType("GlueDemoBuffer", { { "int64", "base" } });
    declarations.interfaceType("GlueDemoBuffered",
                               { { "int64", "size", {} } },
                               { "Index<int64, int64>", "Multiply<int64, int64>",
                                 "ExplicitConvert<int64>" });
    declarations.implementation("GlueDemoBuffer", "GlueDemoBuffered", "glue_demo_buffer");
    if (!registry.bindTypedMethod<std::int64_t, chali_glue::Record<GlueDemoBufferTag>>(
            "glue_demo_buffer", "size",
            [](chali_glue::Record<GlueDemoBufferTag> self) {
                return self.field<std::int64_t>(0u);
            },
            error)) return false;
    if (!registry.bindTypedMethod<std::int64_t, chali_glue::Record<GlueDemoBufferTag>, std::int64_t>(
            "glue_demo_buffer", "index",
            [](chali_glue::Record<GlueDemoBufferTag> self, std::int64_t key) {
                return self.field<std::int64_t>(0u) + key;
            },
            error)) return false;
    if (!registry.bindTypedMethod<std::int64_t, chali_glue::Record<GlueDemoBufferTag>, std::int64_t>(
            "glue_demo_buffer", "leftMul",
            [](chali_glue::Record<GlueDemoBufferTag> self, std::int64_t value) {
                return self.field<std::int64_t>(0u) * value;
            },
            error)) return false;
    if (!registry.bindTypedMethod<std::int64_t, chali_glue::Record<GlueDemoBufferTag>>(
            "glue_demo_buffer", "explicitConvert",
            [](chali_glue::Record<GlueDemoBufferTag> self) {
                return self.field<std::int64_t>(0u);
            },
            error)) return false;
    /* `interfaceInherit`（只继承、不加新方法）+ 系统接口：单方法接口沿用原符号名。 */
    declarations.interfaceInherit("GlueDemoAddable", { "Add<int64, int64>" });
    declarations.implementation("GlueDemoBuffer", "GlueDemoAddable", "glue_demo_buffer_add");
    if (!registry.bindTypedImplementation<std::int64_t, chali_glue::Record<GlueDemoBufferTag>,
                                          std::int64_t>(
            "glue_demo_buffer_add",
            [](chali_glue::Record<GlueDemoBufferTag> self, std::int64_t value) {
                return self.field<std::int64_t>(0u) + value;
            },
            error)) return false;

    /* 方式二：`memberFunctions` —— 每条成员自带宿主符号，编译器自动合成单方法接口 +
     * impl，胶水按符号名逐个绑定即可（重载靠各自的符号区分）。最后一条 `missing`
     * **故意只声明不绑定**：`example/samples-error/glue_member_unbound.chl` 用它锁定"宿主符号
     * 没注册"的运行期报错。 */
    declarations.structType("GlueDemoRect", { { "int64", "width" }, { "int64", "height" } });
    declarations.memberFunctions(
        "GlueDemoRect",
        { { "int64", "area", "glue_demo_rect_area", {} },
          { "int64", "area", "glue_demo_rect_area_scaled", { { "int64", "factor" } } },
          { "int64", "perimeter", "glue_demo_rect_perimeter", {} },
          { "int64", "missing", "glue_demo_rect_missing", {} } });
    if (!registry.bindTypedImplementation<std::int64_t, chali_glue::Record<GlueDemoRectTag>>(
            "glue_demo_rect_area", [](chali_glue::Record<GlueDemoRectTag> self) {
                return self.field<std::int64_t>(0u) * self.field<std::int64_t>(1u);
            },
            error)) return false;
    if (!registry.bindTypedImplementation<std::int64_t, chali_glue::Record<GlueDemoRectTag>,
                                          std::int64_t>(
            "glue_demo_rect_area_scaled",
            [](chali_glue::Record<GlueDemoRectTag> self, std::int64_t factor) {
                return self.field<std::int64_t>(0u) * self.field<std::int64_t>(1u) * factor;
            },
            error)) return false;
    if (!registry.bindTypedImplementation<std::int64_t, chali_glue::Record<GlueDemoRectTag>>(
            "glue_demo_rect_perimeter", [](chali_glue::Record<GlueDemoRectTag> self) {
                return 2 * (self.field<std::int64_t>(0u) + self.field<std::int64_t>(1u));
            },
            error)) return false;

    /* ---- 类型化指针：`T mut&` / `T const&` 过边界是 `Reference<T>`，宿主侧直接按 T
     * 读 / 写，不再手算 VM 地址。标量用 `Reference<int64>` 的 read / write / at；结构体
     * 指针用 `Reference<Record<Tag>>`，read 出视图后 `field` / `set` 按字段读写。
     * 样例 `example/samples/pointer_glue.chl`，语言侧数组直接退化成 `T mut&`。 ---- */
    if (!registry.bindTypedWithContext<std::int64_t, chali_glue::Reference<std::int64_t>>(
            "demo_pointer_bump",
            [](chali_glue::Context &context, chali_glue::Reference<std::int64_t> cell) {
                const std::int64_t old = cell.read(context);
                cell.write(context, old + 1);
                return old;
            },
            error)) return false;
    if (!registry.bindTypedWithContext<
            std::int64_t,
            chali_glue::Reference<chali_glue::Record<GluePointerBoxTag>>>(
            "demo_pointer_sum",
            [](chali_glue::Context &context,
               chali_glue::Reference<chali_glue::Record<GluePointerBoxTag>> box) {
                const auto view = box.read(context);
                return view.template field<std::int64_t>(0u) + view.template field<std::int64_t>(1u);
            },
            error)) return false;
    if (!registry.bindTypedWithContext<
            void,
            chali_glue::Reference<chali_glue::Record<GluePointerBoxTag>>,
            std::int64_t, std::int64_t>(
            "demo_pointer_offset",
            [](chali_glue::Context &context,
               chali_glue::Reference<chali_glue::Record<GluePointerBoxTag>> box,
               std::int64_t dx, std::int64_t dy) {
                const auto view = box.read(context);
                view.template set<std::int64_t>(0u, view.template field<std::int64_t>(0u) + dx);
                view.template set<std::int64_t>(1u, view.template field<std::int64_t>(1u) + dy);
            },
            error)) return false;
    if (!registry.bindTypedWithContext<std::int64_t, chali_glue::Reference<std::int64_t>,
                                       std::int64_t>(
            "demo_pointer_at",
            [](chali_glue::Context &context, chali_glue::Reference<std::int64_t> data,
               std::int64_t index) { return data.at(context, index); },
            error)) return false;
    using Reference = chali_glue::Reference<std::int64_t>;
    using OptionalReference = std::optional<Reference>;
    if (!registry.bindTyped<OptionalReference, bool, Reference>("demo_reference_optional", [](bool present, Reference value) -> OptionalReference { return present ? OptionalReference(value) : std::nullopt; }, error)) return false;
    if (!registry.bindTypedWithContext<std::int64_t, OptionalReference>("demo_reference_read", [](chali_glue::Context &context, const OptionalReference &value) { return value ? value->read(context) : std::int64_t(-1); }, error)) return false;
    if (!registry.bindTypedWithContext<void, OptionalReference>("demo_reference_write", [](chali_glue::Context &context, const OptionalReference &value) {
                                                               if (value) value->write(context, value->read(context) + 1); }, error)) return false;
    return true;
}
