#pragma once

#include <libsettings/Setting.h>
#include <libwidget/Components.h>

namespace Panel
{

struct SettingToggleComponent :
    public Widget::StatefulComponent<bool>
{
private:
    String _name;
    Ref<Graphic::Icon> _icon;
    Box<Settings::Setting> _setting;

public:
    SettingToggleComponent(
        Ref<Graphic::Icon> icon,
        String name,
        const char *setting);

    Ref<Element> build(bool state) override;
};

WIDGET_BUILDER(SettingToggleComponent, setting_toggle);

} // namespace Panel