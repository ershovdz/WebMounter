#ifndef QJSON_PARSER_P_H
#define QJSON_PARSER_P_H

#include "parser.h"

#include <QtCore/QString>
#include <QtCore/QVariant>

class JSonScanner;

namespace yy {
	class json_parser;
}

namespace QJson {

	class ParserPrivate
	{
	public:
		ParserPrivate();
		~ParserPrivate();

		void setError(QString errorMsg, int line);

		JSonScanner* m_scanner;
		bool m_negate;
		bool m_error;
		int m_errorLine;
		QString m_errorMsg;
		QVariant m_result;
		bool m_specialNumbersAllowed;
	};
}

#endif // QJSON_PARSER_H
