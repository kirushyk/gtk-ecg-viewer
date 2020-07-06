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
