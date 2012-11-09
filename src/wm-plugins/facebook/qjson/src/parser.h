#ifndef QJSON_PARSER_H
#define QJSON_PARSER_H

#include "qjson_export.h"

class QIODevice;
class QVariant;

namespace QJson {

	class ParserPrivate;

	/**
	* @brief Main class used to convert JSON data to QVariant objects
	*/
	class QJSON_EXPORT Parser
	{
	public:
		Parser();
		~Parser();

		/**
		* Read JSON string from the I/O Device and converts it to a QVariant object
		* @param io Input output device
		* @param ok if a conversion error occurs, *ok is set to false; otherwise *ok is set to true.
		* @returns a QVariant object generated from the JSON string
		*/
		QVariant parse(QIODevice* io, bool* ok = 0);

		/**
		* This is a method provided for convenience.
		* @param jsonData data containing the JSON object representation
		* @param ok if a conversion error occurs, *ok is set to false; otherwise *ok is set to true.
		* @returns a QVariant object generated from the JSON string
		* @sa errorString
		* @sa errorLine
		*/
		QVariant parse(const QByteArray& jsonData, bool* ok = 0);

		/**
		* This method returns the error message
		* @returns a QString object containing the error message of the last parse operation
		* @sa errorLine
		*/
		QString errorString() const;

		/**
		* This method returns line number where the error occurred
		* @returns the line number where the error occurred
		* @sa errorString
		*/
		int errorLine() const;

		/**
		* Sets whether special numbers (Infinity, -Infinity, NaN) are allowed as an extension to
		* the standard
		* @param  allowSpecialNumbers new value of whether special numbers are allowed
		* @sa specialNumbersAllowed
		*/
		void allowSpecialNumbers(bool allowSpecialNumbers);

		/**
		* @returns whether special numbers (Infinity, -Infinity, NaN) are allowed
		* @sa allowSpecialNumbers
		*/
		bool specialNumbersAllowed() const;

	private:
		Q_DISABLE_COPY(Parser)
			ParserPrivate* const d;
	};
}

#endif // QJSON_PARSER_H
