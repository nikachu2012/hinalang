#include "lexer.hpp"

inline bool lexer::isDecimal(char c)
{
    return '0' <= c && c <= '9';
}

lexer::lexer(std::ifstream &f) : fs(f)
{
}

lexer::~lexer()
{
}

lexer::LEXER_TYPE lexer::lex()
{
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

    default:
        break;
    }

    return LEXER_TYPE::END;
}
