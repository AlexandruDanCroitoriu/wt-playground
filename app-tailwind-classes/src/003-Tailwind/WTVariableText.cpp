#include "003-Tailwind/WTVariableText.h"
#include <Wt/WText.h>
#include <Wt/WLabel.h>

WTVariableText::WTVariableText(std::shared_ptr<TVariableText> tVariable_text) 
    : tVariable_text_(tVariable_text)
{
    setStyleClass("flex items-center dark:bg-gray-800");

    save_btn_ = addWidget(std::make_unique<Wt::WPushButton>("Save"));
    save_btn_->setStyleClass("px-4 py-2 text-sm font-medium text-slate-700 bg-transparent border border-slate-300 rounded-md shadow-sm transition duration-300 ease hover:bg-green-500 hover:text-white focus:outline-none focus:ring-2 focus:ring-green-400 focus:ring-offset-2 disabled:opacity-50 disabled:cursor-not-allowed disabled:bg-transparent disabled:text-slate-600 dark:bg-transparent dark:text-gray-200 dark:border-gray-600 dark:hover:bg-green-600 dark:hover:border-green-500 dark:focus:ring-green-400 dark:disabled:bg-transparent dark:disabled:ring-none dark:hover:disabled:border-gray-500 ");
    save_btn_->setDisabled(true);

    inputs_wrapper_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    inputs_wrapper_->setStyleClass("flex-1 flex items-center");

    delete_btn_ = addWidget(std::make_unique<Wt::WPushButton>("Del"));
    delete_btn_->setStyleClass("btn-red");

    tVariable_text_->input_name_ = inputs_wrapper_->addWidget(std::make_unique<Wt::WLineEdit>(tVariable_text_->getName()));
    tVariable_text_->input_name_->setStyleClass("w-40 bg-transparent placeholder:text-slate-400 text-slate-700 text-sm border border-slate-200 rounded-md px-3 py-2 transition duration-300 ease focus:outline-none focus:border-slate-400 hover:border-slate-300 shadow-sm focus:shadow  dark:bg-gray-800 dark:placeholder:text-gray-500 dark:text-gray-200 dark:border-gray-600 dark:focus:border-gray-400 dark:hover:border-gray-500");
    tVariable_text_->input_value_ = inputs_wrapper_->addWidget(std::make_unique<Wt::WLineEdit>(tVariable_text_->getValue()));
    tVariable_text_->input_value_->setStyleClass("flex-1 bg-transparent placeholder:text-slate-400 text-slate-700 text-sm border border-slate-200 rounded-md px-3 py-2 transition duration-300 ease focus:outline-none focus:border-slate-400 hover:border-slate-300 shadow-sm focus:shadow dark:bg-gray-800 dark:placeholder:text-gray-500 dark:text-gray-200 dark:border-gray-600 dark:focus:border-gray-400 dark:hover:border-gray-500");
    tVariable_text_->input_line_height_ = inputs_wrapper_->addWidget(std::make_unique<Wt::WLineEdit>(tVariable_text_->getLineHeightValue()));
    tVariable_text_->input_line_height_->setStyleClass("flex-2 bg-transparent placeholder:text-slate-400 text-slate-700 text-sm border border-slate-200 rounded-md px-3 py-2 transition duration-300 ease focus:outline-none focus:border-slate-400 hover:border-slate-300 shadow-sm focus:shadow dark:bg-gray-800 dark:placeholder:text-gray-500 dark:text-gray-200 dark:border-gray-600 dark:focus:border-gray-400 dark:hover:border-gray-500");

    tVariable_text_->input_name_->textInput().connect([=] {
        if(tVariable_text_->input_name_->valueText().toUTF8().compare(tVariable_text_->getName()) != 0)
        {
            tVariable_text_->input_name_->toggleStyleClass("!bg-yellow-200/40", true);
            save_btn_->setDisabled(false);
        }else {
            tVariable_text_->input_name_->toggleStyleClass("!bg-yellow-200/40", false);
            save_btn_->setDisabled(true);
        }
    });

}

void WTVariableText::set_read_only()
{
    tVariable_text_->input_name_->setReadOnly(true);
    tVariable_text_->input_value_->setReadOnly(true);
    tVariable_text_->input_line_height_->setReadOnly(true);
    save_btn_->setDisabled(true);
    delete_btn_->setDisabled(true);
}
