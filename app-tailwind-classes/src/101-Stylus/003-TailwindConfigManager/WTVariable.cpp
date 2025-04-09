#include "101-Stylus/003-TailwindConfigManager/WTVariable.h"
#include <Wt/WText.h>
#include <Wt/WLabel.h>
#include <Wt/WMessageBox.h>

namespace Stylus
{

    WTVariable::WTVariable(std::shared_ptr<TVariable> tVariable)
        : tVariable_(tVariable)
    {
        setStyleClass("flex items-center dark:bg-gray-800");

        save_btn_ = addWidget(std::make_unique<Wt::WPushButton>("Save"));
        save_btn_->setStyleClass("px-4 py-2 text-sm font-medium text-slate-700 bg-transparent border border-slate-300 rounded-md shadow-sm transition duration-300 ease hover:bg-green-500 hover:text-white focus:outline-none focus:ring-2 focus:ring-green-400 focus:ring-offset-2 disabled:opacity-50 disabled:cursor-not-allowed disabled:bg-transparent disabled:text-slate-600 dark:bg-transparent dark:text-gray-200 dark:border-gray-600 dark:hover:bg-green-600 dark:hover:border-green-500 dark:focus:ring-green-400 dark:disabled:bg-transparent dark:disabled:ring-none dark:hover:disabled:border-gray-500 ");
        save_btn_->setDisabled(true);

        inputs_wrapper_ = addWidget(std::make_unique<Wt::WContainerWidget>());
        inputs_wrapper_->setStyleClass("flex-1 flex items-center");

        delete_btn_ = addWidget(std::make_unique<Wt::WPushButton>("Del"));
        delete_btn_->setStyleClass("btn-red");

        tVariable_->input_name_ = inputs_wrapper_->addWidget(std::make_unique<Wt::WLineEdit>(tVariable_->getName()));
        tVariable_->input_name_->setStyleClass("w-40 bg-transparent placeholder:text-slate-400 text-slate-700 text-sm border border-slate-200 rounded-md px-3 py-2 transition duration-300 ease focus:outline-none focus:border-slate-400 hover:border-slate-300 shadow-sm focus:shadow  dark:bg-gray-800 dark:placeholder:text-gray-500 dark:text-gray-200 dark:border-gray-600 dark:focus:border-gray-400 dark:hover:border-gray-500");
        tVariable_->input_value_ = inputs_wrapper_->addWidget(std::make_unique<Wt::WLineEdit>(tVariable_->getValue()));
        tVariable_->input_value_->setStyleClass("flex-1 bg-transparent placeholder:text-slate-400 text-slate-700 text-sm border border-slate-200 rounded-md px-3 py-2 transition duration-300 ease focus:outline-none focus:border-slate-400 hover:border-slate-300 shadow-sm focus:shadow dark:bg-gray-800 dark:placeholder:text-gray-500 dark:text-gray-200 dark:border-gray-600 dark:focus:border-gray-400 dark:hover:border-gray-500");

        auto first_dash = tVariable_->getName().find_first_of('-');
        std::string before_dash = tVariable_->getName().substr(0, first_dash);
        std::string after_dash = tVariable_->getName().substr(first_dash + 1);
        if (before_dash.compare("font") == 0)
        {
            std::string next_word_after_dash = after_dash.substr(0, after_dash.find_first_of('-'));
            if (next_word_after_dash.compare("weight") == 0)
            {
                std::string after_weight = after_dash.substr(after_dash.find_first_of('-') + 1);
                tVariable_->input_value_->addStyleClass("font-" + after_weight);
            }
            else
            {
                tVariable_->input_value_->addStyleClass("font-" + after_dash);
            }
        }
        else if (before_dash.compare("color") == 0)
        {
            auto color_display = addWidget(std::make_unique<Wt::WContainerWidget>());
            color_display->setStyleClass("w-10 h-10 rounded-full bg-" + after_dash);
        }
        else if (before_dash.compare("keyframes") == 0)
        {
        }

        tVariable_->input_name_->textInput().connect([=]
                                                     {
        if(tVariable_->input_name_->valueText().toUTF8().compare(tVariable_->getName()) != 0)
        {
            tVariable_->input_name_->toggleStyleClass("!bg-yellow-200/40", true);
            save_btn_->setDisabled(false);
        }else {
            tVariable_->input_name_->toggleStyleClass("!bg-yellow-200/40", false);
            save_btn_->setDisabled(true);
        } });

        delete_btn_->clicked().connect(this, [=]()
                                       {

        auto messageBox =
            addChild(std::make_unique<Wt::WMessageBox>(
                "Are you sure you want to delete this variable ?",
                "<div class='flex-1 text-center font-bold text-2xl'>" + tVariable_->getName() + "</div>", 
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
            if (messageBox->buttonResult() == Wt::StandardButton::Yes)
                std::cout << "/n/nThe rocket is launched!/n/n";
            else
                std::cout << "\n\nThe rocket is ready for launch...\n\n";
            
            removeChild(messageBox);
        });
        
        messageBox->show(); });
    }

    void WTVariable::set_read_only()
    {
        tVariable_->input_name_->setReadOnly(true);
        tVariable_->input_value_->setReadOnly(true);
        save_btn_->setDisabled(true);
        delete_btn_->setDisabled(true);
    }

}
