#include "catch.hpp"

#include "SP/Parser/Parser.h"
#include "SP/Parser/AssignStatementParser.h"

#include <string>
#include "SP/Tokenizer/NameToken.h"
#include "SP/Tokenizer/IntegerToken.h"
#include "SP/Tokenizer/PunctuationToken.h"
#include "SP/Tokenizer/ArithmeticOperatorToken.h"

TEST_CASE("Check if ArithmeticOperationParser works") {
  SECTION("Check if arithmetic expression with only 2 operands and 1 (+ or -) operator"
          " (e.g., 11 + x) parses correctly") {
    Parser::Line expr_line
        {make_shared<IntegerToken>("11"), make_shared<ArithmeticOperatorToken>("+", PLUS), make_shared<NameToken>("x")};
    auto expr_parser = ExpressionParserFactory::GetExpressionParser(expr_line, "assign");
    auto actual = expr_parser->ParseEntity(expr_line);
    pair<shared_ptr<Expression>, shared_ptr<Expression>> arguments;
    arguments.first = make_shared<Constant>("11");
    arguments.second = make_shared<Variable>("x");
    shared_ptr<ArithmeticOperation> expected = make_shared<ArithmeticOperation>("+", arguments);
    REQUIRE(actual->operator==(*expected));
  }SECTION(
      "Check if arithmetic expression with only 2 operands and 1 (* or / or %) operator"
      " (e.g., x * z) parses correctly") {
    Parser::Line expr_line
        {make_shared<NameToken>("x"), make_shared<ArithmeticOperatorToken>("*", MULTIPLY), make_shared<NameToken>("z")};
    auto expr_parser = ExpressionParserFactory::GetExpressionParser(expr_line, "assign");
    auto actual = expr_parser->ParseEntity(expr_line);
    pair<shared_ptr<Expression>, shared_ptr<Expression>> arguments;
    arguments.first = make_shared<Variable>("x");
    arguments.second = make_shared<Variable>("z");
    shared_ptr<ArithmeticOperation> expected = make_shared<ArithmeticOperation>("*", arguments);
    REQUIRE(actual->operator==(*expected));
  }SECTION("Check if arithmetic expression with only 3 operands and 2 (+ or -) operator"
           " (e.g., x + y + z) parses correctly") {
    Parser::Line expr_line
        {make_shared<NameToken>("x"), make_shared<ArithmeticOperatorToken>("+", PLUS), make_shared<NameToken>("y"),
         make_shared<ArithmeticOperatorToken>("+", PLUS), make_shared<NameToken>("z")};
    auto expr_parser = ExpressionParserFactory::GetExpressionParser(expr_line, "assign");
    auto actual = expr_parser->ParseEntity(expr_line);
    pair<shared_ptr<Expression>, shared_ptr<Expression>> left_subtree_args;
    left_subtree_args.first = make_shared<Variable>("x");
    left_subtree_args.second = make_shared<Variable>("y");
    shared_ptr<ArithmeticOperation> left_subtree = make_shared<ArithmeticOperation>("+", left_subtree_args);
    pair<shared_ptr<Expression>, shared_ptr<Expression>> root_args;
    root_args.first = left_subtree;
    root_args.second = make_shared<Variable>("z");
    shared_ptr<ArithmeticOperation> root = make_shared<ArithmeticOperation>("+", root_args);
    REQUIRE(actual->operator==(*root));
  }SECTION(
      "Check if arithmetic expression with only 3 operands with 1 (+ or -) first then 1 (* or / or %) operator"
      "(e.g., x + z * 5) parses correctly") {
    Parser::Line expr_line
        {make_shared<NameToken>("x"), make_shared<ArithmeticOperatorToken>("+", PLUS), make_shared<NameToken>("z"),
         make_shared<ArithmeticOperatorToken>("*", MULTIPLY), make_shared<IntegerToken>("5")};
    auto expr_parser = ExpressionParserFactory::GetExpressionParser(expr_line, "assign");
    auto actual = expr_parser->ParseEntity(expr_line);
    pair<shared_ptr<Expression>, shared_ptr<Expression>> right_subtree_args;
    right_subtree_args.first = make_shared<Variable>("z");
    right_subtree_args.second = make_shared<Constant>("5");
    shared_ptr<ArithmeticOperation> right_subtree = make_shared<ArithmeticOperation>("*", right_subtree_args);
    pair<shared_ptr<Expression>, shared_ptr<Expression>> root_args;
    root_args.first = make_shared<Variable>("x");
    root_args.second = right_subtree;
    shared_ptr<ArithmeticOperation> root = make_shared<ArithmeticOperation>("+", root_args);
    REQUIRE(actual->operator==(*root));
  }SECTION(
      "Check if arithmetic expression with only 3 operands with 2 (* or / or %) operator "
      "(e.g., x / y * z) parses correctly") {
    Parser::Line expr_line
        {make_shared<NameToken>("x"), make_shared<ArithmeticOperatorToken>("/", DIV), make_shared<NameToken>("y"),
         make_shared<ArithmeticOperatorToken>("*", MULTIPLY), make_shared<NameToken>("z")};
    auto expr_parser = ExpressionParserFactory::GetExpressionParser(expr_line, "assign");
    auto actual = expr_parser->ParseEntity(expr_line);
    pair<shared_ptr<Expression>, shared_ptr<Expression>> left_subtree_args;
    left_subtree_args.first = make_shared<Variable>("x");
    left_subtree_args.second = make_shared<Variable>("y");
    shared_ptr<ArithmeticOperation> left_subtree = make_shared<ArithmeticOperation>("/", left_subtree_args);
    pair<shared_ptr<Expression>, shared_ptr<Expression>> root_args;
    root_args.first = left_subtree;
    root_args.second = make_shared<Variable>("z");
    shared_ptr<ArithmeticOperation> root = make_shared<ArithmeticOperation>("*", root_args);
    REQUIRE(actual->operator==(*root));
  }SECTION(
      "Check if arithmetic expression with only 3 operands with 1 (* or / or %) first then 1 (+ or -) operator"
      "(e.g., z * 5 + x) parses correctly") {
    Parser::Line expr_line{make_shared<NameToken>("z"), make_shared<ArithmeticOperatorToken>("*", MULTIPLY),
                           make_shared<IntegerToken>("5"),
                           make_shared<ArithmeticOperatorToken>("+", PLUS), make_shared<NameToken>("x")};
    auto expr_parser = ExpressionParserFactory::GetExpressionParser(expr_line, "assign");
    auto actual = expr_parser->ParseEntity(expr_line);
    pair<shared_ptr<Expression>, shared_ptr<Expression>> left_subtree_args;
    left_subtree_args.first = make_shared<Variable>("z");
    left_subtree_args.second = make_shared<Constant>("5");
    shared_ptr<ArithmeticOperation> left_subtree = make_shared<ArithmeticOperation>("*", left_subtree_args);
    pair<shared_ptr<Expression>, shared_ptr<Expression>> root_args;
    root_args.first = left_subtree;
    root_args.second = make_shared<Variable>("x");
    shared_ptr<ArithmeticOperation> root = make_shared<ArithmeticOperation>("+", root_args);
    REQUIRE(actual->operator==(*root));
  }SECTION(
      "Check if arithmetic expression with only 3 operands where the 1st pair is enclosed by () and uses 1 (+ or -)"
      "followed by 1 (* or / or %) operator and 1 operand (e.g., (x + z) * 5) parses correctly") {
    Parser::Line expr_line{make_shared<PunctuationToken>("(", LEFT_PARENTHESIS), make_shared<NameToken>("x"),
                           make_shared<ArithmeticOperatorToken>("+", PLUS),
                           make_shared<NameToken>("z"), make_shared<PunctuationToken>(")", RIGHT_PARENTHESIS),
                           make_shared<ArithmeticOperatorToken>("*", MULTIPLY),
                           make_shared<IntegerToken>("5")};
    auto expr_parser = ExpressionParserFactory::GetExpressionParser(expr_line, "assign");
    auto actual = expr_parser->ParseEntity(expr_line);
    pair<shared_ptr<Expression>, shared_ptr<Expression>> left_subtree_args;
    left_subtree_args.first = make_shared<Variable>("x");
    left_subtree_args.second = make_shared<Variable>("z");
    shared_ptr<ArithmeticOperation> left_subtree = make_shared<ArithmeticOperation>("+", left_subtree_args);
    pair<shared_ptr<Expression>, shared_ptr<Expression>> root_args;
    root_args.first = left_subtree;
    root_args.second = make_shared<Constant>("5");
    shared_ptr<ArithmeticOperation> root = make_shared<ArithmeticOperation>("*", root_args);
    REQUIRE(actual->operator==(*root));
  }SECTION(
      "Check if arithmetic expression with only > 3 operands using a mixture of operators (+-*/%)"
      "and 1 pair enclosed in () (e.g., z % ost + x * (y * z)) parses correctly") {
    Parser::Line expr_line
        {make_shared<NameToken>("z"), make_shared<ArithmeticOperatorToken>("%", MOD), make_shared<NameToken>("ost"),
         make_shared<ArithmeticOperatorToken>("+", PLUS), make_shared<NameToken>("x"),
         make_shared<ArithmeticOperatorToken>("*", MULTIPLY),
         make_shared<PunctuationToken>("(", LEFT_PARENTHESIS), make_shared<NameToken>("y"),
         make_shared<ArithmeticOperatorToken>("*", MULTIPLY),
         make_shared<NameToken>("z"), make_shared<PunctuationToken>(")", RIGHT_PARENTHESIS)};
    auto expr_parser = ExpressionParserFactory::GetExpressionParser(expr_line, "assign");
    auto actual = expr_parser->ParseEntity(expr_line);
    pair<shared_ptr<Expression>, shared_ptr<Expression>> root_left_subtree_args;
    root_left_subtree_args.first = make_shared<Variable>("z");
    root_left_subtree_args.second = make_shared<Variable>("ost");
    shared_ptr<ArithmeticOperation> root_left_subtree = make_shared<ArithmeticOperation>("%", root_left_subtree_args);

    pair<shared_ptr<Expression>, shared_ptr<Expression>> root_right_right_subtree_args;
    root_right_right_subtree_args.first = make_shared<Variable>("y");
    root_right_right_subtree_args.second = make_shared<Variable>("z");
    shared_ptr<ArithmeticOperation>
        root_right_right_subtree = make_shared<ArithmeticOperation>("*", root_right_right_subtree_args);

    pair<shared_ptr<Expression>, shared_ptr<Expression>> root_right_subtree_args;
    root_right_subtree_args.first = make_shared<Variable>("x");
    root_right_subtree_args.second = root_right_right_subtree;
    shared_ptr<ArithmeticOperation> root_right_subtree = make_shared<ArithmeticOperation>("*", root_right_subtree_args);

    pair<shared_ptr<Expression>, shared_ptr<Expression>> root_args;
    root_args.first = root_left_subtree;
    root_args.second = root_right_subtree;
    shared_ptr<ArithmeticOperation> root = make_shared<ArithmeticOperation>("+", root_args);
    REQUIRE(actual->operator==(*root));
  }SECTION(
      "Check if complex arithmetic expression "
      "[e.g., (((24 + 25) * 26 ) + 27) -  (28 + ( 29 / (30 + 31))] parses correctly ") {
    Parser::Line expr_line
        {make_shared<PunctuationToken>("(", LEFT_PARENTHESIS), make_shared<PunctuationToken>("(", LEFT_PARENTHESIS),
         make_shared<PunctuationToken>("(", LEFT_PARENTHESIS), make_shared<IntegerToken>("24"),
         make_shared<ArithmeticOperatorToken>("+", PLUS), make_shared<IntegerToken>("25"),
         make_shared<PunctuationToken>(")", RIGHT_PARENTHESIS), make_shared<ArithmeticOperatorToken>("*", MULTIPLY),
         make_shared<IntegerToken>("26"), make_shared<PunctuationToken>(")", RIGHT_PARENTHESIS),
         make_shared<ArithmeticOperatorToken>("+", PLUS), make_shared<IntegerToken>("27"),
         make_shared<PunctuationToken>(")", RIGHT_PARENTHESIS), make_shared<ArithmeticOperatorToken>("-", MINUS),
         make_shared<PunctuationToken>("(", LEFT_PARENTHESIS), make_shared<IntegerToken>("28"),
         make_shared<ArithmeticOperatorToken>("+", PLUS), make_shared<IntegerToken>("29"),
         make_shared<ArithmeticOperatorToken>("/", DIV), make_shared<PunctuationToken>("(", LEFT_PARENTHESIS),
         make_shared<IntegerToken>("30"), make_shared<ArithmeticOperatorToken>("+", PLUS),
         make_shared<IntegerToken>("31"), make_shared<PunctuationToken>(")", RIGHT_PARENTHESIS),
         make_shared<PunctuationToken>(")", RIGHT_PARENTHESIS)};
    auto expr_parser = ExpressionParserFactory::GetExpressionParser(expr_line, "assign");
    auto actual = expr_parser->ParseEntity(expr_line);

    pair<shared_ptr<Expression>, shared_ptr<Expression>> t1_args;
    t1_args.first = make_shared<Constant>("24");
    t1_args.second = make_shared<Constant>("25");
    shared_ptr<ArithmeticOperation> t1 = make_shared<ArithmeticOperation>("+", t1_args);

    pair<shared_ptr<Expression>, shared_ptr<Expression>> t2_args;
    t2_args.first = t1;
    t2_args.second = make_shared<Constant>("26");
    shared_ptr<ArithmeticOperation>
        t2 = make_shared<ArithmeticOperation>("*", t2_args);

    pair<shared_ptr<Expression>, shared_ptr<Expression>> t3_args;
    t3_args.first = t2;
    t3_args.second = make_shared<Constant>("27");
    shared_ptr<ArithmeticOperation> t3 = make_shared<ArithmeticOperation>("+", t3_args);

    // (28 + ( 29 / (30 + 31))
    pair<shared_ptr<Expression>, shared_ptr<Expression>> t4_args;
    t4_args.first = make_shared<Constant>("30");
    t4_args.second = make_shared<Constant>("31");
    shared_ptr<ArithmeticOperation> t4 = make_shared<ArithmeticOperation>("+", t4_args);

    pair<shared_ptr<Expression>, shared_ptr<Expression>> t5_args;
    t5_args.first = make_shared<Constant>("29");
    t5_args.second = t4;
    shared_ptr<ArithmeticOperation> t5 = make_shared<ArithmeticOperation>("/", t5_args);

    pair<shared_ptr<Expression>, shared_ptr<Expression>> t6_args;
    t6_args.first = make_shared<Constant>("28");
    t6_args.second = t5;
    shared_ptr<ArithmeticOperation> t6 = make_shared<ArithmeticOperation>("+", t6_args);

    pair<shared_ptr<Expression>, shared_ptr<Expression>> root_args;
    root_args.first = t3;
    root_args.second = t6;
    shared_ptr<ArithmeticOperation> root = make_shared<ArithmeticOperation>("-", root_args);
    REQUIRE(actual->operator==(*root));
  }SECTION("Check if arithmetic expression with unbalanced () [e.g. 2 * (x + 1 ] throws Syntax error") {
    Parser::Line expr_line{make_shared<IntegerToken>("2"), make_shared<ArithmeticOperatorToken>("*", MULTIPLY),
                           make_shared<PunctuationToken>("(", LEFT_PARENTHESIS),
                           make_shared<NameToken>("x"), make_shared<ArithmeticOperatorToken>("+", PLUS),
                           make_shared<IntegerToken>("1")};
    auto expr_parser = ExpressionParserFactory::GetExpressionParser(expr_line, "assign");
    REQUIRE_THROWS_AS(expr_parser->ParseEntity(expr_line), SyntaxErrorException);
  }SECTION("Check if arithmetic expression with unbalanced () [e.g. 2 * x) + 1 ] throws Syntax error") {
    Parser::Line expr_line{make_shared<IntegerToken>("2"), make_shared<ArithmeticOperatorToken>("*", MULTIPLY),
                           make_shared<NameToken>("x"),
                           make_shared<PunctuationToken>(")", RIGHT_PARENTHESIS),
                           make_shared<ArithmeticOperatorToken>("+", PLUS), make_shared<IntegerToken>("1")};
    auto expr_parser = ExpressionParserFactory::GetExpressionParser(expr_line, "assign");
    REQUIRE_THROWS_AS(expr_parser->ParseEntity(expr_line), SyntaxErrorException);
  }SECTION("Check if arithmetic expression with unbalanced () [e.g. (2 * x) + 1) ] throws Syntax error") {
    Parser::Line expr_line{make_shared<PunctuationToken>("(", LEFT_PARENTHESIS), make_shared<IntegerToken>("2"),
                           make_shared<ArithmeticOperatorToken>("*", MULTIPLY),
                           make_shared<NameToken>("x"), make_shared<PunctuationToken>(")", RIGHT_PARENTHESIS),
                           make_shared<ArithmeticOperatorToken>("+", PLUS),
                           make_shared<IntegerToken>("1"), make_shared<PunctuationToken>(")", RIGHT_PARENTHESIS)};
    auto expr_parser = ExpressionParserFactory::GetExpressionParser(expr_line, "assign");
    REQUIRE_THROWS_AS(expr_parser->ParseEntity(expr_line), SyntaxErrorException);
  }SECTION("Check if arithmetic expression with unbalanced () [e.g. ((2+x) ] throws Syntax error") {
    Parser::Line expr_line
        {make_shared<PunctuationToken>("(", LEFT_PARENTHESIS), make_shared<PunctuationToken>("(", LEFT_PARENTHESIS),
         make_shared<IntegerToken>("2"),
         make_shared<ArithmeticOperatorToken>("+", PLUS), make_shared<NameToken>("x"),
         make_shared<PunctuationToken>(")", RIGHT_PARENTHESIS)};
    auto expr_parser = ExpressionParserFactory::GetExpressionParser(expr_line, "assign");
    REQUIRE_THROWS_AS(expr_parser->ParseEntity(expr_line), SyntaxErrorException);
  }
}

TEST_CASE("Check if ConditionalOperationParser & RelationalOperationParser works") {
  SECTION("Check if rel_expr (e.g. x == 1) parses correctly") {
    Parser::Line expr_line{make_shared<NameToken>("x"), make_shared<RelationalOperatorToken>("==", DOUBLE_EQUALS),
                           make_shared<IntegerToken>("1")};
    auto expr_parser = ExpressionParserFactory::GetExpressionParser(expr_line, "if");
    auto actual = expr_parser->ParseEntity(expr_line);
    pair<shared_ptr<Expression>, shared_ptr<Expression>> rel_args;
    rel_args.first = make_shared<Variable>("x");
    rel_args.second = make_shared<Constant>("1");
    shared_ptr<RelationalOperation> rel = make_shared<RelationalOperation>("==", rel_args);

    pair<shared_ptr<Expression>, shared_ptr<Expression>> cond_args;
    cond_args.first = rel;
    shared_ptr<ConditionalOperation> expected = make_shared<ConditionalOperation>("rel_expr", cond_args);
    REQUIRE(actual->operator==(*expected));
  }SECTION("Check if rel_expr with missing rel_op (e.g. x y) throws syntax error") {
    Parser::Line expr_line{make_shared<NameToken>("x"), make_shared<NameToken>("y")};
    auto expr_parser = ExpressionParserFactory::GetExpressionParser(expr_line, "if");
    REQUIRE_THROWS_AS(expr_parser->ParseEntity(expr_line), SyntaxErrorException);
  }SECTION("Check if rel_expr with missing RHS rel_factor (e.g. x != ) throws syntax error") {
    Parser::Line expr_line{make_shared<NameToken>("x"), make_shared<RelationalOperatorToken>("!=", NE)};
    auto expr_parser = ExpressionParserFactory::GetExpressionParser(expr_line, "if");
    REQUIRE_THROWS_AS(expr_parser->ParseEntity(expr_line), SyntaxErrorException);
  }SECTION("Check if ! '(' cond_expr ')' (e.g. ! (x > y) ) parses correctly") {
    Parser::Line expr_line
        {make_shared<ConditionalOperatorToken>("!", NOT), make_shared<PunctuationToken>("(", LEFT_PARENTHESIS),
         make_shared<NameToken>("x"),
         make_shared<RelationalOperatorToken>(">", GT), make_shared<NameToken>("y"),
         make_shared<PunctuationToken>(")", RIGHT_PARENTHESIS)};
    auto expr_parser = ExpressionParserFactory::GetExpressionParser(expr_line, "while");
    auto actual = expr_parser->ParseEntity(expr_line);
    pair<shared_ptr<Expression>, shared_ptr<Expression>> rel_args;
    rel_args.first = make_shared<Variable>("x");
    rel_args.second = make_shared<Variable>("y");
    shared_ptr<RelationalOperation> rel = make_shared<RelationalOperation>(">", rel_args);

    pair<shared_ptr<Expression>, shared_ptr<Expression>> inner_cond_args;
    inner_cond_args.first = rel;
    shared_ptr<ConditionalOperation> inner_cond_expr = make_shared<ConditionalOperation>("rel_expr", inner_cond_args);

    pair<shared_ptr<Expression>, shared_ptr<Expression>> root_cond_args;
    root_cond_args.first = inner_cond_expr;
    shared_ptr<ConditionalOperation> root_cond_expr = make_shared<ConditionalOperation>("!", root_cond_args);
    REQUIRE(actual->operator==(*root_cond_expr));
  }SECTION("Check if '(' cond_expr ')' '&&' or '||' '(' cond_expr ')' (e.g. (x < y) || (y >= 100) ) parses correctly") {
    Parser::Line expr_line{make_shared<PunctuationToken>("(", LEFT_PARENTHESIS), make_shared<NameToken>("x"),
                           make_shared<RelationalOperatorToken>("<", LT),
                           make_shared<NameToken>("y"), make_shared<PunctuationToken>(")", RIGHT_PARENTHESIS),
                           make_shared<ConditionalOperatorToken>("||", OR),
                           make_shared<PunctuationToken>("(", LEFT_PARENTHESIS), make_shared<NameToken>("y"),
                           make_shared<RelationalOperatorToken>(">=", GTE),
                           make_shared<IntegerToken>("100"), make_shared<PunctuationToken>(")", RIGHT_PARENTHESIS)};
    auto expr_parser = ExpressionParserFactory::GetExpressionParser(expr_line, "if");
    auto actual = expr_parser->ParseEntity(expr_line);
    pair<shared_ptr<Expression>, shared_ptr<Expression>> lhs_rel_args;
    lhs_rel_args.first = make_shared<Variable>("x");
    lhs_rel_args.second = make_shared<Variable>("y");
    shared_ptr<RelationalOperation> lhs_rel_expr = make_shared<RelationalOperation>("<", lhs_rel_args);
    pair<shared_ptr<Expression>, shared_ptr<Expression>> lhs_cond_args;
    lhs_cond_args.first = lhs_rel_expr;
    shared_ptr<ConditionalOperation> lhs_cond_expr = make_shared<ConditionalOperation>("rel_expr", lhs_cond_args);

    pair<shared_ptr<Expression>, shared_ptr<Expression>> rhs_rel_args;
    rhs_rel_args.first = make_shared<Variable>("y");
    rhs_rel_args.second = make_shared<Constant>("100");
    shared_ptr<RelationalOperation> rhs_rel_expr = make_shared<RelationalOperation>(">=", rhs_rel_args);
    pair<shared_ptr<Expression>, shared_ptr<Expression>> rhs_cond_args;
    rhs_cond_args.first = rhs_rel_expr;
    shared_ptr<ConditionalOperation> rhs_cond_expr = make_shared<ConditionalOperation>("rel_expr", rhs_cond_args);

    pair<shared_ptr<Expression>, shared_ptr<Expression>> root_cond_args;
    root_cond_args.first = lhs_cond_expr;
    root_cond_args.second = rhs_cond_expr;
    shared_ptr<ConditionalOperation> root_cond_expr = make_shared<ConditionalOperation>("||", root_cond_args);
    REQUIRE(actual->operator==(*root_cond_expr));
  }SECTION("Check if '(' cond_expr ') '&&' or '||' '(' cond_expr ')' [e.g. (!(x!=1)) && (!(x==1))] parses correctly") {
    Parser::Line
        expr_line{make_shared<PunctuationToken>("(", LEFT_PARENTHESIS), make_shared<ConditionalOperatorToken>("!", NOT),
                  make_shared<PunctuationToken>("(", LEFT_PARENTHESIS), make_shared<NameToken>("x"),
                  make_shared<RelationalOperatorToken>("!=", NE), make_shared<IntegerToken>("1"),
                  make_shared<PunctuationToken>(")", RIGHT_PARENTHESIS),
                  make_shared<PunctuationToken>(")", RIGHT_PARENTHESIS),
                  make_shared<ConditionalOperatorToken>("&&", AND),
                  make_shared<PunctuationToken>("(", LEFT_PARENTHESIS),
                  make_shared<ConditionalOperatorToken>("!", NOT), make_shared<PunctuationToken>("(", LEFT_PARENTHESIS),
                  make_shared<NameToken>("x"), make_shared<RelationalOperatorToken>("==", DOUBLE_EQUALS),
                  make_shared<IntegerToken>("1"), make_shared<PunctuationToken>(")", RIGHT_PARENTHESIS),
                  make_shared<PunctuationToken>(")", RIGHT_PARENTHESIS)
    };
    auto expr_parser = ExpressionParserFactory::GetExpressionParser(expr_line, "if");
    auto actual = expr_parser->ParseEntity(expr_line);

    pair<shared_ptr<Expression>, shared_ptr<Expression>> lhs_nested_cond_rel_args;
    lhs_nested_cond_rel_args.first = make_shared<Variable>("x");
    lhs_nested_cond_rel_args.second = make_shared<Constant>("1");
    shared_ptr<RelationalOperation> rel = make_shared<RelationalOperation>("!=", lhs_nested_cond_rel_args);
    pair<shared_ptr<Expression>, shared_ptr<Expression>> lhs_nested_cond_args;
    lhs_nested_cond_args.first = rel;
    shared_ptr<ConditionalOperation>
        lhs_nested_cond_expr = make_shared<ConditionalOperation>("rel_expr", lhs_nested_cond_args);
    pair<shared_ptr<Expression>, shared_ptr<Expression>> lhs_root_cond_args;
    lhs_root_cond_args.first = lhs_nested_cond_expr;
    shared_ptr<ConditionalOperation> lhs_root_cond_expr = make_shared<ConditionalOperation>("!", lhs_root_cond_args);

    pair<shared_ptr<Expression>, shared_ptr<Expression>> rhs_nested_cond_rel_args;
    rhs_nested_cond_rel_args.first = make_shared<Variable>("x");
    rhs_nested_cond_rel_args.second = make_shared<Constant>("1");
    rel = make_shared<RelationalOperation>("==", rhs_nested_cond_rel_args);
    pair<shared_ptr<Expression>, shared_ptr<Expression>> rhs_nested_cond_args;
    rhs_nested_cond_args.first = rel;
    shared_ptr<ConditionalOperation>
        rhs_nested_cond_expr = make_shared<ConditionalOperation>("rel_expr", rhs_nested_cond_args);
    pair<shared_ptr<Expression>, shared_ptr<Expression>> rhs_root_cond_args;
    rhs_root_cond_args.first = rhs_nested_cond_expr;
    shared_ptr<ConditionalOperation> rhs_root_cond_expr = make_shared<ConditionalOperation>("!", rhs_root_cond_args);

    pair<shared_ptr<Expression>, shared_ptr<Expression>> root_cond_args;
    root_cond_args.first = lhs_root_cond_expr;
    root_cond_args.second = rhs_root_cond_expr;
    shared_ptr<ConditionalOperation> root_cond_expr = make_shared<ConditionalOperation>("&&", root_cond_args);

    REQUIRE(actual->operator==(*root_cond_expr));
  }SECTION("Check if cond_expr with rel_factor (var_name) 'any rel_op' rel_factor (expr)"
           "(e.g. x > 1 + 2) parses correctly") {
    Parser::Line expr_line{make_shared<NameToken>("x"), make_shared<RelationalOperatorToken>(">", GT),
                           make_shared<IntegerToken>("1"), make_shared<ArithmeticOperatorToken>("+", PLUS),
                           make_shared<IntegerToken>("2")};
    auto expr_parser = ExpressionParserFactory::GetExpressionParser(expr_line, "while");
    auto actual = expr_parser->ParseEntity(expr_line);

    pair<shared_ptr<Expression>, shared_ptr<Expression>> expr_args;
    expr_args.first = make_shared<Constant>("1");
    expr_args.second = make_shared<Constant>("2");
    shared_ptr<ArithmeticOperation> expr = make_shared<ArithmeticOperation>("+", expr_args);

    pair<shared_ptr<Expression>, shared_ptr<Expression>> rel_args;
    rel_args.first = make_shared<Variable>("x");
    rel_args.second = expr;
    shared_ptr<RelationalOperation> rel_expr = make_shared<RelationalOperation>(">", rel_args);

    pair<shared_ptr<Expression>, shared_ptr<Expression>> cond_expr_args;
    cond_expr_args.first = rel_expr;
    shared_ptr<ConditionalOperation> root_cond_expr = make_shared<ConditionalOperation>("rel_expr", cond_expr_args);
    REQUIRE(actual->operator==(*root_cond_expr));
  }SECTION(
      "Check if rel_expr with complex arithmetic expression "
      "[e.g., (((24 + 25) * 26 ) + 27) - (28 + ( 29 / (30 + 31))) > 0] parses correctly ") {
    Parser::Line expr_line
        {make_shared<PunctuationToken>("(", LEFT_PARENTHESIS), make_shared<PunctuationToken>("(", LEFT_PARENTHESIS),
         make_shared<PunctuationToken>("(", LEFT_PARENTHESIS), make_shared<IntegerToken>("24"),
         make_shared<ArithmeticOperatorToken>("+", PLUS), make_shared<IntegerToken>("25"),
         make_shared<PunctuationToken>(")", RIGHT_PARENTHESIS), make_shared<ArithmeticOperatorToken>("*", MULTIPLY),
         make_shared<IntegerToken>("26"), make_shared<PunctuationToken>(")", RIGHT_PARENTHESIS),
         make_shared<ArithmeticOperatorToken>("+", PLUS), make_shared<IntegerToken>("27"),
         make_shared<PunctuationToken>(")", RIGHT_PARENTHESIS), make_shared<ArithmeticOperatorToken>("-", MINUS),
         make_shared<PunctuationToken>("(", LEFT_PARENTHESIS), make_shared<IntegerToken>("28"),
         make_shared<ArithmeticOperatorToken>("+", PLUS), make_shared<PunctuationToken>("(", LEFT_PARENTHESIS),
         make_shared<IntegerToken>("29"), make_shared<ArithmeticOperatorToken>("/", DIV),
         make_shared<PunctuationToken>("(", LEFT_PARENTHESIS), make_shared<IntegerToken>("30"),
         make_shared<ArithmeticOperatorToken>("+", PLUS), make_shared<IntegerToken>("31"),
         make_shared<PunctuationToken>(")", RIGHT_PARENTHESIS), make_shared<PunctuationToken>(")", RIGHT_PARENTHESIS),
         make_shared<PunctuationToken>(")", RIGHT_PARENTHESIS),
         make_shared<RelationalOperatorToken>(">", GT), make_shared<IntegerToken>("0")};
    auto expr_parser = ExpressionParserFactory::GetExpressionParser(expr_line, "if");
    auto actual = expr_parser->ParseEntity(expr_line);

    pair<shared_ptr<Expression>, shared_ptr<Expression>> arithmetic_t1_args;
    arithmetic_t1_args.first = make_shared<Constant>("24");
    arithmetic_t1_args.second = make_shared<Constant>("25");
    shared_ptr<ArithmeticOperation> arithmetic_t1 = make_shared<ArithmeticOperation>("+", arithmetic_t1_args);

    pair<shared_ptr<Expression>, shared_ptr<Expression>> arithmetic_t2_args;
    arithmetic_t2_args.first = arithmetic_t1;
    arithmetic_t2_args.second = make_shared<Constant>("26");
    shared_ptr<ArithmeticOperation>
        arithmetic_t2 = make_shared<ArithmeticOperation>("*", arithmetic_t2_args);

    pair<shared_ptr<Expression>, shared_ptr<Expression>> arithmetic_t3_args;
    arithmetic_t3_args.first = arithmetic_t2;
    arithmetic_t3_args.second = make_shared<Constant>("27");
    shared_ptr<ArithmeticOperation> arithmetic_t3 = make_shared<ArithmeticOperation>("+", arithmetic_t3_args);

    // (28 + ( 29 / (30 + 31))
    pair<shared_ptr<Expression>, shared_ptr<Expression>> arithmetic_t4_args;
    arithmetic_t4_args.first = make_shared<Constant>("30");
    arithmetic_t4_args.second = make_shared<Constant>("31");
    shared_ptr<ArithmeticOperation> arithmetic_t4 = make_shared<ArithmeticOperation>("+", arithmetic_t4_args);

    pair<shared_ptr<Expression>, shared_ptr<Expression>> arithmetic_t5_args;
    arithmetic_t5_args.first = make_shared<Constant>("29");
    arithmetic_t5_args.second = arithmetic_t4;
    shared_ptr<ArithmeticOperation> arithmetic_t5 = make_shared<ArithmeticOperation>("/", arithmetic_t5_args);

    pair<shared_ptr<Expression>, shared_ptr<Expression>> arithmetic_t6_args;
    arithmetic_t6_args.first = make_shared<Constant>("28");
    arithmetic_t6_args.second = arithmetic_t5;
    shared_ptr<ArithmeticOperation> arithmetic_t6 = make_shared<ArithmeticOperation>("+", arithmetic_t6_args);

    pair<shared_ptr<Expression>, shared_ptr<Expression>> arithmetic_root_args;
    arithmetic_root_args.first = arithmetic_t3;
    arithmetic_root_args.second = arithmetic_t6;
    shared_ptr<ArithmeticOperation> arithmetic_root = make_shared<ArithmeticOperation>("-", arithmetic_root_args);

    pair<shared_ptr<Expression>, shared_ptr<Expression>> relational_root_args;
    relational_root_args.first = arithmetic_root;
    relational_root_args.second = make_shared<Constant>("0");
    shared_ptr<RelationalOperation> relational_root = make_shared<RelationalOperation>(">", relational_root_args);

    pair<shared_ptr<Expression>, shared_ptr<Expression>> conditional_root_args;
    conditional_root_args.first = relational_root;
    shared_ptr<ConditionalOperation> conditional_root = make_shared<ConditionalOperation>("rel_expr",
                                                                                        conditional_root_args);
    REQUIRE(actual->operator==(*conditional_root));
  }SECTION("Check if cond_expr with missing RHS cond_expr [e.g. (x < y) && ] throws syntax error") {
    Parser::Line expr_line{make_shared<PunctuationToken>("(", LEFT_PARENTHESIS), make_shared<NameToken>("x"),
                           make_shared<RelationalOperatorToken>("<", LT),
                           make_shared<NameToken>("y"), make_shared<PunctuationToken>(")", RIGHT_PARENTHESIS),
                           make_shared<ConditionalOperatorToken>("&&", AND)};
    auto expr_parser = ExpressionParserFactory::GetExpressionParser(expr_line, "if");
    REQUIRE_THROWS_AS(expr_parser->ParseEntity(expr_line), SyntaxErrorException);
  }SECTION("Check if cond_expr with unbalanced () [e.g. (x < y) && (z <= 100))] throws syntax error") {
    Parser::Line expr_line{make_shared<PunctuationToken>("(", LEFT_PARENTHESIS), make_shared<NameToken>("x"),
                           make_shared<RelationalOperatorToken>("<", LT),
                           make_shared<NameToken>("y"), make_shared<PunctuationToken>(")", RIGHT_PARENTHESIS),
                           make_shared<ConditionalOperatorToken>("&&", AND),
                           make_shared<PunctuationToken>("(", LEFT_PARENTHESIS), make_shared<NameToken>("z"),
                           make_shared<RelationalOperatorToken>("<=", LTE),
                           make_shared<IntegerToken>("100"), make_shared<PunctuationToken>(")", RIGHT_PARENTHESIS),
                           make_shared<PunctuationToken>(")", RIGHT_PARENTHESIS)};
    auto expr_parser = ExpressionParserFactory::GetExpressionParser(expr_line, "if");
    auto expr = expr_parser->ParseEntity(expr_line);
    REQUIRE_THROWS_AS(expr_parser->ParseEntity(expr_line), SyntaxErrorException);
  }
  SECTION("Check if cond_expr [  ( ! (a != 0) ) && (0 > 1) ] parses without syntax error") {
    Parser::Line expr_line{
        make_shared<PunctuationToken>("(", LEFT_PARENTHESIS), make_shared<ConditionalOperatorToken>("!", NOT),
        make_shared<PunctuationToken>("(", LEFT_PARENTHESIS), make_shared<NameToken>("a"),
        make_shared<RelationalOperatorToken>("!=", NE), make_shared<IntegerToken>("0"),
        make_shared<PunctuationToken>(")", RIGHT_PARENTHESIS), make_shared<PunctuationToken>(")", RIGHT_PARENTHESIS),
        make_shared<ConditionalOperatorToken>("&&", AND), make_shared<PunctuationToken>("(", LEFT_PARENTHESIS),
        make_shared<IntegerToken>("0"), make_shared<RelationalOperatorToken>(">", GT),
        make_shared<IntegerToken>("1"), make_shared<PunctuationToken>(")", RIGHT_PARENTHESIS),
        make_shared<PunctuationToken>(")", RIGHT_PARENTHESIS)
    };
    auto expr_parser = ExpressionParserFactory::GetExpressionParser(expr_line, "if");
    REQUIRE_NOTHROW(expr_parser->ParseEntity(expr_line));
  }
}
