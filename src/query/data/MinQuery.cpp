#include "MinQuery.h"

#include <algorithm>

#include "../../db/Database.h"
#include "../QueryResult.h"

constexpr const char *MinQuery::qname;

QueryResult::Ptr MinQuery::execute(){
    using namespace std;
    if (this->operands.empty())
        return make_unique<ErrorMsgResult>(qname, this->targetTable.c_str(),
                                       "No operand (? operands)."_f %
                                           operands.size());
    Database &db = Database::getInstance();
    try {
        auto &table = db[this->targetTable];
        std::pair<std::string, bool> result = initCondition(table);
        vector<int> int_arr((this->operands).size(),INT32_MAX);
        if(result.second){
          for (auto row = table.begin(); row != table.end(); ++row) {
            for (size_t i(0); i < this->operands.size(); i++) {
              if (int_arr[i] > (*row)[this->operands[i]]) {
                int_arr[i] = (*row)[this->operands[i]];
              }
            }
          }
        }
        return make_unique<SuccessMsgResult>(int_arr);
    } catch (const TableNameNotFound &e) {
      return make_unique<ErrorMsgResult>(qname, this->targetTable,
                                         "No such table."s);
    } catch (const IllFormedQueryCondition &e) {
      return make_unique<ErrorMsgResult>(qname, this->targetTable, e.what());
    } catch (const invalid_argument &e) {
      // Cannot convert operand to string
      return make_unique<ErrorMsgResult>(qname, this->targetTable,
                                         "Unknown error '?'"_f % e.what());
    } catch (const exception &e) {
      return make_unique<ErrorMsgResult>(qname, this->targetTable,
                                         "Unkonwn error '?'."_f % e.what());
    }
}

std::string MinQuery::toString() {
  return "QUERY = MIN " + this->targetTable + "\"";
}
