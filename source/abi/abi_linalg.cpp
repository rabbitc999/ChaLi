/** @file abi_linalg.cpp @brief 线性代数 C 帧入口及私有数值内核，结果由 ChaLi 拥有。 */
#include "chali_standard.h"
#include "scalar_storage.hpp"
#include <cmath>
#include <climits>
#include <cstdint>
#include <cstring>
#include <vector>
#include <limits>
#include <memory>
#include <stdexcept>

namespace
{
/** @brief 记录数值算法失败并停止当前内核。
 * @param frame 当前 C 帧。
 * @param message 原始错误。
 */
[[noreturn]] void fail(ChaliAbiFrame *frame, const char *message)
{
    chali_abi_fail(frame, message);
    throw std::runtime_error(message);
}
/** @brief 从受检查逻辑地址读取一个小端八字节槽。
 * @param frame 当前 C 帧。
 * @param address 聚合对象地址。
 * @param slot 槽序号。
 * @return 整数位模式。
 */
int64_t readSlot(ChaliAbiFrame *frame, int64_t address, size_t slot)
{
    if (address < 0 || slot > size_t(INT64_MAX / 8) || address > INT64_MAX - int64_t(slot * 8))
        fail(frame, "矩阵输入槽地址溢出");
    unsigned char bytes[8];
    if (!chali_abi_read(frame, address + int64_t(slot * 8), bytes, 8)) fail(frame, "矩阵输入槽不可读");
    uint64_t value = 0;
    for (unsigned i = 0; i < 8; ++i) value |= uint64_t(bytes[i]) << (8 * i);
    return int64_t(value);
}
/** @brief 向受检查逻辑地址写入一个小端八字节槽。
 * @param frame 当前 C 帧。
 * @param address 聚合对象地址。
 * @param slot 槽序号。
 * @param value 整数位模式。
 */
void writeSlot(ChaliAbiFrame *frame, int64_t address, size_t slot, int64_t value)
{
    if (address < 0 || slot > size_t(INT64_MAX / 8) || address > INT64_MAX - int64_t(slot * 8))
        fail(frame, "矩阵输出槽地址溢出");
    unsigned char bytes[8];
    for (unsigned i = 0; i < 8; ++i) bytes[i] = unsigned(uint64_t(value) >> (8 * i));
    if (!chali_abi_write(frame, address + int64_t(slot * 8), bytes, 8)) fail(frame, "矩阵输出槽不可写");
}
/** @brief 复制只读标量记录为算法工作数组。
 * @tparam Scalar 工作精度。
 * @param frame 当前 C 帧。
 * @param handle 存储句柄，零代表空数组。
 * @return 独立工作数组。
 */
template <class Scalar>
Memory::Vector<Scalar> readValues(ChaliAbiFrame *frame, int64_t handle)
{
    Memory::Vector<Scalar> result;
    if (!handle) return result;
    auto *storage = static_cast<scalar_buffer::Storage *>(chali_abi_record_probe(frame, handle, "std::linear_algebra::ScalarStorage"));
    if (!storage) fail(frame, "线性代数存储 opcode 已释放或类型不匹配");
    result.reserve(storage->values.size());
    for (const auto &slot : storage->values) result.push_back(scalar_buffer::decode<Scalar>(slot));
    return result;
}
/** @brief 向语言已初始化的拥有字段发布独立数值存储，失败时清理未发布资源。
 * @tparam Values 数值数组类型。
 * @param frame 当前 C 帧。
 * @param output 语言结果对象地址。
 * @param slot 拥有字段的槽序号。
 * @param values 待复制数值。
 */
template <class Values>
void writeOwnedValues(ChaliAbiFrame *frame, int64_t output, size_t slot, const Values &values)
{
    const auto previous = readSlot(frame, output, slot);
    if (previous && !chali_abi_record_probe(frame, previous, "std::linear_algebra::ScalarStorage")) fail(frame, "矩阵输出存储类型不匹配");
    auto storage = Memory::makeCHeapUnique<scalar_buffer::Storage>();
    storage->values.reserve(values.size());
    for (auto value : values) storage->values.push_back(scalar_buffer::encode(value));
    const auto handle = scalar_buffer::publish(frame, std::move(storage));
    if (!handle) fail(frame, "矩阵结果存储创建失败");
    try
    {
        writeSlot(frame, output, slot, handle);
    } catch (...)
    {
        chali_abi_record_release(frame, handle);
        throw;
    }
    if (previous) chali_abi_record_release(frame, previous);
}
struct MatrixData
{
    int rows = 0;
    int cols = 0;
    Memory::Vector<double> values;
};
struct VectorData
{
    Memory::Vector<double> values;
};
struct LUResult
{
    MatrixData lower, upper, permutation;
};
struct EigenResult
{
    VectorData eigenvalues;
    MatrixData eigenvectors;
};
struct SvdResult
{
    MatrixData u;
    VectorData singularValues;
    MatrixData vTranspose;
};
struct JordanResult
{
    MatrixData basis;
    MatrixData jordan;
};

/** @brief 从语言矩阵复制经过形状校验的私有数值数据。
 * @param frame 当前 C 帧。
 * @param input 输入矩阵地址。
 * @return 独立矩阵数据。
 */
MatrixData readMatrix(ChaliAbiFrame *frame, int64_t input)
{
    const auto rows = readSlot(frame, input, 0), cols = readSlot(frame, input, 1);
    const auto values = readValues<double>(frame, readSlot(frame, input, 2));
    if (rows < 0 || cols < 0 || rows > INT_MAX || cols > INT_MAX || (cols == 0 ? !values.empty() : (values.size() / uint64_t(cols) != uint64_t(rows) || values.size() % uint64_t(cols))))
        fail(frame, "矩阵存储长度与维度不匹配");
    return { int(rows), int(cols), Memory::Vector<double>(values.begin(), values.end()) };
}
/** @brief 将矩阵写入语言拥有的结果字段。
 * @param frame 当前 C 帧。
 * @param output 输出对象地址。
 * @param base 矩阵起始槽。
 * @param value 已计算矩阵。
 */
void writeMatrixInto(ChaliAbiFrame *frame, int64_t output, size_t base, const MatrixData &value)
{
    writeSlot(frame, output, base, value.rows);
    writeSlot(frame, output, base + 1, value.cols);
    writeOwnedValues(frame, output, base + 2, value.values);
}
/** @brief 将向量写入语言拥有的结果字段。
 * @param frame 当前 C 帧。
 * @param output 输出对象地址。
 * @param base 向量起始槽。
 * @param value 已计算向量。
 */
void writeVectorInto(ChaliAbiFrame *frame, int64_t output, size_t base, const VectorData &value)
{ writeOwnedValues(frame, output, base, value.values); }
/**
 * @brief 读取已验证行列范围内的矩阵元素。
 * @param m 作为输入或输出的行优先矩阵。
 * @param i 从零开始的行下标。
 * @param j 从零开始的列下标。
 * @return 对应行列的双精度数值。
 */
inline double at(const MatrixData &m, int i, int j) { return m.values[static_cast<std::size_t>(i) * static_cast<std::size_t>(m.cols) + static_cast<std::size_t>(j)]; }
/**
 * @brief 写入已验证行列范围内的矩阵元素。
 * @param m 作为输入或输出的行优先矩阵。
 * @param i 从零开始的行下标。
 * @param j 从零开始的列下标。
 * @param v 写入指定矩阵元素的数值。
 */
inline void set(MatrixData &m, int i, int j, double v) { m.values[static_cast<std::size_t>(i) * static_cast<std::size_t>(m.cols) + static_cast<std::size_t>(j)] = v; }
/**
 * @brief 创建按行优先保存的全零矩阵。
 * @param rows 输出矩阵的行数。
 * @param cols 输出矩阵的列数。
 * @return 具有指定行列数且独占元素数组的矩阵。
 */
inline MatrixData zerosMatrix(int rows, int cols) { return { rows, cols, Memory::Vector<double>(static_cast<std::size_t>(rows) * static_cast<std::size_t>(cols), 0.0) }; }
/**
 * @brief 创建指定长度的全零向量。
 * @param n 输出向量长度或方阵阶数。
 * @return 具有独立元素数组的零向量。
 */
inline VectorData zerosVector(int n) { return { Memory::Vector<double>(static_cast<std::size_t>(n), 0.0) }; }
/**
 * @brief 创建指定阶数的单位矩阵。
 * @param n 输出向量长度或方阵阶数。
 * @return 主对角线为一、其余元素为零的方阵。
 */
inline MatrixData identityMatrix(int n)
{
    MatrixData m = zerosMatrix(n, n);
    for (int i = 0; i < n; ++i) set(m, i, i, 1.0);
    return m;
}
/**
 * @brief 交换矩阵行列并复制元素。
 * @param m 作为输入或输出的行优先矩阵。
 * @return 与输入存储独立的转置矩阵。
 */
inline MatrixData transpose(const MatrixData &m)
{
    MatrixData r = zerosMatrix(m.cols, m.rows);
    for (int i = 0; i < m.rows; ++i)
        for (int j = 0; j < m.cols; ++j) set(r, j, i, at(m, i, j));
    return r;
}
/**
 * @brief 按行优先布局计算矩阵乘积。
 * @param a 运算左侧的输入矩阵或复数数组。
 * @param b 运算右侧的输入矩阵。
 * @return 行数取自左矩阵、列数取自右矩阵的乘积。
 */
inline MatrixData multiply(const MatrixData &a, const MatrixData &b)
{
    MatrixData r = zerosMatrix(a.rows, b.cols);
    for (int i = 0; i < a.rows; ++i)
        for (int j = 0; j < b.cols; ++j)
        {
            double sum = 0.0;
            for (int k = 0; k < a.cols; ++k) sum += at(a, i, k) * at(b, k, j);
            set(r, i, j, sum);
        }
    return r;
}

/* ---- 6 个顶层算法 ---- */
/**
 * @brief 执行带主元选择的 LU 分解并构造排列信息。
 * @param frame 提供内存访问、资源创建和诊断的当前调用上下文。
 * @param input 需要分解、求解或取列的输入矩阵。
 * @return 分解所得上下三角矩阵及排列结果。
 */
inline LUResult luDecomposeImpl(ChaliAbiFrame *frame, const MatrixData &input)
{
    LUResult bad;
    if (input.rows != input.cols)
    {
        fail(frame, "LU 分解要求方阵");
        return bad;
    }
    const int n = input.rows;
    MatrixData lower = identityMatrix(n);
    MatrixData upper = input;
    MatrixData permutation = identityMatrix(n);
    for (int k = 0; k < n; ++k)
    {
        int pivot = k;
        double largest = std::fabs(at(upper, k, k));
        for (int row = k + 1; row < n; ++row)
        {
            const double candidate = std::fabs(at(upper, row, k));
            if (candidate > largest)
            {
                largest = candidate;
                pivot = row;
            }
        }
        if (!(largest > 0.000000000001))
        {
            fail(frame, "LU 分解遇到奇异或近奇异矩阵");
            return bad;
        }
        if (pivot != k)
        {
            for (int col = 0; col < n; ++col)
            {
                const double tmp = at(upper, k, col);
                set(upper, k, col, at(upper, pivot, col));
                set(upper, pivot, col, tmp);
            }
            for (int col = 0; col < n; ++col)
            {
                const double tmp = at(permutation, k, col);
                set(permutation, k, col, at(permutation, pivot, col));
                set(permutation, pivot, col, tmp);
            }
            for (int col = 0; col < k; ++col)
            {
                const double tmp = at(lower, k, col);
                set(lower, k, col, at(lower, pivot, col));
                set(lower, pivot, col, tmp);
            }
        }
        for (int row = k + 1; row < n; ++row)
        {
            const double factor = at(upper, row, k) / at(upper, k, k);
            set(lower, row, k, factor);
            set(upper, row, k, 0.0);
            for (int col = k + 1; col < n; ++col)
                set(upper, row, col, at(upper, row, col) - factor * at(upper, k, col));
        }
    }
    return { lower, upper, permutation };
}

/**
 * @brief 对实对称矩阵迭代求特征值和正交特征向量。
 * @param frame 提供内存访问、资源创建和诊断的当前调用上下文。
 * @param input 需要分解、求解或取列的输入矩阵。
 * @return 特征值对角矩阵及对应特征向量矩阵。
 */
inline EigenResult symmetricEigenImpl(ChaliAbiFrame *frame, const MatrixData &input)
{
    EigenResult bad;
    if (!(input.rows == input.cols && input.rows > 0))
    {
        fail(frame, "特征分解要求非空方阵");
        return bad;
    }
    const int n = input.rows;
    for (int i = 0; i < n; ++i)
        for (int j = i + 1; j < n; ++j)
            if (!(std::fabs(at(input, i, j) - at(input, j, i)) < 0.00000001))
            {
                fail(frame, "symmetricEigen 要求对称矩阵");
                return bad;
            }
    MatrixData a = input;
    MatrixData v = identityMatrix(n);
    int iteration = 0;
    while (iteration < 64 * n * n)
    {
        int p = 0, q = 0;
        double largest = 0.0;
        for (int i = 0; i < n; ++i)
            for (int j = i + 1; j < n; ++j)
            {
                const double magnitude = std::fabs(at(a, i, j));
                if (magnitude > largest)
                {
                    largest = magnitude;
                    p = i;
                    q = j;
                }
            }
        if (largest < 0.0000000001) break;
        const double apq = at(a, p, q);
        const double tau = (at(a, q, q) - at(a, p, p)) / (2.0 * apq);
        double t = 1.0 / (std::fabs(tau) + std::sqrt(1.0 + tau * tau));
        if (tau < 0.0) t = -t;
        const double c = 1.0 / std::sqrt(1.0 + t * t);
        const double s = t * c;
        const double app = at(a, p, p), aqq = at(a, q, q);
        set(a, p, p, app - t * apq);
        set(a, q, q, aqq + t * apq);
        set(a, p, q, 0.0);
        set(a, q, p, 0.0);
        for (int i = 0; i < n; ++i)
        {
            if (i != p && i != q)
            {
                const double aip = at(a, i, p), aiq = at(a, i, q);
                set(a, i, p, c * aip - s * aiq);
                set(a, p, i, c * aip - s * aiq);
                set(a, i, q, s * aip + c * aiq);
                set(a, q, i, s * aip + c * aiq);
            }
            const double vip = at(v, i, p), viq = at(v, i, q);
            set(v, i, p, c * vip - s * viq);
            set(v, i, q, s * vip + c * viq);
        }
        ++iteration;
    }
    if (!(iteration < 64 * n * n))
    {
        fail(frame, "Jacobi 特征分解未收敛");
        return bad;
    }
    VectorData eigenvalues = zerosVector(n);
    MatrixData eigenvectors = v;
    for (int i = 0; i < n; ++i) eigenvalues.values[static_cast<std::size_t>(i)] = at(a, i, i);
    for (int i = 0; i < n; ++i)
    {
        int best = i;
        for (int j = i + 1; j < n; ++j)
            if (eigenvalues.values[static_cast<std::size_t>(j)] > eigenvalues.values[static_cast<std::size_t>(best)])
                best = j;
        if (best != i)
        {
            std::swap(eigenvalues.values[static_cast<std::size_t>(i)],
                      eigenvalues.values[static_cast<std::size_t>(best)]);
            for (int row = 0; row < n; ++row)
                std::swap(eigenvectors.values[static_cast<std::size_t>(row * n + i)],
                          eigenvectors.values[static_cast<std::size_t>(row * n + best)]);
        }
    }
    return { eigenvalues, eigenvectors };
}

/**
 * @brief 通过对称特征分解计算实矩阵的奇异值分解。
 * @param frame 提供内存访问、资源创建和诊断的当前调用上下文。
 * @param input 需要分解、求解或取列的输入矩阵。
 * @return 左右奇异向量及奇异值矩阵。
 */
inline SvdResult svdImpl(ChaliAbiFrame *frame, const MatrixData &input)
{
    SvdResult bad;
    if (!(input.rows > 0 && input.cols > 0))
    {
        fail(frame, "SVD 要求非空矩阵");
        return bad;
    }
    const int m = input.rows, n = input.cols;
    const int r = m < n ? m : n;
    const MatrixData inputTranspose = transpose(input);
    const MatrixData gram = multiply(inputTranspose, input);
    const EigenResult eigen = symmetricEigenImpl(frame, gram);
    if (eigen.eigenvalues.values.empty()) return bad; /* C 帧已记录失败 */
    MatrixData u = zerosMatrix(m, r);
    VectorData singularValues = zerosVector(r);
    MatrixData vTranspose = zerosMatrix(r, n);
    for (int j = 0; j < r; ++j)
    {
        double value = eigen.eigenvalues.values[static_cast<std::size_t>(j)];
        if (value < 0.0) value = 0.0;
        const double sigma = std::sqrt(value);
        singularValues.values[static_cast<std::size_t>(j)] = sigma;
        for (int col = 0; col < n; ++col)
            set(vTranspose, j, col, at(eigen.eigenvectors, col, j));
        if (sigma > 0.0000000001)
        {
            for (int row = 0; row < m; ++row)
            {
                double sum = 0.0;
                for (int col = 0; col < n; ++col)
                    sum += at(input, row, col) * at(eigen.eigenvectors, col, j);
                set(u, row, j, sum / sigma);
            }
        }
        else
        {
            int basis = 0;
            while (basis < m)
            {
                for (int row = 0; row < m; ++row)
                    set(u, row, j, row == basis ? 1.0 : 0.0);
                for (int prev = 0; prev < j; ++prev)
                {
                    const double projection = at(u, basis, prev);
                    for (int row = 0; row < m; ++row)
                        set(u, row, j, at(u, row, j) - projection * at(u, row, prev));
                }
                double length = 0.0;
                for (int row = 0; row < m; ++row) length += at(u, row, j) * at(u, row, j);
                if (length > 0.000000000001)
                {
                    length = std::sqrt(length);
                    for (int row = 0; row < m; ++row) set(u, row, j, at(u, row, j) / length);
                    break;
                }
                ++basis;
            }
        }
    }
    return { u, singularValues, vTranspose };
}

/**
 * @brief 按当前支持的阶数计算实 Jordan 分解。
 * @param frame 提供内存访问、资源创建和诊断的当前调用上下文。
 * @param input 需要分解、求解或取列的输入矩阵。
 * @return 基矩阵及 Jordan 形；不支持的输入通过上下文诊断。
 */
inline JordanResult jordan2Impl(ChaliAbiFrame *frame, const MatrixData &input)
{
    JordanResult bad;
    if (!(input.rows == 2 && input.cols == 2))
    {
        fail(frame, "jordan2 只支持 2x2 矩阵");
        return bad;
    }
    const double a = at(input, 0, 0), b = at(input, 0, 1);
    const double c = at(input, 1, 0), d = at(input, 1, 1);
    double discriminant = (a - d) * (a - d) + 4.0 * b * c;
    if (!(discriminant >= -0.000000000001))
    {
        fail(frame, "jordan2 不支持复特征值");
        return bad;
    }
    if (discriminant < 0.0) discriminant = 0.0;
    const double root = std::sqrt(discriminant);
    const double first = (a + d + root) / 2.0, second = (a + d - root) / 2.0;
    MatrixData basis = identityMatrix(2);
    MatrixData jordan = zerosMatrix(2, 2);
    set(jordan, 0, 0, first);
    set(jordan, 1, 1, second);
    if (root > 0.00000001)
    {
        double x0 = b, y0 = first - a, x1 = b, y1 = second - a;
        if (std::fabs(x0) + std::fabs(y0) < 0.00000001)
        {
            x0 = first - d;
            y0 = c;
        }
        if (std::fabs(x1) + std::fabs(y1) < 0.00000001)
        {
            x1 = second - d;
            y1 = c;
        }
        set(basis, 0, 0, x0);
        set(basis, 1, 0, y0);
        set(basis, 0, 1, x1);
        set(basis, 1, 1, y1);
    }
    else if (std::fabs(a - first) + std::fabs(b) + std::fabs(c) + std::fabs(d - first) > 0.00000001)
    {
        set(jordan, 0, 1, 1.0);
        if (std::fabs(b) > 0.00000001)
        {
            set(basis, 0, 0, b);
            set(basis, 1, 0, d - first);
            set(basis, 0, 1, 0.0);
            set(basis, 1, 1, 1.0);
        }
        else
        {
            set(basis, 0, 0, a - first);
            set(basis, 1, 0, c);
            set(basis, 0, 1, 1.0);
            set(basis, 1, 1, 0.0);
        }
    }
    return { basis, jordan };
}

/**
 * @brief 计算消元差值，浮点逐步舍入，整数按位宽回绕，避免 C++ 有符号溢出。
 * @param value 被减数。
 * @param factor 乘数。
 * @param other 被乘数。
 * @return value - factor * other。
 */
template <typename Scalar>
inline Scalar subtractProduct(Scalar value, Scalar factor, Scalar other)
{
    if constexpr (std::is_integral_v<Scalar>)
    {
        using Unsigned = std::make_unsigned_t<Scalar>;
        const Unsigned bits = static_cast<Unsigned>(static_cast<std::uint64_t>(value) - static_cast<std::uint64_t>(factor) * static_cast<std::uint64_t>(other));
        Scalar result;
        std::memcpy(&result, &bits, sizeof(result));
        return result;
    }
    else
    {
        const Scalar product = static_cast<Scalar>(factor * other);
        return static_cast<Scalar>(value - product);
    }
}

/**
 * @brief 标量除法，整数最小值除以负一时按原位宽回绕。
 * @param numerator 分子。
 * @param denominator 已检查非零的主元。
 * @return 标量商。
 */
template <typename Scalar>
inline Scalar divideScalar(Scalar numerator, Scalar denominator)
{
    if constexpr (std::is_integral_v<Scalar> && std::is_signed_v<Scalar>)
        if (numerator == std::numeric_limits<Scalar>::min() && denominator == -1)
            return numerator;
    return static_cast<Scalar>(numerator / denominator);
}

/**
 * @brief 按原标量精度进行部分主元 LU 求解，不修改输入矩阵与右端向量。
 * @param frame 当前运行时上下文。
 * @param input 行优先矩阵视图。
 * @param rhs 右端向量视图。
 * @param output 语言拥有的已初始化结果向量地址。
 */
template <typename Scalar>
inline void matrixSolve(ChaliAbiFrame *frame, std::int64_t input, std::int64_t rhs, std::int64_t output)
{
    const auto rows = readSlot(frame, input, 0);
    const auto cols = readSlot(frame, input, 1);
    auto lu = readValues<Scalar>(frame, readSlot(frame, input, 2));
    auto b = readValues<Scalar>(frame, readSlot(frame, rhs, 0));
    if (rows < 0 || rows != cols || static_cast<std::uint64_t>(rows) != b.size() || (rows != 0 && (lu.size() / static_cast<std::uint64_t>(rows) != static_cast<std::uint64_t>(rows) || lu.size() % static_cast<std::uint64_t>(rows) != 0)) || (rows == 0 && !lu.empty()))
    {
        fail(frame, "Matrix.solve 维度不匹配");
        return;
    }
    const auto n = static_cast<std::size_t>(rows);
    auto at = [&](std::size_t row, std::size_t col) -> Scalar & { return lu[row * n + col]; };
    for (std::size_t k = 0; k < n; ++k)
    {
        std::size_t pivot = k;
        double largest = std::fabs(static_cast<double>(at(k, k)));
        for (std::size_t row = k + 1; row < n; ++row)
        {
            const double candidate = std::fabs(static_cast<double>(at(row, k)));
            if (candidate > largest)
            {
                largest = candidate;
                pivot = row;
            }
        }
        if (!(largest > 1e-12))
        {
            fail(frame, "Matrix.solve 奇异或近奇异矩阵");
            return;
        }
        if (pivot != k)
        {
            for (std::size_t col = 0; col < n; ++col) std::swap(at(k, col), at(pivot, col));
            std::swap(b[k], b[pivot]);
        }
        for (std::size_t row = k + 1; row < n; ++row)
        {
            const Scalar factor = divideScalar(at(row, k), at(k, k));
            at(row, k) = factor;
            for (std::size_t col = k + 1; col < n; ++col)
                at(row, col) = subtractProduct(at(row, col), factor, at(k, col));
            b[row] = subtractProduct(b[row], factor, b[k]);
        }
    }
    Memory::Vector<Scalar> answer(n, Scalar{});
    for (std::size_t remaining = n; remaining > 0; --remaining)
    {
        const auto i = remaining - 1;
        Scalar sum = b[i];
        for (std::size_t j = i + 1; j < n; ++j)
            sum = subtractProduct(sum, at(i, j), answer[j]);
        answer[i] = divideScalar(sum, at(i, i));
    }
    writeOwnedValues(frame, output, 0, answer);
}

/** @brief 从 C 帧读取一个聚合引用地址。
 * @param frame 当前 C 帧。
 * @param index 参数位置。
 * @return 逻辑地址。
 */
int64_t argument(ChaliAbiFrame *frame, int64_t index)
{
    int64_t value = 0;
    if (!chali_abi_argument_integer(frame, index, &value)) fail(frame, "矩阵引用参数无效");
    return value;
}
/** @brief 执行指定精度的求解，结果写入语言向量。
 * @tparam Scalar 标量精度。
 * @param frame 输入、右端及结果引用。
 */
template <class Scalar>
void CHALI_ABI_CALL solve(ChaliAbiFrame *frame)
try
{
    const auto input = argument(frame, 0), rhs = argument(frame, 1), output = argument(frame, 2);
    matrixSolve<Scalar>(frame, input, rhs, output);
    chali_abi_return_integer(frame, 0);
} catch (const std::exception &error) { chali_abi_fail(frame, error.what()); } catch (...)
{
    chali_abi_fail(frame, "矩阵求解失败");
}
/** @brief 分解操作类别。 */
enum class Decomposition
{
    LU,
    LUR,
    Eigen,
    SVD,
    Jordan,
    Diagonal
};
/** @brief 执行固定分解，不构造原生临时聚合或通用实参向量。
 * @tparam operation 分解类别。
 * @param frame 输入及结果引用。
 */
template <Decomposition operation>
void CHALI_ABI_CALL decompose(ChaliAbiFrame *frame)
try
{
    const auto inputAddress = argument(frame, 0), output = argument(frame, 1);
    const auto input = readMatrix(frame, inputAddress);
    if constexpr (operation == Decomposition::LU || operation == Decomposition::LUR)
    {
        const auto value = luDecomposeImpl(frame, input);
        writeMatrixInto(frame, output, 0, value.lower);
        writeMatrixInto(frame, output, 3, value.upper);
        writeMatrixInto(frame, output, 6, value.permutation);
    }
    else if constexpr (operation == Decomposition::Eigen)
    {
        const auto value = symmetricEigenImpl(frame, input);
        writeVectorInto(frame, output, 0, value.eigenvalues);
        writeMatrixInto(frame, output, 1, value.eigenvectors);
    }
    else if constexpr (operation == Decomposition::SVD)
    {
        const auto value = svdImpl(frame, input);
        writeMatrixInto(frame, output, 0, value.u);
        writeVectorInto(frame, output, 3, value.singularValues);
        writeMatrixInto(frame, output, 4, value.vTranspose);
    }
    else
    {
        const auto value = jordan2Impl(frame, input);
        if constexpr (operation == Decomposition::Diagonal)
            if (!(std::fabs(at(value.jordan, 0, 1)) < 0.00000001)) fail(frame, "矩阵不可对角化");
        writeMatrixInto(frame, output, 0, value.basis);
        writeMatrixInto(frame, output, 3, value.jordan);
    }
    chali_abi_return_integer(frame, 0);
} catch (const std::exception &error) { chali_abi_fail(frame, error.what()); } catch (...)
{
    chali_abi_fail(frame, "矩阵分解失败");
}
}  // namespace
/** @brief 取得线性代数的 C 帧入口。
 * @param count 接收入口数量，可为空。
 * @return 生命周期覆盖进程的表。
 */
extern "C" const ChaliAbiRecord *CHALI_ABI_CALL abi_linalg_records(size_t *count)
{
    static const ChaliAbiRecord records[] = {
        { "std::linear_algebra::__diagonalize2Into", decompose<Decomposition::Diagonal>, 2 },
        { "std::linear_algebra::__jordan2Into", decompose<Decomposition::Jordan>, 2 },
        { "std::linear_algebra::__luDecomposeInto", decompose<Decomposition::LU>, 2 },
        { "std::linear_algebra::__lurDecomposeInto", decompose<Decomposition::LUR>, 2 },
        { "std::linear_algebra::__matrixSolveInto__float32", solve<float>, 3 },
        { "std::linear_algebra::__matrixSolveInto__float64", solve<double>, 3 },
        { "std::linear_algebra::__matrixSolveInto__int16", solve<int16_t>, 3 },
        { "std::linear_algebra::__matrixSolveInto__int32", solve<int32_t>, 3 },
        { "std::linear_algebra::__matrixSolveInto__int64", solve<int64_t>, 3 },
        { "std::linear_algebra::__matrixSolveInto__int8", solve<int8_t>, 3 },
        { "std::linear_algebra::__matrixSolveInto__uint16", solve<uint16_t>, 3 },
        { "std::linear_algebra::__matrixSolveInto__uint32", solve<uint32_t>, 3 },
        { "std::linear_algebra::__matrixSolveInto__uint64", solve<uint64_t>, 3 },
        { "std::linear_algebra::__matrixSolveInto__uint8", solve<uint8_t>, 3 },
        { "std::linear_algebra::__svdInto", decompose<Decomposition::SVD>, 2 },
        { "std::linear_algebra::__symmetricEigenInto", decompose<Decomposition::Eigen>, 2 },
    };
    if (count) *count = sizeof(records) / sizeof(records[0]);
    return records;
}
