
#pragma once

#ifndef __ENGINE_UI_ELEMENT_H__
#define __ENGINE_UI_ELEMENT_H__

class UIElement
{
public:
    virtual ~UIElement() = default;
    virtual void Render() = 0;
};


#endif // !__ENGINE_UI_ELEMENT_H__
