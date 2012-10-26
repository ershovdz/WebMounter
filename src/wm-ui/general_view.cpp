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
		bool result = _translator.load(QString("wmui_" + settings.appLang), translationDir());
		if(result)
		{
			QApplication::installTranslator(&_translator);
		}	

		_state = 0;
		_parent = parent;
		_langLabel = new QLabel(tr("Language:"));
		_langCombo = new QComboBox;

		_langCombo->addItem(tr("English"));
		_langCombo->addItem(tr("Russian"));

		_langCombo->setCurrentIndex(localeToIndex(settings.appLang));

		connect(_langCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(changeLang(int)));

		_directoryLabel = new QLabel(tr("Cache Directory:"));
		_browseButton = createButton(tr("&Browse..."), SLOT(browse()));

		if(settings.appStoragePath != "")
		{
			_directoryComboBox = createComboBox(settings.appStoragePath);
		}
		else
		{
			_directoryComboBox = createComboBox(QDir::currentPath());
		}

#ifdef Q_OS_WIN
		_diskLetterLabel = new QLabel(tr("Drive:"));
		_diskLetterComboBox = createDiskComboBox(settings.driveLetter);
#endif
		_acceptButton = createButton(tr("Accept settings"), SLOT(acceptClicked()));

		//Proxy Group
		_proxyLabel = new QLabel(tr("Proxy address:port (e.g. myproxy.ru:8080):"));
		_proxyEdit = new QLineEdit;
		_proxyEdit->setText(settings.proxyAddress);

		_proxyLoginLabel = new QLabel(tr("Proxy login:"));
		_proxyLoginEdit = new QLineEdit;
		_proxyLoginEdit->setText(settings.proxyLogin);

		_passwordLabel = new QLabel(tr("Password:"));
		_passwordEdit = new QLineEdit;
		_passwordEdit->setText(settings.proxyPassword);
		_passwordEdit->setEchoMode(QLineEdit::Password);

		_proxyGroup = new QGroupBox(tr("Proxy Configuration"));
		_proxyLayout = new QGridLayout;
		_proxyLayout->addWidget(_proxyLabel, 0, 0);
		_proxyLayout->addWidget(_proxyEdit, 0, 1);
		_proxyLayout->addWidget(_proxyLoginLabel, 1, 0);
		_proxyLayout->addWidget(_proxyLoginEdit, 1, 1);
		_proxyLayout->addWidget(_passwordLabel, 2, 0);
		_proxyLayout->addWidget(_passwordEdit, 2, 1);
		_proxyGroup->setLayout(_proxyLayout);

		//Other settings group
		_miscGroup = new QGroupBox(tr("Misc Settings"));
		_miscLayout = new QGridLayout;
		_miscLayout->addWidget(_directoryLabel, 0, 0);
		_miscLayout->addWidget(_directoryComboBox, 0, 1);
		_miscLayout->addWidget(_browseButton, 0, 2);
		_miscLayout->addWidget(_langLabel, 1, 0);
		_miscLayout->addWidget(_langCombo, 1, 1);

#ifdef Q_OS_WIN
		_miscLayout->addWidget(_diskLetterLabel, 2, 0);
		_miscLayout->addWidget(_diskLetterComboBox, 2, 1);
#endif

		_miscGroup->setLayout(_miscLayout);


		_diskStatusGroup = new QGroupBox(tr("Status"));
		_diskStatusLayout = new QGridLayout;
		_statusLabel = new QLabel(tr("Disk status:"));
		_statusValue = new QLabel(tr("<font color=\"red\"><b>Unmounted</b></font>"));
		_statusValue->setTextFormat(Qt::RichText);

		_diskStatusLayout->addWidget(_statusLabel, 0,0);
		_diskStatusLayout->addWidget(_statusValue, 0,1);
		_diskStatusGroup->setLayout(_diskStatusLayout);


		_mountButton = createButton(tr("Mount Disk"), SLOT(mountClicked()));
		_unmountButton = createButton(tr("Unmount Disk"), SLOT(unmountClicked()));

		_buttonGroup = new QGroupBox();
		_buttonLayout = new QGridLayout;
		_buttonLayout->addWidget(_acceptButton, 0, 0);
		_buttonLayout->addWidget(_mountButton, 0, 1);
		_buttonLayout->addWidget(_unmountButton, 0, 2);
		_buttonGroup->setLayout(_buttonLayout);

		_unmountButton->setEnabled(false);

		_mainLayout = new QVBoxLayout;
		_mainLayout->addWidget(_proxyGroup);
		_mainLayout->addSpacing(12);
		_mainLayout->addWidget(_miscGroup);
		_mainLayout->addSpacing(12);
		_mainLayout->addWidget(_diskStatusGroup);
		_mainLayout->addSpacing(12);
		_mainLayout->addWidget(_buttonGroup);
		_mainLayout->addStretch(1);
		setLayout(_mainLayout);

		LocalDriver::ILVFSDriver* driver = Common::WebMounter::getLocalDriver();
		connect(this, SIGNAL(mount(Data::GeneralSettings&)), driver, SLOT(mount(Data::GeneralSettings&)));
		connect(this, SIGNAL(unmount()), driver, SLOT(unmount())); 
		connect(driver, SIGNAL(mounted()), this, SLOT(mounted())); 
		connect(driver, SIGNAL(unmounted()), this, SLOT(unmounted())); 
	}

	QPushButton *GeneralView::createButton(const QString &text, const char *member)
	{
		QPushButton *button = new QPushButton(text);
		connect(button, SIGNAL(clicked()), this, member);
		return button;
	}

	void GeneralView::mountClicked()
	{
		_directoryLabel->setEnabled(false);
		_directoryComboBox->setEnabled(false);
		_browseButton->setEnabled(false);
		_acceptButton->setEnabled(false);
		_mountButton->setEnabled(false);
		_unmountButton->setEnabled(true);
		
		#ifdef Q_OS_WIN
		_diskLetterLabel->setEnabled(false);
		_diskLetterComboBox->setEnabled(false);
		#endif

		GeneralSettings settings;
		settings.appStoragePath = _directoryComboBox->currentText();
		
		#ifdef Q_OS_WIN
		settings.driveLetter = _diskLetterComboBox->currentText();
		#endif

		emit mount(settings/*settings*/);
	}

	void GeneralView::unmountClicked()
	{
		//_unmountButton->setEnabled(false);
		emit unmount();
	}

	void GeneralView::mounted()
	{
		_statusValue->setText(tr("<font color=\"green\"><b>Mounted</b></font>"));
		_state = 1;
	}

	void GeneralView::unmounted()
	{
		_statusValue->setText(tr("<font color=\"red\"><b>Unmounted</b></font>"));

		_directoryLabel->setEnabled(true);
		_directoryComboBox->setEnabled(true);
		_browseButton->setEnabled(true);
		_acceptButton->setEnabled(true);
		_mountButton->setEnabled(true);
		_unmountButton->setEnabled(false);
		
		#ifdef Q_OS_WIN
		_diskLetterLabel->setEnabled(true);
		_diskLetterComboBox->setEnabled(true);
		#endif
		_state = 0;
	}

	void GeneralView::browse()
	{
		QString directory = QFileDialog::getExistingDirectory(this,
			tr("Find Files"), QDir::currentPath());

		if (!directory.isEmpty()) {
			if (_directoryComboBox->findText(directory) == -1)
				_directoryComboBox->addItem(directory);
			_directoryComboBox->setCurrentIndex(_directoryComboBox->findText(directory));
		}
	}

	void GeneralView::changeLang(int index)
	{
		changeLanguage(indexToLocale(index));

		QApplication::removeTranslator(&_translator);
		bool result = _translator.load(QString("wmui_" + indexToLocale(index)), translationDir());
		if(result)
		{
			QApplication::installTranslator(&_translator);
		}
		
		_langLabel->setText(tr("Language:"));

		disconnect(_langCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(changeLang(int)));

		_langCombo->clear();
		_langCombo->addItem(tr("English"));
		_langCombo->addItem(tr("Russian"));
		_langCombo->setCurrentIndex(index);

		connect(_langCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(changeLang(int)));

		_directoryLabel->setText(tr("Cache Directory:"));
		_browseButton->setText(tr("&Browse..."));

#ifdef Q_OS_WIN
		_diskLetterLabel->setText(tr("Drive:"));
#endif
		_acceptButton->setText(tr("Accept settings"));

		//Proxy Group
		_proxyLabel->setText(tr("Proxy address:port (e.g. myproxy.ru:8080):"));
		_proxyLoginLabel->setText(tr("Proxy login:"));
		_passwordLabel->setText(tr("Password:"));

		_proxyGroup->setTitle(tr("Proxy Configuration"));

		//Other settings group
		_miscGroup->setTitle(tr("Misc Settings"));
		_mountButton->setText(tr("Mount Disk"));
		_unmountButton->setText(tr("Unmount Disk"));

		_diskStatusGroup->setTitle(tr("Status"));
		_statusLabel->setText(tr("Disk status:"));

		if(_state)
		{
			_statusValue->setText(tr("<font color=\"green\"><b>Mounted</b></font>"));
		}
		else
		{
			_statusValue->setText(tr("<font color=\"red\"><b>Unmounted</b></font>"));
		}
	}

	QString GeneralView::indexToLocale(int index)
	{
		switch(index)
		{
		case 1:
			{
				return "ru_RU";
			}
		default:
		case 0:
			{
				return "en_EN";
			}
		}
	}

	int GeneralView::localeToIndex(QString local)
	{
		if(local == "ru_RU")
		{
			return 1;
		}
		return 0;
	}

#ifdef Q_OS_WIN
	QComboBox *GeneralView::createDiskComboBox(const QString& driveLetter)
	{
		QComboBox *comboBox = new QComboBox;
		comboBox->setEditable(false);

		QFileInfoList driveList = QFSFileEngine::drives();
		int index = 0;

		for(char a = 67; a < 91; a++)
		{
			bool insert = true;
			QString letter = (QString::fromAscii(&a, 1) + ":");
			for(QFileInfoList::Iterator iter = driveList.begin(); 
				iter != driveList.end();
				++iter)
			{
				if(iter->canonicalPath() == letter)
				{
					insert = false;
				}
			}
			if(insert)
			{
				comboBox->addItem(letter);
				if(letter == driveLetter)
				{
					comboBox->setCurrentIndex(index);
				}
				index++;
			}
		}


		//comboBox->addItem(text);
		comboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
		return comboBox;
	}
#endif

	QComboBox *GeneralView::createComboBox(const QString &text)
	{
		QComboBox *comboBox = new QComboBox;
		comboBox->setEditable(true);
		comboBox->addItem(text);
		comboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
		return comboBox;
	}
	void GeneralView::acceptClicked()
	{
		Data::GeneralSettings settings;

		settings.appLang			= indexToLocale(_langCombo->currentIndex());
		settings.appStoragePath		= _directoryComboBox->currentText();
#ifdef Q_OS_WIN
		settings.driveLetter		= _diskLetterComboBox->currentText();
#endif
		settings.proxyAddress		= _proxyEdit->text();
		settings.proxyLogin			= _proxyLoginEdit->text();
		settings.proxyPassword		= _passwordEdit->text();

		Common::WebMounter::getSettingStorage()->addSettings(settings);
	}

	QString GeneralView::translationDir()
	{
		QDir appDir(qApp->applicationDirPath());
		printf("appDir = %s\n", qApp->applicationDirPath().toAscii().data());
		appDir.cd("..");
		printf("appDir = %s\n", appDir.absolutePath().toAscii().data());
		appDir.cd("share/webmounter");
		printf("pluginsdir = %s\n", appDir.absolutePath().toAscii().data());
		return appDir.absolutePath();
	}
}
