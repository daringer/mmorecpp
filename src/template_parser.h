#ifndef TEMPLATE_PARSER_H
#define TEMPLATE_PARSER_H

#include <fstream>
#include <vector>
#include <map>

#include <cassert>
#include <cstdlib>

#include "general.h"
#include "xstring.h"

namespace TOOLS {

DEFINE_EXCEPTION(SaveFilenameAmbigous, BaseException);
DEFINE_EXCEPTION(NoTemplateProvided, BaseException);
DEFINE_EXCEPTION(SubTemplateNameNotFound, BaseException);

/**
 * @brief Represents one node inside the constructed abstract syntax tree
 */
class ASTNode {
 public:
  std::string type;
  XString content;

  ASTNode* parent;
  std::vector<ASTNode*> children;

  ASTNode();
  ASTNode(const ASTNode& node);
  ASTNode(const std::string& type, ASTNode* parent, const std::string& content);
  virtual ~ASTNode();
};
/**
 * @brief The class, which actually implements the TemplateParser.
 *        This TemplateEngine/Parser provides a very simple
 *        domain-specific-language, that can be used to generate
 *        files from templates for various languages.
 *
 * There are two types of 'input' the TemplateEngine understands:
 *   - a variable, is simply inserted like this {{ my_var }}
 *   - a controlstructure is always built like this {% if my_second_var %}
 *
 * You have to 'feed' the data and the fitting key/variable -names into
 * the TemplateParser instance. This happens through
 * the set_key() respectivly add_to_key() methods.
 *
 * Usage example:
 * ----------------------------------------------------------------------
 * TemplateParser p("mytemplatefile.tmpl");      // init template parser
 *
 * p.set_key("title", "my_fancy_title");         // fill a key with a string
 *
 * p.add_to_key("mylist", "item1");              // automaticly declare "mylist"
 * p.add_to_key("mylist", "item2");              // as a list of strings
 *
 * tStringList sl = p.get_subtemplates();        // list of sub-templates
 *(names)
 *
 * TemplateParser sub =
 *     p.new_subtemplate("mysubtemplate");       // (Sub)TemplateParser instance
 * sub.set_key("subkey", "datastring");
 * sub.set_key("anotherkey", "datafoo");
 *
 * p.add_to_key("multi_lvl_array", sub);         // add SubTemplateParser
 *instance
 *                                                  to parent TemplateParser
 *
 * p.save_to_file();                             // save rendered file
 *
 * p.replace_template("other_file");             // replace internal template
 *                                               // while keeping keys
 * p.save_to_file("/tmp/my_choosen_filename");   // re-render with new template
 *                                               // and save to given filename
 * -----------------------------------------------------------------------
 * If the template filename ends with ".tmpl", save_to_file() without a
 * parameter leads to a filename without the ".tmpl" suffix - is the suffix
 * and the save_to_file()-argument missing an exception is thrown.
 *
 * The provided set of functionalities at this moment:
 *   - simple replacement of variables {{ simple }}
 *
 *   - simple sub-template (its content is handled as a separate template)
 *     {% subtemplate myname1 %}
 *       Something {{ my var }} Something else ...
 *     {% end %}
 *
 *   - simple resolved replacement of vector variables {{ my_vector.i }}
 *
 *   - length of a filled vector {{ #my_vector }} (also usable in for/if)
 *
 *   - 'if-else-end' control-structure, only one variable and no
 *     expression allowed. There is an exception for 'not', which simply
 *     does what you expect with the expression/variable:
 *     \code
 *     {% if not simple %}it_was_true{% else %}and_here_false{% end %}
 *     \endcode
 *
 *   - 'for' control-structure, can count from any given variable
 *     up to any other. Supports last_loop flag, first_loop flag, outputting
 *     iteration var:
 *     \code
 *     {% for i to range from offset %}
 *       {% if i.first_loop %}something special in 1st loop{% end %}
 *       Write Line {{ i }} - some content outputted every iteration
 *       {% if not i.last_loop %}everytime except in the last loop{% end %}
 *     {% end %}
 *
 *   - "{% end %}" may be any of: {% end[for|if|subtemplate] %}
 *     (is not sematically verified a.t.m.)
 *     \endcode
 *
 * @todo better error handling and catching
 * @todo and how about a "is-this-a-legal-template-file-checker" ==
 * @todo arbitrary depth for loops, thus a possibility to save them
 */
class TemplateParser {
 private:
  std::string content;
  std::string output;
  bool is_rendered;
  bool no_cleanup;

  std::map<std::string, std::string> string_vars;
  std::map<std::string, tStringList> vector_vars;
  std::map<std::string, int> iter_vars;
  std::map<std::string, bool> first_loop;
  std::map<std::string, bool> last_loop;

  std::map<std::string, TemplateParser*> subtemplates;

  ASTNode* root_node;

  void read_template(std::istream& stream);

  void generate_ast();
  void parse(ASTNode* node);
  void parse_children(ASTNode* node);
  void show_ast(ASTNode* node, int level);

  std::string get_val(const std::string& name);

 public:
  std::string tmpl_filename;

  TemplateParser(const std::string& template_path);
  TemplateParser(std::istream& stream);
  TemplateParser(ASTNode* root_node);
  TemplateParser(const TemplateParser& cobj);

  virtual ~TemplateParser();

  void replace_template(const std::string& template_path);

  void set_key(const std::string& name, const std::string& val);
  void set_key(const std::string& name, TemplateParser* tp);
  // set_key() could also understand things like elems.width, which would make
  // this a (2D) array
  // this would mean elems.i.width.j would access the 2-dimensional array
  // void set_key(const std::string& name);                           /// <---
  // this could mean: "I'm an array"
  void add_to_key(const std::string& name,
                  const std::string& val);  /// <--- then this should fail on
                                            /// non-boxes
  void add_to_key(const std::string& name, TemplateParser* tp);

  TemplateParser* new_subtemplate(const std::string& tmpl_name);
  tStringList get_subtemplates();

  std::string& render();

  bool save_to_file();
  bool save_to_file(const std::string& filename);

  void show_ast();
};
}
#endif
