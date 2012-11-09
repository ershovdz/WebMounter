#ifndef BISON_POSITION_HH
# define BISON_POSITION_HH

# include <iostream>
# include <string>

namespace yy
{
	/// Abstract a position.
	class position
	{
	public:

		/// Construct a position.
		position ()
			: filename (0), line (1), column (0)
		{
		}


		/// Initialization.
		inline void initialize (std::string* fn)
		{
			filename = fn;
			line = 1;
			column = 0;
		}

		/** \name Line and Column related manipulators
		** \{ */
	public:
		/// (line related) Advance to the COUNT next lines.
		inline void lines (int count = 1)
		{
			column = 0;
			line += count;
		}

		/// (column related) Advance to the COUNT next columns.
		inline void columns (int count = 1)
		{
			int leftmost = 0;
			int current  = column;
			if (leftmost <= current + count)
				column += count;
			else
				column = 0;
		}
		/** \} */

	public:
		/// File name to which this position refers.
		std::string* filename;
		/// Current line number.
		unsigned int line;
		/// Current column number.
		unsigned int column;
	};

	/// Add and assign a position.
	inline const position&
		operator+= (position& res, const int width)
	{
		res.columns (width);
		return res;
	}

	/// Add two position objects.
	inline const position
		operator+ (const position& begin, const int width)
	{
		position res = begin;
		return res += width;
	}

	/// Add and assign a position.
	inline const position&
		operator-= (position& res, const int width)
	{
		return res += -width;
	}

	/// Add two position objects.
	inline const position
		operator- (const position& begin, const int width)
	{
		return begin + -width;
	}

	/** \brief Intercept output stream redirection.
	** \param ostr the destination output stream
	** \param pos a reference to the position to redirect
	*/
	inline std::ostream&
		operator<< (std::ostream& ostr, const position& pos)
	{
		if (pos.filename)
			ostr << *pos.filename << ':';
		return ostr << pos.line << '.' << pos.column;
	}

}
#endif // not BISON_POSITION_HH
