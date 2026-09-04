#include <gtest/gtest.h>
#include "lexer.h"
#include "parser.h"
#include "netlist.h"

TEST(LexerTest, BasicTokens) {
    Lexer lexer("module test(A); input A; endmodule");
    auto tokens = lexer.tokenize();
    ASSERT_GE(tokens.size(), 5);
    EXPECT_EQ(tokens[0].type, TokenType::Module);
    EXPECT_EQ(tokens[1].value, "test");
}

TEST(ParserTest, BasicParse) {
    Lexer lexer("module test(A); input A; endmodule");
    Parser parser(lexer.tokenize());
    auto ast = parser.parse();
    EXPECT_EQ(ast->name, "test");
    EXPECT_EQ(ast->ports.size(), 1);
    EXPECT_EQ(ast->ports[0], "A");
}
