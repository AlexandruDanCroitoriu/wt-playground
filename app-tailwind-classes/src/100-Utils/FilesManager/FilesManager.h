#pragma once
#include <Wt/WContainerWidget.h>
#include "100-Utils/MonacoEditor/MonacoEditor.h"
#include <Wt/WSignal.h>

class FilesManager : public Wt::WContainerWidget
{
    public:
        FilesManager(std::string default_folder_path, std::string language);
        MonacoEditor* editor_;
        std::vector<std::pair<std::string, std::vector<std::string>>> folders_;
        void setTreeFolderWidgets();
        // void createFolderWidget(Wt::WContainerWidget* folder_wrapper, std::pair<std::string, std::vector<std::string>> folder);
        // void createFileWidget(Wt::WContainerWidget* file_wrapper, std::string file);
        
        std::vector<std::pair<std::string, std::vector<std::string>>> getFolders();

        Wt::Signal<Wt::WString>& file_saved() { return file_saved_; }
        Wt::Signal<std::vector<std::pair<std::string, std::vector<std::string>>>>& get_folders_signal() { return get_folders_signal_; }
    private:
    
        std::string default_folder_path_;
        std::string selected_file_path_;
        std::string file_extension_;
        Wt::WContainerWidget* selected_file_wrapper_;
        Wt::WContainerWidget* folders_tree_wrapper_;
        Wt::WText* tree_header_title_;
        Wt::WMessageBox* createMessageBox(std::string title, std::string temp);

        Wt::Signal<Wt::WString> file_saved_;
        Wt::Signal<std::vector<std::pair<std::string, std::vector<std::string>>>> get_folders_signal_;
        
};
