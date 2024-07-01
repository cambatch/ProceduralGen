#pragma once

class InputListener
{
public:
    virtual void OnKeyPress(int key, int action) = 0;
    virtual void OnMouseMove(double x, double y) = 0;
    virtual void OnMouseButton(int key, int action) = 0;
    virtual void OnMouseScroll(double xOffset, double yOffset) = 0;
};
