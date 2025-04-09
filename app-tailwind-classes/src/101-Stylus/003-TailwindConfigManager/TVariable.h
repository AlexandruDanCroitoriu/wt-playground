#pragma once
#include "100-Utils/tinyxml2/tinyxml2.h"

namespace Stylus {


class TVariable {
    public:
        TVariable(std::shared_ptr<tinyxml2::XMLDocument> doc, tinyxml2::XMLElement* xml_node);
        std::string getName() { return xml_var_node_->Name(); }
        std::string getValue() { return xml_var_node_->GetText(); }

        Wt::WLineEdit* input_name_;
        Wt::WLineEdit* input_value_;
        virtual std::string getCSSVariable(); // Mark as virtual
    private:
        std::shared_ptr<tinyxml2::XMLDocument> doc_;
        tinyxml2::XMLElement* xml_var_node_;
};

class TVariableText : public TVariable
{
    public:
        TVariableText(std::shared_ptr<tinyxml2::XMLDocument> doc, tinyxml2::XMLElement* text_xml_node, tinyxml2::XMLElement* line_height_xml_node);
        Wt::WLineEdit* input_line_height_;
        std::string getLineHeightValue() { return xml_var_node_line_height_->GetText(); }
        std::string getCSSVariable() override; // Override the method
    private:
        tinyxml2::XMLElement* xml_var_node_line_height_;
};

}

