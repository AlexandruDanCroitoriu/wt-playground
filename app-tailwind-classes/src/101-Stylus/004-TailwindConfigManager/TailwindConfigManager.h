#pragma once
#include "100-Utils/tinyxml2/tinyxml2.h"
#include "101-Stylus/StylusState.h"
#include <Wt/WContainerWidget.h>
#include "100-Utils/MonacoEditor/MonacoEditor.h"
#include <Wt/WHBoxLayout.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WComboBox.h>
#include <Wt/WPushButton.h>
#include <Wt/WGridLayout.h>

namespace Stylus
{
    class TailwindConfigManager : public Wt::WContainerWidget
    {
    public:
        TailwindConfigManager(std::shared_ptr<StylusState> state);

        MonacoEditor* config_editor_;
        MonacoEditor* output_editor_;
        
        void generateCssFile();
        private:
        
        Wt::WGridLayout* grid_layout_;
        Wt::WComboBox* config_files_combobox_;
        Wt::WPushButton* delete_file_btn_;
        
        std::shared_ptr<StylusState> state_;
        std::string config_folder_path_;
        std::string default_config_file_name_;
        std::vector<std::string> config_files_;
        
        std::vector<std::string> getConfigFiles();

        Wt::WString current_css_file_;
        Wt::WString prev_css_file_;
    };

}
