// Copyright 2013
// Anton Stepan <stepana@informatik.uni-freiburg.de>


#ifndef ENTITYUNIFICATION_SRC_ENTITY_PTR_H_
#define ENTITYUNIFICATION_SRC_ENTITY_PTR_H_

#include "./entity.h"

class EntityPtr {
 public:
  explicit EntityPtr(Entity* e) : ptr_(e) {}
  Entity* ptr() { return ptr_; }
  void set_ptr(Entity* e) { ptr_ = e ;}
  bool operator== (const EntityPtr& rhs) const {
    return ptr_ == rhs.ptr_;
  }
 private:
  Entity* ptr_;
};

#endif  // ENTITYUNIFICATION_SRC_ENTITY_PTR_H_
