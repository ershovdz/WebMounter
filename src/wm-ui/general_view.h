#ifndef GENERAL_VIEW_H
#define GENERAL_VIEW_H

#include <QWidget>
#include <QtGui>
#include <QTranslator>
#include "../wm-base/data.h"

//#include "control_panel.h"

#if defined(WEBMOUNTER_UI_LIBRARY)
#  define WEBMOUNTER_UI_EXPORT Q_DECL_EXPORT
#else
#  define WEBMOUNTER_UI_EXPORT Q_DECL_IMPORT
#endif

namespace Ui
{
	using namespace Data;
	class WEBMOUNTER_UI_EXPORT GeneralView : public QWidget
	{
		Q_OBJECT
	public:
		GeneralView(const Data::GeneralSettings& settings, QWidget *parent = 0);
	private:
		QPushButton *createButton(const QString &text, const char *member);
		QComboBox *createComboBox(const QString &text = QString());

		QString indexToLocale(int index);
		int localeToIndex(QString local);
		static QString translationDir();

#ifdef Q_OS_WIN
		QComboBox * createDiskComboBox(const QString& driveLetter);
#endif

	private slots:
			void acceptClicked();
			void mountClicked();
			void unmountClicked();
			void browse();
			void changeLang(int);

	public slots:
			void mounted();
			void unmounted();

	signals:
			void mount(Data::GeneralSettings&);
			void unmount();
			void changeLanguage(const QString&);
	private:
		QWidget* _parent;

		QComboBox *_directoryComboBox;
		QPushButton *_browseButton;
		QLabel *_directoryLabel;

		QLabel *_proxyLabel;
		QLineEdit *_proxyEdit;
		QLabel *_proxyLoginLabel;
		QLineEdit *_proxyLoginEdit;
		QLabel *_passwordLabel;
		QLineEdit *_passwordEdit;

		QGroupBox *_miscGroup;
		QGridLayout *_miscLayout;

		QGroupBox* _diskStatusGroup;
		QGridLayout* _diskStatusLayout;

		QLabel* _statusLabel;
		QLabel* _statusValue;
		
		QLabel *_diskLetterLabel;
#ifdef Q_OS_WIN
		QComboBox *_diskLetterComboBox;
#endif
		QPushButton *_acceptButton;
		QLabel *_langLabel;
		QComboBox *_langCombo;

		QGroupBox *_proxyGroup;
		QGridLayout *_proxyLayout;

		QGroupBox *_buttonGroup;
		QGridLayout *_buttonLayout;
		QPushButton *_mountButton;
		QPushButton *_unmountButton;

		QVBoxLayout *_mainLayout;
		unsigned int _state;
		QTranslator _translator;
	};
}
#endif
