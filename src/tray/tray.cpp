#include "tray.hpp"

namespace Soundux
{
    BaseTray *TrayItem::getParent()
    {
        return parent;
    }
    void TrayItem::setParent(BaseTray *parent)
    {
        this->parent = parent;
    }
    std::string TrayItem::getName()
    {
        return name;
    }

    void TrayButton::onClicked()
    {
        clicked();
    }
    void TrayButton::setCallback(std::function<void()> callback)
    {
        clicked = std::move(callback);
    }

    void TrayCheck::onToggled(bool state)
    {
        toggled = state;
        checked(toggled);
    }
    void TrayCheck::setCallback(std::function<void(bool)> callback)
    {
        checked = std::move(callback);
    }
    bool TrayCheck::isChecked() const
    {
        return toggled;
    }
} // namespace Soundux