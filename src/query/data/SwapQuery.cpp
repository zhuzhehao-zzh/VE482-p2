//
// Created by lyj on 22-11-2.
//

#include "SwapQuery.h"
#include "../../db/Database.h"

constexpr const char *SwapQuery::qname;

QueryResult::Ptr SwapQuery::execute() {
    using namespace std;
    if (this->operands.size() != 2)
        return make_unique<ErrorMsgResult>(
                qname,this->targetTable.c_str(),
                "Invalid number of operands (? operands)."_f % operands.size());
    Database &db = Database::getInstance();
    Table::SizeType count = 0;
    try{
        auto &table = db[this->targetTable];
        this->fid1 = table.getFieldIndex(this->operands[0]);
        this->fid2 = table.getFieldIndex(this->operands[1]);
        if (fid1 != fid2) {
            auto result = initCondition(table);
            if (result.second) {
                for (auto it = table.begin(); it != table.end(); ++it) {
                    if (this->evalCondition(*it)) {
                        swap((*it)[this->fid1], (*it)[this->fid2]);
                        ++count;
                    }
                }
            }
        }
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

std::string SwapQuery::toString() {
    return "Query = Swap" + this->targetTable + "\"";
}

