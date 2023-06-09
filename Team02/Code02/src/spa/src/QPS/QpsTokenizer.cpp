#include "QpsTokenizer.h"

QpsTokenizer::QpsTokenizer() : syntax_validator_(new SyntaxValidator()),
semantic_validator_(new SemanticValidator()) {}

std::string QpsTokenizer::GetRegexMatch(const std::string& clause, const std::regex& rgx) {
  std::smatch match;
  std::regex_search(clause, match, rgx);
  if (match.empty()) {
    return "";
  }
  return match[0];
}

size_t QpsTokenizer::FindIndexOfRegexMatch(const std::string& clause, const std::regex& rgx) {
  auto match = GetRegexMatch(clause, rgx);
  if (match.empty()) {
    return std::string::npos;
  }
  return clause.find(match);
}

QueryLinesPair QpsTokenizer::SplitQuery(const std::string& query_extra_whitespace_removed) {
  std::string temp = query_extra_whitespace_removed;
  auto pair = string_util::SplitStringByDelimiter(temp, pql_constants::kSemicolon);
  auto declaration_statements = pair.first;
  auto select_statement = pair.second;
  if (select_statement.empty()) {
    throw SyntaxErrorException("There is no select statement identified");
  }

  if (FindIndexOfRegexMatch(select_statement, pql_constants::kSelectRegex) != 0) {
    throw SyntaxErrorException("There is no valid Select keyword in the Select statement");
  }

  QueryLinesPair declaration_select_statement_pair;
  declaration_select_statement_pair.first = declaration_statements;
  declaration_select_statement_pair.second = select_statement;
  return declaration_select_statement_pair;
}

std::unordered_map<std::string, std::string> QpsTokenizer::ExtractAbstractSyntaxFromDeclarations(
    const std::vector<std::string>& declarations) const {
  std::unordered_map<std::string, std::string> synonym_to_design_entity_map = {};
  std::string design_entity;

  for (const std::string &kDeclaration : declarations) {
    design_entity = string_util::GetFirstWord(kDeclaration);

    if (!QueryUtil::IsDesignEntity(design_entity)) {
      throw SyntaxErrorException("The design entity does not adhere to the lexical rules of design entity");
    }
    std::string synonym_substring = string_util::GetSubStringAfterKeyword(kDeclaration, design_entity);
    if (synonym_substring.empty()) {
      throw SyntaxErrorException("Missing synonym in declaration");
    }

    auto pair = string_util::SplitStringByDelimiter(synonym_substring,
                                                    pql_constants::kComma);
    auto synonym_list = pair.first;
    if (pair.second.length() > 0) {
      synonym_list.push_back(pair.second);
    } else {
      throw SyntaxErrorException("Extra delimiter");
    }

    for (const std::string &kSynonym : synonym_list) {
      if (!QueryUtil::IsSynonym(kSynonym)) {
        throw SyntaxErrorException("The synonym in the declaration does not adhere "
                                   "to the lexical rules of being a synonym");
      }
      if (synonym_to_design_entity_map.find(kSynonym) != synonym_to_design_entity_map.end()) {
        // we want to throw syntax exception first if there are any but this will mean that we will continue to loop
        // and parse the declarations, which takes extra time
        semantic_validator_->SetHasSemanticError(true);
      }
      synonym_to_design_entity_map.insert({kSynonym, design_entity});
    }
  }
  semantic_validator_->SetDeclaration(synonym_to_design_entity_map);

  return synonym_to_design_entity_map;
}


SyntaxPair QpsTokenizer::ExtractAbstractSyntaxFromClause(const std::string& clause) {
  size_t opening_bracket_index = clause.find(pql_constants::kOpeningBracket);
  size_t comma_index = clause.find(pql_constants::kComma);
  size_t closing_bracket_index = clause.find_last_of(pql_constants::kClosingBracket);

  // check for concrete syntax like ( , ) and keywords like such that or pattern
  if ((opening_bracket_index == std::string::npos) ||
      (comma_index == std::string::npos) || (closing_bracket_index == std::string::npos)) {
    throw SyntaxErrorException("There is syntax error with the such that or pattern subclauses");
  }

  std::string relationship = string_util::Trim(clause.substr(0,
                                                             opening_bracket_index));
  std::string parameters_substr = string_util::Trim(clause.substr(opening_bracket_index+1,
                                                                  closing_bracket_index-
                                                                  (opening_bracket_index+1)));

  auto parameters = ParseParameters(parameters_substr);

  std::string remaining_clause = string_util::Trim(clause.substr(closing_bracket_index+1));

  if (!remaining_clause.empty()) {
    throw SyntaxErrorException("There is syntax error with the subclauses and"
                               " remaining characters that remain unparsed");
  }

  SyntaxPair clause_syntax;
  clause_syntax.first = relationship;
  clause_syntax.second = parameters;
  return clause_syntax;
}


std::string QpsTokenizer::ParseIDENT(const std::string& parameter) {
  std::string quotation(1, pql_constants::kQuotationChar);
  return quotation + string_util::Trim(parameter.substr(1, parameter.length() - 2)) + quotation;
}

std::string QpsTokenizer::ParseAttrRef(const std::string& attrRef) {
  auto token_lst = QueryUtil::SplitAttrRef(attrRef);
  return token_lst[0] + pql_constants::kFullStop + token_lst[1];
}

std::string QpsTokenizer::ParseWithClauseParameter(std::string parameter) {
  if (QueryUtil::IsQuoted(parameter)) {
     return ParseIDENT(parameter);
  } else if (QueryUtil::IsAttrRef(parameter)) {
      return ParseAttrRef(parameter);
  } else {
    return parameter;
  }
}

SyntaxPair QpsTokenizer::ExtractAbstractSyntaxFromWithClause(const std::string& clause) {
  size_t equal_index = clause.find(pql_constants::kEqual);

  // check for concrete syntax like ( , ) and keywords like such that or pattern
  if (equal_index == std::string::npos) {
    throw SyntaxErrorException("There is no = in the subclause");
  }

  std::string first_parameter = string_util::Trim(clause.substr(0, equal_index));
  std::string second_parameter = string_util::Trim(clause.substr(equal_index+1));

  first_parameter = ParseWithClauseParameter(first_parameter);
  second_parameter = ParseWithClauseParameter(second_parameter);

  SyntaxPair clause_syntax;
  ParameterVector parameters;
  parameters.push_back(first_parameter);
  parameters.push_back(second_parameter);
  clause_syntax.first = "";
  clause_syntax.second = parameters;

  return clause_syntax;
}

SelectedSynonymTuple QpsTokenizer::ParseSynonym(const std::string& clause_with_select_removed, Map declaration_map) {
  std::string first_word = string_util::GetFirstWord(clause_with_select_removed);
  std::string clause_after_first_word = string_util::GetSubStringAfterKeyword(clause_with_select_removed,
                                                                              first_word);
  SelectedSynonymTuple synonym_vector;
  // BOOLEAN case
  if ((first_word == pql_constants::kSelectBoolean)
  && (declaration_map.find(first_word) == declaration_map.end())) {
    synonym_vector = {};
  } else if (first_word.substr(0, 1) == pql_constants::kTupleOpeningBracket) {
    // Multiple elem case
    synonym_vector = ParseForMultipleSynonyms(clause_with_select_removed);
  } else {
    synonym_vector = ParseSingleSynonym(clause_with_select_removed);
  }

  syntax_validator_->ValidateSelectSyntax(synonym_vector);
  semantic_validator_->ValidateSelectSemantics(synonym_vector);

  return synonym_vector;
}

SelectedSynonymTuple QpsTokenizer::HandleSingleAttrRef(const std::string& syn, const std::string& clause_after_syn) {
  std::string attrName;
  // remove the dot
  std::string attrName_substr = string_util::Trim(clause_after_syn.substr(1));
  std::string first_word = string_util::GetFirstWord(attrName_substr);
  if (first_word.empty()) {
    // case where there is no clause substring
    attrName = string_util::Trim(clause_after_syn.substr(1));
  } else {
    attrName = first_word;
  }
  return {syn + pql_constants::kFullStop + attrName};
}

SelectedSynonymTuple QpsTokenizer::ParseSingleSynonym(const std::string& syn_substring) {
  std::string syn;
  for (auto c : syn_substring) {
    if (c == pql_constants::kFullStopChar) {
      break;
    } else if (std::isspace(c)) {
      break;
    } else {
      syn += c;
    }
  }
  std::string clause_after_first_word = string_util::GetSubStringAfterKeyword(syn_substring, syn);
  auto first_char = clause_after_first_word.substr(0, 1);
  if (first_char == pql_constants::kFullStop) {
    return HandleSingleAttrRef(syn, clause_after_first_word);
  } else {
    return {syn};
  }
}


SelectedSynonymTuple QpsTokenizer::ParseForMultipleSynonyms(const std::string& trimmed_select_keyword_removed_clause) {
  size_t closing_tuple_bracket_index = trimmed_select_keyword_removed_clause
      .find(pql_constants::kTupleClosingBracket);
  if (closing_tuple_bracket_index == std::string::npos) {
    throw SyntaxErrorException("No closing bracket for tuple");
  }
  // condition to run this function is that < is at index 0
  std::string synonyms_seperated_by_comma_substr = string_util::Trim(trimmed_select_keyword_removed_clause
      .substr(1, closing_tuple_bracket_index-1));

  auto pair = string_util::SplitStringByDelimiter(synonyms_seperated_by_comma_substr,
                                                  pql_constants::kComma);
  auto synonym_vector = pair.first;
  if (pair.second.length() > 0) {
    synonym_vector.push_back(pair.second);
  } else {
    throw SyntaxErrorException("Extra delimiter");
  }

  for (int i = 0; i < synonym_vector.size(); i++) {
    auto synonym = synonym_vector[i];
    if (synonym.find(pql_constants::kFullStop) != std::string::npos) {
      synonym_vector[i] = ParseAttrRef(synonym);
    }
  }

  return synonym_vector;
}

std::string QpsTokenizer::GetSubclauseString(const std::string& clause_with_select_removed,
                                             SelectedSynonymTuple syn_vector) {
  std::string first_word = string_util::GetFirstWord(clause_with_select_removed);
  std::string remaining_clause;
  size_t index_start;
  // BOOLEAN case
  if ((first_word == pql_constants::kSelectBoolean)) {
    index_start = clause_with_select_removed.find(pql_constants::kSelectBoolean)
        + pql_constants::kSelectBoolean.size();
  } else if (first_word.substr(0, 1) == pql_constants::kTupleOpeningBracket) {
    // Multiple elem case
    index_start = clause_with_select_removed.find(pql_constants::kTupleClosingBracket)
        + pql_constants::kTupleClosingBracket.size();
  } else {
    auto syn = syn_vector[0];
    if (QueryUtil::IsAttrRef(syn)) {
      auto attrName = QueryUtil::GetAttrNameFromAttrRef(syn);
      index_start = clause_with_select_removed.find(attrName) + attrName.size();
    } else {
      index_start = clause_with_select_removed.find(syn) + syn.size();
    }
  }
  remaining_clause = string_util::Trim(clause_with_select_removed.substr(index_start));
  return remaining_clause;
}

size_t QpsTokenizer::FindEndOfSubClauseStart(const std::string& clause, const std::regex& rgx) {
  auto match = GetRegexMatch(clause, rgx);
  if (match.empty()) {
    return std::string::npos;
  }
  // -1 because regex matches an additional character after e.g. pattern a --> match
  // reason why we must match one more character is because a valid subclause start has to have an alphabet after it
  // and not e.g. a bracket or a comma, which could happen with repeated terminal names assign pattern,
  // a; Select a such that Modifies(pattern,_)
  return clause.find(match) + match.length()-1;
}

std::vector<size_t> QpsTokenizer::GetIndexListOfClauses(const std::string& statement) {
  std::vector<size_t> index_list;
  for (const auto& rgx : pql_constants::kClauseRegexVector) {
    for (sregex_iterator it = sregex_iterator(statement.begin(), statement.end(), rgx);
         it != sregex_iterator(); it++) {
      smatch match;
      match = *it;
      index_list.push_back(match.position(0));
    }
  }
  // sort ascending order
  sort(index_list.begin(), index_list.end());

  // if index list is empty then the subclauses do not contain the valid subclause markers like "such that"
  if (index_list.empty()) {
    throw SyntaxErrorException("Tokenizer GetIndexListOfSubclauses: No valid subclauses could be parsed out");
  }
  // If there are subclauses, then we should have a sub-clause start index at 0 and not e.g. at index 5
  if (index_list[0] > 0) {
    throw SyntaxErrorException("Tokenizer GetIndexListOfSubclauses: Invalid subclause present");
  }
  return index_list;
}

ParameterVector QpsTokenizer::ParseParameters(const std::string& parameters_substr) {
  size_t delimiter_index = parameters_substr.find(pql_constants::kComma);
  auto pair = string_util::SplitStringByDelimiter(parameters_substr,
                                                  pql_constants::kComma);
  auto parameters = pair.first;
  if (pair.second.length() > 0) {
    parameters.push_back(pair.second);
  } else {
    throw SyntaxErrorException("Extra delimiter");
  }

  for (int i=0; i < parameters.size(); i++) {
    if (QueryUtil::IsQuoted(parameters[i])) {
      parameters[i] = ParseIDENT(parameters[i]);
    }
  }

  return parameters;
}

std::shared_ptr<ClauseSyntax> QpsTokenizer::MakePatternClauseSyntax(const std::string& sub_clause) {
  SyntaxPair syntax = ExtractAbstractSyntaxFromClause(sub_clause);
  std::shared_ptr<ClauseSyntax> pattern_syntax = std::make_shared<PatternClauseSyntax>(syntax);

  syntax_validator_->ValidatePatternClauseSyntax(pattern_syntax);
  semantic_validator_->ValidatePatternClauseSemantic(pattern_syntax);

  return pattern_syntax;
}

std::shared_ptr<ClauseSyntax> QpsTokenizer::MakeSuchThatClauseSyntax(const std::string& sub_clause) {
  SyntaxPair syntax = ExtractAbstractSyntaxFromClause(sub_clause);
  std::shared_ptr<ClauseSyntax> such_that_syntax = std::make_shared<SuchThatClauseSyntax>(syntax);

  syntax_validator_->ValidateSuchThatClauseSyntax(such_that_syntax);
  semantic_validator_->ValidateSuchThatClauseSemantic(such_that_syntax);

  return such_that_syntax;
}

std::shared_ptr<ClauseSyntax> QpsTokenizer::MakeWithClauseSyntax(const std::string& sub_clause) {
  SyntaxPair syntax = ExtractAbstractSyntaxFromWithClause(sub_clause);
  std::shared_ptr<ClauseSyntax> with_syntax = std::make_shared<WithClauseSyntax>(syntax);
  syntax_validator_->ValidateWithClauseSyntax(with_syntax);
  semantic_validator_->ValidateWithClauseSemantic(with_syntax);
  return with_syntax;
}

std::shared_ptr<ClauseSyntax> QpsTokenizer::MakeAndClauseSyntax(const std::string& sub_clause, const std::string&
previous_sub_clause) {
  if (FindIndexOfRegexMatch(previous_sub_clause, pql_constants::kPatternRegex) == 0) {
    return MakePatternClauseSyntax(sub_clause);
  } else if (FindIndexOfRegexMatch(previous_sub_clause, pql_constants::kSuchThatRegex) == 0) {
    return MakeSuchThatClauseSyntax(sub_clause);
  } else if (FindIndexOfRegexMatch(previous_sub_clause, pql_constants::kWithRegex) == 0) {
    return MakeWithClauseSyntax(sub_clause);
  } else {
    throw SyntaxErrorException("There is no clause before the and clause");
  }
}

std::vector<std::shared_ptr<ClauseSyntax>> QpsTokenizer::ParseSubClauses(const std::string& statement) {
  std::string sub_clause;
  std::vector<size_t> index_list;
  std::vector<std::shared_ptr<ClauseSyntax>> syntax_pair_list;
  std::string statement_trimmed = string_util::RemoveExtraWhitespacesInString(statement);
  index_list = GetIndexListOfClauses(statement_trimmed);
  index_list.push_back(statement_trimmed.length());

  size_t start_index = 0;
  std::string previous_sub_clause;
  std::string processed_sub_clause;
  for (int i = 0; i < index_list.size()-1; i++) {
    size_t next_index = index_list[i+1];
    sub_clause = string_util::Trim(statement_trimmed.substr(start_index, next_index-start_index));
    start_index = next_index;

    if (FindIndexOfRegexMatch(sub_clause, pql_constants::kPatternRegex) == pql_constants::kStartIndex) {
      previous_sub_clause = sub_clause;
      processed_sub_clause =  sub_clause.substr(FindEndOfSubClauseStart(sub_clause,
                                                                        pql_constants::kPatternRegex));
      auto pattern_syntax = MakePatternClauseSyntax(processed_sub_clause);
      syntax_pair_list.push_back(pattern_syntax);
    } else if (FindIndexOfRegexMatch(sub_clause, pql_constants::kSuchThatRegex) ==
    pql_constants::kStartIndex) {
      previous_sub_clause = sub_clause;
      processed_sub_clause =  sub_clause.substr(FindEndOfSubClauseStart(sub_clause,
                                                                        pql_constants::kSuchThatRegex));
      auto such_that_syntax = MakeSuchThatClauseSyntax(processed_sub_clause);
      syntax_pair_list.push_back(such_that_syntax);
    } else if (FindIndexOfRegexMatch(sub_clause, pql_constants::kWithRegex) == pql_constants::kStartIndex) {
      previous_sub_clause = sub_clause;
      processed_sub_clause =  sub_clause.substr(FindEndOfSubClauseStart(sub_clause,
                                                                        pql_constants::kWithRegex));
      auto with_syntax = MakeWithClauseSyntax(processed_sub_clause);
      syntax_pair_list.push_back(with_syntax);
    } else if (FindIndexOfRegexMatch(sub_clause, pql_constants::kAndRegex) == pql_constants::kStartIndex) {
      processed_sub_clause =  sub_clause.substr(FindEndOfSubClauseStart(sub_clause,
                                                                        pql_constants::kAndRegex));
      if (previous_sub_clause.empty()) {
        throw SyntaxErrorException("The first clause cannot be an and clause");
      }
      auto and_syntax = MakeAndClauseSyntax(processed_sub_clause, previous_sub_clause);
      syntax_pair_list.push_back(and_syntax);
    } else {
      // e.g. such that pattern a(_,_)
      throw SyntaxErrorException("There is an invalid subclause");
    }
  }

  return syntax_pair_list;
}
