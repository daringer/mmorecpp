#include <stdlib.h>
#include <unistd.h>

#include "template_parser.h"

#include "testing_suite.h"


using namespace TOOLS;
using namespace TOOLS::UNIT_TEST;
using namespace std;

START_SUITE(TemplateParserToolsTestSuite) {
      REG_TEST(simple_substitution)
      REG_TEST(for_simple)
      REG_TEST(for_first_last)
      REG_TEST(if_simple)
      REG_TEST(if_else)
      REG_TEST(if_not)
    }

    virtual void setup() { }

    virtual void tear_down() { }

    MAKE_TEST(simple_substitution) {
      string s("{{ foo }}");
      istringstream ss(s);
      TemplateParser p(ss);
      p.set_key("foo", "123");
    
      CHECK(p.render() == "123");
    }

    MAKE_TEST(for_simple) {
      string s("{% for i to #items %}{{ items.i }}{% endfor %}");
      istringstream ss(s);
      TemplateParser p(ss);
      p.add_to_key("items", "start");
      p.add_to_key("items", "--");
      p.add_to_key("items", "++");
      p.add_to_key("items", "--");
      p.add_to_key("items", "end");
      
      CHECK(p.render() == "start--++--end");
    }

    MAKE_TEST(for_first_last) {
      string s("{% for i to #items %}{% if i.first_loop %}[first]{% endif %}{{ items.i }}{% if i.last_loop %}[last]{% endif %}{% endfor %}");
      istringstream ss(s);
      TemplateParser p(ss);
      p.add_to_key("items", "start");
      p.add_to_key("items", "--");
      p.add_to_key("items", "++");
      p.add_to_key("items", "--");
      p.add_to_key("items", "end");

      CHECK(p.render() == "[first]start--++--end[last]");
    }

    MAKE_TEST(if_simple) {
      string s("{% if bool_var %}was true{% endif %}");
      istringstream ss(s);
      TemplateParser p(ss);

      p.set_key("bool_var", "false");
      CHECK(p.render() == "");

      p.set_key("bool_var", "true");
      CHECK(p.render() == "was true");
    }

    MAKE_TEST(if_else) {
      string s("{% if bool_var %}was true{% else %}was false{% endif %}");
      istringstream ss(s);
      TemplateParser p(ss);

      p.set_key("bool_var", "true");
      CHECK(p.render() == "was true");
      
      p.set_key("bool_var", "false");
      CHECK(p.render() == "was false");
    }

    MAKE_TEST(if_not) {
      string s("{% if not bool_var %}was false{% endif %}");
      istringstream ss(s);
      TemplateParser p(ss);
      
      p.set_key("bool_var", "false");
      CHECK(p.render() == "was false");

      p.set_key("bool_var", "true");
      CHECK(p.render() == "");

      p.set_key("bool_var", "false");
      CHECK(p.render() == "was false");
    }

END_SUITE()
