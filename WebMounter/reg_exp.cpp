#include "reg_exp.h"
#include <QRegExp>

namespace Data
{
	QString RegExp::getByPattern(const QString& pattern, const QString& text)
	{
		QString res;
		QRegExp rx(pattern);
		rx.setCaseSensitivity(Qt::CaseSensitive);
		rx.setMinimal(true);
		rx.setPatternSyntax(QRegExp::RegExp);

		rx.indexIn(text);
		res = rx.cap(1);

		return res;
	}
}