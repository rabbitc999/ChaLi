/* ParseEngine %posttokens：在原始 Token 中识别容器类型实参。 */
if (tokens.empty()) return;

const bool typeFragment = tokens.front().type == TOKEN_MACRO_PARSE_TYPE;
ParserMemory::Vector<std::pair<std::size_t, Token>> splits;
for (std::size_t open = 1; open < tokens.size(); ++open)
{
    if (tokens[open].type != TOKEN_LT) continue;
    const Token &name = tokens[open - 1];
    // cast、宏调用和泛型闭包的最外层仍由文法消费 LT / GT。
    const bool plainOuter = name.type == TOKEN_CAST_KW || name.type == TOKEN_MACRO_NAME ||
                            name.type == TOKEN_FN;
    if (!plainOuter)
    {
        if ((name.type != TOKEN_VARIABLE && name.type != TOKEN_COPY) || name.text.empty())
            continue;
        if (open >= 2 && (tokens[open - 2].type == TOKEN_MACRO_DEFINE ||
                          tokens[open - 2].type == TOKEN_MACRO_REPLACE))
            continue;
        if (!typeFragment && !(name.text.front() >= 'A' && name.text.front() <= 'Z') &&
            !M_IsKnownGenericName(name.text))
            continue;
    }

    std::size_t depth = 1;
    int rounds = 0;
    int squares = 0;
    std::size_t close = open + 1;
    for (; close < tokens.size(); ++close)
    {
        const auto kind = tokens[close].type;
        if (kind == TOKEN_LT) ++depth;
        if (kind == TOKEN_GT || kind == TOKEN_GE || kind == TOKEN_SHR || kind == TOKEN_SHR_ASSIGN)
        {
            const std::size_t width = kind == TOKEN_SHR || kind == TOKEN_SHR_ASSIGN ? 2 : 1;
            // 只有一个未闭合角括号时，整个 >> 仍可能是表达式里的右移。
            if (depth <= width)
            {
                if (depth == width && !rounds && !squares) depth = 0;
                break;
            }
            if (kind == TOKEN_GE || kind == TOKEN_SHR_ASSIGN) break;
            depth -= width;
        }
        if (kind == TOKEN_LPAREN) ++rounds;
        if (kind == TOKEN_LBRACKET) ++squares;
        if ((kind == TOKEN_RPAREN && --rounds < 0) ||
            (kind == TOKEN_RBRACKET && --squares < 0))
            break;
        if (kind == TOKEN_ASSIGN && depth == 1 && name.type == TOKEN_FN) continue;
        switch (kind)
        {
        case TOKEN_SEMI: case TOKEN_LBRACE: case TOKEN_RBRACE:
        case TOKEN_ASSIGN: case TOKEN_PLUS: case TOKEN_SLASH:
        case TOKEN_STRING: case TOKEN_CHRLIT: case TOKEN_AND: case TOKEN_OR:
        case TOKEN_LE: case TOKEN_SHL: case TOKEN_SHL_ASSIGN:
        case TOKEN_EQ: case TOKEN_NE: case TOKEN_FATARROW:
        case TOKEN_ADD_ASSIGN: case TOKEN_SUB_ASSIGN: case TOKEN_MUL_ASSIGN:
        case TOKEN_DIV_ASSIGN: case TOKEN_MOD_ASSIGN: case TOKEN_AND_ASSIGN:
        case TOKEN_OR_ASSIGN: case TOKEN_XOR_ASSIGN:
        case TOKEN_INCREMENT:
            break;
        default:
            continue;
        }
        break;
    }
    if (depth != 0) continue;

    depth = 0;
    for (std::size_t index = open; index <= close; ++index)
    {
        Token &token = tokens[index];
        if (token.type == TOKEN_LT)
        {
            if (!plainOuter || depth != 0) token.type = TOKEN_TYPE_ARGS_BEGIN;
            ++depth;
        }
        else if (token.type == TOKEN_GT)
        {
            if (!plainOuter || depth != 1) token.type = TOKEN_TYPE_ARGS_END;
            --depth;
        }
        else if (token.type == TOKEN_GE || token.type == TOKEN_SHR || token.type == TOKEN_SHR_ASSIGN)
        {
            const auto original = token.type;
            const auto piece = [&token](TokenType type, const char *text, std::uint32_t delta) {
                Token part = token;
                part.type = type;
                part.text = text;
                part.name = nullptr;
                part.pName = text;
                part.column += delta;
                part.offset += delta;
                return part;
            };
            const auto first = plainOuter && depth == 1 ? TOKEN_GT : TOKEN_TYPE_ARGS_END;
            if (original == TOKEN_GE)
                splits.emplace_back(index, piece(TOKEN_ASSIGN, "=", 1));
            else
            {
                const auto second = plainOuter && depth == 2 ? TOKEN_GT : TOKEN_TYPE_ARGS_END;
                splits.emplace_back(index, piece(second, ">", 1));
                if (original == TOKEN_SHR_ASSIGN)
                    splits.emplace_back(index, piece(TOKEN_ASSIGN, "=", 2));
                --depth;
            }
            token = piece(first, ">", 0);
            --depth;
        }
    }
    open = close;
}

/* 从后往前合并拆分结果，避免每次拆分都搬动后续 Token。 */
std::size_t read = tokens.size();
tokens.resize(read + splits.size());
std::size_t write = tokens.size();
while (!splits.empty())
{
    const std::size_t index = splits.back().first;
    while (read > index + 1) tokens[--write] = tokens[--read];
    tokens[--write] = splits.back().second;
    splits.pop_back();
}
