#pragma once
#include "100-Utils/FilesManager/FilesManager.h"
#include "101-Stylus/Brain.h"

namespace Stylus
{

class JsFilesManager : public FilesManager
{
public:
    JsFilesManager(std::shared_ptr<Brain> brain);

private:
    std::shared_ptr<Brain> brain_;
    };
}