//
// Created by Ying on 22-11-4.
//

// for DELETE

#include "DeleteQuery.h"

#include <algorithm>

#include "../../db/Database.h"
#include "../QueryResult.h"

constexpr const char *DeleteQuery::qname;

QueryResult::Ptr DeleteQuery::execute() {
    using namespace std;
    if (!this->operands.empty())
        return make_unique<ErrorMsgResult>(qname, this->targetTable.c_str(), "Invalid number of operands (? operands)."_f % operands.size());
    Database &db = Database::getInstance();
    Table::SizeType counter = 0;
    try {
        auto &table = db[this->targetTable];
        auto result = initCondition(table);
        if (result.second) {
            if (this->condition.empty()) {
                counter = table.size();
                table.clear();
            } else {
                for (auto it = table.begin(); it != table.end(); ++it) {
                    if (this->evalCondition(*it)) {
                        table.deleteByIndex(it->key());
                        counter++;
                        it--;
                    }
                }
            }
        }
        return make_unique<RecordCountResult>(counter);
    } catch (const TableNameNotFound &e) {
        return make_unique<ErrorMsgResult>(qname, this->targetTable, "No such table."s);
    } catch (const IllFormedQueryCondition &e) {
        return make_unique<ErrorMsgResult>(qname, this->targetTable, e.what());
    } catch (const invalid_argument &e) {
        return make_unique<ErrorMsgResult>(qname, this->targetTable, "Unknown error '?'"_f % e.what());
    } catch (const exception &e) {
        return make_unique<ErrorMsgResult>(qname, this->targetTable, "Unknown error '?'."_f % e.what());
    }
}

std::string DeleteQuery::toString() {
    return "QUERY = DELETE " + this->targetTable + "\"";
}