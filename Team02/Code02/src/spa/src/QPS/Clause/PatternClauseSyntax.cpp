#include "PatternClauseSyntax.h"

PatternClauseSyntax::PatternClauseSyntax(const SyntaxPair& pair) : ClauseSyntax(pair) {}

bool PatternClauseSyntax::Equals(ClauseSyntax &other) {
  // make sure that the passed type is the same
  auto *pc = dynamic_cast<PatternClauseSyntax*>(&other);
  if (pc) {
    return (this->GetEntity() == pc->GetEntity()) && (this->GetFirstParameter() == pc->GetFirstParameter()) &&
        (this->GetSecondParameter() == pc->GetSecondParameter());
  }
  return false;
}

bool PatternClauseSyntax::IsBooleanClause(Map &declaration_map) {
  return false;
}

std::shared_ptr<ClauseEvaluator> PatternClauseSyntax::CreateClauseEvaluator(Map &declaration_map,
                                                                            std::shared_ptr<PkbReadFacade> pkb
                                                                            ) {
  std::string syn = declaration_map.at(ClauseSyntax::GetEntity());
  std::shared_ptr<ClauseEvaluator> eval;
  if (syn == pql_constants::kPqlAssignEntity) {
    eval = std::make_shared<AssignPatternClauseEvaluator>(declaration_map,
                                                          ClauseSyntax::GetSyntaxPair(),
                                                          pkb,
                                                          ClauseSyntax::GetExpression());
  } else if (syn == pql_constants::kPqlIfEntity) {
    eval = std::make_shared<IfPatternClauseEvaluator>(declaration_map, ClauseSyntax::GetSyntaxPair(), pkb);
  } else {
    eval = std::make_shared<WhilePatternClauseEvaluator>(declaration_map, ClauseSyntax::GetSyntaxPair(), pkb);
  }

  return eval;
}

int PatternClauseSyntax::GetClauseScore(Map &declaration_map) {
  std::string entity = declaration_map.at(ClauseSyntax::GetEntity());
  auto pattern_score_map = pql_constants::kPatternScoreMap;
  return pattern_score_map[entity];
}
