# Note: plugindir is set in configure

##############################################################################
# TODO: change libgstplugin.la to something else, e.g. libmysomething.la     #
##############################################################################
plugin_LTLIBRARIES = liblensecorrection.la

##############################################################################
# TODO: for the next set of variables, name the prefix if you named the .la, #
#  e.g. libmysomething.la => libmysomething_la_SOURCES                       #
#                            libmysomething_la_CFLAGS                        #
#                            libmysomething_la_LIBADD                        #
#                            libmysomething_la_LDFLAGS                       #
##############################################################################

# sources used to compile this plug-in
liblensecorrection_la_SOURCES = lensecorrection.c lensecorrection.h

# compiler and linker flags used to compile this plugin, set in configure.ac
liblensecorrection_la_CFLAGS = $(GST_CFLAGS)
liblensecorrection_la_LIBADD = $(GST_LIBS)
liblensecorrection_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS) -lgstvideo-0.10 -lgstinterfaces-0.10
liblensecorrection_la_LIBTOOLFLAGS = --tag=disable-static

# headers we need but don't want installed
noinst_HEADERS = lensecorrection.h
