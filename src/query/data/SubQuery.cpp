
#include "SubQuery.h"


#include "../../db/Database.h"
#include "../QueryResult.h"

constexpr const char *SubQuery::qname;

QueryResult::Ptr SubQuery::execute() {
    using namespace std;
    if (this->operands.size() <= 1)
        return make_unique<ErrorMsgResult>(qname, this->targetTable.c_str(),
                                           "Invalid number of operands (? operands), should be larger than 1."_f %
                                           operands.size());
    Database &db = Database::getInstance();
    try {
        auto &table = db[this->targetTable];
        std::pair<std::string, bool> result = initCondition(table);
        int count = 0;
        if(result.second){
            auto it = table.begin();
            while(it != table.end()){
                if(evalCondition(*it)){
                    auto it_field = this->operands.begin();
                    int ans = (*it)[*it_field];
                    it_field++;
                    for(;it_field < this->operands.end()-1;it_field++){
                        ans -= (*it)[*it_field];
                    }
                    (*it)[*(it_field++)] = ans;
                    count++;
                }
                it ++ ;
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

std::string SubQuery::toString() {
    return "QUERY = SUB " + this->targetTable + "\"";
}

