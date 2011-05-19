#ifndef REG_EXP_H
#define REG_EXP_H

#include <QString>
#include "common_stuff.h"

namespace Data
{
	class WEBMOUNTER_EXPORT RegExp
	{
	public:
			static QString getByPattern(const QString& pattern, const QString& text);
	};
}

#endif //REG_EXP_H