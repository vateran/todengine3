#############################################################################
# Makefile for building: todeditor.app/Contents/MacOS/todeditor
# Generated by qmake (3.1) (Qt 5.9.0)
# Project:  todeditor.pro
# Template: app
# Command: /Users/vateran/Qt/5.9/clang_64/bin/qmake -o todeditor.xcodeproj/project.pbxproj todeditor.pro -spec macx-xcode
#############################################################################

MAKEFILE      = project.pbxproj

MOC       = /Users/vateran/Qt/5.9/clang_64/bin/moc
UIC       = /Users/vateran/Qt/5.9/clang_64/bin/uic
LEX       = flex
LEXFLAGS  = 
YACC      = yacc
YACCFLAGS = -d
DEFINES       = -DQT_DEPRECATED_WARNINGS -DQT_NO_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB
INCPATH       = -I. -I../../../Qt/5.9/clang_64/lib/QtWidgets.framework/Headers -I../../../Qt/5.9/clang_64/lib/QtGui.framework/Headers -I../../../Qt/5.9/clang_64/lib/QtCore.framework/Headers -I. -I/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.12.sdk/System/Library/Frameworks/OpenGL.framework/Headers -I/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.12.sdk/System/Library/Frameworks/AGL.framework/Headers -I. -I../../../Qt/5.9/clang_64/mkspecs/macx-clang -F/Users/vateran/Qt/5.9/clang_64/lib
DEL_FILE  = rm -f
MOVE      = mv -f

preprocess: compilers
clean preprocess_clean: compiler_clean

mocclean: compiler_moc_header_clean compiler_moc_source_clean

mocables: compiler_moc_header_make_all compiler_moc_source_make_all

check: first

benchmark: first

compilers: moc_predefs.h moc_todeditor.cpp ui_todeditor.h
compiler_rcc_make_all:
compiler_rcc_clean:
compiler_moc_predefs_make_all: moc_predefs.h
compiler_moc_predefs_clean:
	-$(DEL_FILE) moc_predefs.h
moc_predefs.h: ../../../Qt/5.9/clang_64/mkspecs/features/data/dummy.cpp
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang++ -pipe -stdlib=libc++ -O2 -std=gnu++11 -Wall -W -dM -E -o moc_predefs.h ../../../Qt/5.9/clang_64/mkspecs/features/data/dummy.cpp

compiler_moc_header_make_all: moc_todeditor.cpp
compiler_moc_header_clean:
	-$(DEL_FILE) moc_todeditor.cpp
moc_todeditor.cpp: ../../../Qt/5.9/clang_64/lib/QtWidgets.framework/Headers/QMainWindow \
		../../../Qt/5.9/clang_64/lib/QtWidgets.framework/Headers/qmainwindow.h \
		todeditor.h \
		moc_predefs.h \
		../../../Qt/5.9/clang_64/bin/moc
	/Users/vateran/Qt/5.9/clang_64/bin/moc $(DEFINES) --include ./moc_predefs.h -I/Users/vateran/Qt/5.9/clang_64/mkspecs/macx-clang -I/Users/vateran/project/todengine3/todeditor -I/Users/vateran/Qt/5.9/clang_64/lib/QtWidgets.framework/Headers -I/Users/vateran/Qt/5.9/clang_64/lib/QtGui.framework/Headers -I/Users/vateran/Qt/5.9/clang_64/lib/QtCore.framework/Headers -I/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/include/c++/v1 -I/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/lib/clang/8.1.0/include -I/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/include -I/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.12.sdk/usr/include -F/Users/vateran/Qt/5.9/clang_64/lib todeditor.h -o moc_todeditor.cpp

compiler_moc_source_make_all:
compiler_moc_source_clean:
compiler_uic_make_all: ui_todeditor.h
compiler_uic_clean:
	-$(DEL_FILE) ui_todeditor.h
ui_todeditor.h: todeditor.ui \
		../../../Qt/5.9/clang_64/bin/uic
	/Users/vateran/Qt/5.9/clang_64/bin/uic todeditor.ui -o ui_todeditor.h

compiler_rez_source_make_all:
compiler_rez_source_clean:
compiler_yacc_decl_make_all:
compiler_yacc_decl_clean:
compiler_yacc_impl_make_all:
compiler_yacc_impl_clean:
compiler_lex_make_all:
compiler_lex_clean:
compiler_clean: compiler_moc_predefs_clean compiler_moc_header_clean compiler_uic_clean 
