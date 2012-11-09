
#ifndef _JSON_SCANNER
#define _JSON_SCANNER

#include <fstream>
#include <string>

#include <QtCore/QIODevice>
#include <QtCore/QVariant>

#define YYSTYPE QVariant

#include "parser_p.h"

namespace yy 
{
	class location;
	int yylex(YYSTYPE *yylval, yy::location *yylloc, QJson::ParserPrivate* driver);
}

class JSonScanner
{
public:
	explicit JSonScanner(QIODevice* io);
	int yylex(YYSTYPE* yylval, yy::location *yylloc);
	void allowSpecialNumbers(bool allow);

protected:
	bool m_quotmarkClosed;
	bool m_allowSpecialNumbers;
	unsigned int m_quotmarkCount;
	QIODevice* m_io;
};

#endif

