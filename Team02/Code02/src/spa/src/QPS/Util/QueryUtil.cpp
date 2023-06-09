#include "QPS/Util/QueryUtil.h"

bool QueryUtil::IsPartialMatchExpressionSpecification(const std::string& s) {
  bool result = s[0] == pql_constants::kWildcardChar && s[s.length() - 1] == pql_constants::kWildcardChar &&
      s.length() > 2;
  return result;
}

bool QueryUtil::IsQuoted(const std::string& s) {
  return s.length() >= 2 && (s[0] == pql_constants::kQuotationChar) &&
  (s[s.length()-1] == pql_constants::kQuotationChar);
}

bool QueryUtil::IsQuotedIdent(const std::string &s) {
  return (s[0] == pql_constants::kQuotationChar && LexicalRuleValidator::IsIdent(s.substr(1, s.length()
  - 2)) && s[s.length()-1] == pql_constants::kQuotationChar);
}

bool QueryUtil::IsWildcard(const std::string& s) {
  std::string wildcard(1, pql_constants::kWildcardChar);
  return s == wildcard;
}

bool QueryUtil::IsAttrRef(const std::string& s) {
  std::vector<std::string> token_lst = SplitAttrRef(s);
  if (token_lst.size() != 2 || !IsSynonym(token_lst[0])
  || pql_constants::kAttrName.find(token_lst[1]) == pql_constants::kAttrName.end()) {
    return false;
  }
  return true;
}

bool QueryUtil::IsSynonym(const std::string& s) {
  return LexicalRuleValidator::IsIdent(s);
}

bool QueryUtil::IsStmtRef(const std::string& s) {
  return (IsWildcard(s) || IsSynonym(s) || LexicalRuleValidator::IsInteger(s));
}

bool QueryUtil::IsEntRef(const std::string& s) {
  return (IsWildcard(s) || IsSynonym(s) || IsQuotedIdent(s));
}

bool QueryUtil::IsRef(const std::string &s) {
  return IsQuotedIdent(s) || LexicalRuleValidator::IsInteger(s) || IsAttrRef(s);
}

bool QueryUtil::IsDesignEntity(const std::string& s) {
  return pql_constants::kDesignEntities.find(s) != pql_constants::kDesignEntities.end();
}

bool QueryUtil::IsVariableSynonym(Map &declaration, const std::string& expression) {
  return IsCorrectSynonymType(declaration, expression, pql_constants::kPqlVariableEntity);
}

bool QueryUtil::IsConstantSynonym(Map &declaration, const std::string& expression) {
  return IsCorrectSynonymType(declaration, expression, pql_constants::kPqlConstantEntity);
}


bool QueryUtil::IsStatementSynonym(Map &declaration, const std::string& expression) {
  return IsCorrectSynonymType(declaration, expression, pql_constants::kPqlStatementEntity);
}

bool QueryUtil::IsReadSynonym(Map &declaration, const std::string& expression) {
  return IsCorrectSynonymType(declaration, expression, pql_constants::kPqlReadEntity);
}

/*
* Checks if the expression is a print synonym
*/
bool QueryUtil::IsPrintSynonym(Map &declaration, const std::string& expression) {
  return IsCorrectSynonymType(declaration, expression, pql_constants::kPqlPrintEntity);
}

bool QueryUtil::IsWhileSynonym(Map &declaration, const std::string& expression) {
  return IsCorrectSynonymType(declaration, expression, pql_constants::kPqlWhileEntity);
}

/*
* Checks if the expression is an if synonym
*/
bool QueryUtil::IsIfSynonym(Map &declaration, const std::string& expression) {
  return IsCorrectSynonymType(declaration, expression, pql_constants::kPqlIfEntity);
}

bool QueryUtil::IsAssignSynonym(Map &declaration, const std::string& expression) {
  return IsCorrectSynonymType(declaration, expression, pql_constants::kPqlAssignEntity);
}

bool QueryUtil::IsProcedureSynonym(Map &declaration, const std::string& expression) {
  return IsCorrectSynonymType(declaration, expression, pql_constants::kPqlProcedureEntity);
}

bool QueryUtil::IsCorrectSynonymType(Map &declaration, const std::string &expression, const std::string& type) {
  if (declaration.count(expression) == 0) {
    return false;
  }

  std::string t = declaration[expression];
  return t == type;
}

std::string QueryUtil::RemoveQuotations(const std::string& quoted_ident) {
  return string_util::Trim(quoted_ident.substr(1, quoted_ident.length() - 2));
}

StatementType QueryUtil::GetStatementType(Map &declaration, const std::string& synonym) {
  if (declaration.count(synonym) == 0) {
    return StatementType::UNK;
  }
  auto entity = declaration[synonym];
  return pql_constants::kEntityToStatementType.at(entity);
}

std::vector<std::string> QueryUtil::SplitAttrRef(const std::string &s) {
  std::stringstream attr_ref(s);
  std::vector<std::string> token_lst;

  for (std::string token; std::getline(attr_ref, token, '.'); ) {
    token_lst.push_back(string_util::Trim(token));
  }

  return token_lst;
}


std::string QueryUtil::GetAttrNameFromAttrRef(const std::string& attrRef) {
  auto token_lst = SplitAttrRef(attrRef);
  return token_lst[1];
}

std::string QueryUtil::GetSynonymFromAttrRef(const std::string& attrRef) {
  auto token_lst = SplitAttrRef(attrRef);
  return token_lst[0];
}


std::string QueryUtil::AdjustSynonymWithTrivialAttrRefValue(Synonym syn, Map &declaration_map) {
  auto token_lst = QueryUtil::SplitAttrRef(syn);
  if (token_lst.size() == 2) {
    bool is_trivial_attr_name = IsTrivialAttrRef(token_lst, declaration_map);
    if (is_trivial_attr_name) {
      return token_lst[0];
    }
  }
  return syn;
}

bool QueryUtil::IsTrivialAttrRef(std::vector<std::string> attr_ref_token_lst, Map &declaration_map) {
  // remove trivial attr name e.g. r.stmt# is same as r except for c.procName, read.varName and print.varName
  bool is_call_proc_name_attr_ref = (declaration_map[attr_ref_token_lst[0]] == pql_constants::kPqlCallEntity)
      && (attr_ref_token_lst[1] == pql_constants::kProcName);
  bool is_read_var_name_attr_ref = (declaration_map[attr_ref_token_lst[0]] == pql_constants::kPqlReadEntity)
      && (attr_ref_token_lst[1] == pql_constants::kVarname);
  bool is_print_var_name_attr_ref = (declaration_map[attr_ref_token_lst[0]] == pql_constants::kPqlPrintEntity)
      && (attr_ref_token_lst[1] == pql_constants::kVarname);
  return !is_call_proc_name_attr_ref && !is_read_var_name_attr_ref && !is_print_var_name_attr_ref;
}

bool QueryUtil::IsAffectsReturnsEmpty(const std::string& first_arg, const std::string& second_arg, Map
&declaration_map) {
  bool is_first_arg_an_invalid_syn = IsSynonym(first_arg)
      && !IsAssignSynonym(declaration_map, first_arg)
      && !IsStatementSynonym(declaration_map, first_arg);
  bool is_second_arg_an_invalid_syn = IsSynonym(second_arg)
      && !IsAssignSynonym(declaration_map, second_arg)
      && !IsStatementSynonym(declaration_map, second_arg);
  return is_first_arg_an_invalid_syn || is_second_arg_an_invalid_syn;
}

bool QueryUtil::IsMatchingEntities(const std::string& first_arg, const std::string& second_arg) {
  return (first_arg == second_arg) && !IsWildcard(first_arg);
}

bool QueryUtil::IsContainerSynonym(const std::string& arg,  Map
&declaration_map) {
  return QueryUtil::IsIfSynonym(declaration_map, arg) ||
      QueryUtil::IsWhileSynonym(declaration_map, arg)
      || QueryUtil::IsStatementSynonym(declaration_map, arg);
}
