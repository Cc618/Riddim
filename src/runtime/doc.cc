#include "doc.hh"
#include "null.hh"
#include "str.hh"
#include "utils.hh"

using namespace std;

str_t autodoc(size_t indent, const str_t &name, const str_t &current_doc,
              const std::vector<std::pair<str_t, Object *>> &children) {
    str_t doc;

    if (name.size()) {
        doc += str_t(indent, '#') + " " + name + "\n";
    }

    if (current_doc.size()) {
        doc +=  current_doc + "\n\n";
    }

    auto children_header = str_t(indent + 1, '#') + " ";

    for (const auto &[child_name, child] : children) {
        // TODO A : mod.mod inf loop
        // TODO : Avoid infinite loops
        auto child_doc = child->doc();

        if (!child_doc) {
            return "";
        }

        if (child_doc == null) {
            continue;
        }

        auto child_docstr = reinterpret_cast<Str*>(child_doc)->data;

        if (child_docstr.empty()) {
            continue;
        }

        auto compound_name = child_name;

        if (name.size()) {
            compound_name = name + "." + child_name;
        }

        doc += children_header + compound_name + "\n";
        doc += child_docstr + "\n\n";
    }

    return doc;
}
