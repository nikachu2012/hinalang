#include "lexer.hpp"

lexer::lexer(std::ifstream &f) : fs(f)
{
}

lexer::~lexer()
{
}

lexer::LEXER_TYPE lexer::lex()
{
    char c;

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

    default:
        break;
    }

    return LEXER_TYPE::END;
}
