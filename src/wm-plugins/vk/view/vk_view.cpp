#include "common_stuff.h"
#include "vk_view.h"
#include "../driver/vk_driver.h"

namespace Ui
{
	using namespace Common;

	VkView::VkView(const Data::PluginSettings* settings, const QString& title)
		: PluginView(settings, title)
	{
		_urlEdit->setText("vkontakte.ru");
		_urlEdit->setEnabled(false);

#ifdef WM_FULL_VERSION
		_keyUrl->setText(tr("<a href=\"http://jmediamanager.com/index.php?option=com_wrapper&view=wrapper&Itemid=102&lang=ru\">Get activation key</a>"));
#endif
	}

	bool VkView::isKeyValueValid(const Data::PluginSettings& settings)
	{
		if(_driver->checkKey(settings) == eERROR)
		{
			QMessageBox::information(this, tr("Warning"),
				tr("Activation key is not valid. The program is now working in demo mode."), QMessageBox::Ok);
		}

		return true;
	}

	void VkView::changeLang()
	{
		PluginView::changeLang();
#ifdef WM_FULL_VERSION
		_keyUrl->setText(tr("<a href=\"http://jmediamanager.com/index.php?option=com_wrapper&view=wrapper&Itemid=102&lang=ru\">Get activation key</a>"));
#endif
	}

	void VkView::updateView(int progress, int state)
	{
		PluginView::updateView(progress, state);
		_urlEdit->setEnabled(false);
	}
}
