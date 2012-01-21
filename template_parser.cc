
#include "template_parser.h"

using namespace std;
using namespace TOOLS;

/**
 * @brief default constructor for the tree node
 */
ASTNode::ASTNode() : type("root"), content(""), parent(NULL) { }
/**
 * @brief copy constructor
 * @param node the node to be copied
 */
ASTNode::ASTNode(const ASTNode& node)
  : type(node.type), content(node.content), parent(node.parent) {}
/**
 * @brief create a specific node with the given properties
 * @param type the global type of the node at this point one of
 *        ["text", "for", "if", "else"]
 * @param parent a pointer to the parent node, a root has parent NULL
 * @param content the string containing the data assigned to this node
 */
ASTNode::ASTNode(const string& type, ASTNode* parent, const string& content)
  : type(type), content(content), parent(parent) { }
/**
 * @brief a destructor is necassary as the child-nodes live on the heap
 */
ASTNode::~ASTNode() {
  for(vector<ASTNode*>::iterator i=children.begin(); i!=children.end(); ++i)
    delete *i;
}
/**
 * @brief a "helper" constructor which is called by all constructors to
 *        read the templatefile and set some needed instancevariables
 * @param stream the input-stream, which should be read to get the template
 */
void TemplateParser::read_template(ifstream& stream) {
  content = string((istreambuf_iterator<char>(stream)), istreambuf_iterator<char>());
  output = "";
  is_rendered = false;
}
/**
 * @brief constructor, to handle plain filenames
 * @param template_path the path to the template file to be used
 */
TemplateParser::TemplateParser(const string& template_path) : root_node(new ASTNode) {
  ifstream stream(template_path.c_str());
  read_template(stream);
}
/**
 * @brief constructor, to directly handle a stream
 * @param stream the input-stream, which should be read to get the template
 */
TemplateParser::TemplateParser(ifstream& stream) : root_node(new ASTNode) {
  read_template(stream);
}
/**
 * @brief replace the used template with the given one
 * @param template_path the path to the template file
 */
void TemplateParser::replace_template(const string& template_path) {
  ifstream stream(template_path.c_str());
  delete root_node;
  root_node = new ASTNode;
  read_template(stream);
}
/**
 * @brief destructor just deletes the root node, which triggers the chain
 *        reaction that deletes all children below the root_node
 */
TemplateParser::~TemplateParser() {
  delete root_node;
}
/**
 * @brief assign a specific value to a given key to be used during rendering
 * @param name the key/name/variable to get an assignment
 * @param val the value, which should be used to replace any occurences of name
 *            inside the template
 */
void TemplateParser::set_key(const string& name, const string& val) {
  string_vars[name] = val;
}
/**
 * @brief push a specific value to the back of the named vector
 * @param name the vector's key/name/variable that gets an item appended
 * @param val the value to be pushed into the vector
 */
void TemplateParser::add_to_key(const string& name, const string& val) {
  vector_vars[name].push_back(val);
}
/**
 * @brief resolve a given variable, look it up in the tables and
 *        return the correct string. If a variable was not found the
 *        system "fails" silently by returning an empty string
 * @param name the variablename the value is requested for
 * @return the result/content of the given variable and "" if not found
 */
string TemplateParser::get_val(const string& name) {
  // integer will be returned directly, as they are
  try {
    return str(integer(name));
  } catch(ConvertValueError e) { }

  tStringList name_tokens = XString(name).split(".");
  // handle {first,last}_loop and direct vector item access
  if(name_tokens.size() == 2) {
    if(vector_vars.count(name_tokens[0]) == 1 &&
        iter_vars.count(name_tokens[1]) == 1 &&
        vector_vars[name_tokens[0]].size() > (string::size_type) iter_vars[name_tokens[1]])
      return vector_vars[name_tokens[0]][iter_vars[name_tokens[1]]];
    else if(name_tokens[1] == "first_loop" && first_loop.count(name_tokens[0]) == 1)
      return (first_loop[name_tokens[0]]) ? "true" : "false";
    else if(name_tokens[1] == "last_loop" && last_loop.count(name_tokens[0]) == 1)
      return (last_loop[name_tokens[0]]) ? "true" : "false";
    else
      return "";
    // handle length of vector and simple variables
  } else if(name_tokens.size() == 1) {
    // special operation, return size of vector after #
    if(name[0] == '#') {
      string real_name = name.substr(1);
      if(vector_vars.count(real_name) == 1)
        return str(vector_vars[real_name].size());
      else
        return str("0");
    } else if(string_vars.count(name) == 1) {
      return string_vars[name];
    } else if(iter_vars.count(name) == 1) {
      return str(iter_vars[name]);
    }
    return "";
  } else {
    cerr << "Found unknown variable-type in template: '" << name << "'" << endl;
    exit(1);
  }
}
/**
 * @brief the high-level render method controls the render process
 * @return the fully rendered template according to the available data
 */
string& TemplateParser::render() {
  generate_ast();
  parse_children(root_node);
  is_rendered = true;
  return output;
}
/**
 * @brief scanning the template and generating the abstract syntax tree
 */
void TemplateParser::generate_ast() {
  string::size_type pos, size;
  string::size_type offset = 0;

  ASTNode* active_node = root_node;
  while((pos = content.find("{%", offset)) != string::npos) {
    // regular text (also containing variables) is handled here
    if(pos-offset > 0) {
      size = pos-offset;
      active_node->children.push_back(
        new ASTNode("text", active_node, content.substr(offset, size)));
      offset += size;
      continue;
    }
    // here we handle control strutures (if, for, else, end)
    string::size_type end_pos = content.find("%}", pos+2);
    XString raw_cmd = XString(content.substr(pos+2, end_pos-pos-2));
    XString cmd = XString(raw_cmd).strip().subs_all("  ", " ");
    if(cmd.substr(0, 2) == "if") {
      active_node->children.push_back(new ASTNode("if", active_node, cmd));
      active_node = active_node->children.back();
    } else if(cmd.substr(0, 3) == "for") {
      active_node->children.push_back(new ASTNode("for", active_node, cmd));
      active_node = active_node->children.back();
    } else if(cmd.substr(0, 4) == "else") {
      active_node->children.push_back(new ASTNode("else", active_node, cmd));
      active_node = active_node->children.back();
    } else if(cmd.substr(0, 3) == "end") {
      if(active_node->type == "else")
        active_node = active_node->parent;
      active_node = active_node->parent;
    } else {
      cerr << "Found unknown control structure: \"" << cmd << "\"" << endl;
      cerr << "exiting..." << endl;
      exit(1);
    }
    offset += raw_cmd.size() + 4;
  }
  // end of file as text
  if(offset < content.size()-1) {
    active_node->children.push_back(
      new ASTNode("text", active_node, content.substr(offset)));
  }
}
/**
 * @brief recursivly calls the parse() method for each item inside children
 * @param node children of this node will be parsed
 */
void TemplateParser::parse_children(ASTNode* node) {
  for(vector<ASTNode*>::iterator i=node->children.begin();
      i!=node->children.end(); ++i) {
    parse(*i);
  }
}
/**
 * @brief show the abstract syntax tree in the console
 */
void TemplateParser::show_ast() {
  show_ast(this->root_node, 0);
}
/**
 * @brief actually generating the output for showing one specific node,
 *        including the depth inside the AST, then recursivly calling
 *        itself for all children
 * @param node the node to be showed and recursivly re-called
 * @param level the depth of the node, the method is processing right now
 */
void TemplateParser::show_ast(ASTNode* node, int level) {
  for(int j=0; j<level; ++j)
    cout << "|";
  cout << "- " << node->type << " children_count: " << node->children.size() << endl;
  for(vector<ASTNode*>::iterator i=node->children.begin();
      i!=node->children.end(); ++i) {
    show_ast(*i, level+1);
  }
}
/**
 * @brief parse the abstract syntax tree and actually execute the template
 *        code to generate the output
 * @param node the node inside the tree, which is processed at the moment
 */
void TemplateParser::parse(ASTNode* node) {
  if(node->type == "if") {
    vector<string> tokens = node->content.split();

    // 2 tokens or 3 with the second = "not"
    assert(
      (tokens.size() == 2) ||
      (tokens.size() == 3 && tokens[1] == "not")
    );
    string var = get_val((tokens.size() == 3) ? tokens[2] : tokens[1]);
    bool is_true = (var == "0" || var == "" || var == "false") ? false : true;
    if(tokens.size() == 3)
      is_true = !is_true;

    // first handling potential else
    if(!is_true) {
      if(node->children.back()->type == "else")
        parse_children(node->children.back());
    } else {
      // temporary removing the else-node to parse children
      ASTNode* else_node = NULL;
      if(node->children.back()->type == "else") {
        else_node = node->children.back();
        node->children.pop_back();
      }
      parse_children(node);
      if(else_node)
        node->children.push_back(else_node);
    }
    // arbitrary integer ranges can be used either: "for i to count" or
    // "for i to count from start_count"
  } else if(node->type == "for") {
    vector<string> tokens = node->content.split();
    if(tokens.size() == 4) {
      if(tokens[2] != "to") {
        cerr << "node type: " << node->type << " content: " << node->content << endl;
        cerr << "found 4 tokens, but the 4rd wasn't 'to'" << endl;
        exit(1);
      }
    } else if(tokens.size() == 6) {
      if(tokens[2] != "to" || tokens[4] != "from") {
        cerr << "node type: '" << node->type << "' content: '" << node->content << "'" << endl;
        cerr << "found 6 tokens, but either the 3rd wasn't 'to' or the 5th wasn't 'from'" << endl;
        exit(1);
      }
    }
    int start, end;
    try {
      start = (tokens.size() == 6) ? integer(tokens[5]) : 0;
      end = integer(get_val(tokens[3]));
    } catch(ConvertValueError e) {
      cout << e.output << endl;
      exit(1);
    }
    string iter = tokens[1];
    for(iter_vars[iter]=start, first_loop[iter]=true, last_loop[iter]=false;
        iter_vars[iter]<end; ++iter_vars[iter]) {
      if(iter_vars[iter] != start)
        first_loop[iter] = false;
      if(iter_vars[iter] == end-1)
        last_loop[iter] = true;
      parse_children(node);
    }
    // parse "else"
  } else if(node->type == "else") {
    parse_children(node);
    // parse regular text
  } else if(node->type == "text") {
    string::size_type var_pos;
    string out = node->content;
    while((var_pos = out.find("{{")) != string::npos) {
      string::size_type var_end_pos = out.find("}}");
      string var = XString(out.substr(var_pos+2, var_end_pos-var_pos-2)).strip();
      out.replace(var_pos, var_end_pos-var_pos+2, get_val(var));
    }
    output += out;
  }
}
/**
 * @brief save the rendered template to a file, if the render() method
 *        wasn't called yet, just do it and write its output to the file
 * @param filename the filename to which the rendered template should be saved
 * @return true if the open-file operation was successful
 */
bool TemplateParser::save_to_file(const string& filename) {
  ofstream fd(filename.c_str());
  if(!fd) {
    cerr << "Could not open file: " << filename << endl;
    return false;
  }
  if(!is_rendered)
    render();
  fd << output;
  fd.close();
  return true;
}
