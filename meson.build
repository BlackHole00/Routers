project('router', 'c')

lib_vx_utils = subproject('vx_utils')
lib_vx_utils_dep = lib_vx_utils.get_variable('lib_vx_utils_dep')

lib_raylib = subproject('raylib')
lib_raylib_dep = lib_raylib.get_variable('lib_raylib_dep')

lib_nuklear = subproject('nuklear')
lib_nuklear_dep = lib_nuklear.get_variable('lib_nuklear_dep')

executable('router',
    'src/main.c',
    'src/dk_matrix.c',
    'src/dk_state.c',
    'src/dk_geometry.c',
    'src/dk_gui.c',
    'src/dk_router.c',
    'src/dk_misc.c',
    dependencies : [ lib_vx_utils_dep, lib_raylib_dep, lib_nuklear_dep ],
    link_args: [ '-static-libgcc', '-static' ,'-pthread' ],
    install : true
)