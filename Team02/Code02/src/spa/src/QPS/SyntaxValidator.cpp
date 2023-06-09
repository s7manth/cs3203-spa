#include "SyntaxValidator.h"

#include <utility>

void SyntaxValidator::ValidateSelectSyntax(SelectedSynonymTuple &synonym_vector) {
  for (auto &syn : synonym_vector) {
    if (!QueryUtil::IsSynonym(syn) && !QueryUtil::IsAttrRef(syn)) {
      throw SyntaxErrorException("Not a valid syn or attr-ref");
    }
  }
}

void SyntaxValidator::ValidateSuchThatClauseSyntax(std::shared_ptr<ClauseSyntax> clause) {
  std::shared_ptr<SuchThatBaseHandler> handler_1 = std::make_shared<SuchThatBaseHandler>();
  std::shared_ptr<StmtToStmtHandler> handler_2 = std::make_shared<StmtToStmtHandler>();
  std::shared_ptr<StmtProcToVarHandler> handler_3 = std::make_shared<StmtProcToVarHandler>();
  std::shared_ptr<CallsHandler> handler_4 = std::make_shared<CallsHandler>();

  handler_1->SetNext(handler_2)->SetNext(handler_3)->SetNext(handler_4);

  handler_1->HandleSyntax(std::move(clause));
}

void SyntaxValidator::ValidatePatternClauseSyntax(std::shared_ptr<ClauseSyntax> clause) {
  std::shared_ptr<PatternHandler> handler = std::make_shared<PatternHandler>();

  handler->HandleSyntax(std::move(clause));
}

void SyntaxValidator::ValidateWithClauseSyntax(std::shared_ptr<ClauseSyntax> clause) {
  std::shared_ptr<WithHandler> handler = std::make_shared<WithHandler>();

  handler->HandleSyntax(std::move(clause));
}
