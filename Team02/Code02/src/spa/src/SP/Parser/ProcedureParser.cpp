#include <string>

#include "ProcedureParser.h"

shared_ptr<Procedure> ProcedureParser::ParseEntity(TokenStream &tokens) {
  auto line = tokens.front();
  tokens.pop_front();
  std::string proc_name = ExtractProcName(line);
  auto proc = make_shared<Procedure>(proc_name);
  line = tokens.front();

  // Adding Statements of a Procedure
  while (!IsProcedureEnd(line)) {
    auto stmt_parser = StatementParserFactory::GetStatementParser(tokens);
    stmt_parser->SetProcName(proc_name);
    auto stmt = stmt_parser->ParseEntity(tokens);
    proc->AddToStatementList(stmt);
    line = tokens.front();
  }
  if (IsProcedureEnd(line)) {
    tokens.pop_front();
  }

  return proc;
}

std::string ProcedureParser::ExtractProcName(Line &line) {
  if (line[k_pos_proc_token_]->GetValue() != sp_constants::k_procedure_ || line.size() < k_min_tokens_) {
    throw SyntaxErrorException("A procedure Line should start with procedure");
  }
  if (line[k_pos_proc_name_]->GetType() != NAME) {
    throw SyntaxErrorException("proc_name should be a NAME");
  }
  return line[k_pos_proc_name_]->GetValue();
}

bool ProcedureParser::IsProcedureEnd(Line &line) {
  // Check if the current line contains '}'
  return std::find_if(std::begin(line), std::end(line),
                      [&](shared_ptr<Token> const p) {
                        return p->GetType() == RIGHT_BRACE;
                      }) != std::end(line);
}
