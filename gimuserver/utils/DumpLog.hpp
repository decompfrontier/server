#pragma once

namespace DumpLogInternal
{
	/*!
	* Checks if the passed type is a number.
	*/
	template <typename T>
	concept numeric = std::is_arithmetic<T>::value;

	/*!
	* Checks if the passed type is a string.
	*/
	template <typename T>
	concept string = std::is_convertible_v<T, std::string_view>;
}

/*!
* Simple dump logger.
*/
class DumpLog final
{
public:
	/*!
	* Constructs a new DumpLog object.
	*/
	DumpLog() : m_open(false), m_log() {}

	/*!
	* Default deleted specialization as only custom specializations are supported.
	* @param t Type to print
	* @return Pointer to self
	*/
	template <typename T>
	constexpr DumpLog& operator<<(const T& t) = delete; // only specialization approved

	/*!
	* Prints a string.
	* @param str String to print
	* @return Updated reference to self
	*/
	template <DumpLogInternal::string T>
	constexpr DumpLog& operator<<(const T& str)
	{
		std::string_view v(str);

		if (isOpen()) {
			m_log.output(v.data(), v.length());
		}

		return *this;
	}

	/*!
	* Prints a number.
	* @param p Number to print
	* @return Updated reference to self
	*/
	template <DumpLogInternal::numeric T>
	constexpr DumpLog& operator<<(const T& p)
	{
		auto x = std::to_string(p);
		return operator<<(x);
	}

	/*!
	* Prints a drogon string map.
	* @param map Drogon string map
	* @return Updated reference to self
	*/
	template <typename T>
	constexpr DumpLog& operator<<(const drogon::SafeStringMap<T>& map)
	{
		for (const auto& [k, v] : map)
		{
			(*this) << "key=" << k << ",value=" << v << "\n";
		}

		return (*this);
	}

	/*!
	* Prints an HTTP request.
	* @param rq HTTP request
	* @return Updated reference to self
	*/
	template <>
	constexpr DumpLog& operator<< <drogon::HttpRequestPtr>(const drogon::HttpRequestPtr& rq)
	{
		return (*this) << "request dump: " << rq->getPath() << " (" << rq->getMethodString() << ")\n"
			<< "Cookies:\n" << rq->getCookies()
			<< "Parameters:\n" << rq->getParameters()
			<< "Headers:\n" << rq->getHeaders()
			<< "Body:\n" << rq->getBody()
			;
		return *this;
	}

	/*!
	* Opens the file and start logging.
	* @param[in] fileName Base name of the file
	* @param[in] path Path where the file should be stored
	*/
	void open(const std::string& fileName, const std::string& path)
	{
		m_log.setFileName(fileName, ".log", path);
		m_log.startLogging();
		m_open = true;
	}

	/*!
	* Checks if the file is opened.
	* @return true if the file is opened
	*/
	constexpr bool isOpen() const {
		return m_open;
	}


private:
	/*!
	* Checks if the file is opened.
	*/
	bool m_open;

	/*!
	* Trantor file logger.
	*/
	trantor::AsyncFileLogger m_log;
};
