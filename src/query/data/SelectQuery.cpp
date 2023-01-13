#include "SelectQuery.h"

#include <queue>

#include "../../db/Database.h"
#include <iostream>

struct key_comparer {
  bool operator()(Table::Iterator &a, Table::Iterator &b) {
    return (a->key() > b->key());
  }
};

QueryResult::Ptr SelectQuery::execute() {
  using namespace std;
  Database &db = Database::getInstance();
  // start of try
  try {
    auto &table = db[this->targetTable];
    std::pair<std::string, bool> result = initCondition(table);
    std::priority_queue<Table::Iterator, std::vector<Table::Iterator>, key_comparer> records;
    if (result.second) {
      for (auto it(table.begin()); it != table.end(); ++it) {
        if (this->evalCondition(*it))
          records.push(it);
      }
    }
    if (records.empty()) {
      return make_unique<NullQueryResult>();
    }
    ostringstream query_result;
    while (!records.empty()) {
      auto top = records.top();
      records.pop();
      query_result << "( ";
      for (auto it = this->operands.begin(); it != this->operands.end(); ++it) {
        if (*it == "KEY") {
          query_result << top->key();
        } else {
          query_result << (*top)[*it];
        }
        query_result << " ";
      }
      query_result << ")\n";
    }
    std::string string_result = query_result.str();
    std::cout << string_result;
    return make_unique<SuccessMsgResult>(qname);
  } catch (const TableNameNotFound &e) {
    return make_unique<ErrorMsgResult>(qname, this->targetTable, e.what());
  } catch (const IllFormedQueryCondition &e) {
    return make_unique<ErrorMsgResult>(qname, this->targetTable, e.what());
  } catch (const invalid_argument &e) {
    // Cannot convert operand to string
    return make_unique<ErrorMsgResult>(qname, this->targetTable, e.what());
  } catch (const exception &e) {
    return make_unique<ErrorMsgResult>(qname, this->targetTable, e.what());
  }
}

std::string SelectQuery::toString() {
  return "QUERY = SELECT " + this->targetTable + "\"";
}
