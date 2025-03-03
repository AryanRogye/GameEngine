#pragma once

#ifndef __ENGINE_UI_MANAGER_H__
#define __ENGINE_UI_MANAGER_H__

#include "engine/ui/ui_element.h"

#include "vector"

class UIManager
{
private:
    std::vector<UIElement*> elements;

public:
    UIManager();
    ~UIManager();

    void RenderUI();
};

#endif
