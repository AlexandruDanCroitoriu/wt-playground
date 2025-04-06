#include "003-Tailwind/WTConfig.h"
#include <Wt/WApplication.h>
#include <iostream>
#include <fstream>
#include <Wt/WText.h>
#include <Wt/WCheckBox.h>
#include <filesystem>
#include <Wt/WComboBox.h>
#include <Wt/WPanel.h>
#include <boost/regex.hpp>
#include "003-Tailwind/WTVariable.h"
#include "003-Tailwind/WTVariableText.h"
#include <Wt/WMessageBox.h>
#include <Wt/WDialog.h>
#include <Wt/WLabel.h>
#include <Wt/WServer.h>
#include <Wt/WRandom.h>
#include <Wt/WIOService.h>

WTConfig::WTConfig() 
{
    auto header = addWidget(std::make_unique<Wt::WContainerWidget>());
    vars_wrapper_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    setStyleClass("w-[880px] rounded-md dark:bg-gray-950 shadow-xl dark:shadow-md dark:shadow-gray-800 border border-solid border-gray-200 dark:border-gray-700");
    
    auto files_wrapper = header->addWidget(std::make_unique<Wt::WContainerWidget>());
    auto buttons_wrapper = header->addWidget(std::make_unique<Wt::WContainerWidget>());
    
    header->setStyleClass("flex items-center border-b border-solid border-gray-200 dark:border-gray-700");
    files_wrapper->setStyleClass("flex mr-4");
    buttons_wrapper->setStyleClass("flex space-x-2");
    
    auto files_combobox = files_wrapper->addWidget(std::make_unique<Wt::WComboBox>());
    files_combobox->setStyleClass("w-60 bg-transparent placeholder:text-slate-400 text-slate-700 text-sm border border-slate-200 rounded-md px-3 py-2 transition duration-300 ease focus:outline-none focus:border-slate-400 hover:border-slate-300 shadow-sm focus:shadow  dark:bg-gray-800 dark:placeholder:text-gray-500 dark:text-gray-200 dark:border-gray-600 dark:focus:border-gray-400 dark:hover:border-gray-500");

    delete_file_btn_ = buttons_wrapper->addWidget(std::make_unique<Wt::WPushButton>("Delete"));
    delete_file_btn_->setStyleClass("btn-red");

    duplicate_file_btn_ = buttons_wrapper->addWidget(std::make_unique<Wt::WPushButton>("Duplicate"));
    duplicate_file_btn_->setStyleClass("btn-default");

    
    files_combobox->activated().connect([=] (int index) {
        std::string selected_file = files_combobox->itemText(index).toUTF8();
        files_combobox->setValueText(selected_file);
        std::cout << "\nSelected file: " << selected_file << "\n";
        if(selected_file.compare(selected_config_file_) != 0)
        {
            readConfigFromXML(default_config_path_ + selected_file);
            writeConfig();
        }
    });

    delete_file_btn_->clicked().connect(this, [=](){

        auto messageBox =
            addChild(std::make_unique<Wt::WMessageBox>(
                "Are you sure you want to delete the file ?",
                "<div class='flex-1 text-center font-bold text-2xl'>" + selected_config_file_ + "</div>", 
                Wt::Icon::Warning , Wt::StandardButton::None));
        messageBox->setStyleClass("bg-white dark:bg-gray-800 dark:text-gray-200");
        messageBox->titleBar()->setStyleClass("flex items-center justify-center bg-white p-2 cursor-pointer dark:bg-gray-800 hover:bg-gray-200 dark:text-gray-300 dark:hover:bg-gray-700 border-b border-solid border-gray-200 dark:border-gray-700 text-xl font-bold");
        messageBox->contents()->addStyleClass("flex items-center bg-white dark:bg-gray-800 dark:text-gray-200");
        messageBox->footer()->setStyleClass("flex items-center justify-between bg-white p-2 dark:bg-gray-800 hover:bg-gray-200 dark:text-gray-300 dark:hover:bg-gray-700");
        auto delete_btn = messageBox->addButton("Delete", Wt::StandardButton::Yes);
        auto cancel_btn = messageBox->addButton("Cancel", Wt::StandardButton::No);
        delete_btn->setStyleClass("btn-red");
        cancel_btn->setStyleClass("btn-default");
        messageBox->setModal(true);
        
        messageBox->buttonClicked().connect([=] {
            if (messageBox->buttonResult() == Wt::StandardButton::Yes && 
                selected_config_file_.compare(default_config_file_) != 0)
                {
                std::filesystem::path file_path = default_config_path_ + selected_config_file_;
                // delete file
                if (std::filesystem::remove(file_path)) {
                    files_combobox->clear();
                    auto config_files = getConfigFiles();
                    for (const auto& file : config_files) {
                        files_combobox->addItem(file);
                    }
                    files_combobox->activated().emit(0);
                } else {
                    Wt::WApplication::instance()->log("ERROR") << "\n\nError deleting file.\n\n";                    
                }
            }
            removeChild(messageBox);
        });
        
        messageBox->show();
    });

    duplicate_file_btn_->clicked().connect(this, [=](){
        auto dialog = addChild(std::make_unique<Wt::WDialog>("Duplicate file " + selected_config_file_));

        dialog->setModal(true);
        dialog->rejectWhenEscapePressed();
        dialog->setStyleClass("bg-white dark:bg-gray-800 dark:text-gray-200");
        dialog->titleBar()->setStyleClass("flex items-center justify-center bg-white p-2 cursor-pointer dark:bg-gray-800 hover:bg-gray-200 dark:text-gray-300 dark:hover:bg-gray-700 border-b border-solid border-gray-200 dark:border-gray-700 text-xl font-bold");
        dialog->contents()->setStyleClass("flex flex-col bg-white dark:bg-gray-800 dark:text-gray-200");

        auto content = dialog->contents()->addWidget(std::make_unique<Wt::WContainerWidget>());
        auto footer = dialog->contents()->addWidget(std::make_unique<Wt::WContainerWidget>());
        
        content->setStyleClass("p-2");
        footer->setStyleClass("flex items-center justify-between p-2");
        
        auto input_wrapper = content->addWidget(std::make_unique<Wt::WContainerWidget>());
        input_wrapper->setStyleClass("flex items-center justify-center");
        auto error_label = content->addWidget(std::make_unique<Wt::WText>(""));
        error_label->setStyleClass("w-full text-red-500 text-md font-semibold");
        
        auto label = input_wrapper->addWidget(std::make_unique<Wt::WLabel>("File Name:"));
        auto new_file_name_input = input_wrapper->addWidget(std::make_unique<Wt::WLineEdit>());
        new_file_name_input->setStyleClass("w-full bg-transparent placeholder:text-slate-400 text-slate-700 text-sm border border-slate-200 rounded-md px-3 py-2 transition duration-300 ease focus:outline-none focus:border-slate-400 hover:border-slate-300 shadow-sm focus:shadow  dark:bg-gray-800 dark:placeholder:text-gray-500 dark:text-gray-200 dark:border-gray-600 dark:focus:border-gray-400 dark:hover:border-gray-500");
        label->setBuddy(new_file_name_input);
        
        auto confirm_btn = footer->addWidget(std::make_unique<Wt::WPushButton>("Confirm"));
        confirm_btn->setStyleClass("btn-default");
        auto cancel_btn = footer->addWidget(std::make_unique<Wt::WPushButton>("Cancel"));
        cancel_btn->setStyleClass("btn-red");

        cancel_btn->clicked().connect(this, [=](){ dialog->reject(); });
        confirm_btn->clicked().connect(this, [=](){ 
            // check if the file already exists
            std::string new_file_name = new_file_name_input->text().toUTF8();
            std::filesystem::path old_path(default_config_path_ + selected_config_file_);
            std::filesystem::path new_path(default_config_path_ + new_file_name + ".xml");
            if (std::filesystem::exists(new_path)) {
                error_label->setText("File with the same name already exists.");
            }else {
                dialog->accept();                
            }
         });
        new_file_name_input->enterPressed().connect(this, [=](){ confirm_btn->clicked().emit(Wt::WMouseEvent()); });
        
        dialog->finished().connect(this, [=](){
            if (dialog->result() == Wt::DialogCode::Accepted) {
                std::string new_file_name = new_file_name_input->text().toUTF8();
                std::filesystem::path old_path(default_config_path_ + selected_config_file_);
                std::filesystem::path new_path(default_config_path_ + new_file_name + ".xml");
                std::filesystem::copy(old_path, new_path);
                files_combobox->clear();
                auto config_files = getConfigFiles();
                for (const auto& file : config_files) {
                    files_combobox->addItem(file);
                }
                files_combobox->activated().emit(files_combobox->findText(new_file_name + ".xml"));
            }
        });
        dialog->show();
        
    });
    
    auto config_files = getConfigFiles();

    for (const auto& file : config_files) {
        files_combobox->addItem(file);
    }

    files_combobox->activated().emit(1);
}

std::vector<std::string> WTConfig::getConfigFiles()
{
    std::vector<std::string> config_files;
    for (const auto& entry : std::filesystem::directory_iterator(default_config_path_)) {
        if (entry.is_regular_file()) {
            config_files.push_back(entry.path().filename().string());
        }
    }
    std::sort(config_files.begin(), config_files.end());

    return config_files;
}

void WTConfig::createVariableWidgets()
{
    vars_wrapper_->clear();

    Wt::WPanel *panel;
    Wt::WContainerWidget *content;

    panel = vars_wrapper_->addWidget(std::make_unique<Wt::WPanel>());
    content = setPanelConfig(panel, "font family");
    for(auto& tVar : var_font_family_){
        auto wTVariable = content->addWidget(std::make_unique<WTVariable>(tVar.second));
        if(default_config_file_.compare(selected_config_file_) == 0) wTVariable->set_read_only();
    }
    panel = vars_wrapper_->addWidget(std::make_unique<Wt::WPanel>());
    content = setPanelConfig(panel, "color");
    for(auto& tVar : var_color_){
        auto wTVariable = content->addWidget(std::make_unique<WTVariable>(tVar.second));
        if(default_config_file_.compare(selected_config_file_) == 0) wTVariable->set_read_only();
    }
    panel = vars_wrapper_->addWidget(std::make_unique<Wt::WPanel>());
    content = setPanelConfig(panel, "spacing");
    for(auto& tVar : var_spacing_){
        auto wTVariable = content->addWidget(std::make_unique<WTVariable>(tVar.second));
        if(default_config_file_.compare(selected_config_file_) == 0) wTVariable->set_read_only();
    }
    panel = vars_wrapper_->addWidget(std::make_unique<Wt::WPanel>());
    content = setPanelConfig(panel, "breakpoint");
    for(auto& tVar : var_breakpoint_){
        auto wTVariable = content->addWidget(std::make_unique<WTVariable>(tVar.second));
        if(default_config_file_.compare(selected_config_file_) == 0) wTVariable->set_read_only();
    }
    panel = vars_wrapper_->addWidget(std::make_unique<Wt::WPanel>());
    content = setPanelConfig(panel, "container");
    for(auto& tVar : var_container_){
        auto wTVariable = content->addWidget(std::make_unique<WTVariable>(tVar.second));
        if(default_config_file_.compare(selected_config_file_) == 0) wTVariable->set_read_only();
    }
    panel = vars_wrapper_->addWidget(std::make_unique<Wt::WPanel>());
    content = setPanelConfig(panel, "text");
    for(auto& tVar : var_text_){
        auto wTVariableText = content->addWidget(std::make_unique<WTVariableText>(tVar.second));
        if(default_config_file_.compare(selected_config_file_) == 0) wTVariableText->set_read_only();
    }
    panel = vars_wrapper_->addWidget(std::make_unique<Wt::WPanel>());
    content = setPanelConfig(panel, "font weight");
    for(auto& tVar : var_font_weight_){
        auto wTVariable = content->addWidget(std::make_unique<WTVariable>(tVar.second));
        if(default_config_file_.compare(selected_config_file_) == 0) wTVariable->set_read_only();
    }
    panel = vars_wrapper_->addWidget(std::make_unique<Wt::WPanel>());
    content = setPanelConfig(panel, "tracking");
    for(auto& tVar : var_tracking_){
        auto wTVariable = content->addWidget(std::make_unique<WTVariable>(tVar.second));
        if(default_config_file_.compare(selected_config_file_) == 0) wTVariable->set_read_only();
    }
    panel = vars_wrapper_->addWidget(std::make_unique<Wt::WPanel>());
    content = setPanelConfig(panel, "leading");
    for(auto& tVar : var_leading_){
        auto wTVariable = content->addWidget(std::make_unique<WTVariable>(tVar.second));
        if(default_config_file_.compare(selected_config_file_) == 0) wTVariable->set_read_only();
    }
    panel = vars_wrapper_->addWidget(std::make_unique<Wt::WPanel>());
    content = setPanelConfig(panel, "radius");
    for(auto& tVar : var_radius_){
        auto wTVariable = content->addWidget(std::make_unique<WTVariable>(tVar.second));
        if(default_config_file_.compare(selected_config_file_) == 0) wTVariable->set_read_only();
    }
    panel = vars_wrapper_->addWidget(std::make_unique<Wt::WPanel>());
    content = setPanelConfig(panel, "shadow");
    for(auto& tVar : var_shadow_){
        auto wTVariable = content->addWidget(std::make_unique<WTVariable>(tVar.second));
        if(default_config_file_.compare(selected_config_file_) == 0) wTVariable->set_read_only();
    }
    panel = vars_wrapper_->addWidget(std::make_unique<Wt::WPanel>());
    content = setPanelConfig(panel, "inset");
    for(auto& tVar : var_inset_){
        auto wTVariable = content->addWidget(std::make_unique<WTVariable>(tVar.second));
        if(default_config_file_.compare(selected_config_file_) == 0) wTVariable->set_read_only();
    }
    panel = vars_wrapper_->addWidget(std::make_unique<Wt::WPanel>());
    content = setPanelConfig(panel, "drop");
    for(auto& tVar : var_drop_){
        auto wTVariable = content->addWidget(std::make_unique<WTVariable>(tVar.second));
        if(default_config_file_.compare(selected_config_file_) == 0) wTVariable->set_read_only();
    }
    panel = vars_wrapper_->addWidget(std::make_unique<Wt::WPanel>());
    content = setPanelConfig(panel, "blur");
    for(auto& tVar : var_blur_){
        auto wTVariable = content->addWidget(std::make_unique<WTVariable>(tVar.second));
        if(default_config_file_.compare(selected_config_file_) == 0) wTVariable->set_read_only();
    }
    panel = vars_wrapper_->addWidget(std::make_unique<Wt::WPanel>());
    content = setPanelConfig(panel, "perspective");
    for(auto& tVar : var_perspective_){
        auto wTVariable = content->addWidget(std::make_unique<WTVariable>(tVar.second));
        if(default_config_file_.compare(selected_config_file_) == 0) wTVariable->set_read_only();
    }
    panel = vars_wrapper_->addWidget(std::make_unique<Wt::WPanel>());
    content = setPanelConfig(panel, "aspect");
    for(auto& tVar : var_aspect_){
        auto wTVariable = content->addWidget(std::make_unique<WTVariable>(tVar.second));
        if(default_config_file_.compare(selected_config_file_) == 0) wTVariable->set_read_only();
    }
    panel = vars_wrapper_->addWidget(std::make_unique<Wt::WPanel>());
    content = setPanelConfig(panel, "ease");
    for(auto& tVar : var_ease_){
        auto wTVariable = content->addWidget(std::make_unique<WTVariable>(tVar.second));
        if(default_config_file_.compare(selected_config_file_) == 0) wTVariable->set_read_only();
    }
    panel = vars_wrapper_->addWidget(std::make_unique<Wt::WPanel>());
    content = setPanelConfig(panel, "animate");
    for(auto& tVar : var_animate_){
        auto wTVariable = content->addWidget(std::make_unique<WTVariable>(tVar.second));
        if(default_config_file_.compare(selected_config_file_) == 0) wTVariable->set_read_only();
    }
    panel = vars_wrapper_->addWidget(std::make_unique<Wt::WPanel>());
    content = setPanelConfig(panel, "keyframes");
    for(auto& tVar : var_keyframes_){
        auto wTVariable = content->addWidget(std::make_unique<WTVariable>(tVar.second));
        if(default_config_file_.compare(selected_config_file_) == 0) wTVariable->set_read_only();
    }

    if(default_config_file_.compare(selected_config_file_) == 0) delete_file_btn_->setDisabled(true);
    else delete_file_btn_->setDisabled(false);

}

Wt::WContainerWidget* WTConfig::setPanelConfig(Wt::WPanel* panel, std::string title)
{
    panel->setCollapsible(true);
    panel->setCollapsed(true);
    panel->setAnimation(Wt::WAnimation(Wt::AnimationEffect::SlideInFromTop, Wt::TimingFunction::EaseInOut, 250));
    panel->setStyleClass("!border-none dark:bg-gray-800");
    panel->titleBarWidget()->setStyleClass("flex items-center px-2 cursor-pointer dark:bg-gray-800 hover:bg-gray-200 dark:text-gray-500 dark:hover:bg-gray-700 border-b border-solid border-gray-200 dark:border-gray-700");
    panel->titleBarWidget()->addWidget(std::make_unique<Wt::WText>(title))->setStyleClass("text-xl font-bold m-2");
    auto add_btn = panel->titleBarWidget()->addWidget(std::make_unique<Wt::WPushButton>("Add"));
    add_btn->setStyleClass("btn-default ml-auto");
    add_btn->clicked().preventPropagation();
    auto central_widget = panel->setCentralWidget(std::make_unique<Wt::WContainerWidget>());
    central_widget->setStyleClass("dark:bg-gray-800 ");
    
    return central_widget;
}


void WTConfig::writeConfig()
{
    const std::string import_statement = "@import \"tailwindcss\";\n\n";
    
    std::ofstream file("../tailwind4/input.css");
    if (!file.is_open()) {
        // Attempt to create the file if it doesn't exist
        file.open("tailwind-config.css", std::ios::out);
        if (!file.is_open()) {
            Wt::WApplication::instance()->log("\n Failed to create and open tailwind-input.css for writing\n");
            return;
        }
    }
    
    file << "/* Import TailwindCSS base styles */\n";
    file << "@import \"tailwindcss\";\n\n";
    
    file << "/* Import custom CSS files for additional styles */\n";
    file << "@import \"./css/tests/0.css\";\n";
    file << "@import \"./css/tests/button-classes.css\";\n";
    file << "@import \"./css/tests/experiments.css\";\n";
    file << "@import \"./css/tests/gradients.css\";\n";
    file << "@import \"./css/tests/input-classes.css\";\n";
    file << "@import \"./css/tests/override-wt.css\";\n";
    file << "@import \"./css/tests/scroll-bar.css\";\n\n";
    
    file << "/* Source additional templates and styles */\n";
    file << "@source \"../../xml-templates\";\n";
    file << "@source \"../../app-stylus/\";\n";
    file << "@source \"../../app-tailwind-classes/\";\n\n";
    
    file << "/* Define custom variants */\n";
    file << "@custom-variant dark (&:where(.dark, .dark *));\n\n";
    
    file << "/* Define custom theme */\n";
    file << "@theme {\n";
    file << "\t--*: initial;\n";
    
    std::string previous_value = "";
  
    {
        for(auto& var_pair : var_font_family_)
        {
            file << var_pair.second->getCSSVariable();
        }
        file << "\n";
        for(auto& var_pair : var_color_)
        {
            file << var_pair.second->getCSSVariable();
        }
        file << "\n";
        for(auto& var_pair : var_spacing_)
        {
            file << var_pair.second->getCSSVariable();
        }
        file << "\n";
        for(auto& var_pair : var_breakpoint_)
        {
            file << var_pair.second->getCSSVariable();
        }
        file << "\n";
        for(auto& var_pair : var_container_)
        {
            file << var_pair.second->getCSSVariable();
        }
        file << "\n";
        for(auto& var_pair : var_text_)
        {
            file << var_pair.second->getCSSVariable();
        }
        file << "\n";
        for(auto& var_pair : var_font_weight_)
        {
            file << var_pair.second->getCSSVariable();
        }
        file << "\n";
        for(auto& var_pair : var_tracking_)
        {
            file << var_pair.second->getCSSVariable();
        }
        file << "\n";
        for(auto& var_pair : var_leading_)
        {
            file << var_pair.second->getCSSVariable();
        }
        file << "\n";
        for(auto& var_pair : var_radius_)
        {
            file << var_pair.second->getCSSVariable();
        }
        file << "\n";
        for(auto& var_pair : var_shadow_)
        {
            file << var_pair.second->getCSSVariable();
        }
        file << "\n";
        for(auto& var_pair : var_inset_)
        {
            file << var_pair.second->getCSSVariable();
        }
        file << "\n";
        for(auto& var_pair : var_drop_)
        {
            file << var_pair.second->getCSSVariable();
        }
        file << "\n";
        for(auto& var_pair : var_blur_)
        {
            file << var_pair.second->getCSSVariable();
        }
        file << "\n";
        for(auto& var_pair : var_perspective_)
        {
            file << var_pair.second->getCSSVariable();
        }
        file << "\n";
        for(auto& var_pair : var_aspect_)
        {
            file << var_pair.second->getCSSVariable();
        }
        file << "\n";
        for(auto& var_pair : var_ease_)
        {
            file << var_pair.second->getCSSVariable();
        }
        file << "\n";
        for(auto& var_pair : var_animate_)
        {
            file << var_pair.second->getCSSVariable();
        }
        file << "\n";
        for(auto& var_keyframes_pair : var_keyframes_)
        {
            auto variable = var_keyframes_pair.second;
            auto first_dash = variable->getName().find_first_of('-');
            std::string keyframes_name = variable->getName().substr(first_dash+1);
            file << "\t@keyframes " << keyframes_name << " " << variable->getValue() << "\n";
        }
        file << "\n";
    }
    
    file << "}\n";
    
    file.close();
    auto session_id = Wt::WApplication::instance()->sessionId();
    Wt::WServer::instance()->ioService().post([this, session_id]() {
        std::system("cd ../tailwind4 && npm run build");
        Wt::WServer::instance()->post(session_id, [this]() {
            Wt::WApplication::instance()->useStyleSheet(Wt::WApplication::instance()->docRoot() + "/../static/css/tailwind.css?v=" + Wt::WRandom::generateId());
            Wt::WApplication::instance()->triggerUpdate();
        });
    });
    
}

std::vector<std::pair<std::string, std::string>> WTConfig::getCssVariables()
{
    std::vector<std::pair<std::string, std::string>> css_variables;
    for(auto& var_pair : var_font_family_)
    {
        css_variables.push_back(std::make_pair(var_pair.second->getName(), var_pair.second->getValue()));
    }
   
    return css_variables;
}

void WTConfig::readConfigFromXML(std::string config_path)
{
    var_font_family_.clear();
    var_color_.clear();
    var_spacing_.clear();
    var_breakpoint_.clear();
    var_container_.clear();
    var_text_.clear();
    var_font_weight_.clear();
    var_tracking_.clear();
    var_leading_.clear();
    var_radius_.clear();
    var_shadow_.clear();
    var_inset_.clear();
    var_drop_.clear();
    var_blur_.clear();
    var_perspective_.clear();
    var_aspect_.clear();
    var_ease_.clear();
    var_animate_.clear();
    var_keyframes_.clear();

    if(config_path.compare("") == 0)
    {
        config_path = default_config_path_ + default_config_file_;
    }    

    selected_config_file_ = config_path.substr(config_path.find_last_of("/") + 1);
    
    doc_ = std::make_shared<tinyxml2::XMLDocument>();
    
    doc_->LoadFile(config_path.c_str());

    if(doc_->Error())
    {
        Wt::WApplication::instance()->log("\n Failed to load tailwind-config.xml\n");
        return;
    }
    
    tinyxml2::XMLElement* tailwind_variables = doc_->FirstChildElement("tailwind-variables");
    if(tailwind_variables == nullptr)
    {
        Wt::WApplication::instance()->log("\n Failed to load tailwind-config.xml\n");
        return;
    }

    tinyxml2::XMLElement* variable_node = tailwind_variables->FirstChildElement();
    while(variable_node != nullptr)
    {
        std::string var_name = variable_node->Name();
        if(boost::regex_match(var_name, boost::regex(var_font_family_regex_)))
        {
            std::shared_ptr<TVariable> variable_ptr = std::make_shared<TVariable>(doc_, variable_node);
            var_font_family_.push_back(std::make_pair(var_name, variable_ptr));
        }
        else if(boost::regex_match(var_name, boost::regex(var_color_regex_)))
        {
            std::shared_ptr<TVariable> variable_ptr = std::make_shared<TVariable>(doc_, variable_node);
            var_color_.push_back(std::make_pair(var_name, variable_ptr));
        }
        else if(boost::regex_match(var_name, boost::regex(var_spacing_regex_)))
        {
            std::shared_ptr<TVariable> variable_ptr = std::make_shared<TVariable>(doc_, variable_node);
            var_spacing_.push_back(std::make_pair(var_name, variable_ptr));
        }
        else if(boost::regex_match(var_name, boost::regex(var_breakpoint_regex_)))
        {
            std::shared_ptr<TVariable> variable_ptr = std::make_shared<TVariable>(doc_, variable_node);
            var_breakpoint_.push_back(std::make_pair(var_name, variable_ptr));
        }
        else if(boost::regex_match(var_name, boost::regex(var_container_regex_)))
        {
            std::shared_ptr<TVariable> variable_ptr = std::make_shared<TVariable>(doc_, variable_node);
            var_container_.push_back(std::make_pair(var_name, variable_ptr));
        }
        else if(boost::regex_match(var_name, boost::regex(var_text_regex_)))
        {
            std::shared_ptr<TVariableText> variable_ptr = std::make_shared<TVariableText>(doc_, variable_node, variable_node->NextSiblingElement());
            var_text_.push_back(std::make_pair(var_name, variable_ptr));
            variable_node = variable_node->NextSiblingElement();
        }
        else if(boost::regex_match(var_name, boost::regex(var_font_weight_regex_)))
        {
            std::shared_ptr<TVariable> variable_ptr = std::make_shared<TVariable>(doc_, variable_node);
            var_font_weight_.push_back(std::make_pair(var_name, variable_ptr));
        }
        else if(boost::regex_match(var_name, boost::regex(var_tracking_regex_)))
        {
            std::shared_ptr<TVariable> variable_ptr = std::make_shared<TVariable>(doc_, variable_node);
            var_tracking_.push_back(std::make_pair(var_name, variable_ptr));
        }
        else if(boost::regex_match(var_name, boost::regex(var_leading_regex_)))
        {
            std::shared_ptr<TVariable> variable_ptr = std::make_shared<TVariable>(doc_, variable_node);
            var_leading_.push_back(std::make_pair(var_name, variable_ptr));
        }
        else if(boost::regex_match(var_name, boost::regex(var_radius_regex_)))
        {
            std::shared_ptr<TVariable> variable_ptr = std::make_shared<TVariable>(doc_, variable_node);
            var_radius_.push_back(std::make_pair(var_name, variable_ptr));
        }
        else if(boost::regex_match(var_name, boost::regex(var_shadow_regex_)))
        {
            std::shared_ptr<TVariable> variable_ptr = std::make_shared<TVariable>(doc_, variable_node);
            var_shadow_.push_back(std::make_pair(var_name, variable_ptr));
        }
        else if(boost::regex_match(var_name, boost::regex(var_inset_regex_)))
        {
            std::shared_ptr<TVariable> variable_ptr = std::make_shared<TVariable>(doc_, variable_node);
            var_inset_.push_back(std::make_pair(var_name, variable_ptr));
        }
        else if(boost::regex_match(var_name, boost::regex(var_drop_regex_)))
        {
            std::shared_ptr<TVariable> variable_ptr = std::make_shared<TVariable>(doc_, variable_node);
            var_drop_.push_back(std::make_pair(var_name, variable_ptr));
        }
        else if(boost::regex_match(var_name, boost::regex(var_blur_regex_)))
        {
            std::shared_ptr<TVariable> variable_ptr = std::make_shared<TVariable>(doc_, variable_node);
            var_blur_.push_back(std::make_pair(var_name, variable_ptr));
        }
        else if(boost::regex_match(var_name, boost::regex(var_perspective_regex_)))
        {
            std::shared_ptr<TVariable> variable_ptr = std::make_shared<TVariable>(doc_, variable_node);
            var_perspective_.push_back(std::make_pair(var_name, variable_ptr));
        }
        else if(boost::regex_match(var_name, boost::regex(var_aspect_regex_)))
        {
            std::shared_ptr<TVariable> variable_ptr = std::make_shared<TVariable>(doc_, variable_node);
            var_aspect_.push_back(std::make_pair(var_name, variable_ptr));
        }
        else if(boost::regex_match(var_name, boost::regex(var_ease_regex_)))
        {
            std::shared_ptr<TVariable> variable_ptr = std::make_shared<TVariable>(doc_, variable_node);
            var_ease_.push_back(std::make_pair(var_name, variable_ptr));
        }
        else if(boost::regex_match(var_name, boost::regex(var_animate_regex_)))
        {
            std::shared_ptr<TVariable> variable_ptr = std::make_shared<TVariable>(doc_, variable_node);
            var_animate_.push_back(std::make_pair(var_name, variable_ptr));
        }
        else if(boost::regex_match(var_name, boost::regex(var_keyframes_regex_)))
        {
            std::shared_ptr<TVariable> variable_ptr = std::make_shared<TVariable>(doc_, variable_node);
            var_keyframes_.push_back(std::make_pair(var_name, variable_ptr));
        }
        variable_node = variable_node->NextSiblingElement();
    }
    createVariableWidgets();
}


