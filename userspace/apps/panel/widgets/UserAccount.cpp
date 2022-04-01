#include <skift/Environment.h>

#include "panel/widgets/UserAccount.h"

using namespace Widget;

namespace Panel
{

Ref<Element> UserAccountComponent::build()
{
    return label(environment()
                     .get("POSIX")
                     .get("LOGNAME")
                     .as_string());
}

} // namespace Panel