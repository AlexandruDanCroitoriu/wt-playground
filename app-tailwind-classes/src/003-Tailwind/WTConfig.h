#pragma once
#include "100-Utils/tinyxml2/tinyxml2.h"
#include "003-Tailwind/TVariable.h"
#include <vector>
#include <Wt/WContainerWidget.h>
#include <map>
#include <Wt/WPushButton.h>


class WTConfig : public Wt::WContainerWidget 
{
    public:
        WTConfig();

        void writeConfig();
        std::vector<std::pair<std::string, std::string>> getCssVariables();
        
    private:
        
        std::vector<std::string> getConfigFiles();
        void readConfigFromXML(std::string config_path = "");   
        void createVariableWidgets();

        Wt::WContainerWidget* setPanelConfig(Wt::WPanel* panel, std::string title);
        Wt::WContainerWidget* vars_wrapper_;
        Wt::WPushButton* duplicate_file_btn_;
        Wt::WPushButton* delete_file_btn_;

        std::string default_config_path_ = "../../xml-base-templates/tailwind-config/";
        std::string default_config_file_ = "tailwind-config.xml";
        std::string selected_config_file_; 


        std::shared_ptr<tinyxml2::XMLDocument> doc_;
        // std::vector<std::pair<std::string, std::shared_ptr<TVariable>>> variables_;
        std::vector<std::pair<std::string, std::shared_ptr<TVariable>>> var_font_family_;
        std::vector<std::pair<std::string, std::shared_ptr<TVariable>>> var_color_;
        std::vector<std::pair<std::string, std::shared_ptr<TVariable>>> var_spacing_;
        std::vector<std::pair<std::string, std::shared_ptr<TVariable>>> var_breakpoint_;
        std::vector<std::pair<std::string, std::shared_ptr<TVariable>>> var_container_;
        std::vector<std::pair<std::string, std::shared_ptr<TVariableText>>> var_text_;
        std::vector<std::pair<std::string, std::shared_ptr<TVariable>>> var_font_weight_;
        std::vector<std::pair<std::string, std::shared_ptr<TVariable>>> var_tracking_;
        std::vector<std::pair<std::string, std::shared_ptr<TVariable>>> var_leading_;
        std::vector<std::pair<std::string, std::shared_ptr<TVariable>>> var_radius_;
        std::vector<std::pair<std::string, std::shared_ptr<TVariable>>> var_shadow_;
        std::vector<std::pair<std::string, std::shared_ptr<TVariable>>> var_inset_;
        std::vector<std::pair<std::string, std::shared_ptr<TVariable>>> var_drop_;
        std::vector<std::pair<std::string, std::shared_ptr<TVariable>>> var_blur_;
        std::vector<std::pair<std::string, std::shared_ptr<TVariable>>> var_perspective_;
        std::vector<std::pair<std::string, std::shared_ptr<TVariable>>> var_aspect_;
        std::vector<std::pair<std::string, std::shared_ptr<TVariable>>> var_ease_;
        std::vector<std::pair<std::string, std::shared_ptr<TVariable>>> var_animate_;
        std::vector<std::pair<std::string, std::shared_ptr<TVariable>>> var_keyframes_;

        std::string var_font_family_regex_ = R"(^(?<!\S)font-[a-z0-9]+$)";
        std::string var_color_regex_ = R"(^(?<!\S)color-[a-z0-9]+(-[a-z0-9]+)?$)";
        std::string var_spacing_regex_ = R"(^(?<!\S)spacing$)";
        std::string var_breakpoint_regex_ = R"(^(?<!\S)breakpoint-[a-z0-9]+$)";
        std::string var_container_regex_ = R"(^(?<!\S)container-[a-z0-9]+$)";
        std::string var_text_regex_ = R"(^(?<!\S)text-[a-z0-9]+$)";
        std::string var_text_line_height_regex_ = R"(^(?<!\S)text-[a-z0-9]+-line-height$)";
        std::string var_font_weight_regex_ = R"(^(?<!\S)font-weight-[a-z0-9]+$)";
        std::string var_tracking_regex_ = R"(^(?<!\S)tracking-[a-z0-9]+$)";
        std::string var_leading_regex_ = R"(^(?<!\S)leading-[a-z0-9]+$)";
        std::string var_radius_regex_ = R"(^(?<!\S)radius-[a-z0-9]+$)";
        std::string var_shadow_regex_ = R"(^(?<!\S)shadow-[a-z0-9]+$)";
        std::string var_inset_regex_ = R"(^(?<!\S)inset-shadow-[a-z0-9]+$)";
        std::string var_drop_regex_ = R"(^(?<!\S)drop-shadow-[a-z0-9]+$)";
        std::string var_blur_regex_ = R"(^(?<!\S)blur-[a-z0-9]+$)";
        std::string var_perspective_regex_ = R"(^(?<!\S)perspective-[a-z0-9]+$)";
        std::string var_aspect_regex_ = R"(^(?<!\S)aspect-[a-z0-9]+$)";
        std::string var_ease_regex_ = R"(^(?<!\S)ease-[a-z0-9]+$)";
        std::string var_animate_regex_ = R"(^(?<!\S)animate-[a-z0-9]+$)";
        std::string var_keyframes_regex_ = R"(^(?<!\S)keyframes-[a-z0-9]+$)";



    };