#pragma once

#include <libfilepicker/model/ArchiveListing.h>
#include <libfilepicker/model/Navigation.h>
#include <libwidget/Views.h>

#include <libfile/Archive.h>

namespace FilePicker
{

struct ArchiveBrowser : public Widget::Table
{
private:
    Ref<Navigation> _navigation;
    Ref<Archive> _archive;
    Ref<ArchiveListing> _listing;
    Box<Async::Observer<Navigation>> _navigation_observer;

public:
    ArchiveBrowser(Ref<Navigation> navigation, Ref<Archive> archive)
        : Table(), _navigation(navigation), _archive(archive)
    {
        _listing = make<ArchiveListing>(navigation, archive);
        model(_listing);

        flags(Element::FILL);

        empty_message("This archive is empty.");

        _navigation_observer = navigation->observe([this](auto &)
            {
                select(-1);
                scroll_to_top();
            });

        on(Widget::Event::ACTION, [this](auto)
            {
                if (selected() >= 0)
                {
                    if (_listing->info(selected()).type == HJ_FILE_TYPE_DIRECTORY)
                    {
                        _navigation->navigate(_listing->info(selected()).name);
                    }
                    else
                    {
                        // FIXME: Extract and Open the file.
                    }
                }
            });
    }
};

} // namespace FilePicker