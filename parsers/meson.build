parsers_sources = [
    'ecg-parser.c',
    'tlc5000.bin.c',
    'tlc5000.ecg.c'
]

vapi_dir =  meson.current_source_dir()

add_project_arguments(['--vapidir', vapi_dir], language: 'vala')

parsers = static_library('parsers', parsers_sources,
    install: false
)

parsers_bin_sources = parsers_sources + ['tlc-ecg-parser.c']
executable('tlc-ecg-parser', parsers_bin_sources)
