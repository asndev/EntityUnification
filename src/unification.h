// Copyright 2013
// Anton Stepan <stepana@informatik.uni-freiburg.de>

#ifndef ENTITYUNIFICATION_SRC_UNIFICATION_H_
#define ENTITYUNIFICATION_SRC_UNIFICATION_H_

#include <gtest/gtest.h>
#include <unordered_map>
#include <sstream>
#include <map>
#include <string>
#include <list>
#include <vector>
#include "./entity.h"
#include "./entity-ptr.h"
#include "./file-writer.h"

class Unification {
 public:
  typedef std::unordered_map<std::string, unsigned int> IsACountMap;
  Unification();
  // returns false if not all required params are fullfilled.
  bool ParseCommandlineArguments(int argc, char** argv);
  // tries to unify the entities from _entities1 and _entities2.
  void ProcessFiles();
  FRIEND_TEST(UnificationTest, processFiles);
  void UnifyEntities();
  FRIEND_TEST(UnificationTest, unifyEntities);
  void GenerateOutput();
  /* prints */
  void PrintUsage() const;
  void PrintStatistics();
  /* Getter for private data */
  std::string output_filename() const { return output_filename_; }
  std::string debug_filename() const { return debug_filename_; }
  int entities_1_size() const { return entities_1_size_; }
  int entities_2_size() const { return entities_2_size_; }
  size_t iterations() const { return iterations_; }
  bool debugging() const { return debugging_; }
  bool show_merged() const { return show_merged_; }
  unsigned int unified() const { return unified_; }
  unsigned int line_count_file_1() const { return line_count_file_1_; }
  unsigned int line_count_file_2() const { return line_count_file_2_; }
  IsACountMap is_a_count() const { return is_a_count_; }
  std::string scores_filename() { return scores_filename_; }
  double Score(const std::string& value);

 private:
  /* private methods */
  unsigned int ProcessFile(const std::string& filename,
                            std::map<std::string, EntityPtr*>* map);
  bool Unify(EntityPtr* lE, EntityPtr* rE);
  double CheckRelation(const std::string relation, EntityPtr* rE,
                        EntityPtr* lE);
  double CheckLocatedIn(const std::vector<EntityPtr*>& lLoc,
                        const std::vector<EntityPtr*>& rLoc);
  double CheckGeodata(const std::vector<EntityPtr*>& l,
                      const std::vector<EntityPtr*>& r);
  double CheckPopulation(const std::vector<EntityPtr*>& l,
                         const std::vector<EntityPtr*>& r);
  // Corrects population from 'year_count' to 'count'
  std::string ParsePopulation(const std::string& s);
  // Checks for illegal strings
  bool CheckStrings(const std::string& a, const std::string& b,
                    const std::string& c);
  // i.e. a contains b => b located-in a.
  std::string TransformRelation(const std::string& s);
  FRIEND_TEST(UnificationTest, _checkGeodata);
  // Checks, whether two entities might be the same.
  bool PreCheck(const Entity* const lhs, const Entity* const rhs);
  void ParseRelationMappingFile();
  void ParseScoresFile();
  // Set standard parameters regarding the config file located in path p.
  void SetStandardParameters(const std::string& path);
  bool ParseConfig(const std::string& option, const std::string& value);
  void WriteDebug(const std::string& s);
  void WriteDebug(Entity* const rhs, Entity* const lhs,
                   const std::string& title, FileWriter* const fw) const;
  std::string ParsePrefix(const std::string& s) const;
  std::string ParseName(const std::string& s) const;
  void UnifyEntities(EntityPtr* l, EntityPtr* r);
  void GenerateOutput(std::map<std::string, EntityPtr*>* map);
  void GenerateExampleFiles(const std::string& p);
  void GenerateConfigExample(const std::string& p);
  void GenerateRelationMapExample(const std::string& p);
  void GenerateScoresExample(const std::string& p);
  /* statistical information */
  unsigned int unified_ = 0;
  unsigned int line_count_file_1_ = 0;
  unsigned int line_count_file_2_ = 0;
  bool debugging_ = false;
  // std::ostringstream _debug;
  // map of 'filename' => is-A relation count
  std::unordered_map<std::string, unsigned int> is_a_count_;
  /* private containers */
  // id -> entity
  std::map<std::string, EntityPtr*> entities_1_;
  std::map<std::string, EntityPtr*> entities_2_;
  std::list<Entity> entity_container_;
  std::list<EntityPtr> pointer_container_;
  std::unordered_map<std::string, std::string> relation_mapping_;
  std::unordered_map<std::string, std::string> double_relations_;
  std::unordered_map<std::string, double> scores_;
  /* private options */
  FileWriter debug_info_;
  FileWriter debug_error_;
  FileWriter output_writer_;
  size_t iterations_ = 1;
  std::string std_directory_ = "";
  std::string debug_filename_ = "debug.txt";
  std::string filename_1_;
  std::string filename_2_;
  bool show_merged_ = false;
  std::string relation_mapping_filename_;
  std::string output_filename_ = "out.txt";
  std::string scores_filename_;
  unsigned int entities_1_size_;
  unsigned int entities_2_size_;
};

#endif  // ENTITYUNIFICATION_SRC_UNIFICATION_H_
