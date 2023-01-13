//
// Created by luyij on 2022/11/3.
//

#include "DuplicateQuery.h"
#include "../../db/Database.h"

constexpr const char *DuplicateQuery::qname;

QueryResult::Ptr DuplicateQuery::execute() {
    using namespace std;
    Database &db = Database::getInstance();
    Table::SizeType count = 0;
    try {
        auto &table = db[this->targetTable];
        auto result = initCondition(table);
        vector<Table::Iterator> data;
        if (result.second){
            for (auto it = table.begin(); it != table.end(); ++it) {
                if (this->evalCondition(*it)) {
                    data.push_back(it);
                    ++count;
                }
            }
        }
        if (count > 0) table.duplicate(data, count);
        return make_unique<RecordCountResult>(count);
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

std::string DuplicateQuery::toString() {
    return "Query = DUPLICATE " + this->targetTable + "\"";
}
