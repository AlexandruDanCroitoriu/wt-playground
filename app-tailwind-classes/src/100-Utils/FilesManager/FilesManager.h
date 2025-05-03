#pragma once
#include <Wt/WContainerWidget.h>
#include "100-Utils/MonacoEditor/MonacoEditor.h"
#include <Wt/WSignal.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WIconPair.h>
#include <Wt/WTree.h>
#include <Wt/WTreeNode.h>
#include <Wt/WInPlaceEdit.h>
#include <Wt/WLineEdit.h>



class FilesManagerSidebar : public Wt::WContainerWidget
{
public:
    FilesManagerSidebar();
    Wt::WContainerWidget* header_;
    Wt::WContainerWidget* contents_;
    Wt::WContainerWidget* footer_;
    Wt::WTemplate* add_folder_btn_;
    Wt::Signal<Wt::WString> width_changed_;
protected:
    // Custom implementation
    void layoutSizeChanged(int width, int height) override;
    
};

class TreeNode : public Wt::WTreeNode
{
public:
    TreeNode(std::string name, std::unique_ptr<Wt::WIconPair> icon);
    Wt::WContainerWidget* aditional_buttons_wrapper_;
    Wt::WLineEdit* label_edit_;
    Wt::WPushButton* delete_btn_;
    Wt::Signal<> label_clicked_;
private:
};

class FilesManager : public Wt::WContainerWidget
{
public:
    FilesManager(std::string default_folder_path, std::string language, int sidebar_width = 240);

    Wt::WHBoxLayout *layout_;
    MonacoEditor* editor_;

    std::vector<std::pair<std::string, std::vector<std::string>>> folders_;
    
    void setTreeFolderWidgets();
    void setTreeFolderWidgetsV2();
    virtual Wt::WContainerWidget* setTreeFileWidget(Wt::WContainerWidget* files_wrapper, std::string folder_name, std::string file_name);
    // void createFolderWidget(Wt::WContainerWidget* folder_wrapper, std::pair<std::string, std::vector<std::string>> folder);
    // void createFileWidget(Wt::WContainerWidget* file_wrapper, std::string file);
    
    std::vector<std::pair<std::string, std::vector<std::string>>> getFolders();

    void createNewFolderDialog();
    void createNewFileDialog(std::string folder_name);
    void deleteFileMessageBox(std::string file_name, std::string folder_name);

    Wt::Signal<Wt::WString>& file_saved() { return file_saved_; }
    Wt::Signal<Wt::WString>& file_selected() { return file_selected_; }

    std::string default_folder_path_;
    std::string selected_file_path_;
    std::string selected_tree_path_;
    std::string file_extension_;
    
    FilesManagerSidebar* sidebar_;
    Wt::WContainerWidget* selected_file_wrapper_;

    Wt::WMessageBox* createMessageBox(std::string title, std::string temp);

    
    
    
private:
    enum icon_type
    {
        folder,
        file,
        temp
    };
    std::unique_ptr<Wt::WIconPair> getIcon(icon_type type);
    Wt::WTree* tree_;

    Wt::Signal<Wt::WString> file_saved_; // returns path of the file
    Wt::Signal<Wt::WString> file_selected_; // returns path of the file

};
