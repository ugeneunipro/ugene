// -*- Mode: C++; tab-width: 2; -*-
// vi: set ts=2:
//
// $Id: logStream.C,v 1.42.18.1 2007/03/25 22:00:03 oliver Exp $
//

#include <limits>
#include <string>
#include <cstring>
#include <cstdio>

#include <BALL/COMMON/logStream.h>

#define BUFFER_LENGTH 32768

#ifdef BALL_HAS_ANSI_IOSTREAM
#	define BALL_IOS std::basic_ios<char>
#	define BALL_OSTREAM std::ostream
#else
#	define BALL_IOS std::ios
#	define BALL_OSTREAM std::ostream
#endif

namespace BALL 
{
	// at this point, it is not yet possible to
	// include BALL/COMMON/limits.h (which were a 
	// much nicer solution...). Ugly header dependencies...
	const int LogStreamBuf::MIN_LEVEL = std::numeric_limits<int>::min();
	const int LogStreamBuf::MAX_LEVEL = std::numeric_limits<int>::max();
	const Time LogStreamBuf::MAX_TIME = std::numeric_limits<Time>::max();

	LogStreamBuf::LogStreamBuf() 
		: std::streambuf(),
			pbuf_(0),
			loglines_(),
			level_(0),
			tmp_level_(0),
			stream_list_(),
			incomplete_line_()
	{
		pbuf_ = new char [BUFFER_LENGTH];
		std::streambuf::setp(pbuf_, pbuf_ + BUFFER_LENGTH - 1);
	}
		
	LogStreamBuf::~LogStreamBuf() 
	{
		sync();

		delete [] pbuf_;
	}

	void LogStreamBuf::dump(std::ostream& stream) 
	{
		char buf[BUFFER_LENGTH];
		Size line;
		for (line = (Size)loglines_.size(); line > 0; --line) 
		{
			strftime(&(buf[0]), BUFFER_LENGTH - 1, "%d.%m.%Y %H:%M:%S ", localtime(&(loglines_[line - 1].time)));
			stream << buf << "[" << loglines_[line - 1].level
						 << "]:" << loglines_[line - 1].text.c_str() << std::endl;
		}
	}

	int LogStreamBuf::sync()
	{
		return sync(false);
	}
 
	int LogStreamBuf::sync(bool force_flush)
	{
		static char buf[BUFFER_LENGTH];

		// sync our streambuffer...
		if (pptr() != pbase()) 
		{
				
			char*	line_start = pbase();
			char*	line_end = pbase();

			while (line_end < pptr())
			{
				// search for the first end of line
				for (; line_end < pptr() && *line_end != '\n'; line_end++) {};

				if (!force_flush && line_end >= pptr())
				{
					// Copy the incomplete line to the incomplete_line_ buffer
					size_t length = line_end - line_start + 1;
					length = std::min(length, (size_t)(BUFFER_LENGTH - 1));
					strncpy(&(buf[0]), line_start, length);

					// if length was too large, we copied one byte less than BUFFER_LENGTH to have
					// room for the final \0
					buf[length] = '\0';

					incomplete_line_ += &(buf[0]);

					// mark everything as read
					line_end = pptr() + 1;
				} 
				else 
				{
					// note: pptr() - pbase() should be bounded by BUFFER_LENGTH, so this should always work
					memcpy(&(buf[0]), line_start, line_end - line_start + 1);
					buf[line_end - line_start] = '\0';
						
					// assemble the string to be written
					// (consider leftovers of the last buffer from incomplete_line_)
					string outstring = incomplete_line_;
					incomplete_line_ = "";
					outstring += &(buf[0]);

					// if there are any streams in our list, we
					// copy the line into that streams, too and flush them
					std::list<StreamStruct>::iterator list_it = stream_list_.begin();
					for (; list_it != stream_list_.end(); ++list_it)
					{
						// if the stream is open for that level, write to it...
						if ((list_it->min_level <= tmp_level_) && (list_it->max_level >= tmp_level_) && !list_it->disabled)
						{
							*(list_it->stream) << expandPrefix_(list_it->prefix, tmp_level_, time(0)).c_str()
																 << outstring.c_str() << std::endl;
							if (list_it->target != 0)
							{
								list_it->target->logNotify();
							}
						}
					}
			
					// update the line pointers (increment both)
					line_start = ++line_end;
					
					// remove cr/lf from the end of the line				
					while (outstring.size() && (outstring[outstring.size() - 1] == 10 || outstring[outstring.size() - 1] == 13))
					{
						std::string::iterator p = outstring.end();
						p--;
						outstring.erase(p);
					}
		
					// store the line 
					Logline	logline;

					logline.text = outstring;
					logline.level = tmp_level_;
					logline.time = time(0);
			
					// store the new line
					loglines_.push_back(logline);

					// reset tmp_level_ to the previous level
					// (needed for LogStream::level() only)
					tmp_level_ = level_;
				}
			}

			// remove all processed lines from the buffer
			pbump((int)(pbase() - pptr()));
		}
	
		return 0;
	}

	string LogStreamBuf::expandPrefix_
		(const string& prefix, int level, Time time) const
	{
		string::size_type	index = 0;
		Size copied_index = 0;
		string result("");

		while ((index = prefix.find("%", index)) != string::npos) 
		{
			// append any constant parts of the string to the result
			if (copied_index < index) 
			{
				result.append(prefix.substr(copied_index, index - copied_index));
				copied_index = (Index)index;
			}
			
			if (index < prefix.size()) 
			{
				char	buffer[64];
				char*	buf = &(buffer[0]);

				switch (prefix[index + 1]) 
				{
					case '%': // append a '%' (escape sequence)
						result.append("%");
						break;

					case 'l': // append the loglevel
						sprintf(buf, "%d", level);
						result.append(buf);
						break;

					case 'y':	// append the message type (error/warning/information)
						if (level >= LogStream::ERROR_LEVEL) 
						{
							result.append("ERROR");
						}
						else 
						{
							if (level >= LogStream::WARNING_LEVEL) 
							{
								result.append("WARNING");
							}
							else 
							{
								if (level >= LogStream::INFORMATION_LEVEL) 
								{
									result.append("INFORMATION");
								}
								else 
								{
									result.append("LOG");
								}
							}
						}
						break;

					case 'T':	// time: HH:MM:SS
						strftime(buf, BUFFER_LENGTH - 1, "%H:%M:%S", localtime(&time));
						result.append(buf);
						break;

					case 't': // time: HH:MM	
						strftime(buf, BUFFER_LENGTH - 1, "%H:%M", localtime(&time));
						result.append(buf);
						break;

					case 'D':	// date: DD.MM.YYYY
						strftime(buf, BUFFER_LENGTH - 1, "%d.%m.%Y", localtime(&time));
						result.append(buf);
						break;

					case 'd':	// date: DD.MM.
						strftime(buf, BUFFER_LENGTH - 1, "%d.%m.", localtime(&time));
						result.append(buf);
						break;

					case 'S':	// time+date: DD.MM.YYYY, HH:MM:SS
						strftime(buf, BUFFER_LENGTH - 1, "%d.%m.%Y, %H:%M:%S", localtime(&time));
						result.append(buf);
						break;

					case 's':	// time+date: DD.MM., HH:MM
						strftime(buf, BUFFER_LENGTH - 1, "%d.%m., %H:%M", localtime(&time));
						result.append(buf);
						break;

					default:
						break;
				}
				index += 2;
				copied_index += 2;
			}
		}

		if (copied_index < prefix.size()) 
		{
			result.append(prefix.substr(copied_index, prefix.size() - copied_index));
		}

		return result;
	}

	LogStreamNotifier::LogStreamNotifier()
		: registered_at_(0)
	{
	}

	LogStreamNotifier::~LogStreamNotifier()
	{
		unregister();
	}

	void LogStreamNotifier::logNotify()
	{
	}

	void LogStreamNotifier::unregister()
	{
		if (registered_at_ == 0) return;

		registered_at_->remove(stream_);
		registered_at_ = 0;
	}

	void LogStreamNotifier::registerAt(LogStream& log, int min_level, int max_level)
	{
		unregister();

		registered_at_ = &log;
		log.insertNotification(stream_, *this, min_level, max_level);
	}

	// keep the given buffer	
	LogStream::LogStream(LogStreamBuf* buf, bool delete_buf, bool associate_stdio)
		: BALL_IOS(buf),
			BALL_OSTREAM(buf),
			delete_buffer_(delete_buf),
			disable_output_(false)
	{
		if (associate_stdio) 
		{
			// associate cout to informations and warnings,
			// cerr to errors by default
			insert(std::cout, INFORMATION_LEVEL, ERROR_LEVEL - 1);
			insert(std::cerr, ERROR_LEVEL);
		}
	}

	LogStream::~LogStream()
	{
		if (delete_buffer_)
		{
			// remove the streambuffer
			delete rdbuf();
		}
	}
	
	void LogStream::clear()
	{
		rdbuf()->loglines_.clear();
	}

	void LogStream::insert(std::ostream& stream, int min_level, int max_level) 
	{
		if (!bound_() || hasStream_(stream))
		{
			return;
		}
			
		// we didn`t find it - create a new entry in the list
		LogStreamBuf::StreamStruct s_struct;
		s_struct.min_level = min_level;
		s_struct.max_level = max_level;
		s_struct.disabled = false;
		s_struct.stream = &stream;
		rdbuf()->stream_list_.push_back(s_struct);
	}

	void LogStream::remove(std::ostream& stream) 
	{
		if (!bound_()) return;

		StreamIterator it = findStream_(stream);
		if (it != rdbuf()->stream_list_.end())
		{
			rdbuf()->stream_list_.erase(it);
		}
	}

	void LogStream::insertNotification(std::ostream& s, LogStreamNotifier& target,
																		int min_level, int max_level)
	{
		if (!bound_()) return;

		insert(s, min_level, max_level);

		StreamIterator it = findStream_(s);
		(*it).target = &target;
	}

	LogStream::StreamIterator LogStream::findStream_(const std::ostream& s)
	{
		StreamIterator list_it = rdbuf()->stream_list_.begin();
		for (; list_it != rdbuf()->stream_list_.end(); ++list_it)
		{
			if (list_it->stream == &s) 
			{
				return list_it;
			}
		}

		return list_it;
	}

	bool LogStream::hasStream_(std::ostream& stream)
	{
		if (!bound_()) return false;

		return findStream_(stream) != rdbuf()->stream_list_.end();
	}

	void LogStream::setMinLevel(const std::ostream& stream, int level) 
	{
		if (!bound_()) return;
			
		StreamIterator it = findStream_(stream);
		if (it != rdbuf()->stream_list_.end())
		{
			(*it).min_level = level;
		}
	}

	void LogStream::setMaxLevel(const std::ostream& stream, int level) 
	{
		if (!bound_()) return;
			
		StreamIterator it = findStream_(stream);
		if (it != rdbuf()->stream_list_.end())
		{
			(*it).max_level = level;
		}
	}
	
	void LogStream::setPrefix(const std::ostream& s, const string& prefix) 
	{
		if (!bound_()) return;

		StreamIterator it = findStream_(s);
		if (it != rdbuf()->stream_list_.end())
		{
			(*it).prefix = prefix;
		}		
	}
	
	Size LogStream::getNumberOfLines(int min_level, int max_level) const  
	{
		if (!bound_()) return 0;

		// iterate over all loglines and count the lines of interest
		auto non_const_this = const_cast<LogStream*>(this);
		vector<LogStreamBuf::Logline>::iterator	it = non_const_this->rdbuf()->loglines_.begin();
		Size	count = 0;

		for (; it != non_const_this->rdbuf()->loglines_.end(); ++it) 
		{
			if ((*it).level >= min_level && (*it).level <= max_level)
			{
				count++;
			}
		}

		return count;
	}

	string LogStream::getLineText(const Index& index) const
	{
		if ((signed)getNumberOfLines() < index)
		{
			return "";
		}

		if (!bound_()) return "";

		return const_cast<LogStream*>(this)->rdbuf()->loglines_[index].text;	
	}

	int LogStream::getLineLevel(const Index& index) const
	{
		if ((signed)getNumberOfLines() < index)
		{
			return -1;
		}

		if (!bound_()) return -1;

		return const_cast<LogStream*>(this)->rdbuf()->loglines_[index].level;	
	}


	time_t LogStream::getLineTime(const Index& index) const
	{
		if ((signed)getNumberOfLines() < index)
		{
			return 0;
		}

		if (!bound_()) return 0;

		return const_cast<LogStream*>(this)->rdbuf()->loglines_[index].time;	
	}

	std::list<int>	LogStream::filterLines
		(int min_level, int max_level,
		 Time earliest, Time latest, const string& s) const
	{
		std::list<int>	list_indices;
		Position pos = 0;
		auto log = const_cast<LogStream*>(this)->rdbuf();

		while (pos < log->loglines_.size() && 
					 log->loglines_[pos].time < earliest)
		{
			pos++;
		}
		while (pos < log->loglines_.size() && 
					 log->loglines_[pos].time <= latest)
		{
			if (log->loglines_[pos].level >= min_level &&
					log->loglines_[pos].level <= max_level)
			{
				if (s.length() > 0)
				{
					if (log->loglines_[pos].text.find(s, 0) != string::npos)
					{
						list_indices.push_back((int)pos);
					}
				}
				else
				{
					list_indices.push_back((int)pos);
				}
			}
			pos++;
		}
		return list_indices;
	}

	void LogStream::disableOutput()
		
	{
		disable_output_ = true;
		for(list<LogStreamBuf::StreamStruct>::iterator it=rdbuf()->stream_list_.begin(); it!=rdbuf()->stream_list_.end(); it++)
		{
			it->disabled = true;
		}
	}

	void LogStream::enableOutput()
		
	{
		disable_output_ = false;
		for(list<LogStreamBuf::StreamStruct>::iterator it=rdbuf()->stream_list_.begin(); it!=rdbuf()->stream_list_.end(); it++)
		{
			it->disabled = false;
		}
		std::ostream::flush();
	}

	bool LogStream::outputEnabled() const
		
	{
		return disable_output_;
	}

	std::ostream& LogStream::flush()
	{
		if (disable_output_) return *this;

		rdbuf()->sync(true);
		std::ostream::flush();
		return *this;
	}

	bool LogStream::bound_() const
	{
		auto non_const_this = const_cast<LogStream*>(this);

		return (non_const_this->rdbuf() != 0);
	}

	// global default logstream
	BALL_EXPORT		LogStream	Log(new LogStreamBuf, true, true);

#	ifdef	BALL_NO_INLINE_FUNCTIONS
#		include <BALL/COMMON/logStream.iC>
#	endif

} // namespace BALL
