project('crypticizer', 'cpp', default_options : ['cpp_std=c++17'])

ssldep = dependency('openssl', version : '>=3.0.0')
cursesdep = dependency('ncurses')
exe = executable('crypticizer', 'src/crypticizer.cpp', 'src/menu.cpp', 'src/cryptor.cpp', 'src/session.cpp', 'src/filestrhelper.cpp',
  dependencies : [ssldep, cursesdep], install : true)
