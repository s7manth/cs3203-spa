#pragma once

#include "QPS/Evaluator/SuchThatClauseEvaluator/SuchThatClauseEvaluator.h"
#include "PKB/Types/PkbCommunicationTypes.h"

class FollowsStarClauseEvaluator : public SuchThatClauseEvaluator {
 private:
  bool is_same_syn_or_value_pairs_;

 public:
  FollowsStarClauseEvaluator(Map d,
                             SyntaxPair syntax_pair,
                             std::shared_ptr<PkbReadFacade> pkb
                             ) : SuchThatClauseEvaluator(std::move(d), std::move(syntax_pair), std::move(pkb)) {
    is_same_syn_or_value_pairs_ = first_arg_ == second_arg_;
  }

  bool HandleBothWildcard() override;
  bool HandleFirstWildcardSecondValue() override;
  bool HandleFirstValueSecondWildcard() override;
  bool HandleBothValue() override;
  ResultTable HandleBothSynonym() override;
  ResultTable HandleFirstSynonymSecondWildcard() override;
  ResultTable HandleFirstSynonymSecondValue() override;
  ResultTable HandleFirstWildcardSecondSynonym() override;
  ResultTable HandleFirstValueSecondSynonym() override;
};
