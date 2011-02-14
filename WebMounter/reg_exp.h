#ifndef REG_EXP_H
#define REG_EXP_H

#include <QString>

namespace Data
{
	class RegExp
	{
	public:
			static QString getByPattern(const QString& pattern, const QString& text);
	};
}

#endif //REG_EXP_H