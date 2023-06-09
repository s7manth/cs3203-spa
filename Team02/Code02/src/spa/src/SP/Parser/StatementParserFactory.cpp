#include "StatementParserFactory.h"

shared_ptr<StatementParser> StatementParserFactory::GetStatementParser(std::deque<
    StatementParserFactory::Line> &tokens) {
  auto line = tokens.front();
  if (CheckKeywordType(line, sp_constants::k_if_stmt_, true)) {
    return make_shared<IfStatementParser>();
  } else if (CheckKeywordType(line, sp_constants::k_while_stmt_, true)) {
    return make_shared<WhileStatementParser>();
  } else if (CheckAssignmentType(line)) {
    return make_shared<AssignStatementParser>();
  } else if (CheckKeywordType(line, sp_constants::k_print_stmt_, false)) {
    return make_shared<PrintStatementParser>();
  } else if (CheckKeywordType(line, sp_constants::k_read_stmt_, false)) {
    return make_shared<ReadStatementParser>();
  } else if (CheckKeywordType(line, sp_constants::k_call_stmt_, false)) {
    return make_shared<CallStatementParser>();
  }
  throw SemanticErrorException("Unknown Statement type");
}

bool StatementParserFactory::CheckKeywordType(Line &line,
                                              std::string_view type_to_check,
                                              bool has_parenthesis) {
  if (line.size() < k_min_stmt_size_) {
    throw SyntaxErrorException("Invalid statement");
  }
  auto entity_itr =
      std::find_if(std::begin(line),
                   std::end(line),
                   [&](shared_ptr<Token> const &p) {
                     return p->GetValue() == type_to_check;
                   });

  if (has_parenthesis) {
    return entity_itr == std::begin(line)
        && next(entity_itr)->get()->GetType() == LEFT_PARENTHESIS;
  }
  return entity_itr == std::begin(line);
}

bool StatementParserFactory::CheckAssignmentType(Line &line) {
  auto entity_itr =
      std::find_if(std::begin(line),
                   std::end(line),
                   [&](shared_ptr<Token> const &p) {
                     return p->GetType() == SINGLE_EQUAL;
                   });

  return entity_itr != std::end(line);
}

