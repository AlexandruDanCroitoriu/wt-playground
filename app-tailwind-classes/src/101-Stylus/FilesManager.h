#pragma once
#include "100-Utils/MonacoEditor/MonacoEditor.h"
#include "101-Stylus/StylusState.h"

#include <Wt/WContainerWidget.h>
#include <Wt/WSignal.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WTree.h>
#include <Wt/WTreeNode.h>
#include <Wt/WPopupMenu.h>
#include <Wt/WGridLayout.h>

namespace Stylus
{

    class FilesManagerSidebar : public Wt::WContainerWidget
    {
    public:
        FilesManagerSidebar();
        Wt::WContainerWidget* contents_;
        Wt::WContainerWidget* footer_;
        Wt::Signal<Wt::WString>& width_changed() { return width_changed_; }

    protected:
        // Custom implementation
        void layoutSizeChanged(int width, int height) override;
        
    private:
        Wt::Signal<Wt::WString> width_changed_;
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
        Wt::WContainerWidget* label_wrapper_;
        std::string path_;
    private:
        TreeNodeType type_;
        std::unique_ptr<Wt::WPopupMenu> popup_;

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
        FilesManager(std::shared_ptr<StylusState> steate, StylusEditorManagementData data, int sidebar_width = 240, std::string selected_file_path = "");

        Wt::WGridLayout *grid_layout_;
        MonacoEditor* editor_;

        std::vector<std::pair<std::string, std::vector<std::string>>> folders_;
        
        void setTreeFolderWidgets();
        
        std::vector<std::pair<std::string, std::vector<std::string>>> getFolders();

        Wt::Signal<Wt::WString>& file_saved() { return file_saved_; }
        // Wt::Signal<Wt::WString>& node_selected() { return node_selected_; }
        Wt::Signal<>& file_selected() { return file_selected_; }

        std::string selected_file_path_;
        
        FilesManagerSidebar* sidebar_;
        StylusEditorManagementData data_;

    private:
        std::shared_ptr<StylusState> state_;

        Wt::WTree* tree_;

        Wt::Signal<Wt::WString> file_saved_; // returns path of the file
        // Wt::Signal<Wt::WString> node_selected_; // returns path of the file
        Wt::Signal<> file_selected_; // returns path of the selected file 

    };
    
}