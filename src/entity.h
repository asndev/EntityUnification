// Copyright 2013
// Anton Stepan <stepana@informatik.uni-freiburg.de>


#ifndef ENTITYUNIFICATION_SRC_ENTITY_H_
#define ENTITYUNIFICATION_SRC_ENTITY_H_

#include <map>
#include <string>
#include <vector>
// #include "./EntityPtr.h"
class EntityPtr;

class Entity {
 public:
  typedef std::map<std::string, std::vector<EntityPtr*>> RelationsMap;
  Entity();
  explicit Entity(const std::string& id);
  Entity(const std::string& id, const std::string& name);
  Entity(const std::string& id, const std::string& name,
         const std::string& prefix);
  // Map of relation-name pointing to a Vector of entity pointers
  std::map<std::string, std::vector<EntityPtr*>> relations;
  // Getters
  std::string id() const { return id_; }
  std::string name() const { return name_; }
  std::string prefix() const { return prefix_; }
  std::string alternate_name() const { return alternate_name_; }
  size_t number_of_words() const { return number_of_words_; }
  bool merged() const { return merged_; }
  std::vector<std::string> other_ids() const { return other_ids_; }
  unsigned int contains() const { return contains_; }
  unsigned int contained_by() const { return contained_by_; }
  // Setters
  void set_id(const std::string& id) { id_ = id; }
  void set_name(const std::string& name) { name_ = name; }
  void set_prefix(const std::string& prefix) { prefix_ = prefix_; }
  void set_alternate_name(const std::string& a) { alternate_name_ = a; }
  void set_merged(const bool merged) { merged_ = merged; }
  void set_merged() { merged_ = true; }
  void set_number_of_words(const std::string& s);
  void increment_contains() { ++contains_; }
  void increment_contained_by() { ++contained_by_; }
  // Adds another id to the other ids vector.
  void AddId(std::string i) { other_ids_.push_back(i); }
  // Returns true if location fulfills specific attributes. (|contains| > x, ..)
  bool IsImportantLocation();
  // Prints the entity in a readable format.
  void PrettyPrint();
  // Returns the entity in a readable format as a string.
  std::string GetPretty();
  // Adds a relation to the relation vector of the entity.
  void AddRelation(std::string name, EntityPtr* entity);
  // Deletes duplcate relations from the relation vector.
  void EraseDuplicateRelations();
  // Deletes all relation 'relation' until only the first is left.
  void EraseMultipleRelations(const std::string& relation);
  // Returns a vector of the top most locations.
  std::vector<EntityPtr*> GetTopMostLocations();
  // override c++ methods.
  bool operator== (const Entity& rhs) const {
    return (name_ == rhs.name() && id_ == rhs.id());
  };
  static bool before(const Entity* e1, const Entity* e2) {
    return e1->id() < e2->id();
  }

 private:
  std::string id_ = "unknown";
  std::string name_ = "";
  std::string prefix_ = "";
  std::string alternate_name_ = "";
  size_t number_of_words_ = 0;
  bool merged_ = false;
  unsigned int contains_ = 0;
  unsigned int contained_by_ = 0;
  std::vector<std::string> other_ids_;
  void EraseDuplicatRelation(std::vector<EntityPtr*>* vec);
};

#endif  // ENTITYUNIFICATION_SRC_ENTITY_H_
