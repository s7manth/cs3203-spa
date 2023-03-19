#include "Pkb.h"

Pkb::Pkb() {
  entity_store_ = new EntityStore();
  statement_store_ = new StatementStore();
  modifies_store_ = new ModifiesStore();
  follows_store_ = new FollowsStore();
  uses_store_ = new UsesStore();
  parent_store_ = new ParentStore();
  expression_store_ = new ExpressionStore();
  control_flow_store_ = new ControlFlowStore();
  assignment_store_ = new AssignmentStore();
  calls_store_ = new CallsStore();
  cfg_store_ = new CfgStore();
  next_store_ = new NextStore();
}

Pkb::~Pkb() = default;

