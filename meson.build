project('sp01', 'cpp',
    version : '0.1',
    default_options : [
        'cpp_std=c++20',
        'default_library=static'
    ]
)

# ImGui
imgui_root = 'lib' / 'imgui'
imgui_include_dirs = ['lib/imgui', 'lib/imgui/backends']
imgui_src_base = [
    'imgui_demo.cpp',
    'imgui_draw.cpp',
    'imgui_tables.cpp',
    'imgui_widgets.cpp',
    'imgui.cpp',
    'misc' / 'cpp' / 'imgui_stdlib.cpp',
]
imgui_backends_base = [['glfw3', 'glfw'], ['opengl', 'opengl3']]
imgui_src = []
imgui_deps = []
num_imgui_deps = 0

foreach be: imgui_backends_base
    if be[0] in ['opengl']
        imgui_src_base += 'backends' / ('imgui_impl_' + be[1] + '.cpp')
        num_imgui_deps += 1
    else
        be_dep = dependency(be[0], required: false)

        if be_dep.found()
            imgui_src_base += 'backends' / ('imgui_impl_' + be[1] + '.cpp')
            imgui_deps += be_dep
            num_imgui_deps += 1
        else
            warning('ImGui backend "@be_dep[0]@" not found!')
        endif
    endif
endforeach

if num_imgui_deps < 2
    error('No supported ImGui renderer + platform found! Please install them separately first.')
endif

foreach v: imgui_src_base
    imgui_src += imgui_root / v
endforeach

imgui_dep = declare_dependency(
    include_directories: imgui_include_dirs,
    link_with: library('imgui',
        imgui_src,
        dependencies: imgui_deps,
        include_directories: imgui_include_dirs,
    ),
)

# Catch2
catch2_root = 'lib' / 'catch2'

catch2_dep = declare_dependency(
    include_directories: catch2_root / 'extras',
    link_with: library('catch2_amalgamated',
        [
            catch2_root / 'extras' / 'catch_amalgamated.cpp'
        ],
        include_directories: catch2_root / 'extras',
    ),
)

# Program
prog_cpp_flags = []
prog_link_flags = []

if build_machine.system() == 'darwin'
    prog_link_flags += ['-framework OpenGL', '-framework Cocoa', '-framework IOKit', '-framework CoreVideo']
    prog_link_flags += ['-L/usr/local/lib', '-L/opt/local/lib', '-L/opt/homebrew/lib']
    prog_link_flags += '-lglfw'

    prog_cpp_flags += ['-L/usr/local/include', '-L/opt/local/include', '-L/opt/homebrew/include']
elif build_machine.system() == 'windows'
    prog_link_flags += ['-lglfw3', '-lgdi32', '-lopengl32', '-limm32']
endif

# TODO: Add custom C++ files inside the array below
main_src = files(
    'src' / 'bk_tree.cpp',
    'src' / 'string_metric.cpp', # You may stop editing from this line onward
    'src' / 'wordlist_base.cpp',
)

main_lib = static_library(
    'sp01',
    main_src,
    include_directories: 'src',
)

main_dep = declare_dependency(
    link_with: [main_lib],
    include_directories: 'src',
)

main_prog_exe = executable(
    'smartmessage',
    ['main.cpp'],
    dependencies: [imgui_dep, main_dep],
    cpp_args: prog_cpp_flags,
    link_args: prog_link_flags,
)

# Compile tests
subdir('./tests')