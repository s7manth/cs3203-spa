#include "ParentStore.h"

ParentStore::ParentStore() = default;

ParentStore::~ParentStore() = default;

void ParentStore::addParentRelation(PkbTypes::STATEMENT_NUMBER first_statement,
                                    PkbTypes::STATEMENT_NUMBER second_statement) {
  this->parent_store_.insert(first_statement, second_statement);
  this->parent_star_store_.insert(first_statement, second_statement);

  std::unordered_set<PkbTypes::STATEMENT_NUMBER> parents =
      this->parent_star_store_.retrieveFromValue(first_statement);

  while (!parents.empty()) {
    std::unordered_set<PkbTypes::STATEMENT_NUMBER> updated_parents;
    for (const auto& p : parents) {
      std::unordered_set<PkbTypes::STATEMENT_NUMBER> grand_parents = this->parent_star_store_.retrieveFromValue(p);
      updated_parents.insert(grand_parents.begin(), grand_parents.end());
      this->parent_star_store_.insert(p, second_statement);
    }
    parents = updated_parents;
  }
}

std::unordered_set<std::pair<PkbTypes::STATEMENT_NUMBER, PkbTypes::STATEMENT_NUMBER>, PairHasherUtil::hash_pair>
    ParentStore::retrieveAllParentPairs() {
  return this->parent_store_.retrieveAll();
}

std::unordered_set<std::pair<PkbTypes::STATEMENT_NUMBER, PkbTypes::STATEMENT_NUMBER>, PairHasherUtil::hash_pair>
    ParentStore::retrieveAllParentStarPairs() {
  return this->parent_star_store_.retrieveAll();
}

bool ParentStore::hasParentRelation(PkbTypes::STATEMENT_NUMBER first_statement,
                                    PkbTypes::STATEMENT_NUMBER second_statement) {
  return this->parent_store_.contains(std::move(first_statement), std::move(second_statement));
}

bool ParentStore::hasParentStarRelation(PkbTypes::STATEMENT_NUMBER first_statement,
                                        PkbTypes::STATEMENT_NUMBER second_statement) {
  return this->parent_star_store_.contains(std::move(first_statement), std::move(second_statement));
}

bool ParentStore::hasAnyParentRelation() {
  return this->parent_store_.length() > 0;
}

bool ParentStore::hasAnyParentStarRelation() {
  return this->parent_star_store_.length() > 0;
}

bool ParentStore::hasParentStar(PkbTypes::STATEMENT_NUMBER statement) {
  return !this->parent_star_store_.retrieveFromValue(statement).empty();
}

bool ParentStore::hasParentStarBy(PkbTypes::STATEMENT_NUMBER statement) {
  return !this->parent_star_store_.retrieveFromKey(statement).empty();
}

std::unordered_set<PkbTypes::STATEMENT_NUMBER> ParentStore::retrieveAllParents() {
  return this->parent_store_.retrieveAllKeys();
}

PkbTypes::STATEMENT_NUMBER ParentStore::retrieveAllParents(PkbTypes::STATEMENT_NUMBER statement) {
  return this->parent_store_.retrieveFromValue(statement);
}

std::unordered_set<PkbTypes::STATEMENT_NUMBER> ParentStore::retrieveAllChildren(PkbTypes::STATEMENT_NUMBER statement) {
  return this->parent_store_.retrieveFromKey(statement);
}

std::unordered_set<PkbTypes::STATEMENT_NUMBER> ParentStore::retrieveAllChildren() {
  return this->parent_store_.retrieveAllValues();
}

std::unordered_set<PkbTypes::STATEMENT_NUMBER> ParentStore::retrieveAllAncestors() {
  return this->parent_star_store_.retrieveAllKeys();
}

std::unordered_set<PkbTypes::STATEMENT_NUMBER> ParentStore::retrieveAllAncestors(PkbTypes::STATEMENT_NUMBER statement) {
  return this->parent_star_store_.retrieveFromValue(statement);
}

std::unordered_set<PkbTypes::STATEMENT_NUMBER> ParentStore::retrieveAllDescendants() {
  return this->parent_star_store_.retrieveAllValues();
}

std::unordered_set<PkbTypes::STATEMENT_NUMBER> ParentStore::retrieveAllDescendants(PkbTypes::STATEMENT_NUMBER statement) {
  return this->parent_star_store_.retrieveFromKey(statement);
}

