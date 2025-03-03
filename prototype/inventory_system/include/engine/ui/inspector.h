#pragma once

#ifndef __ENGINE_UI_INSPECTOR_H__
#define __ENGINE_UI_INSPECTOR_H__

#include "engine/ui/ui_element.h"

class Inspector : public UIElement
{
    public:
    void Render() override;
};

#endif // __ENGINE_UI_INSPECTOR_H__
