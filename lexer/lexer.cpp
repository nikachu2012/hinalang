#include "lexer.hpp"

inline bool lexer::isDecimal(char c)
{
    return '0' <= c && c <= '9';
}

inline bool lexer::isKeyword(char c)
{
    return ('0' <= c && c <= '9') || ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z') || ('_' == c) ||
           (0x80 <= (unsigned char)c && (unsigned char)c <= 0xff);
}

lexer::lexer(std::ifstream &f) : fs(f)
{
}

lexer::~lexer()
{
}

void lexer::pbToken()
{
    fs.seekg(prev_pos);
}

lexer::LEXER_TYPE lexer::lex()
{
    // 1トークン読む前の位置を保存
    prev_pos = fs.tellg();

    char c;
    buf = "";

next:
    c = read();
    if (fs.eof() || fs.fail()) // 読んでからEOFチェック
    {
        return LEXER_TYPE::END;
    }

    switch (c)
    {
    case ' ':
    case '\t':
    case '\n':
        // ホワイトスペースの無視
        goto next;
        break;

    case '/':
        c = read();
        if (c == '/')
        {
            // １行コメント
            while ((read()) != '\n')
                ;
            goto next;
        }
        else if (c == '*')
        {
            // 複数行コメント
            while (true)
            {
                while ((read()) != '*')
                    ;
                if (read() == '/') // */が来た時
                    break;
                else // *が来ても*/でなければ読み続ける
                    continue;
            }
            goto next;
        }
        else
        {
            // 除算演算子
            pb();
            buf += '/';
            return LEXER_TYPE::OPERATOR;
        }
        break;

    case '+':
    case '*':
    case '%':
        // 加乗と剰余演算子
        buf += c;
        return LEXER_TYPE::OPERATOR;
        break;

    case '-':
        c = read();
        if (c == '>')
        {
            // 右矢印演算子
            buf = "->";
            return LEXER_TYPE::RIGHT_ARROW;
        }
        else
        {
            pb();
            buf = "-";
            return LEXER_TYPE::OPERATOR;
        }
        break;

    case '>':
        c = read();
        if (c == '>')
        {
            // 右シフト演算子
            buf = ">>";
            return LEXER_TYPE::OPERATOR;
        }
        else if (c == '=')
        {
            // 大なりイコール演算子
            buf = ">=";
            return LEXER_TYPE::OPERATOR;
        }
        else
        {
            // 大なり演算子
            pb();
            buf = ">";
            return LEXER_TYPE::OPERATOR;
        }
        break;

    case '<':
        c = read();
        if (c == '<')
        {
            // 右シフト演算子
            buf = "<<";
            return LEXER_TYPE::OPERATOR;
        }
        else if (c == '=')
        {
            // 小なりイコール演算子
            buf = "<=";
            return LEXER_TYPE::OPERATOR;
        }
        else
        {
            // 小なり演算子
            pb();
            buf = "<";
            return LEXER_TYPE::OPERATOR;
        }
        break;

    case '=':
        c = read();
        if (c == '=')
        {
            // 等価演算子
            buf = "==";
            return LEXER_TYPE::OPERATOR;
        }
        else
        {
            // 代入
            pb();
            buf = "=";
            return LEXER_TYPE::OPERATOR;
        }
        break;

    case '!':
        c = read();
        if (c == '=')
        {
            // 非等価演算子
            buf = "!=";
            return LEXER_TYPE::OPERATOR;
        }
        else
        {
            pb();
            buf = "!";
            return LEXER_TYPE::OPERATOR;
        }

    case ',':
        buf = c;
        return LEXER_TYPE::COMMA;
        break;

    case '(':
        buf = c;
        return LEXER_TYPE::LEFT_BRACKET;
        break;

    case ')':
        buf = c;
        return LEXER_TYPE::RIGHT_BRACKET;
        break;

    case '{':
        buf = c;
        return LEXER_TYPE::LEFT_BRACE;
        break;

    case '}':
        buf = c;
        return LEXER_TYPE::RIGHT_BRACE;
        break;

    case ';':
        buf = c;
        return LEXER_TYPE::SEMICOLON;
        break;

    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        // 数値の時
        buf += c;
        while (isDecimal(c = read()))
        {
            buf += c;
        }
        pb(); // whileの時に読みすぎるので1文字戻す
        return LEXER_TYPE::NUMBER;
        break;

    case '"':
        // 文字列 簡易版
        // while ((c=read()) != '"')
        // {
        //     buf += c;
        // }
        // return LEXER_TYPE::STRING;
        // break;

        // 文字列 エスケープシーケンスに対応
        while (true)
        {
            c = read();

            if (c == '\\')
            {
                buf += c;
                // バックスラッシュの次を読む
                buf += read();
            }
            else if (c == '"')
            {
                break;
            }
            else if (fs.eof())
            {
                Error::err("\" is required.");
            }
            else
            {
                buf += c;
            }
        }
        return LEXER_TYPE::STRING;
        break;

    default:
        if (isKeyword(c))
        {
            buf += c;
            while (isKeyword((c = read())))
            {
                buf += c;
            }
            pb();
            return LEXER_TYPE::KEYWORD;
        }
        else
        {
            Error::err("Undefined char '%c'", c);
        }
        break;
    }

    return LEXER_TYPE::END;
}
