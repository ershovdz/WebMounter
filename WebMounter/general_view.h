#ifndef GENERALVIEW_H
#define GENERALVIEW_H

//#include "JmmRVFSDriver.h"
#include <QWidget>
#include <QtGui>
#include "data.h"

#include "control_panel.h"
//#include "rvfs_driver.h"

//using namespace RemoteDriver;

namespace Ui
{
	using namespace Data;
	class GeneralView : public QWidget
	{
		Q_OBJECT
	public:
		GeneralView(const Data::GeneralSettings& settings, ControlPanel *parent = 0);
	private:
		QPushButton *createButton(const QString &text, const char *member);
		QComboBox *createComboBox(const QString &text = QString());

		QString indexToLocale(int index);
		int localeToIndex(QString local);

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

	private:
		ControlPanel* _parent;

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
		QComboBox *_diskLetterComboBox;

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
		UINT _state; 
	};
}
#endif
