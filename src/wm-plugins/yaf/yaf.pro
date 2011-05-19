QT += network webkit
TEMPLATE        = lib
CONFIG         += plugin

INCLUDEPATH     += ../../wm-base ../../wm-ui

HEADERS         = yaf_plugin.h \
	./driver/yaf_driver.h \
	./view/yaf_view.h \
	./connector/yaf_connector.h \
	./connector/cp_rsa.h \
	./connector/base64.h \
	./view/yaf_oauth.h

SOURCES         = yaf_plugin.cpp \
	./driver/yaf_driver.cpp \
	./view/yaf_view.cpp \
	./connector/yaf_connector.cpp \
	./connector/cp_rsa.cpp \
	./connector/base64.cpp \
	./view/yaf_oauth.cpp 

win32 {
	INCLUDEPATH += $(BOOST_DIR) $(LIBCURL_DIR)\include

	QMAKE_LIBDIR += $(LIBCURL_DIR)\lib\DLL-Debug 

	LIBS += -llibcurld -lwmbase -lwmui
}
else {
	LIBS += -lcurl

	CONFIG(release) system(lrelease ./lang/*.ts)
	LRELEASE = $$[QT_INSTALL_BINS]/lrelease

	updateqm.input = TRANSLATIONS
	updateqm.output = ../../build/share/webmounter/${QMAKE_FILE_BASE}.qm
	updateqm.commands = $$LRELEASE ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_OUT}
	updateqm.name = LRELEASE
	updateqm.CONFIG += no_link
	QMAKE_EXTRA_COMPILERS += updateqm
	PRE_TARGETDEPS += compiler_updateqm_make_all

	qmfiles.files = ./lang/*.qm
	qmfiles.path = /usr/share/webmounter

	INSTALLS += qmfiles
}

QMAKE_RPATHDIR += ../../build/lib
QMAKE_LIBDIR += ../../build/lib

TARGET          = $$qtLibraryTarget(wm-yandex-plugin)
DESTDIR         = ../../build/lib/plugins

target.path = /usr/lib/webmounter/plugins

RESOURCES += yaf.qrc

TRANSLATIONS += ./lang/yaf_wm_pl_ru.ts \
	./lang/yaf_wm_pl_en.ts

INSTALLS += target

