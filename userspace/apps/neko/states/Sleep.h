#pragma once

#include "neko/model/Behavior.h"

namespace neko
{

struct Sleep : public Behavior
{
private:
    Math::Vec2i _last_mouse_position;

public:
    const char *name() override { return "Sleep"; }

    Sleep();

    void update(Neko &neko) override;

    Animation animation(Neko &neko) override;
};

} // namespace neko
