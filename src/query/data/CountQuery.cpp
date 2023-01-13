#include "CountQuery.h"

#include <algorithm>

#include "../../db/Database.h"
#include "../QueryResult.h"

constexpr const char *CountQuery::qname;

QueryResult::Ptr CountQuery::execute() {
    using namespace std;
    if (!this->getOperands().empty())
        return make_unique<ErrorMsgResult>(qname, this->targetTable.c_str(),
                                           "Invalid number of operands (? operands), should be 0."_f %
                                           operands.size());
    Database &db = Database::getInstance();
    try {
        auto &table = db[this->targetTable];
        std::pair<std::string, bool> result = initCondition(table);
        int count = 0;
        if(result.second){
            auto it = table.begin();
            while(it != table.end()){
                if(evalCondition(*it)) count++;
                it ++ ;
            }
        }
        return make_unique<SuccessMsgResult>(count);
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

std::string CountQuery::toString() {
    return "QUERY = COUNT " + this->targetTable + "\"";
}
