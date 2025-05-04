#pragma once
#include "101-Stylus/FilesManager.h"
#include "101-Stylus/StylusState.h"

namespace Stylus
{

class JsFilesManager : public FilesManager
{
public:
    JsFilesManager(std::shared_ptr<StylusState> state);

private:
    std::shared_ptr<StylusState> state_;
    };
}