#pragma once
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace Soundux
{
    class BaseTray;
    class TrayItem
    {
      protected:
        std::string name;
        BaseTray *parent = nullptr;

      public:
        virtual ~TrayItem() = default;
        TrayItem(std::string name) : name(std::move(name)) {}

        BaseTray *getParent();
        std::string getName();
        void setName(std::string);
        virtual void setParent(BaseTray *);
    };
    class BaseTray
    {
      protected:
        std::string name, icon;
        std::vector<std::shared_ptr<TrayItem>> children;

      public:
        template <typename... T>
        BaseTray(std::string name, std::string icon, const T &...items) : name(std::move(name)), icon(std::move(icon))
        {
            (addItem<std::decay_t<decltype(items)>, false>(items), ...);
        }

        template <typename... T> void addItems(const T &...items)
        {
            (addItem(items), ...);
        }
        template <typename T, bool shouldUpdate = true> auto addItem(const T &item)
        {
            children.emplace_back(std::make_shared<T>(item));
            children.back()->setParent(this);
            if constexpr (shouldUpdate)
            {
                update();
            }
            return std::dynamic_pointer_cast<std::decay_t<T>>(children.back());
        }

        BaseTray(const BaseTray &) = delete;
        BaseTray &operator=(const BaseTray &) = delete;

        virtual bool run() = 0;
        virtual void exit() = 0;
        virtual void update() = 0;
        virtual std::vector<std::shared_ptr<TrayItem>> getChildren();
    };

    class TrayButton : public TrayItem
    {
        std::function<void()> clicked;

      public:
        ~TrayButton() override = default;
        TrayButton(std::string name, std::function<void()> clicked)
            : TrayItem(std::move(name)), clicked(std::move(clicked))
        {
        }

        void onClicked();
        void setCallback(std::function<void()>);
    };
    class TrayCheck : public TrayItem
    {
        bool toggled;
        std::function<void(bool)> checked;

      public:
        ~TrayCheck() override = default;
        TrayCheck(std::string name, bool toggled, std::function<void(bool)> callback)
            : TrayItem(std::move(name)), toggled(toggled), checked(std::move(callback))
        {
        }

        void onToggled(bool);
        bool isChecked() const;
        void setCallback(std::function<void(bool)>);
    };
    class TraySyncedCheck : public TrayItem
    {
        bool &toggled;
        std::function<void(bool)> checked;

      public:
        ~TraySyncedCheck() override = default;
        TraySyncedCheck(std::string name, bool &toggled, std::function<void(bool)> callback)
            : TrayItem(std::move(name)), toggled(toggled), checked(std::move(callback))
        {
        }

        void onToggled(bool);
        bool isChecked() const;
        void setCallback(std::function<void(bool)>);
    };
    class TraySubmenu : public TrayItem
    {
        std::vector<std::shared_ptr<TrayItem>> children;

      public:
        ~TraySubmenu() override = default;
        template <typename... T> TraySubmenu(std::string name, const T &...items) : TrayItem(std::move(name))
        {
            (addItem(items), ...);
        }

        template <typename... T> void addItems(const T &...items)
        {
            (addItem(items), ...);
        }
        template <typename T> auto addItem(const T &item)
        {
            children.emplace_back(std::make_shared<T>(item));
            children.back()->setParent(parent);
            if (parent)
            {
                parent->update();
            }
            return std::dynamic_pointer_cast<std::decay_t<T>>(children.back());
        }

        void setParent(BaseTray *tray) override;
        std::vector<std::shared_ptr<TrayItem>> getChildren();
    };
} // namespace Soundux