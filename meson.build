project('crypticizer', 'cpp')

ssldep = dependency('openssl', version : '>=3.0.0')
cursesdep = dependency('ncurses')
exe = executable('crypticizer', 'src/crypticizer.cpp', 'src/menu.cpp', 'src/cryptor.cpp', 'src/session.cpp',
  dependencies : [ssldep, cursesdep], install : true)
