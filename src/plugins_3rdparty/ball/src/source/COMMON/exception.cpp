#include <U2Core/disable-warnings.h>
U2_DISABLE_WARNINGS

// -*- Mode: C++; tab-width: 2; -*-
// vi: set ts=2:
//

#include <BALL/COMMON/exception.h>
#include <BALL/COMMON/logStream.h>
#include <BALL/DATATYPE/string.h>

#include <iostream>
#include <typeinfo>
#include <exception>
#include <cstdio>
#include <cstdlib>	// for getenv in terminate()
#include <sys/types.h>
#include <csignal> // for SIGSEGV and kill

#ifdef BALL_HAS_UNISTD_H
#	include <unistd.h> // fot getpid
#endif
#ifdef BALL_HAS_PROCESS_H
#	include <process.h>
#endif

#define BALL_CORE_DUMP_ENVNAME "BALL_DUMP_CORE"

#define DEF_EXCEPTION(a,b) \
	a :: a (const char* file, int line) \
		: GeneralException(file, line, #a, b)\
	{\
	}\
	

using std::string;
using std::endl;

namespace BALL 
{

	namespace Exception 
	{

			GeneralException::GeneralException() 
				:	file_("?"),
					line_(-1),
					name_("GeneralException"),
					message_("unspecified error")
			{
				globalHandler.set(file_, line_, String(name_), String(message_));
			}

			GeneralException::GeneralException(const char* file, int line, const String& name, const String& message) 
				:	file_(file),
					line_(line),
					name_(name),
					message_(message)
			{
				globalHandler.set(file_, line_, name_, message_);
			}

			GeneralException::GeneralException(const char* file, int line) 
				:	file_(file),
					line_(line),
					name_("GeneralException"),
					message_("unknown error")
			{
				globalHandler.set(file_, line_, name_, message_);
			}

			GeneralException::GeneralException(const GeneralException& exception)
				:	std::exception(exception),
					file_(exception.file_),
					line_(exception.line_),
					name_(exception.name_),
					message_(exception.message_)
			{
			}

			GeneralException::~GeneralException() throw()
			{
			}
		
			const char* GeneralException::getName() const
			{
				return name_.c_str();
			}

			const char* GeneralException::getMessage() const
			{
				return message_.c_str();
			}

			void GeneralException::setMessage(const std::string& message)
			{
				message_ = message;
			}

			const char* GeneralException::getFile() const
			{
				return file_;
			}
			
			int GeneralException::getLine() const
			{
				return line_;
			}

			IndexUnderflow::IndexUnderflow(const char* file, int line, Index index, Size size)
				: GeneralException(file, line, "IndexUnderflow", ""),
					size_(size),
					index_(index)
			{
				message_ = "the given index was too small: ";
				char buf[40];

				sprintf(buf, "%ld", (long)index);
				message_ += buf;
				message_ += " (size = ";

				sprintf(buf, "%ld", (long)size);
				message_ += buf;
				message_ += ")";

				globalHandler.setMessage(message_);
			}

			IndexOverflow::IndexOverflow(const char* file, int line, Index index, Size size)
				:	GeneralException(file, line, "IndexOverflow", "an index was too large"),
					size_(size),
					index_(index)
			{
				message_ = "the given index was too large: ";
				char buf[40];

				sprintf(buf, "%ld", (long)index);
				message_ += buf;
				message_ += " (size = ";

				sprintf(buf, "%ld", (long)size);
				message_ += buf;
				message_ += ")";

				globalHandler.setMessage(message_);
			}

			OutOfMemory::OutOfMemory(const char* file, int line, Size size)
				:	GeneralException(file, line, "OutOfMemory", "a memory allocation failed"),
					size_(size)
			{
				message_ = "unable to allocate enough memory (size = ";
				char buf[40];

				sprintf(buf, "%ld", (long)size_);
				message_ += buf;
				message_ += " bytes) ";

				globalHandler.setMessage(message_);
			}

			OutOfMemory::~OutOfMemory() throw()
			{
			}

			SizeUnderflow::SizeUnderflow(const char* file, int line, Size size)
				:	GeneralException(file, line, "SizeUnderflow", ""),
					size_(size)
			{
				message_ = "the given size was too small: ";
				char buf[40];
				sprintf(buf, "%ld", (long)size);
				
				message_ += buf;
				globalHandler.setMessage(message_);
			}

			InvalidArgument::InvalidArgument(const char* file, int line, const String& arg)
				: GeneralException(file, line, "InvalidArgument",
				                   "An invalid argument has been passed: ")
			{
				message_ += arg;
			}

			InvalidSize::InvalidSize(const char* file, int line, Size size)
				:	GeneralException(file, line, "InvalidSize", ""),
					size_(size)
			{
				message_ = "the given size was not expected: ";
				char buf[40];
				sprintf(buf, "%ld", (long)size);
				
				message_ += buf;
				globalHandler.setMessage(message_);
			}

			IllegalPosition::IllegalPosition(const char* file, int line, float x, float y, float z)
				:	GeneralException(file, line, "IllegalPosition:", "")
			{
				char buf1[40];
				sprintf(buf1, "%f", x);
				char buf2[40];
				sprintf(buf2, "%f", y);
				char buf3[40];
				sprintf(buf3, "%f", z);

				message_ += "(";
				message_ += buf1;
				message_ += ",";
				message_ += buf2;
				message_ += ",";
				message_ += buf3;
				message_ += ")";
				globalHandler.setMessage(message_);
			}

			ParseError::ParseError(const char* file, int line, 
					const String& expression, const String& message)
				: GeneralException(file, line, "Parse Error", "")
			{

				message_ += message;
				message_ += " in ";
				message_ += expression;
				globalHandler.setMessage(message_);
			}

			Precondition::Precondition(const char* file, int line, const char* condition)
				: GeneralException(file, line, "Precondition failed", "")
			{
				message_ += std::string(condition);
				globalHandler.setMessage(message_);
			}

			Postcondition::Postcondition(const char* file, int line, const char* condition)
				: GeneralException(file, line, "Postcondition failed", "")
			{
				message_ += std::string(condition);
				globalHandler.setMessage(message_);
			}

			FileNotFound::FileNotFound(const char* file, int line, const String& filename)
				:	GeneralException(file, line, "FileNotFound", ""),
					filename_(filename)
			{
				message_ = "the file " + filename + " could not be found";
				globalHandler.setMessage(message_);
			}

			FileNotFound::~FileNotFound() throw()
			{
			}

			String FileNotFound::getFilename() const
			{
				return filename_;
			}

			InvalidFormat::InvalidFormat(const char* file, int line, const String& s)
				:	GeneralException(file, line, "InvalidFormat", ""),
					format_(s)
			{
				message_ = "problem converting '";
				message_.append(s + "' to a number.");

				globalHandler.setMessage(message_);
			}

			InvalidFormat::~InvalidFormat() throw()
			{
			}
		
			InvalidOption::InvalidOption(const char* file, int line, String option)
				: GeneralException(file, line, "Invalid option: ", option)
			{
			}

			TooManyErrors::TooManyErrors(const char* file, int line)
				: GeneralException(file, line, "Too many errors", "")
			{
			}

			TooManyBonds::TooManyBonds(const char* file, int line, const String& error)
				: GeneralException(file, line, 
						String("Unable to create additional bond between ") + error, "")
			{
			}


			InvalidRange::InvalidRange(const char* file, int line, float value)
				: GeneralException(file, line, "The argument was out of range: ", String(value))
			{
			}

			NoBufferAvailable::NoBufferAvailable(const char* file, int line, const String& reason)
				: GeneralException(file, line, "NoBufferAvailable", String("Unavailable because: ") + reason)
			{
			}

			NotInitialized::NotInitialized(const char* file, int line, const String& reason)
				: GeneralException(file, line, "NotInitialized", reason)
			{
			}

			DEF_EXCEPTION(OutOfRange, "the range of the operation was invalid")

			DEF_EXCEPTION(DivisionByZero, "a division by zero was requested")

			DEF_EXCEPTION(NullPointer, "a null pointer was specified")

			DEF_EXCEPTION(InvalidIterator, "the iterator is invalid - probably it is not bound to a container")

			DEF_EXCEPTION(IncompatibleIterators, "the iterator could not be assigned because it is bound to a different container")

			DEF_EXCEPTION(NotImplemented, "this method has not been implemented yet. Feel free to complain about it!")

			DEF_EXCEPTION(IllegalSelfOperation, "cannot perform operation on the same object")

			DEF_EXCEPTION(IllegalTreeOperation, "an illegal tree operation was requested")

			DEF_EXCEPTION(BufferOverflow, "the maximum buffersize has been reached")

			DEF_EXCEPTION(OutOfGrid, "a point was outside a grid")	

			DEF_EXCEPTION(FormatUnsupported, "given framebuffer format is not supported")

		
			GlobalExceptionHandler::GlobalExceptionHandler()
			{
//				std::set_terminate(terminate);
//				std::set_unexpected(terminate);
//				std::set_new_handler(newHandler);
			}

			void GlobalExceptionHandler::newHandler()
				throw(Exception::OutOfMemory)
			{
				throw Exception::OutOfMemory(__FILE__, __LINE__);
			}
				
			void GlobalExceptionHandler::terminate()
			{
				// add cerr to the log stream
				// and write all available information on
				// the exception to the log stream (potentially with an assigned file!)
				// and cerr

				Log.insert(std::cerr);
				Log.error() << endl;
				Log.error() << "---------------------------------------------------" << endl;
				Log.error() << "FATAL: uncaught exception!" << endl;
				Log.error() << "---------------------------------------------------" << endl;
				if ((line_ != -1) && (name_ != "unknown"))
				{
					Log.error() << "last entry in the exception handler: " << endl;
					Log.error() << "exception of type " << name_.c_str() << " occurred in line " 
											<< line_ << " of " << file_.c_str() << endl;
					Log.error() << "error message: " << message_.c_str() << endl;
				}
				Log.error() << "---------------------------------------------------" << endl;

				// if the environment variable declared in BALL_CORE_DUMP_ENVNAME
				// is set, provoke a core dump (this is helpful to get s stack traceback)
				if (getenv(BALL_CORE_DUMP_ENVNAME) != 0)
				{
					#ifdef BALL_HAS_KILL
						Log.error() << "dumping core file.... (to avoid this, unset " << BALL_CORE_DUMP_ENVNAME 
									<< " in your environment)" << endl;
						// provoke a core dump 
						kill(getpid(), SIGSEGV);
					#endif
				}

				// otherwise exit cleanly
				exit(1);
			}
				
			void GlobalExceptionHandler::set
				(const String& file, int line,
				 const String& name, const String& message)
			{
				name_ = name;
				line_ = line;
				message_ = message;
				file_ = file;
			}
			
			void GlobalExceptionHandler::setName(const String& name)
			{
				name_ = name;
			}
			
			void GlobalExceptionHandler::setMessage(const String& message)
			{
				message_ = message;
			}
			
			void GlobalExceptionHandler::setFile(const String& file)
			{
				file_ = file;
			}
			
			void GlobalExceptionHandler::setLine(int line) 
			{
				line_ = line;
			}

			std::string	GlobalExceptionHandler::name_			= "unknown exception";
			int					GlobalExceptionHandler::line_			= -1;
			std::string	GlobalExceptionHandler::message_	= " - ";
			std::string	GlobalExceptionHandler::file_			= "unknown";



			// create a global instance of the exception handler
			BALL_EXPORT GlobalExceptionHandler globalHandler;

	} // namespace Exception

	std::ostream& operator << (std::ostream& os, const Exception::GeneralException& e)
	{
		os << e.getName() << " @ " << e.getFile() << ":" << e.getLine() << ": " << e.getMessage();

		return os;
	}


} // namespace BALL
