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

		static QString translationDir();

#ifdef Q_OS_WIN
		QComboBox * createDiskComboBox(const QString& driveLetter);
#endif

	private slots:
			void mountClicked();
			void unmountClicked();

  public slots:
			void mounted();
			void unmounted();

	signals:
			void mount();
			void unmount();
			
	private:
		QWidget* _parent;

		QGroupBox* _diskStatusGroup;
		QGridLayout* _diskStatusLayout;

		QLabel* _statusLabel;
		QLabel* _statusValue;
		
//#ifdef Q_OS_WIN
//    QLabel *_diskLetterLabel;
//    QComboBox *_diskLetterComboBox;
//#endif
		
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
