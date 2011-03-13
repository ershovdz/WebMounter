#include "google_view.h"
#include "../driver/google_driver.h"
#include "WebMounter.h"

namespace Ui
{
	using namespace RemoteDriver;

	GoogleView::GoogleView(const Data::PluginSettings* settings, const QString& title)
		: PluginView(settings, title)
	{
		_urlEdit->setText("docs.google.com");
		_urlEdit->setEnabled(false);

		_keyLabel->setVisible(false);
		_keyEdit->setVisible(false);
		_keyUrl->setVisible(false);
	}

	bool GoogleView::isKeyValueValid(const Data::PluginSettings&)
	{
		return true;
	}

	void GoogleView::updateView(int progress, int state)
	{
		PluginView::updateView(progress, state);
		_urlEdit->setEnabled(false);
	}
}
