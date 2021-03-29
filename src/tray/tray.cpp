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
    void TrayItem::setName(std::string newName)
    {
        name = std::move(newName);
        parent->update();
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

    void TraySyncedCheck::onToggled(bool state)
    {
        checked(toggled);
        toggled = state;
    }
    void TraySyncedCheck::setCallback(std::function<void(bool)> callback)
    {
        checked = std::move(callback);
    }
    bool TraySyncedCheck::isChecked() const
    {
        return toggled;
    }

    void TraySubmenu::setParent(BaseTray *tray)
    {
        TrayItem::setParent(tray);
        for (auto &child : children)
        {
            child->setParent(tray);
        }
    }
    std::vector<std::shared_ptr<TrayItem>> TraySubmenu::getChildren()
    {
        return children;
    }

    std::vector<std::shared_ptr<TrayItem>> BaseTray::getChildren()
    {
        return children;
    }
} // namespace Soundux