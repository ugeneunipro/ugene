// -*- Mode: C++; tab-width: 2; -*-
// vi: set ts=2:
//
   
#ifndef BALL_COMMON_EXCEPTION_H
#define BALL_COMMON_EXCEPTION_H

#ifndef BALL_CONFIG_CONFIG_H
#	include <BALL/CONFIG/config.h>
#endif

#ifndef BALL_COMMON_GLOBAL_H
#	include <BALL/COMMON/global.h>
#endif

#ifdef BALL_COMPILER_MSVC
	#pragma warning(push)
	#pragma warning( disable : 4251 ) //disable needs to have dll-interface to be used by clients of class 'std::string'
#endif

#include <new>
#include <string>

namespace BALL 
{

	class String;

	/** Exception
	 		\ingroup Common	
	 */
	namespace Exception 
	{
		
		/**	General exception class.
				This class is intended as a base class for all other exceptions.
				Each exception class should define a constructor taking a string
				and an int as parameters. These two values are interpreted as the
				current filename and line number and is usually printed in case of
				an uncaught exception.  To support this feature, each <b>throw</b>
				directive should look as follows:  \par
				<tt><b>throw Exception::GeneralException</b>(__FILE__, __LINE__);</tt> \par
				<tt>__FILE__</tt> and <tt>__LINE__</tt> are built-in preprocessor
				macros that hold the desired information.
				 \par
				BALL provides its own  \link BALL::Exception::GlobalExceptionHandler::terminate terminate \endlink  handler. This handler
				extracts as much information as possible from the exception, prints
				it to <tt>cerr</tt> and  \link BALL::LogStream Log \endlink , and finally calls exits the program
				cleanly (with exit code 1).  This can be rather inconvenient for
				debugging, since you are told where the exception was thrown, but
				in general you do not know anything about the context.  Therefore
				<tt>terminate</tt> can also create a core dump. Using a debugger (e.g.
				dbx or gdb) you can then create a stack traceback.  To create a
				core dump, you should set the environment variable <tt>
				BALL_DUMP_CORE</tt> to any (non empty) value.
		\ingroup Common
		*/
		
		class BALL_EXPORT GeneralException 
			:	public std::exception
		{
			public:

			/**	@name	Constructors and Destructors
			*/
			//@{

			/// Default constructor
			GeneralException();
			
			/// Constructor
			GeneralException(const char* file, int line);

			/// Constructor
			GeneralException
				(const char* file, int line,
				 const String& name , const String& message);

			/// Copy constructor
			GeneralException(const GeneralException& exception);

			/// Destructor
			virtual ~GeneralException() throw();
			//@}

			/**	@name	Accessors
			*/
			//@{
	
			///	Returns the name of the exception 
			const char* getName() const;

			///	Returns the error message of the exception
			const char* getMessage() const;

			/// Modify the exception's error message
			void setMessage(const std::string& message);

			/// Returns the line number where it occured
			int getLine() const;
	
			/// Returns the file where it occured
			const char* getFile() const;
			//@}

			protected:
			const char*	file_;
			int					line_;

			std::string name_;
			std::string message_;
		};		

		/**	Index underflow.
				Throw this exception to indicate an index that was smaller than
				allowed.  The constructor has two additional arguments, the values
				of which should be set to the index that caused the failure and the
				smallest allowed value to simplify debugging.
				@param	index the value of the index causing the problem
				@param	size	smallest value allowed for index
		*/
		class BALL_EXPORT IndexUnderflow 
			: public GeneralException
		{
			public:

			IndexUnderflow(const char* file, int line, Index index = 0, Size size = 0);


			protected:

			Size size_;
			Index index_;
		};

		/**	Size underflow.
				Throw this exception to indicate a size was smaller than allowed.
				The constructor has an additional argument: the value of of the
				requested size.  This exception is thrown, if buffer sizes are
				insufficient.
				@param	size the size causing the problem
		*/
		class BALL_EXPORT SizeUnderflow 
			: public GeneralException
		{
			public:

			SizeUnderflow(const char* file, int line, Size size = 0);

			protected:
			Size size_;
		};

		/**	Index overflow.
				Throw this exception to indicate an index that was larger than
				allowed.  The constructor has two additional arguments, the values
				of which should be set to the index that caused the failure and the
				largest allowed value to simplify debugging.
				@param	index the value of the index causing the problem
				@param	size	largest value allowed for index
		*/
		class BALL_EXPORT IndexOverflow 
			: public GeneralException
		{
			public:
			IndexOverflow(const char* file, int line, Index index = 0, Size size = 0);

			protected:

			Size size_;
			Index index_;
		};

		/** Invalid Argument
		 * Use this exception when a function is called with an invalid argument and no other exception applies.
		 */
		class BALL_EXPORT InvalidArgument
			: public GeneralException
		{
			public:
				InvalidArgument(const char* file, int line, const String& arg);
		};

		/**	Invalid range.
				Use this exception to indicate a general range problems.
		*/
		class BALL_EXPORT InvalidRange 
			: public GeneralException
		{
			public:
			InvalidRange(const char* file, int line, float value);
		};


		/**	Invalid Size
				Throw this exception to indicate that a size was unexpected.
				The constructor has an additional argument: the value of of the
				requested size. 
				@param	size the size causing the problem
		*/
		class BALL_EXPORT InvalidSize 
			: public GeneralException
		{
			public:

			InvalidSize(const char* file, int line, Size size = 0);

			protected:
			Size size_;
		};


		/**	Out of range.
				Use this exception to indicate that a given value is out of a
				defined range, i. e. not within the domain of a function.
		*/
		class BALL_EXPORT OutOfRange 
			: public GeneralException
		{
			public:
			OutOfRange(const char* file, int line);
		};

		/**	Invalid format.
				This exception indicates a conversion problem when converting from
				one type to another. It is thrown, if a conversion from ascii to
				numeric formats or vice versa failed.
		*/
		class BALL_EXPORT InvalidFormat 
			: public GeneralException
		{
			public:
			InvalidFormat(const char* file, int line, const String& s);
			
			~InvalidFormat() throw();

			protected:

			std::string format_;
		};

		/**	Illegal self operation.	
				Throw this excpetion to indicate an invalid operation on the object
				itself. In general these operations are self assignments or related
				methods.
		*/
		class BALL_EXPORT IllegalSelfOperation
			: public GeneralException
		{
			public:
			IllegalSelfOperation(const char* file, int line);
		};

		/**	Null pointer argument is invalid.
				Use this exception to indicate a failure due to an argument not
				containing a pointer to a valid object, but a null pointer.
		*/
		class BALL_EXPORT NullPointer 
			: public GeneralException
		{
			public:
			NullPointer(const char* file, int line);
		};

		/**	Invalid iterator.
				The iterator on which an operation should be performed was invalid.
		*/
		class BALL_EXPORT InvalidIterator
			: public GeneralException
		{
			public:
			InvalidIterator(const char* file, int line);
		};

		/**	Incompatible iterator.
				The iterators could not be assigned because they are bound to
				different containers.
		*/
		class BALL_EXPORT IncompatibleIterators
			: public GeneralException
		{
			public:
			IncompatibleIterators(const char* file, int line);
		};

		/**	Not implemented exception. 
				This exception should be thrown to indicate not yet inplemented
				methods.  If you take the time to use the detailed constructor
				instead of the default constructor, identification of the concerned
				source will get <b>  much </b> easier!
		*/
		class BALL_EXPORT NotImplemented
			: public GeneralException
		{
			public:
			NotImplemented(const char* file, int line);
		};

		/** Illegal tree operation.
				This exception is thrown to indicate that an illegal tree operation
				i.e. node->setLeftChild(node) was requested.
		*/
		class BALL_EXPORT IllegalTreeOperation
			: public GeneralException
		{
			public:
			IllegalTreeOperation(const char* file, int line);
		};

		/**	Out of memory.
				Throw this exception to indicate that an allocation failed.
				This exception is thrown in the BALL new handler.
				@param	size	the number of bytes that should have been allocated
				@see GlobalException::newHandler
		*/
		class BALL_EXPORT OutOfMemory
			: public GeneralException, public std::bad_alloc
		{
			public:
			OutOfMemory(const char* file, int line, Size size = 0);
			
			virtual ~OutOfMemory() 
				throw();

			protected:
			Size size_;
		};

		/**	Buffer overflow exception.	
		*/
		class BALL_EXPORT BufferOverflow 
			: public GeneralException
		{
			public:
			BufferOverflow(const char* file, int line);
		};

		/**	Division by zero error.
		*/
		class BALL_EXPORT DivisionByZero 
			: public GeneralException
		{
			public:
			DivisionByZero(const char* file, int line);
		};

		/**	Out of grid error.
		*/
		class BALL_EXPORT OutOfGrid 
			: public GeneralException
		{
			public:
			OutOfGrid(const char* file, int line);
		};

		/**	File not found.
				A given file could not be found.
		*/
		class BALL_EXPORT FileNotFound 
			: public GeneralException
		{
			public:
			FileNotFound(const char* file, int line, const String& filename);

			~FileNotFound()
				throw();
			String getFilename() const;

			protected:
			std::string filename_;
		};

		/**	Invalid Position.
				A given position in three dimensional is invalid.
		*/
		class BALL_EXPORT IllegalPosition 
			: public GeneralException
		{
			public:
			IllegalPosition(const char* file, int line, float x, float y, float z);
		};

		/**	Parse Error.
				A given expression could not be parsed.
		*/
		class BALL_EXPORT ParseError
			: public GeneralException
		{
			public:
			///
			ParseError(const char* file, int line, const String& expression,
					const String& message);
		};

		/**	Precondition failed.
				A precondition (as defined by BALL_PRECONDITION_EXCEPTION) has failed.
		*/
		class BALL_EXPORT Precondition
			: public GeneralException
		{
			public:
			///
			Precondition(const char* file, int line, const char* condition);
		};

		/**	Postcondition failed.
				A postcondition (as defined by BALL_POSTCONDITION_EXCEPTION) has failed.
		*/
		class BALL_EXPORT Postcondition
			: public GeneralException
		{
			public:
			///
			Postcondition(const char* file, int line, const char* condition);
		};

		/// Exception to be thrown if an invalid option is given
		class BALL_EXPORT InvalidOption: public Exception::GeneralException
		{
			public:

				///
				InvalidOption(const char* file, int line, String option);
		};
		
		/// Exception to be thrown if too many errors occur, e.g. in ForceField
		class BALL_EXPORT TooManyErrors
			: public Exception::GeneralException
		{
			public:
			///
			TooManyErrors(const char* file, int line);
		};
		 
		/// Exception to be thrown if too many bonds for one atom
		class BALL_EXPORT TooManyBonds
			: public Exception::GeneralException
		{
			public:
			///
			TooManyBonds(const char* file, int line, const String& error);
		};
		
		/**
		* RenderTarget error
		* A buffer was requested from \link RenderTarget \endlink but could not be supplied
		* for some reason at the given moment.
		* Additional parameter <tt>reason</tt> gives the reason why the buffer could not be 
		* provided.
		*/
		class BALL_EXPORT NoBufferAvailable 
			: public Exception::GeneralException
		{
		public:
			NoBufferAvailable(const char* file, int line, const String& reason);
		};

		/**
		 * BufferedRenderer error
		 * An unsuported format was requested from \link BufferedRenderer \endlink via
		 * <tt>setFrameBufferFormat</tt>.
		 */
		class BALL_EXPORT FormatUnsupported
			: public Exception::GeneralException
		{
		public:
			FormatUnsupported(const char* file, int line);
		};

		/**
		 * NotInitialized Exception
		 *
		 * Objects may throw this exception if they have not been sufficiently initialized.
		 */
		class BALL_EXPORT NotInitialized : public Exception::GeneralException
		{
			public:
				NotInitialized(const char* file, int line, const String& reason);
		};

		/** Class handling uncaught exception globally.
		*/
		class BALL_EXPORT GlobalExceptionHandler
		{
			public:
			/**	@name	Constructors
			*/
			//@{

			/**	Default constructor.
					This constructor installs the BALL specific handlers for
					<tt>terminate</tt>, <tt>unexpected</tt>, and <tt>new_handler</tt>.
					<tt>terminate</tt> or <tt>unexpected</tt> are called to abort 
					a program if an exception was not caught or a function 
					exits via an exception that is not allowed by its exception
					specification. Both functions are replaced by a function of 
					GlobalExceptionHandler that tries to determine the 
					last exception thrown. This mechanism only works, if all 
					exceptions are defrived from  \link GeneralException GeneralException \endlink . \par

					The default <tt>new_handler</tt> is replaced by  \link newHandler newHandler \endlink 
					and throws an exception of type  \link OutOfMemory OutOfMemory \endlink  instead of 
					<tt>bad_alloc</tt> (the default behaviour defined in the ANSI C++ 
					standard).
			*/
			GlobalExceptionHandler();
			//@}
			
			/**	@name	Accessors
			*/
			//@{
				
			/// Assign the name of the exception. This should agree with the class name.
			static void setName(const String& name);
				
			/// Set the error message
			static void setMessage(const String& message);

			/// Set the line number the exception was thrown. Should be set to __LINE__ in most cases.
			static void setLine(int line);

			/// The source file name where the exception was thrown
			static void setFile(const String& file);

			/// Set all exception attributes
			static void set
				(const String& file, int line, 
				 const String& name, const String& message);
			//@}	
			
			protected:

			/// The BALL replacement for terminate
			static void terminate();

			/// The BALL new handler
			static void newHandler() throw(Exception::OutOfMemory);

			static std::string file_;
			static int				 line_;
			static std::string name_;
			static std::string message_;
		};

		/**	Global static instance of GlobalExceptionHandler
		*/
		BALL_EXPORT extern GlobalExceptionHandler globalHandler;

	}
		/**	Output operator for exceptions.
				All BALL exceptions can be printed to an arbitrary output stream.
				Information written contains the exception class, the error message,
        and the location (file, line number). The following code block
        can thus be used to catch any BALL exceptions and convert them to
        human readable information:
				\verbatim
				try
				{
					.... // some code which potentially throws an exception
				}
				catch (Exception::GeneralException& e)
				{
					Log.error() << "caught exception: " << e << std::endl;
				}
				\endverbatim
				 \ingroup Common
		*/
		BALL_EXPORT
		std::ostream& operator << (std::ostream& os, const Exception::GeneralException& e);
	
} // namespace BALL

#ifdef BALL_COMPILER_MSVC
	#pragma warning(pop)
#endif

#endif // BALL_COMMON_EXCEPTION_H
