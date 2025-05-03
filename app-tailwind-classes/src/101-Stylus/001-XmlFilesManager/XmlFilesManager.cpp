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
        FilesManager("../stylus-resources/xml-templates/", "xml", state->xml_node_->IntAttribute("sidebar-width"))
    {
        
        auto temp_wrapper = layout_->insertWidget(2, std::make_unique<Wt::WContainerWidget>(), 1);
        // temp_wrapper->setStyleClass("rounded-md p-2 bg-radial-[at_50%_75%] from-gray-50 via-gray-100 to-gray-50"); 
        temp_wrapper->setStyleClass("rounded-md p-2"); 

        temp_wrapper->setOverflow(Wt::Overflow::Auto);
        temp_wrapper->setMinimumSize(Wt::WLength(240, Wt::LengthUnit::Pixel), Wt::WLength(100, Wt::LengthUnit::ViewportHeight));
        temp_wrapper->setMaximumSize(Wt::WLength::Auto, Wt::WLength(100, Wt::LengthUnit::ViewportHeight));
        // layout_->addLayout(std::make_unique<Wt::WLayout>(), 1);
        // test_template->setMinimumSize(Wt::WLength(240, Wt::LengthUnit::Pixel), Wt::WLength(100, Wt::LengthUnit::ViewportHeight));
        auto test_template = temp_wrapper->addWidget(std::make_unique<Wt::WTemplate>("<div></div>"));
        // auto test_template = addWidget(std::make_unique<Wt::WTemplate>("<div></div>"));
        dark_mode_toggle_ = sidebar_->footer_->addWidget(std::make_unique<DarkModeToggle>());
        
        file_selected().connect(this, [=](Wt::WString file_path)
        {
            test_template->setTemplateText(editor_->getUnsavedText(), Wt::TextFormat::UnsafeXHTML);
        });
        file_saved().connect(this, [=](Wt::WString file_path)
        {
            test_template->setTemplateText(editor_->getUnsavedText(), Wt::TextFormat::UnsafeXHTML);
            // file_saved().emit(file_path);
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

    }

    // Wt::WContainerWidget* XmlFilesManager::setTreeFileWidget(Wt::WContainerWidget* files_wrapper, std::string folder_name, std::string file_name)
    // {
    //     auto file_wrapper = files_wrapper->addWidget(std::make_unique<Wt::WContainerWidget>());
    //     file_wrapper->setStyleClass("group w-full relative flex items-center cursor-pointer rounded-md");

    //     // file buttons
    //     auto save_file_btn = file_wrapper->addWidget(std::make_unique<Wt::WTemplate>(Wt::WString::tr("stylus-svg-green-checked")));
    //     save_file_btn->setStyleClass("absolute left-1 hidden");
    //     save_file_btn->clicked().preventPropagation();
    //     save_file_btn->clicked().connect(this, [=]()
    //     {
    //         std::ofstream sys_file(selected_file_path_);
    //         if (!sys_file.is_open()) {
    //             std::cout << "\n\n Failed to open file: " << selected_file_path_ << "\n\n";
    //             return;
    //         }
    //         sys_file << editor_->getUnsavedText();
    //         sys_file.close();
    //         editor_->textSaved(); 
    //     });

    //     file_wrapper->addWidget(std::make_unique<Wt::WText>(file_name))->setStyleClass("ml-7 font-thin text-md");

    //     auto delete_file_btn = file_wrapper->addWidget(std::make_unique<Wt::WTemplate>(Wt::WString::tr("stylus-svg-trash")));
    //     delete_file_btn->setStyleClass("absolute right-0 rounded-md p-1 group-hover:block hidden");
    //     delete_file_btn->clicked().preventPropagation();
    //     delete_file_btn->clicked().connect(this, [=]()
    //                                        {
    //     // delete file 
    //     std::string title = "Are you sure you want to delete the file ?";
    //     std::string content = "<div class='flex-1 text-center font-bold text-2xl'>" + file_name + "</div>";
    //     auto messageBox = createMessageBox(title, content);
    //     auto delete_btn = messageBox->addButton("Delete", Wt::StandardButton::Yes);
    //     auto cancel_btn = messageBox->addButton("Cancel", Wt::StandardButton::No);
    //     delete_btn->setStyleClass("btn-red");
    //     cancel_btn->setStyleClass("btn-default");
        
    //     messageBox->buttonClicked().connect([=] {
    //         if (messageBox->buttonResult() == Wt::StandardButton::Yes)
    //             {
    //             std::filesystem::path file_path = default_folder_path_ + folder_name + "/" + file_name;

    //             // delete file
    //             if (std::filesystem::remove(file_path)) {
    //                 sidebar_->contents_->clear();
    //                 auto folders = getFolders();
    //                 folders_->clear();
    //                 for (const auto &folder : folders)
    //                 {
    //                     folders_->push_back(folder);
    //                 }
    //                 setTreeFolderWidgets();
    //             } else {
    //                 Wt::WApplication::instance()->log("ERROR") << "\n\nError deleting file.\n\n";                    
    //             }
    //         }
    //         removeChild(messageBox);
    //     });
        
    //     messageBox->show(); });

    //     auto edit_file_name_btn = file_wrapper->addWidget(std::make_unique<Wt::WTemplate>(Wt::WString::tr("stylus-svg-edit")));
    //     edit_file_name_btn->setStyleClass("absolute right-5 rounded-md p-1 group-hover:block hidden");
    //     edit_file_name_btn->clicked().preventPropagation();
    //     edit_file_name_btn->clicked().connect(this, [=]()
    //                                           {
    //     auto dialog = Wt::WApplication::instance()->root()->addChild(std::make_unique<Wt::WDialog>("change file name from " + folder_name));
    //     dialog->setOffsets(100, Wt::Side::Top);
    //     dialog->setModal(true);
    //     dialog->rejectWhenEscapePressed();
    //     dialog->setStyleClass("bg-gray-800 text-gray-200 z-[2]");
    //     dialog->titleBar()->setStyleClass("flex items-center justify-center p-2 cursor-pointer border-b border-solid text-xl font-bold");
    //     dialog->contents()->setStyleClass("flex flex-col bg-gray-800 text-gray-200");

    //     auto content = dialog->contents()->addWidget(std::make_unique<Wt::WContainerWidget>());
    //     auto footer = dialog->contents()->addWidget(std::make_unique<Wt::WContainerWidget>());
        
    //     content->setStyleClass("p-2");
    //     footer->setStyleClass("flex items-center justify-between p-2");
        
    //     auto input_wrapper = content->addWidget(std::make_unique<Wt::WContainerWidget>());
    //     input_wrapper->setStyleClass("flex flex-col items-center justify-center");
    //     auto error_label = content->addWidget(std::make_unique<Wt::WText>(""));
    //     error_label->setStyleClass("w-full text-red-500 text-md font-semibold");
        
    //     auto label = input_wrapper->addWidget(std::make_unique<Wt::WLabel>("Name"));
    //     auto new_file_name_input = input_wrapper->addWidget(std::make_unique<Wt::WLineEdit>());
    //     new_file_name_input->setStyleClass("w-full min-w-[200px] text-sm border rounded-md px-3 py-2 transition duration-300 ease focus:outline-none shadow-sm focus:shadow bg-gray-800 placeholder:text-gray-500 text-gray-200 border-gray-600 focus:border-gray-400 hover:border-gray-500");
    //     label->setBuddy(new_file_name_input);
        
    //     auto confirm_btn = footer->addWidget(std::make_unique<Wt::WPushButton>("Confirm"));
    //     confirm_btn->setStyleClass("btn-default");
    //     auto cancel_btn = footer->addWidget(std::make_unique<Wt::WPushButton>("Cancel"));
    //     cancel_btn->setStyleClass("btn-red");

    //     cancel_btn->clicked().connect(this, [=](){ dialog->reject(); });
    //     new_file_name_input->enterPressed().connect(this, [=](){ confirm_btn->clicked().emit(Wt::WMouseEvent()); });

    //     confirm_btn->clicked().connect(this, [=](){ 
    //         // check if the folder already exists
    //         std::string new_file_name = new_file_name_input->text().toUTF8();
    //         std::string pattern = R"(^[a-z-]+$)";
    //         if(std::regex_match(new_file_name, std::regex(pattern)) == false) {
    //             error_label->setText("Match reges:" + pattern);
    //             return;
    //         }
    //         std::filesystem::path new_path(default_folder_path_ + folder_name + "/" + new_file_name + ".css");
    //         if (std::filesystem::exists(new_path)) {
    //             error_label->setText("file with the same name already exists.");
    //         }else {
    //             dialog->accept();                
    //         }
    //     });
    //     dialog->finished().connect(this, [=](){
    //         if (dialog->result() == Wt::DialogCode::Accepted) {
    //             std::string new_file_name = new_file_name_input->text().toUTF8();
    //             std::filesystem::path old_path(default_folder_path_ + folder_name + "/" + file_name);
    //             std::filesystem::path new_path(default_folder_path_ + folder_name + "/" + new_file_name + "." + file_extension_);
    //             std::filesystem::rename(old_path, new_path);
    //             sidebar_->contents_->clear();
    //             auto folders = getFolders();
    //             folders_->clear();
    //             for (const auto &folder : folders)
    //             {
    //                 folders_->push_back(folder);
    //             }
    //             setTreeFolderWidgets();
    //         }
    //         removeChild(dialog);
    //     });

    //     dialog->show(); });

    //     file_wrapper->clicked().connect(this, [=]()
    //     {
    //         // Same file was chosen 
    //         if(selected_file_path_.compare(default_folder_path_ + folder_name + "/" + file_name) == 0)
    //         {  
    //             // if(editor_->unsavedChanges()) return;
    //             editor_->reuploadText();
    //             return;
    //         }
    //         // Other file was chosen but the current file has unsaved changes
    //         else  if(editor_->unsavedChanges())
    //         {
    //             // std::cout << "\nUnsaved changes, please save the file first\n";
    //             auto title = "Save the changes from the current file ?";
    //             auto messageBox = createMessageBox(title, "");

    //             auto save_btn = messageBox->addButton("Save", Wt::StandardButton::Yes);
    //             auto cancel_btn = messageBox->addButton("Cancel", Wt::StandardButton::No);
    //             auto discard_btn = messageBox->addButton("Discard", Wt::StandardButton::Ignore);
    //             save_btn->setStyleClass("btn-green");
    //             cancel_btn->setStyleClass("btn-default");
    //             discard_btn->setStyleClass("btn-red");
                
                
    //             messageBox->buttonClicked().connect(this, [=] {
    //                 if (messageBox->buttonResult() == Wt::StandardButton::Yes)
    //                 {
    //                     std::cout << "\n\n Save the file: " << selected_file_path_ << "\n\n";
    //                     std::ofstream sys_file(selected_file_path_);
    //                     if (!sys_file.is_open()) {
    //                         std::cout << "\n\n Failed to open file: " << selected_file_path_ << "\n\n";
    //                         return;
    //                     }
    //                     // std::cout << "\n\n curent text: " << editor_->getUnsavedText() << "\n\n";
    //                     sys_file << editor_->getUnsavedText();
    //                     sys_file.close();
    //                     editor_->textSaved();
    //                     selected_file_wrapper_->removeStyleClass("?");
    //                     selected_file_wrapper_ = file_wrapper;
    //                     selected_file_wrapper_->addStyleClass("?");
    //                     selected_file_path_ = default_folder_path_ + folder_name + "/" + file_name;
    //                     editor_->setFile(default_folder_path_ + folder_name + "/" + file_name);
    //                 }else if(messageBox->buttonResult() == Wt::StandardButton::Ignore)
    //                 {
    //                     selected_file_wrapper_->toggleStyleClass("[&>*:first-child]:block", false);
    //                     selected_file_wrapper_->removeStyleClass("?");
    //                     selected_file_wrapper_ = file_wrapper;
    //                     selected_file_wrapper_->addStyleClass("?");
    //                     selected_file_path_ = default_folder_path_ + folder_name + "/" + file_name;
    //                     editor_->setFile(default_folder_path_ + folder_name + "/" + file_name);
    //                 }
    //                 removeChild(messageBox);
    //             });
                
    //             messageBox->show();
    //         }
    //         // Other file was chosen and the current file has no unsaved changes
    //         else 
    //         {
    //             selected_file_path_ = default_folder_path_ + folder_name + "/" + file_name;
    //             editor_->setFile(default_folder_path_ + folder_name + "/" + file_name);
    //             if(selected_file_wrapper_ != nullptr) 
    //             {
    //                 selected_file_wrapper_->removeStyleClass("?");
    //             }
    //             selected_file_wrapper_ = file_wrapper;
    //             selected_file_wrapper_->addStyleClass("?");
    //             selected_file_path_ = default_folder_path_ + folder_name + "/" + file_name;
    //             editor_->setFile(default_folder_path_ + folder_name + "/" + file_name);
    //         } 
    //         file_selected_.emit(default_folder_path_ + folder_name + "/" + file_name);
            
    //     });
    //     return file_wrapper;
    // }


}