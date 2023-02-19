#include "PrintStatementParser.h"

shared_ptr<Statement> PrintStatementParser::ParseEntity(TokenStream &tokens) {
  auto line = tokens.front();
  tokens.pop_front();
  std::string var_name = ExtractVariableName(line);
  Variable var(var_name);
  auto print_stmt =
      make_shared<PrintStatement>(Program::GetAndIncreaseStatementNumber(), var, "main");
  CheckEndOfStatement(line);
  return print_stmt;
}

std::string PrintStatementParser::ExtractVariableName(Line &line) const {
  auto print_keyword_itr =
      std::find_if(std::begin(line), std::end(line), [&](shared_ptr<Token> const p) {
        return p->GetValue() == "print";
      });

  if (print_keyword_itr != line.begin()) {
    throw SyntaxErrorException("Print statement should start with print keyword");
  }

  if (line.size() < 2) {
    throw SyntaxErrorException("Print statement does not have a variable");
  }

  return line[1]->GetValue();

}

void PrintStatementParser::CheckEndOfStatement(Line &line) const {
  if ((*prev(line.end()))->GetValue() != ";") {
    throw SyntaxErrorException("AssignStatement does not end with ;");
  }

  if (line.size() > 3) {
    throw SyntaxErrorException("Too many tokens in a PrintStatement");
  }
}