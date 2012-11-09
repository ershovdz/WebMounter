#include "parser.h"
#include "parser_p.h"
#include "json_parser.hh"
#include "json_scanner.h"

#include <QtCore/QBuffer>
#include <QtCore/QStringList>
#include <QtCore/QTextStream>
#include <QtCore/QDebug>

using namespace QJson;

ParserPrivate::ParserPrivate() :
m_scanner(0)
	, m_negate(false)
	, m_error(false)
	, m_errorLine(0)
	, m_specialNumbersAllowed(false)
{
}

ParserPrivate::~ParserPrivate()
{
	delete m_scanner;
}

void ParserPrivate::setError(QString errorMsg, int errorLine) {
	m_error = true;
	m_errorMsg = errorMsg;
	m_errorLine = errorLine;
}

Parser::Parser() :
d(new ParserPrivate)
{
}

Parser::~Parser()
{
	delete d;
}

QVariant Parser::parse (QIODevice* io, bool* ok)
{
	d->m_errorMsg.clear();
	delete d->m_scanner;
	d->m_scanner = 0;

	if (!io->isOpen()) {
		if (!io->open(QIODevice::ReadOnly)) {
			if (ok != 0)
				*ok = false;
			qCritical ("Error opening device");
			return QVariant();
		}
	}

	if (!io->isReadable()) {
		if (ok != 0)
			*ok = false;
		qCritical ("Device is not readable");
		io->close();
		return QVariant();
	}

	d->m_scanner = new JSonScanner (io);
	d->m_scanner->allowSpecialNumbers(d->m_specialNumbersAllowed);
	yy::json_parser parser(d);
	parser.parse();

	delete d->m_scanner;
	d->m_scanner = 0;

	if (ok != 0)
		*ok = !d->m_error;

	io->close();
	return d->m_result;
}

QVariant Parser::parse(const QByteArray& jsonString, bool* ok) {
	QBuffer buffer;
	buffer.open(QBuffer::ReadWrite);
	buffer.write(jsonString);
	buffer.seek(0);
	return parse (&buffer, ok);
}

QString Parser::errorString() const
{
	return d->m_errorMsg;
}

int Parser::errorLine() const
{
	return d->m_errorLine;
}

void QJson::Parser::allowSpecialNumbers(bool allowSpecialNumbers) {
	d->m_specialNumbersAllowed = allowSpecialNumbers;
}

bool Parser::specialNumbersAllowed() const {
	return d->m_specialNumbersAllowed;
}
