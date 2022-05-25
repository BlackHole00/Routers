project('lib_nuklear', 'c')

lib_raylib = subproject('raylib')
lib_raylib_dep = lib_raylib.get_variable('lib_raylib_dep')

inc = include_directories('include')
lib = static_library('nuklear',
    'include/_implementation.c',
    include_directories : inc,
    dependencies: lib_raylib_dep,
    install : true
)

lib_nuklear_dep = declare_dependency(include_directories : inc, link_with : lib)
