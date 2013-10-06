#include "_dmp.h"

#include <string>
#include "diff_match_patch-stl/diff_match_patch.h"

#if 0
/*
* A lightweight wrapper around Patch, so Rice can get at it. Delegates 
* to the patch as necissary
*/
class rb_patch_wrapper{
  
  public:

    dmp::Patch patch;

    rb_patch_wrapper(dmp::Patch &the_patch) : patch(the_patch) {}

    int start1(){
      return patch.start1;
    }

    int start2(){
      return patch.start2;
    }

    int length1(){
      return patch.length1;
    }

    int length2(){
      return patch.length2;
    }

    std::string toString(){
      return patch.toString();
    }

    bool isNull(){
      return patch.isNull();
    }

};


/*
* A subclass of diff_match_patch<std::string>, adding methods for
* translating between C++ and Ruby.
*/
class rb_diff_match_patch : dmp {

protected:

  Diffs diffsFromRubyArray(Rice::Array &array, bool clearArray=false){
    Diffs diffs;
    size_t arraySize = array.size();
    for (size_t i = 0; i < arraySize; ++i){
      Rice::Array rb_diff;
      if (clearArray) {
        rb_diff = from_ruby<Rice::Array>(array.shift());
      } else {
        rb_diff = from_ruby<Rice::Array>(array[i]);
      }

      Operation op;
      switch (from_ruby<int>(rb_diff[0])) {
        case 1:
          op = INSERT;
          break;
        case 0:
          op = EQUAL;
          break;
        case -1:
          op = DELETE;
          break;
      }
      diffs.push_back(dmp::Diff(op, from_ruby<std::string>(rb_diff[1])));

    }
    return diffs;
  }

  Rice::Array rubyArrayFromDiffsWithArray(dmp::Diffs diffs, Rice::Array &out){
    Diffs::iterator current_diff;
    for (current_diff = diffs.begin(); current_diff != diffs.end(); ++current_diff) {
      Rice::Array rb_diff;
      switch (current_diff->operation){
        case INSERT:
          rb_diff.push(1);
          break;
        case DELETE:
          rb_diff.push(-1);
          break;
        case EQUAL:
          rb_diff.push(0);
          break;
      }
      rb_diff.push(current_diff->text);
      out.push(rb_diff);
    }
      
    return out;
  }

  Rice::Array rubyArrayFromDiffs(const dmp::Diffs &diffs){
    Rice::Array out;
    return rubyArrayFromDiffsWithArray(diffs, out);
  }

  Rice::Array rubyArrayFromPatches(Patches &patches){
    Rice::Array out;
    Patches::iterator current_patch = patches.begin();
    for (current_patch = patches.begin(); current_patch != patches.end(); ++current_patch){
      rb_patch_wrapper patch = rb_patch_wrapper(*current_patch);
      out.push(patch);
    }
    return out;
  }

  Patches patchesFromRubyArray(Rice::Array &array){
    Patches patches;
    for (size_t i = 0; i < array.size(); ++i) {
      rb_patch_wrapper wrapper = from_ruby<rb_patch_wrapper>(array[i]);
      patches.push_back(wrapper.patch);
    }
    return patches;
  }

public:

  rb_diff_match_patch(){}
  
  Rice::Object rb_diff_main(const std::string &text1, const std::string &text2, bool checklines=false){
    Diffs diffs = diff_main(text1, text2, checklines);
    return rubyArrayFromDiffs(diffs);
  }

  float GetDiff_Timeout(){
    return Diff_Timeout;
  }

  void SetDiff_Timeout(float value){
    Diff_Timeout = value;
  }

  float GetDiff_EditCost(){
    return Diff_EditCost;
  }

  void SetDiff_EditCost(float value){
    Diff_EditCost = value;
  }

  Rice::Array rb_diff_cleanupSemantic(Rice::Array array){
    Diffs diffs = diffsFromRubyArray(array, true);
    diff_cleanupSemantic(diffs);
    return rubyArrayFromDiffsWithArray(diffs, array);
  }

  Rice::Array rb_diff_cleanupEfficiency(Rice::Array array){
    Diffs diffs = diffsFromRubyArray(array, true);
    diff_cleanupEfficiency(diffs);
    return rubyArrayFromDiffsWithArray(diffs, array);
  }

  int rb_diff_levenshtein(Rice::Array array){
    Diffs diffs = diffsFromRubyArray(array);
    return diff_levenshtein(diffs);
  }

  std::string rb_diff_prettyHtml(Rice::Array array){
    Diffs diffs = diffsFromRubyArray(array);
    return diff_prettyHtml(diffs);
  }

  int rb_match_main(const string_t &text, const string_t &pattern, int loc) const {
    return match_main(text, pattern, loc);
  }

  float GetMatch_Threshold(){
    return Match_Threshold;
  }

  void SetMatch_Threshold(float value){
    Match_Threshold = value;
  }

  int GetMatch_Distance(){
    return Match_Distance;
  }

  void SetMatch_Distance(int value){
    Match_Distance = value;
  }

  float GetPatch_DeleteThreshold(){
    return Patch_DeleteThreshold;
  }

  void SetPatch_DeleteThreshold(float value){
    Patch_DeleteThreshold = value;
  }

  Rice::Array rb_patch_fromText(const std::string &str){
    Patches patches = patch_fromText(str);
    return rubyArrayFromPatches(patches);
  }

  std::string rb_patch_toText(Rice::Array array){
    Patches patches = patchesFromRubyArray(array);
    return patch_toText(patches);
  }

  Rice::Array rb_patch_make_from_texts(std::string text1, std::string text2){
    Patches patches = patch_make(text1, text2);
    return rubyArrayFromPatches(patches);
  }

  Rice::Array rb_patch_make_from_diffs(Rice::Array array){
    Diffs diffs = diffsFromRubyArray(array);
    Patches patches = patch_make(diffs);
    return rubyArrayFromPatches(patches);
  }

  Rice::Array rb_patch_make_from_text_and_diff(std::string text1, Rice::Array array){
    Diffs diffs = diffsFromRubyArray(array);
    Patches patches = patch_make(text1, diffs);
    return rubyArrayFromPatches(patches);
  }

  Rice::Array rb_patch_apply(Rice::Array patch_array, std::string text_1){
    Patches patches = patchesFromRubyArray(patch_array);
    std::pair<std::string, std::vector<bool> > results = patch_apply(patches,text_1);
    Rice::Array out;
    out.push(results.first);
    
    Rice::Array bool_array;
    for (size_t i = 0; i < results.second.size(); ++i){
      bool_array.push(results.second[i] ? true : false);
    }
    out.push(bool_array);
    
    return out;
  }

};

#endif

VALUE cDiffMatchPatch;

static void dealloc(void * ctx) {
  delete reinterpret_cast<dmp *>(ctx);
}

static VALUE allocate(VALUE klass) {
  dmp * ctx = new dmp();

  return Data_Wrap_Struct(klass, 0, dealloc, ctx);
}

static VALUE rubyArrayFromDiffsWithArray(dmp::Diffs diffs, VALUE out){
  dmp::Diffs::iterator current_diff;
  for (current_diff = diffs.begin(); current_diff != diffs.end(); ++current_diff) {
    VALUE rb_diff = rb_ary_new();
    switch (current_diff->operation){
      case dmp::INSERT:
        rb_ary_push(rb_diff, INT2NUM(1));
        break;
      case dmp::DELETE:
        rb_ary_push(rb_diff, INT2NUM(-1));
        break;
      case dmp::EQUAL:
        rb_ary_push(rb_diff, INT2NUM(0));
        break;
    }
    rb_ary_push(rb_diff, rb_str_new(current_diff->text.c_str(),
                                    current_diff->text.size()));
    rb_ary_push(out, rb_diff);
  }

  return out;
}

static VALUE rb_diff_main(VALUE self, VALUE text1, VALUE text2, VALUE lines)
{
  dmp * ctx;
  bool flag;
  VALUE rb_diffs;
  Data_Get_Struct(self, dmp, ctx);

  if (lines == Qtrue)
    flag = true;
  else
    flag = false;

  dmp::Diffs diffs = ctx->diff_main(StringValuePtr(text1),
                               StringValuePtr(text2),
                               flag);

  rb_diffs = rb_ary_new();
  return rubyArrayFromDiffsWithArray(diffs, rb_diffs);
}

static VALUE rb_diff_timeout(VALUE self) {
  dmp * ctx;
  Data_Get_Struct(self, dmp, ctx);
  return DBL2NUM(ctx->Diff_Timeout);
}

static VALUE rb_set_diff_timeout(VALUE self, VALUE v) {
  dmp * ctx;
  Data_Get_Struct(self, dmp, ctx);
  ctx->Diff_Timeout = NUM2DBL(v);

  return v;
}

static VALUE rb_diff_edit_cost(VALUE self) {
  dmp * ctx;
  Data_Get_Struct(self, dmp, ctx);
  return DBL2NUM(ctx->Diff_EditCost);
}

static VALUE rb_set_diff_edit_cost(VALUE self, VALUE v) {
  dmp * ctx;
  Data_Get_Struct(self, dmp, ctx);
  ctx->Diff_EditCost = NUM2DBL(v);

  return v;
}

void register_dmp(){

  cDiffMatchPatch = rb_define_class("DiffMatchPatch", rb_cObject);

  rb_define_alloc_func(cDiffMatchPatch, allocate);
  rb_define_method(cDiffMatchPatch, "diff_main", (ruby_method_vararg *)rb_diff_main, 3);
  rb_define_method(cDiffMatchPatch, "diff_timeout", (ruby_method_vararg *)rb_diff_timeout, 0);
  rb_define_method(cDiffMatchPatch, "diff_timeout=", (ruby_method_vararg *)rb_set_diff_timeout, 1);
  rb_define_method(cDiffMatchPatch, "diff_edit_cost", (ruby_method_vararg *)rb_diff_edit_cost, 0);
  rb_define_method(cDiffMatchPatch, "diff_edit_cost=", (ruby_method_vararg *)rb_set_diff_edit_cost, 1);

  /*
  rb_cDMP.define_method("diff_edit_cost", &rb_diff_match_patch::GetDiff_EditCost);
  rb_cDMP.define_method("diff_edit_cost=", &rb_diff_match_patch::SetDiff_EditCost);
  rb_cDMP.define_method("diff_cleanup_semantic!", &rb_diff_match_patch::rb_diff_cleanupSemantic);
  rb_cDMP.define_method("diff_cleanup_efficiency!", &rb_diff_match_patch::rb_diff_cleanupEfficiency);
  rb_cDMP.define_method("diff_levenshtein", &rb_diff_match_patch::rb_diff_levenshtein);
  rb_cDMP.define_method("diff_pretty_html", &rb_diff_match_patch::rb_diff_prettyHtml);

  rb_cDMP.define_method("match_main", &rb_diff_match_patch::rb_match_main);
  rb_cDMP.define_method("match_threshold", &rb_diff_match_patch::GetMatch_Threshold);
  rb_cDMP.define_method("match_threshold=", &rb_diff_match_patch::SetMatch_Threshold);
  rb_cDMP.define_method("match_distance", &rb_diff_match_patch::GetMatch_Distance);
  rb_cDMP.define_method("match_distance=", &rb_diff_match_patch::SetMatch_Distance);

  rb_cDMP.define_method("patch_delete_threshold", &rb_diff_match_patch::GetPatch_DeleteThreshold);
  rb_cDMP.define_method("patch_delete_threshold=", &rb_diff_match_patch::SetPatch_DeleteThreshold);
  rb_cDMP.define_method("patch_from_text", &rb_diff_match_patch::rb_patch_fromText);
  rb_cDMP.define_method("patch_to_text", &rb_diff_match_patch::rb_patch_toText);
  rb_cDMP.define_method("__patch_make_from_texts__", &rb_diff_match_patch::rb_patch_make_from_texts);
  rb_cDMP.define_method("__patch_make_from_diffs__", &rb_diff_match_patch::rb_patch_make_from_diffs);
  rb_cDMP.define_method("__patch_make_from_text_and_diff__", &rb_diff_match_patch::rb_patch_make_from_text_and_diff);
  rb_cDMP.define_method("patch_apply", &rb_diff_match_patch::rb_patch_apply);

  Rice::Data_Type< rb_patch_wrapper > rb_cPatch = Rice::define_class< rb_patch_wrapper >("Patch");
  rb_cPatch.define_method("to_string", &rb_patch_wrapper::toString);
  rb_cPatch.define_method("is_null?", &rb_patch_wrapper::isNull);
  rb_cPatch.define_method("start_1", &rb_patch_wrapper::start1);
  rb_cPatch.define_method("start_2", &rb_patch_wrapper::start2);
  rb_cPatch.define_method("length_1", &rb_patch_wrapper::length1);
  rb_cPatch.define_method("length_2", &rb_patch_wrapper::length2);
  */
}

#undef dmp