#include "101-Stylus/004-TailwindConfigManager/TVariable.h"

namespace Stylus
{

    TVariable::TVariable(std::shared_ptr<tinyxml2::XMLDocument> doc, tinyxml2::XMLElement *xml_node)
        : doc_(doc),
          xml_var_node_(xml_node)
    {
    }

    std::string TVariable::getCSSVariable()
    {
        return "\t--" + getName() + ": " + getValue() + ";\n";
    }

    TVariableText::TVariableText(std::shared_ptr<tinyxml2::XMLDocument> doc, tinyxml2::XMLElement *text_xml_node, tinyxml2::XMLElement *line_height_xml_node)
        : TVariable(doc, text_xml_node), xml_var_node_line_height_(line_height_xml_node)
    {
    }

    std::string TVariableText::getCSSVariable()
    {
        std::string css_variables_string = "\t--" + getName() + ": " + getValue() + ";\n";
        css_variables_string += "\t--" + getName() + "--line-height: " + getLineHeightValue() + ";\n";
        return css_variables_string;
    }

}
