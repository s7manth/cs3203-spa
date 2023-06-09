#include "OperationParser.h"

void OperationParser::Setup(Line &line) {
  if (*pos_ == k_min_pos_) {
    line_ = line;
    GetNext();
  }

  if (is_inherit_args_) {
    line_ = line;
    curr_token_ = line_.at(*pos_ - 1);
    curr_token_value_ = curr_token_->GetValue();
  }
}

void OperationParser::UpdateCurrTokenWithUpdatedPos() {
  curr_token_ =  line_.at(*pos_ - 1);
  curr_token_value_ = curr_token_->GetValue();
}

void OperationParser::GetNext() {
  if (*pos_ < line_.size()) {
    curr_token_ =  line_.at(*pos_);
    curr_token_value_ = curr_token_->GetValue();
    (*is_processed_curr_token_) = false;
    (*pos_)++;
  }
}

shared_ptr<int> OperationParser::GetPos() {
  return pos_;
}

shared_ptr<ExpressionParser::Line> OperationParser::GetLine() {
  return make_shared<ExpressionParser::Line>(line_);
}

void OperationParser::InheritArgs(shared_ptr<int> pos, bool is_sub_expr, shared_ptr<bool> is_processed_curr_token) {
  is_inherit_args_ = true;
  pos_ = pos;
  is_sub_expr_ = is_sub_expr;
  is_processed_curr_token_ = is_processed_curr_token;
}

bool OperationParser::IsEndOfLine() {
  return *pos_ == line_.size();
}

TokenType OperationParser::GetCurrentTokenType() {
  (*is_processed_curr_token_) = true;
  return curr_token_->GetType();
}

string OperationParser::GetCurrentTokenValue() {
  (*is_processed_curr_token_) = true;
  return curr_token_value_;
}

int OperationParser::GetCurrentTokenPos() {
  return (*pos_) - 1;
}

shared_ptr<Expression> OperationParser::ParseEntity(TokenStream &tokens) {
  return ParseEntity(tokens.front());
}

shared_ptr<Expression> OperationParser::ParseEntity(Line &line) {
  Setup(line);
  shared_ptr<Expression> result = Parse();
  ValidateForBalancedParenthesis();
  return result;
}

bool OperationParser::GetIsSubExpr() {
  return is_sub_expr_;
}

void OperationParser::SetIsSubExpr(bool is_sub_expr) {
  is_sub_expr_ = is_sub_expr;
}

shared_ptr<bool> OperationParser::GetIsProcessedCurrToken() {
  return is_processed_curr_token_;
}

void OperationParser::AddParenthesis(TokenType type, string val, int token_pos) {
  if (parentheses_pos_mappings_.find(token_pos) != parentheses_pos_mappings_.end()) {
    // found i.e., added before --> don't add again
    return;
  }

  parentheses_pos_mappings_[token_pos] = val;
  if (type == LEFT_PARENTHESIS) {
    parentheses_container_.push(val);
  } else if (type == RIGHT_PARENTHESIS) {
    parentheses_container_.pop();
  }
}

void OperationParser::ValidateForBalancedParenthesis() {
  if (is_sub_expr_ || (IsEndOfLine() && *is_processed_curr_token_ && parentheses_container_.empty())) {
    return;
  }
  throw SyntaxErrorException("Unbalanced parenthesis ()");
}

void OperationParser::ValidateEnoughTokensToProcess() {
  if (*is_processed_curr_token_) {
    throw SyntaxErrorException("Insufficient tokens to process");
  }
}
