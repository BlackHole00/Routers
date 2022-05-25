project('lib_raygui', 'c')

lib_raylib = subproject('raylib')
lib_raylib_dep = lib_raylib.get_variable('lib_raylib_dep')

inc = include_directories('include')
lib = static_library('raygui',
    'src/implementation.c',
    include_directories : inc,
    dependencies: lib_raylib_dep,
    install : true
)

lib_raygui_dep = declare_dependency(include_directories : inc, link_with : lib)
