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
 * The provided set of functionalities at this moment:
 *   - simple replacement of variables {{ simple }}
 *   - simple resolved replacement of vector variables {{ my_vector.i }}
 *   - length of a filled vector {{ #my_vector }} (also usable in for/if)
 *   - 'if-else-end' control-structure, only one variable and no
 *     expression allowed. There is an exception for 'not', which simply
 *     does what you expectcwith the expression/variable:
 *     \code
 *     {% if not simple %}it_was_true{% else %}and_here_false{% end %}
 *     \endcode
 *
 *   - 'for' control-structure, can count from any given variable
 *     up to any other. Supports last_loop flag, first_loop flag, outputting
 *     iteration var:
 *     \code
 *     {% for i in range from offset %}
 *       {% if i.first_loop %}something special in 1st loop{% end %}
 *       Write Line {{ i }} - some content outputted every iteration
 *       {% if not i.last_loop %}everytime except in the last loop{% end %}
 *     {% end %}
 *     \endcode
 *
 * @todo better error handling and catching
 * @todo and how about a "is-this-a-legal-template-file-checker" ==
 */
class TemplateParser {
  private:
    std::string content;
    std::string output;
    bool is_rendered;

    std::map<std::string, std::string> string_vars;
    std::map<std::string, Stringlist> vector_vars;
    std::map<std::string, int> iter_vars;
    std::map<std::string, bool> first_loop;
    std::map<std::string, bool> last_loop;

    ASTNode* root_node;

    void read_template(std::ifstream& stream);

    void generate_ast();
    void parse(ASTNode* node);
    void parse_children(ASTNode* node);
    void show_ast(ASTNode* node, int level);

    std::string get_val(const std::string& name);


  public:
    TemplateParser(const std::string& template_path);
    TemplateParser(std::ifstream& stream);
    virtual ~TemplateParser();

    void replace_template(const std::string& template_path);

    void set_key(const std::string& name, const std::string& val);
    void add_to_key(const std::string& name, const std::string& val);

    std::string& render();
    bool save_to_file(const std::string& filename);
    void show_ast();
};
}
#endif
