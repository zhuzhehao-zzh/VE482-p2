//
// Created by liu on 18-10-23.
//

#include "Table.h"

#include <iomanip>
#include <iostream>
#include <sstream>

#include "Database.h"

constexpr const Table::ValueType Table::ValueTypeMax;
constexpr const Table::ValueType Table::ValueTypeMin;

Table::FieldIndex
Table::getFieldIndex(const Table::FieldNameType &field) const {
  try {
    return this->fieldMap.at(field);
  } catch (const std::out_of_range &e) {
    throw TableFieldNotFound(R"(Field name "?" doesn't exists.)"_f % (field));
  }
}

void Table::insertByIndex(const KeyType& key, std::vector<ValueType> &&data_t) {
  if (this->keyMap.find(key) != this->keyMap.end()) {
    std::string err = "In Table \"" + this->tableName + "\" : Key \"" + key +
                      "\" already exists!";
    throw ConflictingKey(err);
  }
  this->keyMap.emplace(key, this->data.size());
  this->data.emplace_back(key, data_t);
}

void Table::deleteByIndex(const KeyType& key) {
  if (this->keyMap.find(key) == this->keyMap.end()) {
    std::string err = "In Table \"" + this->tableName + "\" : Key \"" + key + "\"doesn't exist";
    throw ConflictingKey(err);
  }
  this->keyMap.erase(this->keyMap.find(key));
  for (auto it = this->data.begin(); it != this->data.end(); ++it) {
    if (it->key == key) {
      this->data.erase(it);
      break;
    }
  }
}

void Table::duplicate(std::vector<Table::Iterator> &data_t, Table::SizeType & count) {
  for (auto table = data_t.begin(); table != data_t.end(); ++table) {
    KeyType key_copy = table->it->key + "_copy";
      if (this->keyMap.find(key_copy) != this->keyMap.end()) {
          count --;
          continue;
        }
    this->insertByIndex(key_copy, std::vector(table->it->datum));
  }
}

Table::Object::Ptr Table::operator[](const Table::KeyType &key) {
  auto it = keyMap.find(key);
  if (it == keyMap.end()) {
    // not found
    return nullptr;
  } else {
    return createProxy(
        data.begin() +
            static_cast<std::vector<Table::Datum>::difference_type>(it->second),
        this);
  }
}

std::ostream &operator<<(std::ostream &os, const Table &table) {
  const int width = 10;
  std::stringstream buffer;
  buffer << table.tableName << "\t" << (table.fields.size() + 1) << "\n";
  buffer << std::setw(width) << "KEY";
  for (const auto &field : table.fields) {
    buffer << std::setw(width) << field;
  }
  buffer << "\n";
  auto numFields = table.fields.size();
  for (const auto &datum : table.data) {
    buffer << std::setw(width) << datum.key;
    for (decltype(numFields) i = 0; i < numFields; ++i) {
      buffer << std::setw(width) << datum.datum[i];
    }
    buffer << "\n";
  }
  return os << buffer.str();
}
