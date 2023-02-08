from setuptools import Extension, setup
import platform

define_macros = [('HB_NO_MT', '1'), ('HB_EXPERIMENTAL_API', '1')]
extra_compile_args = []
extra_link_args = []
libraries = []
if platform.system() != 'Windows':
    extra_compile_args.append('-std=c++11')
    define_macros.append(('HAVE_MMAP', '1'))
    define_macros.append(('HAVE_UNISTD_H', '1'))
    define_macros.append(('HAVE_SYS_MMAN_H', '1'))
else:
    define_macros.append(('HAVE_DIRECTWRITE', '1'))
    define_macros.append(('HAVE_UNISCRIBE', '1'))
    libraries += ['usp10', 'gdi32', 'user32', 'rpcrt4', 'dwrite']

if platform.system() == 'Darwin':
    define_macros.append(('HAVE_CORETEXT', '1'))
    extra_link_args.extend(['-framework', 'ApplicationServices'])

setup(
    ext_modules=[
        Extension(
            name="fontprofile",
            define_macros=define_macros,
            include_dirs=['harfbuzz/src'],
            sources=[
                'src/fontprofile.cc',
                'harfbuzz/src/harfbuzz.cc',
            ],
            libraries=libraries,
            extra_compile_args=extra_compile_args,
            extra_link_args=extra_link_args,
            language="c++"

        ),
    ]
)
