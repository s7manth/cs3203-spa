#include "TestWrapper.h"

// implementation code of WrapperFactory - do NOT modify the next 5 lines
AbstractWrapper *WrapperFactory::wrapper = 0;
AbstractWrapper *WrapperFactory::createWrapper() {
  if (wrapper == 0) wrapper = new TestWrapper;
  return wrapper;
}
// Do not modify the following line
volatile bool AbstractWrapper::GlobalStop = false;

// a default constructor
TestWrapper::TestWrapper() {
  // create any objects here as instance variables of this class
  // as well as any initialization required for your spa program
  pkb_ = make_shared<Pkb>();
  cfg_ = make_shared<Cfg>();
}

// method for parsing the SIMPLE source
void TestWrapper::parse(std::string filename) {
  // call your parser to do the parsing
  // ...rest of your code...

  std::ifstream source(filename);
  shared_ptr<Sp> sp = make_shared<Sp>();
  bool is_SP_processing_successful = sp->ProcessSIMPLE(source, pkb_, cfg_);
  // Terminate program without evaluating any queries if there's invalid SIMPLE code
  if (!is_SP_processing_successful) {
    cout << "Terminating program due to Invalid SIMPLE code";
    exit(0);
  }
}

// method to evaluating a query
void TestWrapper::evaluate(std::string query, std::list<std::string> &results) {
  // call your evaluator to evaluate the query here
  // ...code to evaluate query...
  // store the answers to the query in the results list (it is initially empty)
  // each result must be a string.
  std::shared_ptr<PkbReadFacade>
      pkb_read = std::make_shared<PkbReadFacade>(*pkb_);
  Qps::ProcessQuery(query, results, pkb_read);
  pkb_read->ClearCache();
}
