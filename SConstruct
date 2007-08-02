# $Id: SConstruct,v 1.7 2007/08/02 21:08:33 ecto Exp $

opts = Options()
opts.AddOptions(
	BoolOption('debug', 'debug output and symbols', 1),
)

env = Environment(
	CXX = 'g++-4.2',
	CPPPATH = ['/usr/local/include' ],
	CPPDEFINES = [ '_REENTRANT' ],
	CPPFLAGS = [ '-Wall', '-Wextra', '-Werror', '-pipe' ],
	CXXFLAGS = [ '-fmessage-length=0', '-fvisibility-inlines-hidden' ], #, '-fvisibility=hidden' ],
	LIBS = [],
	options = opts,
)

if env['debug']:
	env.Append( CXXFLAGS = [ '-ggdb', '-O0' ], )
else:
	env.Append( CXXFLAGS = [ '-O2' ], )

Help(opts.GenerateHelpText(env))

env.Program( target = 'hid_read', source = [ 'hid_read.cc' ] )
