#include <QtGui>

#include "general_view.h"
#include "data.h"
#include "webmounter.h"
#include <QFSFileEngine>
#include "common_stuff.h"

#include LVFS_DRIVER_H

namespace Ui
{
	GeneralView::GeneralView(const Data::GeneralSettings& settings, QWidget *parent)
		: QWidget(parent)
	{
		QString sss =  translationDir();
		bool result = _translator.load(QString("wmui_" + settings.appLang), translationDir());
		if(result)
		{
			QApplication::installTranslator(&_translator);
		}	

		_state = 0;
		_parent = parent;

		_diskStatusGroup = new QGroupBox(tr("Disk"));
		_diskStatusLayout = new QGridLayout;
		_statusLabel = new QLabel(tr("Disk status:"));
		_statusValue = new QLabel(tr("<font color=\"red\"><b>Unmounted</b></font>"));
		_statusValue->setTextFormat(Qt::RichText);

		_diskStatusLayout->addWidget(_statusLabel, 0,0);
		_diskStatusLayout->addWidget(_statusValue, 0,1);
		_diskStatusGroup->setLayout(_diskStatusLayout);


		_mountButton = createButton(tr("Mount Disk"), SLOT(mountClicked()));
		_unmountButton = createButton(tr("Unmount Disk"), SLOT(unmountClicked()));

		_diskStatusLayout->addWidget(_mountButton, 1, 0);
		_diskStatusLayout->addWidget(_unmountButton, 1, 1);

		_unmountButton->setEnabled(false);

		_mainLayout = new QVBoxLayout;
		_mainLayout->addWidget(_diskStatusGroup);
		_mainLayout->addSpacing(12);
		_mainLayout->addStretch(1);
		setLayout(_mainLayout);

	}

	QPushButton *GeneralView::createButton(const QString &text, const char *member)
	{
		QPushButton *button = new QPushButton(text);
		connect(button, SIGNAL(clicked()), this, member);
		return button;
	}

	void GeneralView::mountClicked()
	{
		_mountButton->setEnabled(false);
		_unmountButton->setEnabled(true);

		emit mount();
	}

	void GeneralView::unmountClicked()
	{
		_unmountButton->setEnabled(false);
		emit unmount();
	}

	void GeneralView::mounted()
	{
		_mountButton->setEnabled(false);
		_unmountButton->setEnabled(true);

		_statusValue->setText(tr("<font color=\"green\"><b>Mounted</b></font>"));
		_state = 1;
	}

	void GeneralView::unmounted()
	{
		_statusValue->setText(tr("<font color=\"red\"><b>Unmounted</b></font>"));

		_mountButton->setEnabled(true);
		_unmountButton->setEnabled(false);

		_state = 0;
	}

	QString GeneralView::translationDir()
	{
		QDir appDir(qApp->applicationDirPath());

		QString ddd = qApp->applicationDirPath();
		printf("appDir = %s\n", qApp->applicationDirPath().toAscii().data());
		appDir.cd("..");
		QString ddd11 = appDir.absolutePath();
		printf("appDir = %s\n", appDir.absolutePath().toAscii().data());
		appDir.cd("share/webmounter");
		QString ddd1331 = appDir.absolutePath();
		printf("pluginsdir = %s\n", appDir.absolutePath().toAscii().data());
		return appDir.absolutePath();
	}
}
