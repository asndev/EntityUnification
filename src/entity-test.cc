// Copyright 2013
// Anton Stepan <stepana@informatik.uni-freiburg.de>

#include <gtest/gtest.h>
#include <string>
#include "./entity.h"
#include "./entity-ptr.h"

using std::string;

// _____________________________________________________________________________
TEST(EntityTest, creation) {
  Entity e1("Berlin");
  Entity e2("Deutschland");
  Entity e3("Freiburg");
  Entity e4("Berlin,_(Bundesland)");
  Entity e5("Kreuzberg");
  EntityPtr p1 = EntityPtr(&e2);
  EntityPtr p2 = EntityPtr(&e4);
  EntityPtr p3 = EntityPtr(&e1);

  e1.AddRelation("located-in", &p1);
  e1.AddRelation("located-in", &p2);
  e3.AddRelation("located-in", &p1);
  e5.AddRelation("located-in", &p3);
  ASSERT_EQ(e1.relations["located-in"].size(), size_t(2));
  ASSERT_EQ(e3.relations["located-in"].size(), size_t(1));
  ASSERT_EQ(e4.relations["located-in"].size(), size_t(0));
}

// _____________________________________________________________________________
TEST(EntityTest, unifyWith) {
  Entity e1("Freiburg");
  Entity e2("Deutschland");
  Entity e3("Welt");
  Entity e4("BW");
  Entity e5("Freiburg,_(Deutschland)");
  EntityPtr p2 = EntityPtr(&e2);
  EntityPtr p4 = EntityPtr(&e4);
  EntityPtr p3 = EntityPtr(&e3);
  e1.AddRelation("located-in", &p2);
  e1.AddRelation("located-in", &p2);
  e1.AddRelation("located-in", &p2);
  e1.AddRelation("located-in", &p4);
  e5.AddRelation("located-in", &p3);
  e5.AddRelation("located-in", &p2);
}

// _____________________________________________________________________________
TEST(EntityTest, getTopmostLocation) {
  Entity e1("", "Freiburg");
  Entity e2("", "Deutschland");
  Entity e3("", "Welt");
  Entity e4("", "nothing");
  EntityPtr p2 = EntityPtr(&e2);
  EntityPtr p3 = EntityPtr(&e3);
  EntityPtr p4 = EntityPtr(&e4);
  e1.AddRelation("located-in", &p2);
  e2.AddRelation("located-in", &p3);
  e2.AddRelation("has-nothing", &p4);
  std::vector<EntityPtr*> result = { &p3 };
  ASSERT_EQ(e1.GetTopMostLocations().size(), result.size());
  ASSERT_EQ(e1.GetTopMostLocations().back()->ptr()->name(),
            result.back()->ptr()->name());
  Entity e5("", "OberWelt");
  EntityPtr p5 = EntityPtr(&e5);
  Entity e6("", "OberOberWelt");
  EntityPtr p6 = EntityPtr(&e6);
  Entity e7 = Entity("", "Oberoberoberoberwelt");
  EntityPtr p7 = EntityPtr(&e7);
  Entity e8 = Entity("", "Oberoberoberoberwelt2");
  EntityPtr p8 = EntityPtr(&e8);
  e3.AddRelation("located-in", &p5);
  e5.AddRelation("located-in", &p6);
  e6.AddRelation("located-in", &p7);
  e6.AddRelation("located-in", &p8);
  ASSERT_EQ(e1.GetTopMostLocations().size(), size_t(2));
}
