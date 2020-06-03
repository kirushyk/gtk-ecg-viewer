ui_sources = [
    'ecg-monitor.vala',
    'main-menu.vala',
    'main-window.vala',
    'main.vala'
]

incdir = include_directories('../parsers')

gtk = dependency('gtk+-3.0',
    required: false
)

if gtk.found()
    executable('gtk-ecg-viewer', ui_sources,
        dependencies: gtk,
        include_directories: incdir,
        link_with: parsrs,
        install: true
    )
endif
