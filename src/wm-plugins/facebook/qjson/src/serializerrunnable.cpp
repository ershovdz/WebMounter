#include "serializerrunnable.h"
#include "parserrunnable.h"
#include "serializer.h"

#include <QtCore/QDebug>
#include <QtCore/QVariant>

using namespace QJson;

class SerializerRunnable::Private
{
public:
	QVariant json;
};

SerializerRunnable::SerializerRunnable(QObject* parent)
	: QObject(parent),
	QRunnable(),
	d(new Private)
{
	qRegisterMetaType<QVariant>("QVariant");
}

SerializerRunnable::~SerializerRunnable()
{
	delete d;
}

void SerializerRunnable::setJsonObject( const QVariant& json )
{
	d->json = json;
}

void SerializerRunnable::run()
{
	Serializer serializer;
	bool ok;
	const QByteArray serialized = serializer.serialize( d->json, &ok);
	emit parsingFinished( serialized, ok, serializer.errorMessage() );
}
