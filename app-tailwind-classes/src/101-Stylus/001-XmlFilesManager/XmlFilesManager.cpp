#include "101-Stylus/001-XmlFilesManager/XmlFilesManager.h"
#include <filesystem>
#include <Wt/WPushButton.h>
#include <Wt/WMenu.h>
#include <Wt/WMenuItem.h>
#include <Wt/WPanel.h>
#include <Wt/WTemplate.h>
#include <fstream>
#include <Wt/WMessageBox.h>
#include <Wt/WLineEdit.h>
#include <Wt/WLabel.h>
#include <Wt/WDialog.h>
#include <regex>
#include <Wt/WRandom.h>
#include <Wt/WApplication.h>
#include <Wt/WText.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WLayout.h>

namespace Stylus
{

    XmlFilesManager::XmlFilesManager(std::shared_ptr<StylusState> state)
        : state_(state),
        // FilesManager("../stylus-resources/xml-templates/", "xml")
        FilesManager("../stylus-resources/xml-templates/", "xml", state->xml_node_->IntAttribute("sidebar-width"), state->xml_node_->Attribute("selected-file-path"))
    {
        if(!state_->xml_node_->BoolAttribute("editor-open"))
        {
            editor_->hide();
        }
        auto temp_wrapper = layout_->insertWidget(2, std::make_unique<Wt::WContainerWidget>(), 1);
        // temp_wrapper->setStyleClass("rounded-md p-2 bg-radial-[at_50%_75%] from-gray-50 via-gray-100 to-gray-50"); 
        temp_wrapper->setStyleClass("rounded-md p-[8px] bg-[#FFF]"); 

        temp_wrapper->setOverflow(Wt::Overflow::Auto);
        temp_wrapper->setMinimumSize(Wt::WLength(240, Wt::LengthUnit::Pixel), Wt::WLength(100, Wt::LengthUnit::ViewportHeight));
        temp_wrapper->setMaximumSize(Wt::WLength::Auto, Wt::WLength(100, Wt::LengthUnit::ViewportHeight));
        // layout_->addLayout(std::make_unique<Wt::WLayout>(), 1);
        // temp_view->setMinimumSize(Wt::WLength(240, Wt::LengthUnit::Pixel), Wt::WLength(100, Wt::LengthUnit::ViewportHeight));
        auto temp_view = temp_wrapper->addWidget(std::make_unique<Wt::WTemplate>());
        temp_view->setTemplateText(editor_->getUnsavedText(), Wt::TextFormat::UnsafeXHTML);
        // auto temp_view = addWidget(std::make_unique<Wt::WTemplate>("<div></div>"));
        dark_mode_toggle_ = sidebar_->footer_->addWidget(std::make_unique<DarkModeToggle>());
      
        auto editor_checkbox = sidebar_->footer_->addWidget(std::make_unique<Wt::WCheckBox>("Editor"));
        editor_checkbox->setChecked(state_->xml_node_->BoolAttribute("editor-open"));
        editor_checkbox->keyWentDown().connect(this, [=](Wt::WKeyEvent e) { 
            Wt::WApplication::instance()->globalKeyWentDown().emit(e); // Emit the global key event
        });
        editor_checkbox->setChecked(state_->xml_node_->BoolAttribute("editor-open"));
        editor_checkbox->changed().connect(this, [=]()
        {
            state_->xml_node_->SetAttribute("editor-open", editor_checkbox->isChecked());
            state_->doc_.SaveFile(state_->file_path_.c_str());
            if (editor_checkbox->isChecked())
            {
                editor_->animateShow(Wt::WAnimation(Wt::AnimationEffect::SlideInFromRight, Wt::TimingFunction::EaseInOut, 5));
                state_->xml_node_->SetAttribute("editor-open", "true");
            }
            else
            {
                editor_->animateHide(Wt::WAnimation(Wt::AnimationEffect::SlideInFromRight, Wt::TimingFunction::EaseInOut, 5) );
                state_->xml_node_->SetAttribute("editor-open", "false");
            }
            state_->doc_.SaveFile(state_->file_path_.c_str());
        });

        file_selected().connect(this, [=]()
        {
            temp_view->setTemplateText(editor_->getUnsavedText(), Wt::TextFormat::UnsafeXHTML);
            state_->xml_node_->SetAttribute("selected-file-path", selected_file_path_.c_str());
            state_->doc_.SaveFile(state_->file_path_.c_str());
        });
        file_saved().connect(this, [=](Wt::WString file_path)
        {
            temp_view->setTemplateText(editor_->getUnsavedText(), Wt::TextFormat::UnsafeXHTML);
        });
        
        sidebar_->width_changed_.connect(this, [=](Wt::WString width)
        {
            state_->xml_node_->SetAttribute("sidebar-width", std::stoi(width.toUTF8()));
            state_->doc_.SaveFile(state_->file_path_.c_str());
        });
        layout_->setResizable(1, true, Wt::WLength(state_->xml_node_->IntAttribute("editor-width"), Wt::LengthUnit::Pixel));

        editor_->width_changed_.connect(this, [=](Wt::WString width)
        {
            state_->xml_node_->SetAttribute("editor-width", std::stoi(width.toUTF8()));
            state_->doc_.SaveFile(state_->file_path_.c_str());
        });

        if(state_->stylus_node_->BoolAttribute("dark-mode")){
            dark_mode_toggle_->setDarkMode(true);
            editor_->setDarkTheme(true);
        }
        dark_mode_toggle_->dark_mode_changed_.connect(this, [=](bool dark)
        {
            std::cout << "\nDark mode changed to: " << dark << std::endl;
            editor_->setDarkTheme(dark);
            state_->stylus_node_->SetAttribute("dark-mode", dark ? "true" : "false");
            state_->doc_.SaveFile(state_->file_path_.c_str());
        });
    }

}