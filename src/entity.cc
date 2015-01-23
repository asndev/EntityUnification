// Copyright 2013
// Anton Stepan <stepana@informatik.uni-freiburg.de>

#include "./entity.h"
#include <cassert>
#include <algorithm>
#include <iostream>  // NOLINT
#include <sstream>
#include <string>
#include "./entity-ptr.h"

using std::string;

// _____________________________________________________________________________
Entity::Entity(const std::string& id, const std::string& name,
               const std::string& prefix)
  : id_(id), name_(name), prefix_(prefix) {
    set_number_of_words(name);
}

// _____________________________________________________________________________
Entity::Entity(const std::string& id, const std::string& name)
  : id_(id), name_(name) {
  set_number_of_words(name);
}

// _____________________________________________________________________________
Entity::Entity(const std::string& id) : id_(id) {
  set_number_of_words(id);
}

// _____________________________________________________________________________
Entity::Entity() {}

// _____________________________________________________________________________
bool Entity::IsImportantLocation() {
  return (contains() > 50 && contained_by() < 2);
}

// _____________________________________________________________________________
void Entity::set_number_of_words(const std::string& s) {
  number_of_words_ = 1 + std::count(s.begin(), s.end(), '_');
}

// _____________________________________________________________________________
std::vector<EntityPtr*> Entity::GetTopMostLocations() {
  std::vector<EntityPtr*> result;
  if (relations.find("located-in") == relations.end()) return result;
  std::vector<EntityPtr*> stack = relations["located-in"];
  while (stack.size() > 0) {
    EntityPtr* t = stack.back();
    stack.pop_back();
    if (t->ptr()->contained_by() > 0) {
      for (EntityPtr* e : t->ptr()->relations["located-in"]) {
        stack.push_back(e);
      }
    } else {
      result.push_back(t);
    }
  }
  return result;
}

// _____________________________________________________________________________
void Entity::AddRelation(string name, EntityPtr* entity) {
  assert(entity != NULL);
  if (name == "located-in") {
    ++contained_by_;
    entity->ptr()->increment_contains();
  }
  relations[name].push_back(entity);
  EraseDuplicateRelations();
}

// _____________________________________________________________________________
void Entity::PrettyPrint() {
  std::cout << GetPretty() << std::endl;
}

// _____________________________________________________________________________
std::string Entity::GetPretty() {
  std::ostringstream out;
  out << name_ << " id:" << id_ << "";
  for (auto i : other_ids_) out << " " << i;
  out << ")" << '\n';
  for (auto iter = relations.begin(); iter != relations.end(); ++iter) {
    for (auto e : iter->second) {
      out << "\t" << iter->first;
      out << " " << e->ptr()->name() << "(" << e->ptr()->id() << ")"
          << '\n';
    }
  }
  return out.str();
}

// _____________________________________________________________________________
void Entity::EraseDuplicateRelations() {
  for (auto iter = relations.begin(); iter != relations.end(); ++iter) {
    EraseDuplicatRelation(&(iter->second));
  }
}

// _____________________________________________________________________________
void Entity::EraseDuplicatRelation(std::vector<EntityPtr*>* vec) {
  vec->erase(std::unique(vec->begin(), vec->end()), vec->end());
}

// _____________________________________________________________________________
void Entity::EraseMultipleRelations(const std::string& relation) {
  while (relations[relation].size() > 1) {
    relations[relation].pop_back();
  }
}
