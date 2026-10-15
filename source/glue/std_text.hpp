#ifndef PARSE_ENGINE_STD_TEXT_HPP
#define PARSE_ENGINE_STD_TEXT_HPP

#include "../magel_glue.hpp"

#include <cstdint>

namespace std_text_glue
{

/* 空白：空格 / `\t` / `\n` / `\r` / `\v` / `\f`（与 C 的 `isspace` 同集合）。 */
inline bool isTrimSpace(char byte)
{
    return byte == ' ' || byte == '\t' || byte == '\n' || byte == '\r' || byte == '\v' || byte == '\f';
}

/* `std::text::Text` 的字符串工具族。从 VM 内部原生表（`kSlotNativeEntries`）搬到
 * Registry 胶水，语义逐条照搬原实现：宽度/位置一律按字节算，与 `len(s)` 同一把尺。 */
inline bool bind(magel_glue::Registry &registry, Memory::String *error = nullptr)
{
    using Memory::String;
    return registry.bindTyped<Memory::Vector<String>, String, String>(
               "std::text::__strSplit",
               [](String text, String separator) {
                   Memory::Vector<String> parts;
                   std::size_t start = 0;
                   if (!separator.empty())
                   {
                       for (;;)
                       {
                           const auto at = text.find(separator, start);
                           if (at == String::npos) break;
                           parts.push_back(text.substr(start, at - start));
                           start = at + separator.size();
                       }
                   }
                   parts.push_back(text.substr(start));
                   return parts;
               },
               error)
           && registry.bindTyped<String, Memory::Vector<String>, String>(
               "std::text::__strJoin",
               [](Memory::Vector<String> parts, String separator) {
                   String result;
                   bool first = true;
                   for (const auto &part : parts)
                   {
                       if (!first) result += separator;
                       result += part;
                       first = false;
                   }
                   return result;
               },
               error)
           && registry.bindTyped<std::int64_t, String>(
               "std::text::__strLen",
               [](String text) { return static_cast<std::int64_t>(text.size()); }, error)
           && registry.bindTyped<std::int64_t, String, String>(
               "std::text::__strFind",
               [](String text, String needle) {
                   const std::size_t found = text.find(needle);
                   return found == String::npos ? -1 : static_cast<std::int64_t>(found);
               },
               error)
           && registry.bindTyped<std::int64_t, String, String>(
               "std::text::__strRFind",
               [](String text, String needle) {
                   const std::size_t found = text.rfind(needle);
                   return found == String::npos ? -1 : static_cast<std::int64_t>(found);
               },
               error)
           && registry.bindTyped<std::int64_t, String, String>(
               "std::text::__strCompare",
               [](String left, String right) {
                   const int order = left.compare(right);
                   return order < 0 ? -1 : (order > 0 ? 1 : 0);
               },
               error)
           && registry.bindTyped<bool, String, String>(
               "std::text::__strStartsWith",
               [](String text, String prefix) {
                   return text.size() >= prefix.size() && text.compare(0u, prefix.size(), prefix) == 0;
               },
               error)
           && registry.bindTyped<bool, String, String>(
               "std::text::__strEndsWith",
               [](String text, String suffix) {
                   return text.size() >= suffix.size() && text.compare(text.size() - suffix.size(), suffix.size(), suffix) == 0;
               },
               error)
           && registry.bindTyped<String, String>(
               "std::text::__strUpper",
               [](String text) {
                   for (char &byte : text)
                       if (byte >= 'a' && byte <= 'z') byte = static_cast<char>(byte - 'a' + 'A');
                   return text;
               },
               error)
           && registry.bindTyped<String, String>(
               "std::text::__strLower",
               [](String text) {
                   for (char &byte : text)
                       if (byte >= 'A' && byte <= 'Z') byte = static_cast<char>(byte - 'A' + 'a');
                   return text;
               },
               error)
           && registry.bindTyped<String, String, std::int64_t>(
               "std::text::__strTrim",
               [](String text, std::int64_t mode) {
                   std::size_t begin = 0u;
                   std::size_t end = text.size();
                   if (mode == 0 || mode == 1)
                       while (begin < end && isTrimSpace(text[begin])) ++begin;
                   if (mode == 0 || mode == 2)
                       while (end > begin && isTrimSpace(text[end - 1u])) --end;
                   return text.substr(begin, end - begin);
               },
               error)
           && registry.bindTyped<String, String, String, String>(
               "std::text::__strReplace",
               [](String text, String from, String to) {
                   if (from.empty()) return text;
                   String result;
                   std::size_t at = 0u;
                   while (true)
                   {
                       const std::size_t found = text.find(from, at);
                       if (found == String::npos)
                       {
                           result += text.substr(at);
                           break;
                       }
                       result += text.substr(at, found - at);
                       result += to;
                       at = found + from.size();
                   }
                   return result;
               },
               error)
           && registry.bindTyped<String, String, std::int64_t>(
               "std::text::__strRepeat",
               [](String text, std::int64_t count) {
                   String result;
                   for (std::int64_t index = 0; index < count; ++index) result += text;
                   return result;
               },
               error)
           && registry.bindTyped<String, String, std::int64_t, std::int64_t, bool>(
               "std::text::__strPad",
               [](String text, std::int64_t width, std::int64_t fill, bool left) {
                   if (fill < 0 || fill > 255) return text;
                   String result = text;
                   if (width > 0)
                   {
                       const std::size_t wanted = static_cast<std::size_t>(width);
                       if (result.size() < wanted)
                       {
                           const String padding(wanted - result.size(), static_cast<char>(fill));
                           result = left ? padding + result : result + padding;
                       }
                   }
                   return result;
               },
               error);
}

}  // namespace std_text_glue

#endif
