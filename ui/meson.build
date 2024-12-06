ui_sources = [
    'ecg-monitor.vala',
    'main-window.vala',
    'main.vala'
]

incdir = include_directories('../parsers')

ui_vala_args = ['--pkg', 'ecg-parser']

gtk = dependency('gtk4', required: false)
if gtk.found()
    ui_vala_args += ['-D', 'HAVE_GTK4']
else
    ui_sources += 'main-menu.vala'
    gtk = dependency('gtk+-3.0', required: false)
endif
# gtk = dependency('gtk+-3.0', required: false)

if gtk.found()
    executable('gtk-ecg-viewer', ui_sources,
        dependencies: gtk,
        vala_args: ui_vala_args,
        include_directories: incdir,
        link_with: parsers,
        install: true
    )
endif
