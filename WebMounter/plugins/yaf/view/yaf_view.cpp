#include "yaf_view.h"

namespace Ui
{
	YafView::YafView(const Data::PluginSettings& settings, const QString& title)
		: PluginView(settings, title)
	{
		_urlEdit->setText("fotki.yandex.ru");
		_urlEdit->setEnabled(false);

		_keyLabel->setVisible(false);
		_keyEdit->setVisible(false);
		_keyUrl->setVisible(false);
	}

	bool YafView::isKeyValueValid(const Data::PluginSettings&)
	{
		return true;
	}

	void YafView::updateView(int progress, int state)
	{
		PluginView::updateView(progress, state);
		_urlEdit->setEnabled(false);
	}
}