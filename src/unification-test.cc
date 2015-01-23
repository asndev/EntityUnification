// Copyright 2013
// Anton Stepan <stepana@informatik.uni-freiburg.de>

#include <gtest/gtest.h>
#include <string>
#include "./unification.h"
#include "./entity-ptr.h"

using std::string;

// _____________________________________________________________________________
TEST(UnificationTest, processFiles) {
  {
    Unification uf;
    uf.filename_1_ = "./TestData/testTriple1mini.txt";
    uf.filename_2_ = "./TestData/testTriple2mini.txt";
    uf.ProcessFiles();
    EntityPtr* e1 = uf.entities_1_["Freiburg_Region"];
    ASSERT_EQ(e1->ptr()->relations["located-in"].size(), size_t(3));
    ASSERT_EQ(uf.entities_1_["Freiburg_Region"]->ptr()->relations["abc"].size(),
              size_t(0));
    ASSERT_EQ(uf.entities_1_["Aach"]->ptr()->relations["located-in"].size(),
              size_t(1));
  }
  {
    Unification uf;
    uf.filename_1_ = "./TestData/testTriple1IdSyntax.txt";
    uf.filename_2_ = "./TestData/testTriple2IdSyntax.txt";
    uf.ProcessFiles();
    ASSERT_EQ(uf.entities_1_["Dreamland,_(Territory)_(1337)"]->ptr()->name(),
              "Dreamland");
    ASSERT_TRUE(uf.entities_1_["not_available"] == NULL);
    ASSERT_EQ(uf.entities_2_["Wittnau_(2807215)"]->ptr()->name(), "Wittnau");
  }
  {
    Entity a = Entity("1234", "Freiburg");
    Entity b = Entity("1234", "Berlin");
    Entity c = Entity("1234", "Deutschland");
    EntityPtr aa = EntityPtr(&a);
    EntityPtr bb = EntityPtr(&b);
    aa.ptr()->AddRelation("located-in", &bb);
    // aa.getPtr()->prettyPrint();
    bb.set_ptr(&c);
    // aa.getPtr()->prettyPrint();
  }
}

// _____________________________________________________________________________
TEST(UnificationTest, _checkGeodata) {
    using std::vector;
    Unification uf;
    Entity a1 = Entity("", "48.7349");
    Entity b1 = Entity("", "48.77");
    Entity c1 = Entity("", "48.72222");
    Entity d1 = Entity("", "48");
    Entity e1 = Entity("", "46.7349");
    Entity f1 = Entity("", "-13.8909305556");
    Entity g1 = Entity("", "-13.88333");
    Entity h1 = Entity("", "-171.987377778");
    Entity i1 = Entity("", "-171.96667");
    EntityPtr a = EntityPtr(&a1);
    EntityPtr b = EntityPtr(&b1);
    EntityPtr c = EntityPtr(&c1);
    EntityPtr d = EntityPtr(&d1);
    EntityPtr e = EntityPtr(&e1);
    EntityPtr f = EntityPtr(&f1);
    EntityPtr g = EntityPtr(&g1);
    EntityPtr h = EntityPtr(&h1);
    EntityPtr i = EntityPtr(&i1);
    vector<EntityPtr*> aa = { &a };
    vector<EntityPtr*> bb = { &b };
    vector<EntityPtr*> cc = { &c };
    vector<EntityPtr*> dd = { &d };
    vector<EntityPtr*> ee = { &e };
    vector<EntityPtr*> fail = { &a, &b };
    vector<EntityPtr*> ff = { &f };
    vector<EntityPtr*> gg = { &g };
    vector<EntityPtr*> hh = { &h };
    vector<EntityPtr*> ii = { &i };
    /*ASSERT_EQ(uf.CheckGeodata(aa, bb), LIKELY_TRUE);
    ASSERT_EQ(uf.CheckGeodata(aa, cc), LIKELY_TRUE);
    ASSERT_EQ(uf.CheckGeodata(aa, dd), UNDECIDED);
    ASSERT_EQ(uf.CheckGeodata(aa, ee), VERY_LIKELY_FALSE);
    ASSERT_EQ(uf.CheckGeodata(bb, fail), VERY_LIKELY_TRUE);
    ASSERT_EQ(uf.CheckGeodata(ee, dd), VERY_LIKELY_FALSE);
    ASSERT_EQ(uf.CheckGeodata(ff, gg), LIKELY_TRUE);
    ASSERT_EQ(uf.CheckGeodata(hh, ii), LIKELY_TRUE);*/
}

// _____________________________________________________________________________
TEST(UnificationTest, unifyEntities) {
  {
    Unification uf;
    Entity a = Entity("1234", "Berlin");
    Entity b = Entity("1234", "Berlin");
    Entity c = Entity("1234", "Germany");
    Entity d = Entity("123123", "45.3");
    Entity e = Entity("23131", "76.24");
    Entity f = Entity("123123", "45.3");
    Entity g = Entity("23131", "76.24");
    Entity h = Entity("1234", "Germany");
    EntityPtr aa = EntityPtr(&a);
    EntityPtr bb = EntityPtr(&b);
    EntityPtr cc = EntityPtr(&c);
    EntityPtr hh = EntityPtr(&h);
    EntityPtr dd = EntityPtr(&d);
    EntityPtr ff = EntityPtr(&f);
    EntityPtr ee = EntityPtr(&e);
    EntityPtr gg = EntityPtr(&g);
    a.AddRelation("located-in", &cc);
    b.AddRelation("located-in", &hh);
    a.AddRelation("has-longitude", &dd);
    b.AddRelation("has-longitude", &ff);
    a.AddRelation("has-latitude", &ee);
    b.AddRelation("has-latitude", &gg);
    std::vector<EntityPtr*> aptr = { &aa };
    std::vector<EntityPtr*> bptr = { &bb };
    /*ASSERT_EQ(VERY_LIKELY_TRUE, uf.CheckLocatedIn(aptr, bptr));
    ASSERT_EQ(UNDECIDED, uf.CheckRelation("has-capital", &aa, &bb));
    ASSERT_EQ(UNDECIDED, uf.CheckRelation("not_available_relation", &aa, &bb));*/
  }
  {
    Unification uf;
    uf.filename_1_ = "./TestData/testTriple1mini.txt";
    uf.filename_2_ = "./TestData/testTriple2mini.txt";
    uf.double_relations_["located-in"] = "contains";
    uf.double_relations_["contains"] = "located-in";
    uf.scores_filename_ = "./TestData/scores.conf";
    uf.ProcessFiles();
    uf.UnifyEntities();
    ASSERT_EQ(uf.entities_1_["Freiburg_Region"]->ptr()->relations["located-in"].size(), // NOLINT
              size_t(3));
    ASSERT_EQ(uf.entities_1_["Freiburg_Region"]->ptr()->relations.size(),
              size_t(4));
  }
  {
    Unification uf;
    uf.filename_1_ = "./TestData/testRealData_id_1.txt";
    uf.filename_2_ = "./TestData/testRealData_id_2.txt";
    uf.double_relations_["located-in"] = "contains";
    uf.double_relations_["contains"] = "located-in";
    uf.scores_filename_ = "./TestData/scores.conf";
    uf.debugging_ = true;
    uf.debug_info_.set_stream("./TestData/debug_merged.txt");
    uf.debug_error_.set_stream("./TestData/debug_not_merged.txt");
    uf.ProcessFiles();
    uf.UnifyEntities();
    ASSERT_EQ(uf.unified(), size_t(2));
    /*for (auto i = uf.entities_1_.begin(); i != uf.entities_1_.end(); ++i) {
        std::cout << i->first << " " << i->second->ptr()->merged() << std::endl;
    }
    std::cout << "-----" << std::endl;
    for (auto i = uf.entities_2_.begin(); i != uf.entities_2_.end(); ++i) {
        std::cout << i->first << " " << i->second->ptr()->merged() << std::endl;
    }*/
  }
}
