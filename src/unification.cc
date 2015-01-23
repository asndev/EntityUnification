// Copyright 2013
// Anton Stepan <stepana@informatik.uni-freiburg.de>

#include "./unification.h"
#include <getopt.h>
#include <cmath>
#include <cassert>
#include <fstream>  // NOLINT
#include <string>
#include <sstream>
#include <algorithm>
#include "./utility-tools.h"
#include "./timing-helper.h"

const double kDoubleEpsilon = 0.2;
const int kPopulationEpsilon = 3500;

using std::string;
using std::cout;
using std::endl;

// _____________________________________________________________________________
Unification::Unification() {}

// _____________________________________________________________________________
void Unification::PrintUsage() const {
  std::cerr << std::endl
            << "Either use a standard folder with a config file or"
            << " manually set all parameters." << std::endl
            << Utilities::kBoldMode << "Usage: ./GeoReaderMain [options]"
            << std::endl
            << Utilities::kResetMode
            << Utilities::kBoldMode << "required options:"
            << Utilities::kResetMode << std::endl
            << "--standard-folder [s]: <path to config.conf directory>"
            << std::endl
            << "or" << std::endl
            << "--filename-1 [1]: <filename>" << std::endl
            << "--filename-2 [2]: <filename>" << std::endl
            << "--scores [c]: <filename>" << std::endl
            << Utilities::kBoldMode << "optional options:"
            << Utilities::kResetMode << std::endl
            << "--outputname [o]: <filename> --default: ./out.txt" << std::endl
            << "--relation-map [r]: <filename>" << std::endl
            << "--iterations [i]: <number>" << std::endl
            << "--debug [d]" << std::endl
            << "--generate-example-files [g]: <path>" << std::endl
            << "[g] generates examples for config.conf, relationmapping.conf"
            << " and scores.conf at the target path."
            << std::endl;
}

// _____________________________________________________________________________
void Unification::PrintStatistics() {
  std::cout << Utilities::kBoldMode << "Statistics" << Utilities::kResetMode
            << std::endl;
  std::cout << "Debugging " << (debugging() ? "on" : "off") << std::endl;
  std::cout << "File1: " << filename_1_
            << " (lines: " << line_count_file_1_ << ")" << std::endl;
  std::cout << "File2: " << filename_2_
            << " (lines: " << line_count_file_2_ << ")" << std::endl;
  std::cout << "Entities1: " << entities_1_size() << std::endl;
  std::cout << "Entities2: " << entities_2_size() << std::endl
            << std::endl;
  float percentage = static_cast<float>(unified()) /
        static_cast<float>(std::min(entities_1_size(), entities_2_size()));
  percentage *= 100 * 100;
  int tmp = percentage;
  percentage = static_cast<float>(tmp) / 100;
  std::cout << "Unified: " << unified() << " (" << percentage << "%)"
            << std::endl;
  // Other method to compute unifiation percentage.
  /*double old_sizes = entities_1_size() + entities_2_size();
  double new_sizes = entities_1_.size() + entities_2_.size();
  std::cout << 1.0 - new_sizes / old_sizes << std::endl;*/
}

// _____________________________________________________________________________
void Unification::GenerateOutput() {
  output_writer_.write("Output from " + TimingHelper::GetCurrentTime() + "\n");
  GenerateOutput(&entities_1_);
  GenerateOutput(&entities_2_);
}

// _____________________________________________________________________________
void Unification::GenerateOutput(std::map<std::string, EntityPtr*>* map) {
  std::ostringstream out;
  for (auto entity_it = map->begin();
       entity_it != map->end(); ++entity_it) {
    string entity_name = entity_it->second->ptr()->id();
    if (show_merged() && entity_it->second->ptr()->merged()) {
      entity_name += "[M]";
    }
    for (auto rela_it = entity_it->second->ptr()->relations.begin();
         rela_it != entity_it->second->ptr()->relations.end(); ++rela_it) {
      const string relation_name = rela_it->first;
      for (auto e : rela_it->second) {
        out << entity_name << '\t' << relation_name << '\t'
            << e->ptr()->id();
        if (show_merged() && e->ptr()->merged()) {
          out << "{{";
          std::vector<std::string> tmp_others = e->ptr()->other_ids();
          size_t other_size = tmp_others.size();
          for (size_t i = 0; i < other_size; ++i) {
            if (i > 0) out << " ,";
            out << tmp_others[i];
          }
          out << "}}";
        }
        out << '\n';
      }
    }
  }
  output_writer_.write(out.str());
}

// _____________________________________________________________________________
void Unification::UnifyEntities() {
  assert(entities_1_.size() != 0 && entities_2_.size() != 0);
  auto iter_a = entities_1_.begin(), iter_b = entities_2_.begin();
  size_t ten_percent_size = (entities_1_.size() + entities_2_.size()) / 10;
  size_t actual_size = 0, dots_so_far = 0;
  std::cout << "[ " << std::flush;
  while (iter_a != entities_1_.end() && iter_b != entities_2_.end()) {
    // check if entities could be the same
    while (iter_b != entities_2_.end() &&
           PreCheck(iter_a->second->ptr(), iter_b->second->ptr())) {
      bool unify = Unify(iter_a->second, iter_b->second);
      if (unify) entities_2_.erase(iter_b);
      ++iter_b;
      ++actual_size;
    }
    if (iter_a == entities_1_.end() || iter_b == entities_2_.end()) break;
    // increse pointers to map
    if (iter_a->second->ptr()->id() > iter_b->second->ptr()->id()) {
      ++iter_b;
      ++actual_size;
    } else {
      ++iter_a;
      ++actual_size;
    }
    if (actual_size >= ten_percent_size) {
      actual_size = 0;
      ++dots_so_far;
      std::cout << ". " << std::flush;
    }
  }
  // if less than 10 dots were printed.
  while (dots_so_far++ < 10) std::cout << ". " << std::flush;
  std::cout << "]" << std::flush;
}

// _____________________________________________________________________________
bool Unification::PreCheck(const Entity* const lhs,
                           const Entity* const rhs) {
  int diff = std::abs(lhs->number_of_words() - rhs->number_of_words());
  return (lhs->prefix() == rhs->prefix() && diff < Score("t_number_of_words"));
}

// _____________________________________________________________________________
bool Unification::Unify(EntityPtr* lE, EntityPtr* rE) {
  if (rE->ptr() == lE->ptr()) return false;
  double score = 0;
  int geo_bonus_score = 0;
  if ((rE->ptr()->relations.size() == 0 ||
       lE->ptr()->relations.size() == 0)) {
    if (debugging_) {
      WriteDebug(lE->ptr(), rE->ptr(),
                 "merging, no relations & precheck is true.", &debug_info_);
    }
    UnifyEntities(lE, rE);
    return true;
  }
  if (lE->ptr()->name() == rE->ptr()->name()) score += Score("s_entity_name");
  // check whether entities have similarities and no contradiction
  for (auto i = rE->ptr()->relations.begin(); i != rE->ptr()->relations.end();
       ++i) {
    const std::string& relation = i->first;
    if (relation == "located-in") {
      score += CheckLocatedIn(i->second, lE->ptr()->relations["located-in"]);
    } else if (relation == "contains") {
      score += CheckLocatedIn(i->second, lE->ptr()->relations["contains"]);
    } else if (relation == "has-longitude") {
      double s = CheckGeodata(i->second, lE->ptr()->relations["has-longitude"]);
      score += s;
      geo_bonus_score += s;
    } else if (relation == "has-latitude") {
      double s = CheckGeodata(i->second, lE->ptr()->relations["has-latitude"]);
      score += s;
      geo_bonus_score += s;
    } else if (relation == "has-population") {
      score +=
        CheckPopulation(i->second, lE->ptr()->relations["has-population"]);
    } else {
      score += CheckRelation(relation, lE, rE);
    }
  }
  if (geo_bonus_score > Score("t_geo_both_bonus")) {
    score += Score("s_geo_both_bonus");
  }
  if (score > Score("t_score_threshold")) {
    if (debugging()) {
      std::ostringstream title;
      title << "merged, Score: " << score;
      WriteDebug(lE->ptr(), rE->ptr(), title.str(), &debug_info_);
    }
    UnifyEntities(lE, rE);
    return true;
  } else {
    if (debugging()) {
      std::ostringstream title;
      title << "Equal, but didn't merge. Score: " << score;
      WriteDebug(lE->ptr(), rE->ptr(), title.str(), &debug_error_);
    }
    return false;
  }
}

// _____________________________________________________________________________
void Unification::UnifyEntities(EntityPtr* l, EntityPtr* r) {
  ++unified_;
  l->ptr()->set_merged(true);
  r->ptr()->set_merged(true);
  // Combine relations
  for (auto it = r->ptr()->relations.begin();
       it != r->ptr()->relations.end(); ++it) {
    for (auto e : it->second) {  // EntityPtr
      if (e->ptr() != l->ptr()) l->ptr()->AddRelation(it->first, e);
    }
  }
  assert(l->ptr() != r->ptr());
  l->ptr()->AddId(r->ptr()->id());
  r->set_ptr(l->ptr());  // right entity is getting merged to the left.
  l->ptr()->EraseMultipleRelations("has-longitude");
  l->ptr()->EraseMultipleRelations("has-latitude");
}

// _____________________________________________________________________________
double Unification::CheckLocatedIn(const std::vector<EntityPtr*>& l_loc,
                                   const std::vector<EntityPtr*>& r_loc) {
  // right entity has no 'located-in' relation
  if (r_loc.size() == 0) return 0;
  // check located-in for similarities or contradictions
  double score = 0;
  double score_name = Score("s_loc_in_name");
  double score_prefix = Score("s_loc_in_prefix");
  size_t hits = 0;
  std::unordered_map<string, size_t> locations;
  std::unordered_map<string, size_t> loc_prefix;
  for (auto l : l_loc) {
    locations[l->ptr()->name()] = 1;
    loc_prefix[l->ptr()->prefix()] = 1;
  }
  for (auto r : r_loc) {
    string name = r->ptr()->name();
    string prefix = r->ptr()->prefix();
    if (locations.find(name) != locations.end()) {
      ++hits;
      score += score_name;
    }
    if (loc_prefix.find(prefix) != loc_prefix.end()) score += score_prefix;
  }
  float ratio = hits / static_cast<float>(std::min(l_loc.size(), r_loc.size()));
  if (ratio >= Score("t_loc_in_ratio_bonus")) {
    score += Score("s_loc_in_ratio_bonus");
  }
  return score;
}

// _____________________________________________________________________________
double Unification::CheckGeodata(const std::vector<EntityPtr*>& l,
                                 const std::vector<EntityPtr*>& r) {
  if (l.size() == 0 || r.size() == 0) return 0;
  double lCoord, rCoord;
  double score = 0;
  double t_geo_max = Score("t_geo_max"), s_geo_max = Score("s_geo_max");
  double t_geo_med = Score("t_geo_med"), s_geo_med = Score("s_geo_med");
  double t_geo_min = Score("t_geo_min"), s_geo_min = Score("s_geo_min");
  double t_geo_false = Score("t_geo_false"), s_geo_false = Score("s_geo_false");
  for (auto iter_l : l) {
    std::stringstream(iter_l->ptr()->name().c_str()) >> lCoord;
    for (auto iter_r : r) {
      std::stringstream(iter_r->ptr()->name().c_str()) >> rCoord;
      double diff = std::abs(lCoord - rCoord);
      if (diff <= t_geo_max) {
        score += s_geo_max;
      } else if (diff <= t_geo_med) {
        score += s_geo_med;
      } else if (diff <= t_geo_min) {
        score += s_geo_min;
      } else if (diff >= t_geo_false) {
        score += s_geo_false;
      }
    }
  }
  return score;
}

// _____________________________________________________________________________
double Unification::CheckPopulation(const std::vector<EntityPtr*>& l,
                                    const std::vector<EntityPtr*>& r) {
  if (l.size() == 0 || r.size() == 0) return 0;
  double score = 0;
  std::vector<int> left_populations, right_populations;
  for (auto iter_l : l) {
    left_populations.push_back(
      std::atoi(ParsePopulation(iter_l->ptr()->name()).c_str()));
  }
  for (auto iter_r : r) {
    right_populations.push_back(
      std::atoi(ParsePopulation(iter_r->ptr()->name()).c_str()));
  }
  std::sort(left_populations.begin(), left_populations.end());
  std::sort(right_populations.begin(), right_populations.end());
  double diff = std::abs(left_populations[0] - right_populations[0]);
  if (diff <= Score("t_population_max")) {
    return Score("s_population_max");
  } else if (diff <= Score("t_population_med")) {
    return Score("s_population_med");
  } else if (diff >= Score("t_population_false")) {
    return Score("s_population_false");
  }
  return score;
}

// _____________________________________________________________________________
std::string Unification::ParsePopulation(const std::string& s) {
  size_t pos = s.find('_');
  if (pos != string::npos) {
    return s.substr(pos + 1);
  }
  return s;
}

// _____________________________________________________________________________
double Unification::CheckRelation(const std::string relation, EntityPtr* rE,
                                  EntityPtr* lE) {
  std::map<std::string, std::vector<EntityPtr*>>* rel_1 =
    &(rE->ptr()->relations);
  std::map<std::string, std::vector<EntityPtr*>>* rel_2 =
    &(lE->ptr()->relations);
  double score = 0;
  double name_score = Score("s_misc_relation_name");
  double prefix_score = Score("s_misc_relation_prefix");
  if (rel_1->find(relation) != rel_1->end() &&
      rel_2->find(relation) != rel_2->end()) {
    for (auto ele_1 : (*rel_1)[relation]) {
      for (auto ele_2 : (*rel_2)[relation]) {
        if (ele_1->ptr()->name() == ele_2->ptr()->name())
          score += name_score;;
        if (ele_1->ptr()->prefix() == ele_2->ptr()->prefix())
          score += prefix_score;
      }
    }
  }
  return score;
}

// _____________________________________________________________________________
void Unification::ProcessFiles() {
  if (!relation_mapping_filename_.empty()) ParseRelationMappingFile();
  if (!scores_filename_.empty()) ParseScoresFile();
  if (debugging()) {
    debug_info_.write("Debug from " + TimingHelper::GetCurrentTime() + "\n");
  }
  line_count_file_1_ = ProcessFile(filename_1_, &entities_1_);
  line_count_file_2_ = ProcessFile(filename_2_, &entities_2_);
  entities_1_size_ = entities_1_.size();
  entities_2_size_ = entities_2_.size();
}

// _____________________________________________________________________________
unsigned int Unification::ProcessFile(const std::string& filename,
                                      std::map<std::string, EntityPtr*>* map) {
  assert(map != NULL);
  unsigned int line_count = 0;
  is_a_count_[filename] = 0;
  unsigned int* is_a_ptr;
  is_a_ptr = &is_a_count_[filename];
  std::ifstream file(filename.c_str());
  if (!file.good()) {
    Utilities::Error(filename + " couldn't be opened.");
    exit(0);
  }
  string line;
  while (true) {
    if (file.eof()) break;
    getline(file, line);
    if (line != "") {
      ++line_count;
      size_t pos1 = 0, pos2;
      pos1 = line.find('\t', pos1);
      pos2 = line.find('\t', pos1 + 1);
      if (pos1 != string::npos && pos2 != string::npos) {
        string subject = line.substr(0, pos1);
        string predicate = TransformRelation(
          line.substr(pos1 + 1, pos2 - pos1 - 1));
        string object = line.substr(pos2 + 1);
        string subject_name = ParseName(subject);
        string object_name = ParseName(object);
        string subject_prefix = ParsePrefix(subject);
        string object_prefix = ParsePrefix(object);
        if (CheckStrings(subject, object, predicate)) continue;
        if (predicate == "is-a") (*is_a_ptr)++;
        assert(subject != object);  // is this obvious?
        // find object
        EntityPtr* entity_pointer;
        if (map->find(object) != map->end()) {
          entity_pointer = (*map)[object];
        } else {
          entity_container_.push_back(
            Entity(object, object_name, object_prefix));
          pointer_container_.push_back(EntityPtr(&(entity_container_.back())));
          (*map)[object] = &(pointer_container_.back());
          entity_pointer = (*map)[object];
        }
        if (map->find(subject) == map->end()) {
          entity_container_.push_back(
            Entity(subject, subject_name, subject_prefix));
          pointer_container_.push_back(EntityPtr(&(entity_container_.back())));
          (*map)[subject] = &(pointer_container_.back());
        }
        assert(entity_pointer != NULL);
        // Add Relation
        (*map)[subject]->ptr()->AddRelation(predicate, entity_pointer);
        if (double_relations_.find(predicate) != double_relations_.end()) {
          entity_pointer->ptr()->AddRelation(double_relations_[predicate],
                                             (*map)[subject]);
        }
        // Code for inverse relation.
        /*if (predicate.substr(predicate.size() - 3) == "--I") {
          entity_pointer->ptr()->AddRelation(
            predicate.substr(0, predicate.size() - 3), (*map)[subject]);
        } else {
          (*map)[subject]->ptr()->AddRelation(predicate, entity_pointer);
        }*/
      }
    }
  }
  file.close();
  return line_count;
}

// _____________________________________________________________________________
bool Unification::CheckStrings(const std::string& a, const std::string& b,
                               const std::string& c) {
  bool out = (a == "") || (b == "") || (c == "") || (a == "-") || (b == "-")
             || (c == "-");
  return out;
}

// _____________________________________________________________________________
std::string Unification::ParsePrefix(const std::string& s) const {
  string prefix = s.substr(0, s.find("_"));
  size_t comma_pos = prefix.find(',');
  if (comma_pos != string::npos) {
    prefix = prefix.substr(0, prefix.find(','));
  }
  return prefix;
}

// _____________________________________________________________________________
std::string Unification::ParseName(const std::string& s) const {
  string name = s;
  if (s.find("_(") != string::npos) {
    name = s.substr(0, s.find("_("));
    if (name.find(",") != string::npos) {
      name = name.substr(0, name.find(","));
    }
  }
  return name;
}

// _____________________________________________________________________________
void Unification::WriteDebug(const std::string& s) {
  debug_info_.write(s);
}

// _____________________________________________________________________________
void Unification::WriteDebug(Entity* const rhs, Entity* const lhs,
                             const std::string& title,
                             FileWriter* const fw) const {
  std::ostringstream i;
  i << "\n"
    << title << '\n'
    << "\"" << lhs->name() << "\"" << ":"
    << "\"" << lhs->id() << "\""
    << " == "
    << "\"" << rhs->name() << "\"" << ":"
    << "\"" << rhs->id() << "\"" << '\n'
    << lhs->GetPretty() << '\n'
    << rhs->GetPretty() << '\n'
    << "- - - - -\n";
  fw->write(i.str());
}

// _____________________________________________________________________________
std::string Unification::TransformRelation(const std::string& s) {
  if (relation_mapping_.find(s) != relation_mapping_.end()) {
    return relation_mapping_[s];
  }
  return s;
}

// _____________________________________________________________________________
void Unification::ParseRelationMappingFile() {
  using std::unordered_map;
  std::ifstream file(relation_mapping_filename_.c_str());
  if (!file.good()) {
    Utilities::Error(relation_mapping_filename_ + " couldn't be opened.");
    exit(0);
  }
  std::string line, map_name;
  while (true) {
    if (file.eof()) break;
    getline(file, line);
    if (line != "" && line.substr(0, 2) != "//") {  // NOLINT
      if (line.substr(0, 3) == "###") {
        map_name = line.substr(line.find('\t') + 1);
      }
      size_t pos1 = 0;
      pos1 = line.find('\t', 0);
      if (pos1 != string::npos) {
        string lhs = line.substr(0, pos1);
        string rhs = line.substr(pos1 + 1);
        if (map_name == "relation-mapping") {
          relation_mapping_[lhs] = rhs;
        } else if (map_name == "double-relations") {
          double_relations_[lhs] = rhs;
        } else {
          std::cout << "Unknown relation map name: " << map_name << std::endl;
          std::cout << "Line: " << line << std::endl;
          exit(0);
        }
      }
    }
  }
  file.close();
}

// _____________________________________________________________________________
void Unification::ParseScoresFile() {
  std::ifstream file(scores_filename().c_str());
  if (!file.good()) {
    Utilities::Error(scores_filename() + " couldn't be opened.");
    exit(0);
  }
  std::string line;
  while (true) {
    if (file.eof()) break;
    getline(file, line);
    if (line != "" && line.substr(0, 2) != "//") {  // NOLINT
      size_t pos1 = 0;
      pos1 = line.find('\t', 0);
      if (pos1 != string::npos) {
        string lhs = line.substr(0, pos1);
        string rhs = line.substr(pos1 + 1);
        if (scores_.find(lhs) != scores_.end()) {
          Utilities::Error("Mutliple entries in scores config: " + lhs);
          exit(0);
        }
        scores_[lhs] = std::atof(rhs.c_str());
      }
    }
  }
  file.close();
}

// _____________________________________________________________________________
double Unification::Score(const std::string& value) {
  if (scores_.find(value) == scores_.end()) {
    Utilities::Error("Couldn't find score for: " + value);
    exit(0);
  }
  return scores_[value];
}

// _____________________________________________________________________________
void Unification::GenerateExampleFiles(const std::string& p) {
  string path = p;
  path = (*(path.rbegin()) != '/') ? path += "/" : path;
  GenerateConfigExample(path);
  GenerateRelationMapExample(path);
  GenerateScoresExample(path);
}

// _____________________________________________________________________________
void Unification::GenerateConfigExample(const std::string& p) {
  std::ostringstream oss;
  oss << "// Config File Example, created: " << TimingHelper::GetCurrentTime()
      << '\n'
      << "// i.e.: debug=debug.txt" << '\n'
      << "debug=debug.txt" << '\n'
      << "input1=freebase-31-05.n3" << '\n'
      << "input2=geonames-triples-sorted.n3" << '\n'
      << "relation-map=relationmapping.conf" << '\n'
      << "scores=scores.conf" << '\n'
      << "iterations=2" << '\n'
      << "// true or false" << '\n'
      << "show-merged=true" << '\n';
  Utilities::GenerateFileIfNotExists(p + "config.conf", oss);
}

// _____________________________________________________________________________
void Unification::GenerateRelationMapExample(const std::string& p) {
  std::ostringstream oss;
  oss << "// Relation-Mapping File Example, created: "
      << TimingHelper::GetCurrentTime()
      << '\n'
      << "// Relation Mapping" << '\n'
      << "// '###' are delimiters for sections in this file." << '\n'
      << "// 'relation' from column 0 is getting mapped to 'relation' "
      << "in column 1." << '\n'
      << "// append '--I' at target relation to invert subject &"
      << " object from relation." << '\n'
      << "// i.e. A located-in--I B => B located-in A" << '\n'
      << "// Inverse relation is commented out in unification.cc" << '\n'
      << "###\trelation-mapping" << '\n'
      << "partially-contains\tcontains" << '\n'
      << "// Double Relation" << '\n'
      << "// take both relation, i.e." << '\n'
      << "// if (A located-in B) exists, then also use (B contains A)." << '\n'
      << "###\tdouble-relations" << '\n'
      << "located-in\tcontains" << '\n'
      << "contains\tlocated-in" << '\n';
  Utilities::GenerateFileIfNotExists(p + "relationmapping.conf", oss);
}

// _____________________________________________________________________________
void Unification::GenerateScoresExample(const std::string& p) {
  std::ostringstream oss;
  oss << "// Score Config File, created: " << TimingHelper::GetCurrentTime()
      << "\n"
      << "// best practice: s_ for scores, and t_ for thresholds/tolerances\n"
      << "\n// All threshholds are <= or >=.\n"
      << "\n// abs(#words(entity1) - #words(entity2)) <= t_number_of_words\n"
      << "\n// i.e.: Berlin & Berlin_Train_Station_North\n"
      << "t_number_of_words\t3\n\n"
      << "// UNIFICATION\n\n"
      << "// Score Threshold to unify entities\n"
      << "t_score_threshold\t6\n"
      << "// if both entities have the same name\n"
      << "s_entity_name\t4\n\n"
      << "// RELATIONS\n\n"
      << "// located-in: vector<locations> A, vector<locations> B\n"
      << "s_loc_in_name\t1\n"
      << "s_loc_in_prefix\t0\n\n"
      << "// bonus if a certain ratio of locations are shared\n"
      << "t_loc_in_ratio_bonus\t0.6\n"
      << "s_loc_in_ratio_bonus\t1\n\n"
      << "// GeoData (lat/long): vector<geodata> A, vector<geodata> B\n"
      << "t_geo_max\t0.01\n"
      << "s_geo_max\t3\n\n"
      << "t_geo_med\t0.2\n"
      << "s_geo_med\t1\n\n"
      << "t_geo_min\t2\n"
      << "s_geo_min\t0\n\n"
      << "t_geo_false\t2.1\n"
      << "s_geo_false\t-1\n\n"
      << "// Bonus score if score(lat)+score(long) >= t_geo_both_bonus\n"
      << "t_geo_both_bonus\t6\n"
      << "s_geo_both_bonus\t2\n\n"
      << "// Population: vector<int> A, vector<int> B\n"
      << "t_population_max\t3500\n"
      << "s_population_max\t3\n\n"
      << "t_population_med\t10000\n"
      << "s_population_med\t1\n\n"
      << "// >= t\n"
      << "t_population_false\t25000\n"
      << "s_population_false\t-1\n\n"
      << "// other Relations\n"
      << "s_misc_relation_name\t3\n"
      << "s_misc_relation_prefix\t1\n";
  Utilities::GenerateFileIfNotExists(p + "scores.conf", oss);
}


// _____________________________________________________________________________
void Unification::SetStandardParameters(const std::string& path) {
  std::string config = std::string(path + "config.conf");
  std::ifstream file(config.c_str());
  if (!file.good()) {
    Utilities::Error(path + "config.conf couldn't be opened.");
    exit(0);
  }
  std::string line;
  while (true) {
    if (file.eof()) break;
    getline(file, line);
    if (line != "" && line.substr(0,2) != "//") {  // NOLINT
      size_t pos = 0;
      std::string config, value;
      pos = line.find('=', 0);
      if (pos == string::npos) {
        Utilities::Error("Error in configline: " + line);
        exit(0);
      }
      bool i = ParseConfig(line.substr(0, pos), line.substr(pos + 1));
      if (!i) {
        Utilities::Error("Can't parse configline: " + line);
        exit(0);
      }
    }
  }
}

// _____________________________________________________________________________
bool Unification::ParseConfig(const string& option, const string& value) {
  if (option == "debug") {
    if (value == "") {
      debugging_ = false;
    } else {
      debugging_ = true;
      debug_filename_ = std_directory_ + value;
    }
  } else if (option == "input1") {
    filename_1_ = std_directory_ + value;
  } else if (option == "input2") {
    filename_2_ = std_directory_ + value;
  } else if (option == "output") {
    output_filename_ = std_directory_ + value;
  } else if (option == "iterations") {
    std::stringstream(value) >> iterations_;
  } else if (option == "relation-map") {
    relation_mapping_filename_ = std_directory_ + value;
  } else if (option == "show-merged") {
    std::istringstream(value) >> std::boolalpha >> show_merged_;
  } else if (option == "scores") {
    scores_filename_ = std_directory_ + value;
  } else {
    Utilities::Error("Can't parse config attr.: " + option + " " + value);
    return false;
  }
  return true;
}

// _____________________________________________________________________________
bool Unification::ParseCommandlineArguments(int argc, char** argv) {
  // Structure for the commandline arguments.
  struct option options[] = {
    {"standard-folder"       , 1, NULL, 's'},
    {"filename-1"            , 1, NULL, '1'},
    {"filename-2"            , 1, NULL, '2'},
    {"relation-map"          , 1, NULL, 'r'},
    {"iterations"            , 1, NULL, 'i'},
    {"outputname"            , 1, NULL, 'o'},
    {"scores"                , 1, NULL, 'c'},
    {"generate-example-files", 1, NULL, 'g'},
    {"debug"                 , 0, NULL, 'd'},
    {NULL                    , 0, NULL,  0 }
  };
  // NOTE: optind is a global variable that is declared in getopt.h included
  // above. It is the index of the next argument in argv to be parsed. It is
  // intialized to 1 in getopt.h.
  optind = 1;
  // Now process all options.
  // NOTE: The calls to getopt_long will re-arrange the order of the strings in
  // argv such that after all options have been processed, all the remaining
  // (non-option) arguments will be at the end of argv, and optind will be the
  // index of the first such non-option argument.
  while (true) {
    int c = getopt_long(argc, argv, "s:1:2:r:i:g:c:o:d", options, NULL);
    if (c == -1) break;
    switch (c) {
      case 's':
        std_directory_ = optarg;
        if (*(std_directory_.rbegin()) != '/') std_directory_ += "/";
        break;
      case '1':
        filename_1_ = optarg;
        break;
      case '2':
        filename_2_ = optarg;
        break;
      case 'r':
        relation_mapping_filename_ = optarg;
        break;
      case 'i':
        std::stringstream(optarg) >> iterations_;
        break;
      case 'o':
        output_filename_ = optarg;
        break;
      case 'd':
        debugging_ = true;
        break;
      case 'c':
        scores_filename_ = optarg;
        break;
      case 'g':
        GenerateExampleFiles(optarg);
        std::cout << "Example files created at: " << optarg << std::endl;
        exit(0);
        break;
      default:
        // For all other options, print usage info and exit.
        return false;
    }
  }
  if (!std_directory_.empty()) {
    output_filename_ = std_directory_ + "out.txt";
    SetStandardParameters(std_directory_);
  }
  if (filename_1_.empty() || filename_2_.empty()) {
    Utilities::Error("filename1 and filename2 need to be set!");
    return false;
  }
  int file_1_length = Utilities::GetFileSize(filename_1_);
  int file_2_length = Utilities::GetFileSize(filename_2_);
  if (file_1_length > file_2_length) { filename_1_.swap(filename_2_); }
  if (debugging()) {
    debug_info_.set_stream(debug_filename_);
    debug_error_.set_stream(debug_filename_ + ".not_merged");
  }
  output_writer_.set_stream(output_filename_);
  return true;
}
