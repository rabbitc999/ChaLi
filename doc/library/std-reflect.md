# std::reflect — 反射

<!-- LANGUAGE NAVIGATION -->
[English（中文原文）](std-reflect.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

[返回库目录](README.md) · [全部符号](symbols.md)

## 引入

```chali
import std::reflect;
```

## 公共约定与错误边界

要求工具链构建时同时启用 CHALI_WITH_REFLECTION=1 与 STACKVM_WITH_REFLECTION=1，并在编译/运行时启用 --reflection。类型信息在具体类型和布局确定后生成；动态调用只支持 ReflectValue 的封闭载荷集合。Err 区分未找到、歧义、参数数量、类型、接收者和未暴露等错误。

## 参考代码

完整程序：[doc/examples/library/reflect.chl](../../doc/examples/library/reflect.chl)。

```powershell
chali run doc/examples/library/reflect.chl --reflection
```

需先用 `make reflection` 构建支持反射的工具链；当前二进制未编入支持时，上述命令会明确报缺失宏。

<details>
<summary>展开完整参考代码</summary>

```chali
import std::reflect;

/** @brief 供启用反射的工具链演示类型元数据查询。 */
@Reflect
struct ReferenceRecord { int64 id; String name; };

void main() {
    uint64 id = std::reflect::getTypeId<ReferenceRecord>();
    Option<std::reflect::StructInfo> info = std::reflect::getStructInfo<ReferenceRecord>();
    assert(info.isValue());
    std::reflect::StructInfo record = info.unwrap();
    assert(record.typeId == id && record.fields.len() == 2);
    println("reflect API ok");

}
```

</details>

聚合入口的具体操作参见对应子模块；本页类型/方法的调用形式列在下方。

## 类型、枚举、常量与字段

公开字段按源码原样列出；句柄和游标字段用于包装器状态，通常通过方法操作。枚举的括号表示变体载荷。

| 所属类型 | 声明 | 说明 |
| --- | --- | --- |
| 模块 | <a id="symbol-346"></a>`enum ReflectError : Error` | 反射错误是普通枚举值，不保存目标地址或拥有资源。 |
| ReflectError | `NotFound, Ambiguous, ArgumentCount, TypeMismatch, ReceiverMismatch, UnsupportedSignature, NotExposed, InvalidTarget` | 反射错误是普通枚举值，不保存目标地址或拥有资源。 变体及载荷见声明。 |
| 模块 | <a id="symbol-347"></a>`enum ReflectValue` | 封闭值集合：精确保存位宽、符号及字符串所有权。 |
| ReflectValue | `Empty, Bool(bool value), Char(char value), I8(int8 value), I16(int16 value), I32(int32 value), I64(int64 value), U8(uint8 value), U16(uint16 value), U32(uint32 value), U64(uint64 value), F32(float32 value), F64(float64 value), Text(std::lang::String value)` | 封闭值集合：精确保存位宽、符号及字符串所有权。 变体及载荷见声明。 |
| 模块 | <a id="symbol-348"></a>`struct ParameterInfo` | 反射参数的名称与类型身份。 |
| ParameterInfo | <a id="symbol-349"></a>`std::lang::String name` | 声明或选项的名称。 |
| ParameterInfo | <a id="symbol-350"></a>`uint64 typeId` | 编译器登记的类型身份，不是对象地址。 |
| 模块 | <a id="symbol-351"></a>`struct AnnotationParameter` | 注解参数的名称、类型及可选默认值。 |
| AnnotationParameter | <a id="symbol-352"></a>`std::lang::String name` | 声明或选项的名称。 |
| AnnotationParameter | <a id="symbol-353"></a>`uint64 typeId` | 编译器登记的类型身份，不是对象地址。 |
| AnnotationParameter | <a id="symbol-354"></a>`Option<std::reflect::ReflectValue> defaultValue` | 可选的注解默认实参；None 表示没有默认值。 |
| 模块 | <a id="symbol-355"></a>`struct AnnotationInfo` | 注解定义的名称、类型身份与参数声明。 |
| AnnotationInfo | <a id="symbol-356"></a>`std::lang::String name` | 声明或选项的名称。 |
| AnnotationInfo | <a id="symbol-357"></a>`uint64 typeId` | 编译器登记的类型身份，不是对象地址。 |
| AnnotationInfo | <a id="symbol-358"></a>`List<std::reflect::AnnotationParameter> params` | 按声明顺序保存的参数信息。 |
| 模块 | <a id="symbol-359"></a>`struct Annotation` | 注解实例的定义身份与实际参数值。 |
| Annotation | <a id="symbol-360"></a>`uint64 annotationId` | 注解定义的身份。 |
| Annotation | <a id="symbol-361"></a>`List<std::reflect::ReflectValue> args` | 注解实例的实际参数值。 |
| 模块 | <a id="symbol-362"></a>`struct FuncInfo` | 函数反射信息，包含参数、返回类型和注解。 |
| FuncInfo | <a id="symbol-363"></a>`std::lang::String name` | 声明或选项的名称。 |
| FuncInfo | <a id="symbol-364"></a>`uint64 typeId` | 编译器登记的类型身份，不是对象地址。 |
| FuncInfo | <a id="symbol-365"></a>`List<std::reflect::ParameterInfo> params` | 按声明顺序保存的参数信息。 |
| FuncInfo | <a id="symbol-366"></a>`uint64 returnTypeId` | 函数返回类型的身份。 |
| FuncInfo | <a id="symbol-367"></a>`List<std::reflect::Annotation> annotations` | 附加在该声明上的注解实例。 |
| 模块 | <a id="symbol-368"></a>`struct MethodInfo` | 方法反射信息，包含函数签名及 static、const、构造器等标志。 |
| MethodInfo | <a id="symbol-369"></a>`FuncInfo function` | 方法的完整函数签名与注解信息。 |
| MethodInfo | <a id="symbol-370"></a>`bool isStatic` | 方法是否为静态方法。 |
| MethodInfo | <a id="symbol-371"></a>`bool isConstructor` | 方法是否为构造器。 |
| MethodInfo | <a id="symbol-372"></a>`bool isConst` | 方法是否只读访问接收者。 |
| MethodInfo | <a id="symbol-373"></a>`bool hasDefaultBody` | 接口方法是否提供默认实现。 |
| 模块 | <a id="symbol-374"></a>`struct FieldInfo` | 实例字段反射信息，包含字节偏移、类型身份和拥有属性。 |
| FieldInfo | <a id="symbol-375"></a>`std::lang::String name` | 声明或选项的名称。 |
| FieldInfo | <a id="symbol-376"></a>`uint32 offset` | 相对起始位置的字节偏移。 |
| FieldInfo | <a id="symbol-377"></a>`uint64 typeId` | 编译器登记的类型身份，不是对象地址。 |
| FieldInfo | <a id="symbol-378"></a>`bool isOwned` | 成员是否负责所存资源的所有权。 |
| FieldInfo | <a id="symbol-379"></a>`List<std::reflect::Annotation> annotations` | 附加在该声明上的注解实例。 |
| 模块 | <a id="symbol-380"></a>`struct StaticMemberInfo` | 静态成员反射信息，包含类型身份与拥有属性。 |
| StaticMemberInfo | <a id="symbol-381"></a>`std::lang::String name` | 声明或选项的名称。 |
| StaticMemberInfo | <a id="symbol-382"></a>`uint64 typeId` | 编译器登记的类型身份，不是对象地址。 |
| StaticMemberInfo | <a id="symbol-383"></a>`bool isOwned` | 成员是否负责所存资源的所有权。 |
| StaticMemberInfo | <a id="symbol-384"></a>`List<std::reflect::Annotation> annotations` | 附加在该声明上的注解实例。 |
| 模块 | <a id="symbol-385"></a>`struct VariantInfo` | 枚举变体的名称、标签与载荷参数。 |
| VariantInfo | <a id="symbol-386"></a>`std::lang::String name` | 声明或选项的名称。 |
| VariantInfo | <a id="symbol-387"></a>`uint32 tag` | 枚举变体的数值标签。 |
| VariantInfo | <a id="symbol-388"></a>`List<std::reflect::ParameterInfo> payloads` | 枚举变体的载荷参数声明。 |
| 模块 | <a id="symbol-389"></a>`struct StructInfo` | 结构体反射信息，包含字段、方法、静态成员、实现接口及注解。 |
| StructInfo | <a id="symbol-390"></a>`std::lang::String name` | 声明或选项的名称。 |
| StructInfo | <a id="symbol-391"></a>`uint64 typeId` | 编译器登记的类型身份，不是对象地址。 |
| StructInfo | <a id="symbol-392"></a>`List<std::reflect::FieldInfo> fields` | 结构体的实例字段信息。 |
| StructInfo | <a id="symbol-393"></a>`List<std::reflect::MethodInfo> methods` | 类型或接口的方法信息。 |
| StructInfo | <a id="symbol-394"></a>`List<std::reflect::StaticMemberInfo> staticMembers` | 类型的静态成员信息。 |
| StructInfo | <a id="symbol-395"></a>`List<uint64> implementedInterfaces` | 该类型实现的接口身份。 |
| StructInfo | <a id="symbol-396"></a>`List<std::reflect::Annotation> annotations` | 附加在该声明上的注解实例。 |
| 模块 | <a id="symbol-397"></a>`struct EnumInfo` | 枚举反射信息，包含变体、方法、静态成员、实现接口及注解。 |
| EnumInfo | <a id="symbol-398"></a>`std::lang::String name` | 声明或选项的名称。 |
| EnumInfo | <a id="symbol-399"></a>`uint64 typeId` | 编译器登记的类型身份，不是对象地址。 |
| EnumInfo | <a id="symbol-400"></a>`List<std::reflect::VariantInfo> variants` | 枚举的变体信息。 |
| EnumInfo | <a id="symbol-401"></a>`List<std::reflect::MethodInfo> methods` | 类型或接口的方法信息。 |
| EnumInfo | <a id="symbol-402"></a>`List<std::reflect::StaticMemberInfo> staticMembers` | 类型的静态成员信息。 |
| EnumInfo | <a id="symbol-403"></a>`List<uint64> implementedInterfaces` | 该类型实现的接口身份。 |
| EnumInfo | <a id="symbol-404"></a>`List<std::reflect::Annotation> annotations` | 附加在该声明上的注解实例。 |
| 模块 | <a id="symbol-405"></a>`struct InterfaceInfo` | 接口反射信息，包含方法、实现者类型及注解。 |
| InterfaceInfo | <a id="symbol-406"></a>`std::lang::String name` | 声明或选项的名称。 |
| InterfaceInfo | <a id="symbol-407"></a>`uint64 typeId` | 编译器登记的类型身份，不是对象地址。 |
| InterfaceInfo | <a id="symbol-408"></a>`List<std::reflect::MethodInfo> methods` | 类型或接口的方法信息。 |
| InterfaceInfo | <a id="symbol-409"></a>`List<uint64> implementedBy` | 实现该接口的具体类型身份。 |
| InterfaceInfo | <a id="symbol-410"></a>`List<std::reflect::Annotation> annotations` | 附加在该声明上的注解实例。 |

## 函数索引

| 名称 | 声明 |
| --- | --- |
| [requireReflection](#api-0) | `void requireReflection()` |
| [ReflectError::toString](#api-1) | `String toString()` |
| [ReflectValue::typeId](#api-2) | `uint64 typeId()` |
| [ReflectValue::asBool](#api-3) | `Result<bool,std::reflect::ReflectError> asBool()` |
| [ReflectValue::asChar](#api-4) | `Result<char,std::reflect::ReflectError> asChar()` |
| [ReflectValue::asI8](#api-5) | `Result<int8,std::reflect::ReflectError> asI8()` |
| [ReflectValue::asI16](#api-6) | `Result<int16,std::reflect::ReflectError> asI16()` |
| [ReflectValue::asI32](#api-7) | `Result<int32,std::reflect::ReflectError> asI32()` |
| [ReflectValue::asI64](#api-8) | `Result<int64,std::reflect::ReflectError> asI64()` |
| [ReflectValue::asU8](#api-9) | `Result<uint8,std::reflect::ReflectError> asU8()` |
| [ReflectValue::asU16](#api-10) | `Result<uint16,std::reflect::ReflectError> asU16()` |
| [ReflectValue::asU32](#api-11) | `Result<uint32,std::reflect::ReflectError> asU32()` |
| [ReflectValue::asU64](#api-12) | `Result<uint64,std::reflect::ReflectError> asU64()` |
| [ReflectValue::asF32](#api-13) | `Result<float32,std::reflect::ReflectError> asF32()` |
| [ReflectValue::asF64](#api-14) | `Result<float64,std::reflect::ReflectError> asF64()` |
| [ReflectValue::asText](#api-15) | `Result<std::lang::String,std::reflect::ReflectError> asText()` |
| [getFuncInfo](#api-16) | `Option<std::reflect::FuncInfo> getFuncInfo(std::lang::String name)` |
| [getAnnotationInfo](#api-17) | `Option<std::reflect::AnnotationInfo> getAnnotationInfo(std::lang::String name)` |
| [getTypeId](#api-18) | `uint64 getTypeId<T>()` |
| [getInterfaceId](#api-19) | `uint64 getInterfaceId<I>()` |
| [getStructInfo](#api-20) | `Option<std::reflect::StructInfo> getStructInfo<T>()` |
| [getEnumInfo](#api-21) | `Option<std::reflect::EnumInfo> getEnumInfo<T>()` |
| [getInterfaceInfo](#api-22) | `Option<std::reflect::InterfaceInfo> getInterfaceInfo<I>()` |
| [implementsInterface](#api-23) | `bool implementsInterface<T>(uint64 interfaceId)` |
| [getAnnotations](#api-24) | `List<std::reflect::Annotation> getAnnotations<T>()` |
| [getFieldAnnotations](#api-25) | `List<std::reflect::Annotation> getFieldAnnotations<T>(std::lang::String name)` |
| [getFuncAnnotations](#api-26) | `List<std::reflect::Annotation> getFuncAnnotations(std::lang::String name)` |
| [getMethodAnnotations](#api-27) | `List<std::reflect::Annotation> getMethodAnnotations<T>(std::lang::String name)` |
| [getStaticMemberAnnotations](#api-28) | `List<std::reflect::Annotation> getStaticMemberAnnotations<T>(std::lang::String name)` |
| [call](#api-29) | `Result<std::reflect::ReflectValue,std::reflect::ReflectError> call(std::lang::String name, List<std::reflect::ReflectValue> args)` |
| [callMethod](#api-30) | `Result<std::reflect::ReflectValue,std::reflect::ReflectError> callMethod<T>(T mut& obj, std::lang::String name, List<std::reflect::ReflectValue> args)` |
| [callMethodConst](#api-31) | `Result<std::reflect::ReflectValue,std::reflect::ReflectError> callMethodConst<T>(T const& obj, std::lang::String name, List<std::reflect::ReflectValue> args)` |
| [callStatic](#api-32) | `Result<std::reflect::ReflectValue,std::reflect::ReflectError> callStatic<T>(std::lang::String name, List<std::reflect::ReflectValue> args)` |
| [callEnumMethod](#api-33) | `Result<std::reflect::ReflectValue,std::reflect::ReflectError> callEnumMethod<T>(T mut& obj, std::lang::String name, List<std::reflect::ReflectValue> args)` |
| [callEnumMethodConst](#api-34) | `Result<std::reflect::ReflectValue,std::reflect::ReflectError> callEnumMethodConst<T>(T const& obj, std::lang::String name, List<std::reflect::ReflectValue> args)` |
| [callEnumStatic](#api-35) | `Result<std::reflect::ReflectValue,std::reflect::ReflectError> callEnumStatic<T>(std::lang::String name, List<std::reflect::ReflectValue> args)` |
| [construct](#api-36) | `Result<T,std::reflect::ReflectError> construct<T>(List<std::reflect::ReflectValue> args)` |
| [getField](#api-37) | `Result<F,std::reflect::ReflectError> getField<T,F>(T const& obj, std::lang::String name)` |
| [setField](#api-38) | `Result<bool,std::reflect::ReflectError> setField<T,F>(T mut& obj, std::lang::String name, F value)` |
| [callInterface](#api-39) | `Result<std::reflect::ReflectValue,std::reflect::ReflectError> callInterface<I>(I mut& obj, std::lang::String name, List<std::reflect::ReflectValue> args)` |
| [callInterfaceConst](#api-40) | `Result<std::reflect::ReflectValue,std::reflect::ReflectError> callInterfaceConst<I>(I const& obj, std::lang::String name, List<std::reflect::ReflectValue> args)` |

<a id="api-0"></a>

### requireReflection（1）

**声明**

```chali
void requireReflection();
```

**行为**

检查编译器是否编入反射；缺失时 panic。

反射裁剪构建调用依赖反射的高级接口时直接 panic。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。
实现包含 panic 分支；调用前应处理空值或失败状态。

**调用形式**

```chali
std::reflect::requireReflection();
```

**源码**：[source/std/reflect.chl:5](../../source/std/reflect.chl#L5)。

**完整示例**：[doc/examples/library/reflect.chl](../../doc/examples/library/reflect.chl)。

<a id="api-1"></a>

### ReflectError::toString（2）

**声明**

```chali
String toString();
```

**行为**

由实现者将当前值转换为文本，Formattable 重载接收 style。

返回具体错误原因，并保留载荷中的位置或宿主诊断。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`String`。由实现者将当前值转换为文本，Formattable 重载接收 style。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.toString();
```

**源码**：[source/std/reflect.chl:14](../../source/std/reflect.chl#L14)。

**完整示例**：[doc/examples/library/reflect.chl](../../doc/examples/library/reflect.chl)。

<a id="api-2"></a>

### ReflectValue::typeId（3）

**声明**

```chali
uint64 typeId();
```

**行为**

返回 ReflectValue 当前载荷的精确类型 ID。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`uint64`。返回 ReflectValue 当前载荷的精确类型 ID。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.typeId();
```

**源码**：[source/std/reflect.chl:44](../../source/std/reflect.chl#L44)。

**完整示例**：[doc/examples/library/reflect.chl](../../doc/examples/library/reflect.chl)。

<a id="api-3"></a>

### ReflectValue::asBool（4）

**声明**

```chali
Result<bool,std::reflect::ReflectError> asBool();
```

**行为**

精确提取 Bool 载荷；变体不匹配返回 Err(TypeMismatch)，不进行隐式数值转换。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Result<bool,std::reflect::ReflectError>`。精确提取 Bool 载荷；变体不匹配返回 Err(TypeMismatch)，不进行隐式数值转换。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.asBool();
```

**源码**：[source/std/reflect.chl:64](../../source/std/reflect.chl#L64)。

**完整示例**：[doc/examples/library/reflect.chl](../../doc/examples/library/reflect.chl)。

<a id="api-4"></a>

### ReflectValue::asChar（5）

**声明**

```chali
Result<char,std::reflect::ReflectError> asChar();
```

**行为**

精确提取 Char 载荷；变体不匹配返回 Err(TypeMismatch)，不进行隐式数值转换。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Result<char,std::reflect::ReflectError>`。精确提取 Char 载荷；变体不匹配返回 Err(TypeMismatch)，不进行隐式数值转换。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.asChar();
```

**源码**：[source/std/reflect.chl:69](../../source/std/reflect.chl#L69)。

**完整示例**：[doc/examples/library/reflect.chl](../../doc/examples/library/reflect.chl)。

<a id="api-5"></a>

### ReflectValue::asI8（6）

**声明**

```chali
Result<int8,std::reflect::ReflectError> asI8();
```

**行为**

精确提取 I8 载荷；变体不匹配返回 Err(TypeMismatch)，不进行隐式数值转换。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Result<int8,std::reflect::ReflectError>`。精确提取 I8 载荷；变体不匹配返回 Err(TypeMismatch)，不进行隐式数值转换。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.asI8();
```

**源码**：[source/std/reflect.chl:74](../../source/std/reflect.chl#L74)。

**完整示例**：[doc/examples/library/reflect.chl](../../doc/examples/library/reflect.chl)。

<a id="api-6"></a>

### ReflectValue::asI16（7）

**声明**

```chali
Result<int16,std::reflect::ReflectError> asI16();
```

**行为**

精确提取 I16 载荷；变体不匹配返回 Err(TypeMismatch)，不进行隐式数值转换。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Result<int16,std::reflect::ReflectError>`。精确提取 I16 载荷；变体不匹配返回 Err(TypeMismatch)，不进行隐式数值转换。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.asI16();
```

**源码**：[source/std/reflect.chl:79](../../source/std/reflect.chl#L79)。

**完整示例**：[doc/examples/library/reflect.chl](../../doc/examples/library/reflect.chl)。

<a id="api-7"></a>

### ReflectValue::asI32（8）

**声明**

```chali
Result<int32,std::reflect::ReflectError> asI32();
```

**行为**

精确提取 I32 载荷；变体不匹配返回 Err(TypeMismatch)，不进行隐式数值转换。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Result<int32,std::reflect::ReflectError>`。精确提取 I32 载荷；变体不匹配返回 Err(TypeMismatch)，不进行隐式数值转换。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.asI32();
```

**源码**：[source/std/reflect.chl:84](../../source/std/reflect.chl#L84)。

**完整示例**：[doc/examples/library/reflect.chl](../../doc/examples/library/reflect.chl)。

<a id="api-8"></a>

### ReflectValue::asI64（9）

**声明**

```chali
Result<int64,std::reflect::ReflectError> asI64();
```

**行为**

精确提取 I64 载荷；变体不匹配返回 Err(TypeMismatch)，不进行隐式数值转换。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Result<int64,std::reflect::ReflectError>`。精确提取 I64 载荷；变体不匹配返回 Err(TypeMismatch)，不进行隐式数值转换。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.asI64();
```

**源码**：[source/std/reflect.chl:89](../../source/std/reflect.chl#L89)。

**完整示例**：[doc/examples/library/reflect.chl](../../doc/examples/library/reflect.chl)。

<a id="api-9"></a>

### ReflectValue::asU8（10）

**声明**

```chali
Result<uint8,std::reflect::ReflectError> asU8();
```

**行为**

精确提取 U8 载荷；变体不匹配返回 Err(TypeMismatch)，不进行隐式数值转换。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Result<uint8,std::reflect::ReflectError>`。精确提取 U8 载荷；变体不匹配返回 Err(TypeMismatch)，不进行隐式数值转换。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.asU8();
```

**源码**：[source/std/reflect.chl:94](../../source/std/reflect.chl#L94)。

**完整示例**：[doc/examples/library/reflect.chl](../../doc/examples/library/reflect.chl)。

<a id="api-10"></a>

### ReflectValue::asU16（11）

**声明**

```chali
Result<uint16,std::reflect::ReflectError> asU16();
```

**行为**

精确提取 U16 载荷；变体不匹配返回 Err(TypeMismatch)，不进行隐式数值转换。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Result<uint16,std::reflect::ReflectError>`。精确提取 U16 载荷；变体不匹配返回 Err(TypeMismatch)，不进行隐式数值转换。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.asU16();
```

**源码**：[source/std/reflect.chl:99](../../source/std/reflect.chl#L99)。

**完整示例**：[doc/examples/library/reflect.chl](../../doc/examples/library/reflect.chl)。

<a id="api-11"></a>

### ReflectValue::asU32（12）

**声明**

```chali
Result<uint32,std::reflect::ReflectError> asU32();
```

**行为**

精确提取 U32 载荷；变体不匹配返回 Err(TypeMismatch)，不进行隐式数值转换。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Result<uint32,std::reflect::ReflectError>`。精确提取 U32 载荷；变体不匹配返回 Err(TypeMismatch)，不进行隐式数值转换。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.asU32();
```

**源码**：[source/std/reflect.chl:104](../../source/std/reflect.chl#L104)。

**完整示例**：[doc/examples/library/reflect.chl](../../doc/examples/library/reflect.chl)。

<a id="api-12"></a>

### ReflectValue::asU64（13）

**声明**

```chali
Result<uint64,std::reflect::ReflectError> asU64();
```

**行为**

精确提取 U64 载荷；变体不匹配返回 Err(TypeMismatch)，不进行隐式数值转换。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Result<uint64,std::reflect::ReflectError>`。精确提取 U64 载荷；变体不匹配返回 Err(TypeMismatch)，不进行隐式数值转换。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.asU64();
```

**源码**：[source/std/reflect.chl:109](../../source/std/reflect.chl#L109)。

**完整示例**：[doc/examples/library/reflect.chl](../../doc/examples/library/reflect.chl)。

<a id="api-13"></a>

### ReflectValue::asF32（14）

**声明**

```chali
Result<float32,std::reflect::ReflectError> asF32();
```

**行为**

精确提取 F32 载荷；变体不匹配返回 Err(TypeMismatch)，不进行隐式数值转换。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Result<float32,std::reflect::ReflectError>`。精确提取 F32 载荷；变体不匹配返回 Err(TypeMismatch)，不进行隐式数值转换。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.asF32();
```

**源码**：[source/std/reflect.chl:114](../../source/std/reflect.chl#L114)。

**完整示例**：[doc/examples/library/reflect.chl](../../doc/examples/library/reflect.chl)。

<a id="api-14"></a>

### ReflectValue::asF64（15）

**声明**

```chali
Result<float64,std::reflect::ReflectError> asF64();
```

**行为**

精确提取 F64 载荷；变体不匹配返回 Err(TypeMismatch)，不进行隐式数值转换。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Result<float64,std::reflect::ReflectError>`。精确提取 F64 载荷；变体不匹配返回 Err(TypeMismatch)，不进行隐式数值转换。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.asF64();
```

**源码**：[source/std/reflect.chl:119](../../source/std/reflect.chl#L119)。

**完整示例**：[doc/examples/library/reflect.chl](../../doc/examples/library/reflect.chl)。

<a id="api-15"></a>

### ReflectValue::asText（16）

**声明**

```chali
Result<std::lang::String,std::reflect::ReflectError> asText();
```

**行为**

精确提取 Text 载荷；变体不匹配返回 Err(TypeMismatch)，不进行隐式数值转换。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Result<std::lang::String,std::reflect::ReflectError>`。精确提取 Text 载荷；变体不匹配返回 Err(TypeMismatch)，不进行隐式数值转换。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.asText();
```

**源码**：[source/std/reflect.chl:124](../../source/std/reflect.chl#L124)。

**完整示例**：[doc/examples/library/reflect.chl](../../doc/examples/library/reflect.chl)。

<a id="api-16"></a>

### getFuncInfo（17）

**声明**

```chali
Option<std::reflect::FuncInfo> getFuncInfo(std::lang::String name);
```

**行为**

按名称查找函数元数据，未找到时 None。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `name` | `std::lang::String name` | 名称 | 无 |

**返回值**

`Option<std::reflect::FuncInfo>`。按名称查找函数元数据，未找到时 None。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::reflect::getFuncInfo(name);
```

**源码**：[source/std/reflect.chl:149](../../source/std/reflect.chl#L149)。

**完整示例**：[doc/examples/library/reflect.chl](../../doc/examples/library/reflect.chl)。

<a id="api-17"></a>

### getAnnotationInfo（18）

**声明**

```chali
Option<std::reflect::AnnotationInfo> getAnnotationInfo(std::lang::String name);
```

**行为**

按名称查找注解定义元数据。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `name` | `std::lang::String name` | 名称 | 无 |

**返回值**

`Option<std::reflect::AnnotationInfo>`。按名称查找注解定义元数据。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::reflect::getAnnotationInfo(name);
```

**源码**：[source/std/reflect.chl:150](../../source/std/reflect.chl#L150)。

**完整示例**：[doc/examples/library/reflect.chl](../../doc/examples/library/reflect.chl)。

<a id="api-18"></a>

### getTypeId（19）

**声明**

```chali
uint64 getTypeId<T>();
```

**行为**

取得具体 T 的类型 ID。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`uint64`。取得具体 T 的类型 ID。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::reflect::getTypeId<T>();
```

**源码**：[source/std/reflect.chl:152](../../source/std/reflect.chl#L152)。

**完整示例**：[doc/examples/library/reflect.chl](../../doc/examples/library/reflect.chl)。

<a id="api-19"></a>

### getInterfaceId（20）

**声明**

```chali
uint64 getInterfaceId<I>();
```

**行为**

取得接口 I 的类型 ID。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`uint64`。取得接口 I 的类型 ID。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::reflect::getInterfaceId<I>();
```

**源码**：[source/std/reflect.chl:153](../../source/std/reflect.chl#L153)。

**完整示例**：[doc/examples/library/reflect.chl](../../doc/examples/library/reflect.chl)。

<a id="api-20"></a>

### getStructInfo（21）

**声明**

```chali
Option<std::reflect::StructInfo> getStructInfo<T>();
```

**行为**

读取具体结构体的布局和成员元数据。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Option<std::reflect::StructInfo>`。读取具体结构体的布局和成员元数据。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::reflect::getStructInfo<T>();
```

**源码**：[source/std/reflect.chl:154](../../source/std/reflect.chl#L154)。

**完整示例**：[doc/examples/library/reflect.chl](../../doc/examples/library/reflect.chl)。

<a id="api-21"></a>

### getEnumInfo（22）

**声明**

```chali
Option<std::reflect::EnumInfo> getEnumInfo<T>();
```

**行为**

读取枚举变体及成员元数据。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Option<std::reflect::EnumInfo>`。读取枚举变体及成员元数据。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::reflect::getEnumInfo<T>();
```

**源码**：[source/std/reflect.chl:155](../../source/std/reflect.chl#L155)。

**完整示例**：[doc/examples/library/reflect.chl](../../doc/examples/library/reflect.chl)。

<a id="api-22"></a>

### getInterfaceInfo（23）

**声明**

```chali
Option<std::reflect::InterfaceInfo> getInterfaceInfo<I>();
```

**行为**

读取接口方法及实现关系元数据。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Option<std::reflect::InterfaceInfo>`。读取接口方法及实现关系元数据。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::reflect::getInterfaceInfo<I>();
```

**源码**：[source/std/reflect.chl:156](../../source/std/reflect.chl#L156)。

**完整示例**：[doc/examples/library/reflect.chl](../../doc/examples/library/reflect.chl)。

<a id="api-23"></a>

### implementsInterface（24）

**声明**

```chali
bool implementsInterface<T>(uint64 interfaceId);
```

**行为**

判断 T 是否实现给定接口 ID。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `interfaceId` | `uint64 interfaceId` | 接口类型 ID | 无 |

**返回值**

`bool`。判断 T 是否实现给定接口 ID。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::reflect::implementsInterface<T>(interfaceId);
```

**源码**：[source/std/reflect.chl:157](../../source/std/reflect.chl#L157)。

**完整示例**：[doc/examples/library/reflect.chl](../../doc/examples/library/reflect.chl)。

<a id="api-24"></a>

### getAnnotations（25）

**声明**

```chali
List<std::reflect::Annotation> getAnnotations<T>();
```

**行为**

取得类型上的注解列表。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`List<std::reflect::Annotation>`。取得类型上的注解列表。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::reflect::getAnnotations<T>();
```

**源码**：[source/std/reflect.chl:164](../../source/std/reflect.chl#L164)。

**完整示例**：[doc/examples/library/reflect.chl](../../doc/examples/library/reflect.chl)。

<a id="api-25"></a>

### getFieldAnnotations（26）

**声明**

```chali
List<std::reflect::Annotation> getFieldAnnotations<T>(std::lang::String name);
```

**行为**

按字段名取得字段注解。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `name` | `std::lang::String name` | 名称 | 无 |

**返回值**

`List<std::reflect::Annotation>`。按字段名取得字段注解。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::reflect::getFieldAnnotations<T>(name);
```

**源码**：[source/std/reflect.chl:170](../../source/std/reflect.chl#L170)。

**完整示例**：[doc/examples/library/reflect.chl](../../doc/examples/library/reflect.chl)。

<a id="api-26"></a>

### getFuncAnnotations（27）

**声明**

```chali
List<std::reflect::Annotation> getFuncAnnotations(std::lang::String name);
```

**行为**

按函数名取得函数注解。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `name` | `std::lang::String name` | 名称 | 无 |

**返回值**

`List<std::reflect::Annotation>`。按函数名取得函数注解。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::reflect::getFuncAnnotations(name);
```

**源码**：[source/std/reflect.chl:175](../../source/std/reflect.chl#L175)。

**完整示例**：[doc/examples/library/reflect.chl](../../doc/examples/library/reflect.chl)。

<a id="api-27"></a>

### getMethodAnnotations（28）

**声明**

```chali
List<std::reflect::Annotation> getMethodAnnotations<T>(std::lang::String name);
```

**行为**

按方法名取得方法注解。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `name` | `std::lang::String name` | 名称 | 无 |

**返回值**

`List<std::reflect::Annotation>`。按方法名取得方法注解。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::reflect::getMethodAnnotations<T>(name);
```

**源码**：[source/std/reflect.chl:185](../../source/std/reflect.chl#L185)。

**完整示例**：[doc/examples/library/reflect.chl](../../doc/examples/library/reflect.chl)。

<a id="api-28"></a>

### getStaticMemberAnnotations（29）

**声明**

```chali
List<std::reflect::Annotation> getStaticMemberAnnotations<T>(std::lang::String name);
```

**行为**

按静态成员名取得注解。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `name` | `std::lang::String name` | 名称 | 无 |

**返回值**

`List<std::reflect::Annotation>`。按静态成员名取得注解。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::reflect::getStaticMemberAnnotations<T>(name);
```

**源码**：[source/std/reflect.chl:189](../../source/std/reflect.chl#L189)。

**完整示例**：[doc/examples/library/reflect.chl](../../doc/examples/library/reflect.chl)。

<a id="api-29"></a>

### call（30）

**声明**

```chali
Result<std::reflect::ReflectValue,std::reflect::ReflectError> call(std::lang::String name, List<std::reflect::ReflectValue> args);
```

**行为**

按名称动态调用函数，返回封闭值或 ReflectError。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `name` | `std::lang::String name` | 名称 | 无 |
| `args` | `List<std::reflect::ReflectValue> args` | 与目标参数数量、位宽和类型匹配的封闭反射值列表 | 无 |

**返回值**

`Result<std::reflect::ReflectValue,std::reflect::ReflectError>`。按名称动态调用函数，返回封闭值或 ReflectError。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::reflect::call(name, args);
```

**源码**：[source/std/reflect.chl:198](../../source/std/reflect.chl#L198)。

**完整示例**：[doc/examples/library/reflect.chl](../../doc/examples/library/reflect.chl)。

<a id="api-30"></a>

### callMethod（31）

**声明**

```chali
Result<std::reflect::ReflectValue,std::reflect::ReflectError> callMethod<T>(T mut& obj, std::lang::String name, List<std::reflect::ReflectValue> args);
```

**行为**

通过可写接收者动态调用实例方法。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `obj` | `T mut& obj` | 实例接收者，其引用可写性须匹配方法 | 无 |
| `name` | `std::lang::String name` | 名称 | 无 |
| `args` | `List<std::reflect::ReflectValue> args` | 与目标参数数量、位宽和类型匹配的封闭反射值列表 | 无 |

**返回值**

`Result<std::reflect::ReflectValue,std::reflect::ReflectError>`。通过可写接收者动态调用实例方法。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::reflect::callMethod<T>(obj, name, args);
```

**源码**：[source/std/reflect.chl:199](../../source/std/reflect.chl#L199)。

**完整示例**：[doc/examples/library/reflect.chl](../../doc/examples/library/reflect.chl)。

<a id="api-31"></a>

### callMethodConst（32）

**声明**

```chali
Result<std::reflect::ReflectValue,std::reflect::ReflectError> callMethodConst<T>(T const& obj, std::lang::String name, List<std::reflect::ReflectValue> args);
```

**行为**

通过只读接收者动态调用允许的实例方法。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `obj` | `T const& obj` | 实例接收者，其引用可写性须匹配方法 | 无 |
| `name` | `std::lang::String name` | 名称 | 无 |
| `args` | `List<std::reflect::ReflectValue> args` | 与目标参数数量、位宽和类型匹配的封闭反射值列表 | 无 |

**返回值**

`Result<std::reflect::ReflectValue,std::reflect::ReflectError>`。通过只读接收者动态调用允许的实例方法。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::reflect::callMethodConst<T>(obj, name, args);
```

**源码**：[source/std/reflect.chl:200](../../source/std/reflect.chl#L200)。

**完整示例**：[doc/examples/library/reflect.chl](../../doc/examples/library/reflect.chl)。

<a id="api-32"></a>

### callStatic（33）

**声明**

```chali
Result<std::reflect::ReflectValue,std::reflect::ReflectError> callStatic<T>(std::lang::String name, List<std::reflect::ReflectValue> args);
```

**行为**

动态调用 T 的静态方法。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `name` | `std::lang::String name` | 名称 | 无 |
| `args` | `List<std::reflect::ReflectValue> args` | 与目标参数数量、位宽和类型匹配的封闭反射值列表 | 无 |

**返回值**

`Result<std::reflect::ReflectValue,std::reflect::ReflectError>`。动态调用 T 的静态方法。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::reflect::callStatic<T>(name, args);
```

**源码**：[source/std/reflect.chl:201](../../source/std/reflect.chl#L201)。

**完整示例**：[doc/examples/library/reflect.chl](../../doc/examples/library/reflect.chl)。

<a id="api-33"></a>

### callEnumMethod（34）

**声明**

```chali
Result<std::reflect::ReflectValue,std::reflect::ReflectError> callEnumMethod<T>(T mut& obj, std::lang::String name, List<std::reflect::ReflectValue> args);
```

**行为**

通过可写枚举接收者动态调用方法。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `obj` | `T mut& obj` | 实例接收者，其引用可写性须匹配方法 | 无 |
| `name` | `std::lang::String name` | 名称 | 无 |
| `args` | `List<std::reflect::ReflectValue> args` | 与目标参数数量、位宽和类型匹配的封闭反射值列表 | 无 |

**返回值**

`Result<std::reflect::ReflectValue,std::reflect::ReflectError>`。通过可写枚举接收者动态调用方法。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::reflect::callEnumMethod<T>(obj, name, args);
```

**源码**：[source/std/reflect.chl:202](../../source/std/reflect.chl#L202)。

**完整示例**：[doc/examples/library/reflect.chl](../../doc/examples/library/reflect.chl)。

<a id="api-34"></a>

### callEnumMethodConst（35）

**声明**

```chali
Result<std::reflect::ReflectValue,std::reflect::ReflectError> callEnumMethodConst<T>(T const& obj, std::lang::String name, List<std::reflect::ReflectValue> args);
```

**行为**

通过只读枚举接收者动态调用方法。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `obj` | `T const& obj` | 实例接收者，其引用可写性须匹配方法 | 无 |
| `name` | `std::lang::String name` | 名称 | 无 |
| `args` | `List<std::reflect::ReflectValue> args` | 与目标参数数量、位宽和类型匹配的封闭反射值列表 | 无 |

**返回值**

`Result<std::reflect::ReflectValue,std::reflect::ReflectError>`。通过只读枚举接收者动态调用方法。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::reflect::callEnumMethodConst<T>(obj, name, args);
```

**源码**：[source/std/reflect.chl:203](../../source/std/reflect.chl#L203)。

**完整示例**：[doc/examples/library/reflect.chl](../../doc/examples/library/reflect.chl)。

<a id="api-35"></a>

### callEnumStatic（36）

**声明**

```chali
Result<std::reflect::ReflectValue,std::reflect::ReflectError> callEnumStatic<T>(std::lang::String name, List<std::reflect::ReflectValue> args);
```

**行为**

动态调用枚举的静态方法。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `name` | `std::lang::String name` | 名称 | 无 |
| `args` | `List<std::reflect::ReflectValue> args` | 与目标参数数量、位宽和类型匹配的封闭反射值列表 | 无 |

**返回值**

`Result<std::reflect::ReflectValue,std::reflect::ReflectError>`。动态调用枚举的静态方法。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::reflect::callEnumStatic<T>(name, args);
```

**源码**：[source/std/reflect.chl:204](../../source/std/reflect.chl#L204)。

**完整示例**：[doc/examples/library/reflect.chl](../../doc/examples/library/reflect.chl)。

<a id="api-36"></a>

### construct（37）

**声明**

```chali
Result<T,std::reflect::ReflectError> construct<T>(List<std::reflect::ReflectValue> args);
```

**行为**

动态调用 T 的构造函数，返回 T 或错误。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `args` | `List<std::reflect::ReflectValue> args` | 与目标参数数量、位宽和类型匹配的封闭反射值列表 | 无 |

**返回值**

`Result<T,std::reflect::ReflectError>`。动态调用 T 的构造函数，返回 T 或错误。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::reflect::construct<T>(args);
```

**源码**：[source/std/reflect.chl:205](../../source/std/reflect.chl#L205)。

**完整示例**：[doc/examples/library/reflect.chl](../../doc/examples/library/reflect.chl)。

<a id="api-37"></a>

### getField（38）

**声明**

```chali
Result<F,std::reflect::ReflectError> getField<T,F>(T const& obj, std::lang::String name);
```

**行为**

按字段名读取精确类型的独立值。

按名称读取字段的独立值；字段类型必须与 F 精确一致，不返回内部引用。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `obj` | `T const& obj` | 实例接收者，其引用可写性须匹配方法 | 无 |
| `name` | `std::lang::String name` | 名称 | 无 |

**返回值**

`Result<F,std::reflect::ReflectError>`。按字段名读取精确类型的独立值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::reflect::getField<T,F>(obj, name);
```

**源码**：[source/std/reflect.chl:207](../../source/std/reflect.chl#L207)。

**完整示例**：[doc/examples/library/reflect.chl](../../doc/examples/library/reflect.chl)。

<a id="api-38"></a>

### setField（39）

**声明**

```chali
Result<bool,std::reflect::ReflectError> setField<T,F>(T mut& obj, std::lang::String name, F value);
```

**行为**

按字段名写入精确类型的值。

按名称写入字段，沿用普通赋值的复制、释放与借用规则。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `obj` | `T mut& obj` | 实例接收者，其引用可写性须匹配方法 | 无 |
| `name` | `std::lang::String name` | 名称 | 无 |
| `value` | `F value` | 输入值或写入的新值 | 无 |

**返回值**

`Result<bool,std::reflect::ReflectError>`。按字段名写入精确类型的值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::reflect::setField<T,F>(obj, name, value);
```

**源码**：[source/std/reflect.chl:209](../../source/std/reflect.chl#L209)。

**完整示例**：[doc/examples/library/reflect.chl](../../doc/examples/library/reflect.chl)。

<a id="api-39"></a>

### callInterface（40）

**声明**

```chali
Result<std::reflect::ReflectValue,std::reflect::ReflectError> callInterface<I>(I mut& obj, std::lang::String name, List<std::reflect::ReflectValue> args);
```

**行为**

通过可写接口接收者动态调用方法。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `obj` | `I mut& obj` | 实例接收者，其引用可写性须匹配方法 | 无 |
| `name` | `std::lang::String name` | 名称 | 无 |
| `args` | `List<std::reflect::ReflectValue> args` | 与目标参数数量、位宽和类型匹配的封闭反射值列表 | 无 |

**返回值**

`Result<std::reflect::ReflectValue,std::reflect::ReflectError>`。通过可写接口接收者动态调用方法。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::reflect::callInterface<I>(obj, name, args);
```

**源码**：[source/std/reflect.chl:210](../../source/std/reflect.chl#L210)。

**完整示例**：[doc/examples/library/reflect.chl](../../doc/examples/library/reflect.chl)。

<a id="api-40"></a>

### callInterfaceConst（41）

**声明**

```chali
Result<std::reflect::ReflectValue,std::reflect::ReflectError> callInterfaceConst<I>(I const& obj, std::lang::String name, List<std::reflect::ReflectValue> args);
```

**行为**

通过只读接口接收者动态调用方法。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `obj` | `I const& obj` | 实例接收者，其引用可写性须匹配方法 | 无 |
| `name` | `std::lang::String name` | 名称 | 无 |
| `args` | `List<std::reflect::ReflectValue> args` | 与目标参数数量、位宽和类型匹配的封闭反射值列表 | 无 |

**返回值**

`Result<std::reflect::ReflectValue,std::reflect::ReflectError>`。通过只读接口接收者动态调用方法。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::reflect::callInterfaceConst<I>(obj, name, args);
```

**源码**：[source/std/reflect.chl:211](../../source/std/reflect.chl#L211)。

**完整示例**：[doc/examples/library/reflect.chl](../../doc/examples/library/reflect.chl)。
