#pragma once
#include <Wt/WContainerWidget.h>
#include "100-Utils/MonacoEditor/MonacoEditor.h"
#include <Wt/WSignal.h>
#include <Wt/WHBoxLayout.h>


class FilesManagerSidebar : public Wt::WContainerWidget
{
public:
    FilesManagerSidebar();
    Wt::WContainerWidget* header_;
    Wt::WContainerWidget* contents_;
    Wt::WContainerWidget* footer_;
    Wt::WText* header_title_;
    Wt::WTemplate* add_folder_btn_;
protected:
    // Custom implementation
    void layoutSizeChanged(int width, int height) override;      
};

class FilesManager : public Wt::WContainerWidget
{
public:
    FilesManager(std::string default_folder_path, std::string language);

    Wt::WHBoxLayout *layout_;
    MonacoEditor* editor_;

    std::shared_ptr<std::vector<std::pair<std::string, std::vector<std::string>>>> folders_;
    
    void setTreeFolderWidgets();
    virtual Wt::WContainerWidget* setTreeFileWidget(Wt::WContainerWidget* files_wrapper, std::string folder_name, std::string file_name);
    // void createFolderWidget(Wt::WContainerWidget* folder_wrapper, std::pair<std::string, std::vector<std::string>> folder);
    // void createFileWidget(Wt::WContainerWidget* file_wrapper, std::string file);
    
    std::vector<std::pair<std::string, std::vector<std::string>>> getFolders();

    Wt::Signal<Wt::WString>& file_saved() { return file_saved_; }
    Wt::Signal<Wt::WString>& file_selected() { return file_selected_; }
    Wt::Signal<>& get_folders_signal() { return get_folders_signal_; }
    
    std::string default_folder_path_;
    std::string selected_file_path_;
    std::string file_extension_;
    
    FilesManagerSidebar* sidebar_;
    Wt::WContainerWidget* selected_file_wrapper_;

    Wt::WMessageBox* createMessageBox(std::string title, std::string temp);

    Wt::Signal<Wt::WString> file_saved_; // returns path of the file
    Wt::Signal<Wt::WString> file_selected_; // returns path of the file
    Wt::Signal<> get_folders_signal_;


    
private:

    
};
