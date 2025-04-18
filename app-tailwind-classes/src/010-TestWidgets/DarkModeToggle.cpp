#include "010-TestWidgets/DarkModeToggle.h"
#include <Wt/WApplication.h>
#include <Wt/WLabel.h>

DarkModeToggle::DarkModeToggle(bool dark)
{
    setHtmlTagName("label");
    setStyleClass("inline-flex cursor-pointer items-center");
    checkBox_ = addWidget(std::make_unique<Wt::WCheckBox>());
    checkBox_->setChecked(dark);
    setAttributeValue("for", checkBox_->id());
    
    auto display = addWidget(std::make_unique<Wt::WContainerWidget>());

    checkBox_->setStyleClass("peer sr-only");
    display->setStyleClass("peer text-md relative h-6 w-11 rounded-full bg-gray-200 peer-checked:bg-blue-600 peer-focus:ring-4 peer-focus:ring-blue-300 peer-focus:outline-none after:absolute after:start-[2px] after:top-[2px] after:h-5 after:w-5 after:rounded-full after:border after:border-gray-300 after:bg-white after:leading-[16px] after:transition-all after:content-['☀'] peer-checked:after:translate-x-full peer-checked:after:border-white peer-checked:after:content-['🌙'] rtl:peer-checked:after:-translate-x-full dark:border-gray-600 dark:bg-gray-700 dark:peer-checked:bg-blue-600 dark:peer-focus:ring-blue-800");
    checkBox_->changed().connect(this, [=](){
        dark_mode_changed_.emit(checkBox_->isChecked());
    });
}

void DarkModeToggle::setDarkMode(bool dark)
{
    checkBox_->setChecked(dark);
    checkBox_->changed().emit();
}