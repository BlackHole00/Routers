project('lib_raylib', 'c')

gl_dep = dependency('gl')

inc = include_directories('include')

lib_raylib_dep = declare_dependency(
    include_directories : inc,
    dependencies: [ gl_dep ],
    #compile_args: '-Lbuild -lraylib',
    link_args: [ 
        '-L' + meson.current_source_dir() + '/build', 
        '-lraylib',
        '-lwinmm'
    ]
)
