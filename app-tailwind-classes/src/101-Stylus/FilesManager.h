#pragma once
#include <Wt/WContainerWidget.h>
#include "100-Utils/MonacoEditor/MonacoEditor.h"
#include <Wt/WSignal.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WTree.h>
#include <Wt/WTreeNode.h>
#include <Wt/WPopupMenu.h>

namespace Stylus
{

class FilesManagerSidebar : public Wt::WContainerWidget
{
public:
    FilesManagerSidebar();
    Wt::WContainerWidget* contents_;
    Wt::WContainerWidget* footer_;
    Wt::Signal<Wt::WString> width_changed_;
protected:
    // Custom implementation
    void layoutSizeChanged(int width, int height) override;
    
};

enum TreeNodeType
{
    Folder,
    File
};

class TreeNode : public Wt::WTreeNode
{
public:
    TreeNode(std::string name, TreeNodeType type, std::string path);

    Wt::Signal<> folders_changed_;
    Wt::Signal<> label_clicked_;

    void showPopup(const Wt::WMouseEvent& event);

private:
    TreeNodeType type_;
    std::unique_ptr<Wt::WPopupMenu> popup_;
    std::string path_;
    std::string name_;

    void createNewFolderDialog();
    void createRenameFolderDialog();
    void createRemoveFolderMessageBox();

    void createNewFileDialog();
    void createRenameFileDialog();
    void deleteFileMessageBox();
};

class FilesManager : public Wt::WContainerWidget
{
public:
    FilesManager(std::string default_folder_path, std::string language, int sidebar_width = 240, std::string selected_file_path = "");

    Wt::WHBoxLayout *layout_;
    MonacoEditor* editor_;

    std::vector<std::pair<std::string, std::vector<std::string>>> folders_;
    
    void setTreeFolderWidgets();
    
    std::vector<std::pair<std::string, std::vector<std::string>>> getFolders();

    Wt::Signal<Wt::WString>& file_saved() { return file_saved_; }
    Wt::Signal<Wt::WString>& node_selected() { return node_selected_; }
    Wt::Signal<>& file_selected() { return file_selected_; }

    std::string default_folder_path_;
    std::string selected_file_path_;
    std::string selected_tree_path_;
    std::string file_extension_;
    
    FilesManagerSidebar* sidebar_;

private:

    Wt::WTree* tree_;

    Wt::Signal<Wt::WString> file_saved_; // returns path of the file
    Wt::Signal<Wt::WString> node_selected_; // returns path of the file
    Wt::Signal<> file_selected_; // returns path of the selected file 

};
}