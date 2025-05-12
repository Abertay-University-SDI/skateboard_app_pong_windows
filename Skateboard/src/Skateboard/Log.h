#pragma once

#include "Skateboard/Utilities/StringConverters.h"
#include "Flossy.hpp"

//#define SKTBD_LOG_COMPONENT "Log"

#ifndef SKTBD_LOG_COMPONENT
#define  SKTBD_LOG_COMPONENT ""
#endif // SKTBD_LOG_COMPONENT

//#ifdef SKTBD_PLATFORM_PLAYSTATION
	//	#define SCE_DBG_LOG_COMPONENT SKTBD_LOG_COMPONENT
	//#endif // SKTBD_PLATFORM_PLAYSTATION

//void A()
//{
//	auto B = flossy::format(L"string");
//
//	static_assert(std::is_convertible_v<decltype(B), std::wstring>);
//}

namespace Skateboard::Log
{
	enum LogSeverity : uint8_t
	{
		Trace,
		Debug,
		Info,
		Warn,
		Error,
		Critical
	};

	class Logger
	{
	public:
		virtual ~Logger() = default;
		static void RegisterLogger(const std::shared_ptr<Logger>& PlatformLogger)
		{
			m_Logger = PlatformLogger;
			LogMessage(Trace, "Starting up Log");
		}

		template<typename CharTString>
		static void LogMessage(const LogSeverity& severity, const std::string& Component, CharTString Message)
		{
			if constexpr (std::is_constructible_v<std::string, CharTString>)
			{
				m_Logger->LogMessage_(severity, Component, Message);
			}
			else if constexpr (std::is_constructible_v<std::wstring, CharTString>)
			{
				m_Logger->LogMessage_(severity, Component, ToString(Message));
			} else 
			{
				//Logger doth not know the type of string you are trying to shove in it
				//static_assert(false);
			}
		}

		template<typename CharTString>
		static void LogMessage(const LogSeverity& severity, CharTString Message)
		{
			if constexpr (std::is_convertible_v<decltype(Message), std::string>)
			{
				m_Logger->LogMessage_(severity, "Generic", Message);
			}
			else if constexpr (std::is_convertible_v<decltype(Message),std::wstring>)
			{
				m_Logger->LogMessage_(severity, "Generic", ToString(std::wstring(Message)));
			}
			else
			{
				//Logger doth not know the type of string you are trying to shove in
				//static_assert(false);
			}
		}
		
	protected:
		virtual void LogMessage_(const LogSeverity& severity, const std::string& Component, const std::string& Message) = 0;
		virtual void SetOutputSeverity_(const LogSeverity& severity) = 0;

		static std::shared_ptr<Logger> m_Logger;
	};

}

#if defined(SKTBD_SHIP)
// Core log macros
#define SKTBD_CORE_CRITICAL(...)
#define SKTBD_CORE_ERROR(...)
#define SKTBD_CORE_WARN(...)
#define SKTBD_CORE_INFO(...)
#define SKTBD_CORE_TRACE(...)

// Client log macros
#define SKTBD_APP_CRITICAL(...)
#define SKTBD_APP_ERROR(...)
#define SKTBD_APP_WARN(...)
#define SKTBD_APP_INFO(...)
#define SKTBD_APP_TRACE(...)

#else

// Core log macros
#define SKTBD_LOG(severity, component, ...)		{::Skateboard::Log::Logger::LogMessage(severity, component ,flossy::format(__VA_ARGS__));}

// Core log macros
#define SKTBD_LOG_CRITICAL(component,...)		SKTBD_LOG(::Skateboard::Log::LogSeverity::Critical,	 component ,flossy::format(__VA_ARGS__))
#define SKTBD_LOG_ERROR(component,...)			SKTBD_LOG(::Skateboard::Log::LogSeverity::Error,	 component ,flossy::format(__VA_ARGS__))
#define SKTBD_LOG_WARN(component,...)			SKTBD_LOG(::Skateboard::Log::LogSeverity::Warn,		 component ,flossy::format(__VA_ARGS__))
#define SKTBD_LOG_INFO(component,...)			SKTBD_LOG(::Skateboard::Log::LogSeverity::Info,		 component ,flossy::format(__VA_ARGS__))
#define SKTBD_LOG_DEBUG(component,...)			SKTBD_LOG(::Skateboard::Log::LogSeverity::Debug,	 component ,flossy::format(__VA_ARGS__))
#define SKTBD_LOG_TRACE(component,...)			SKTBD_LOG(::Skateboard::Log::LogSeverity::Trace,	 component ,flossy::format(__VA_ARGS__))

// Core log macros
#define SKTBD_CORE_CRITICAL(...)		{::Skateboard::Log::Logger::LogMessage(::Skateboard::Log::LogSeverity::Critical, SKTBD_LOG_COMPONENT ,flossy::format(__VA_ARGS__));}
#define SKTBD_CORE_ERROR(...)			{::Skateboard::Log::Logger::LogMessage(::Skateboard::Log::LogSeverity::Error,	 SKTBD_LOG_COMPONENT ,flossy::format(__VA_ARGS__));}
#define SKTBD_CORE_WARN(...)			{::Skateboard::Log::Logger::LogMessage(::Skateboard::Log::LogSeverity::Warn,	 SKTBD_LOG_COMPONENT ,flossy::format(__VA_ARGS__));}
#define SKTBD_CORE_INFO(...)			{::Skateboard::Log::Logger::LogMessage(::Skateboard::Log::LogSeverity::Info,	 SKTBD_LOG_COMPONENT ,flossy::format(__VA_ARGS__));}
#define SKTBD_CORE_DEBUG(...)			{::Skateboard::Log::Logger::LogMessage(::Skateboard::Log::LogSeverity::Debug,	 SKTBD_LOG_COMPONENT ,flossy::format(__VA_ARGS__));}
#define SKTBD_CORE_TRACE(...)			{::Skateboard::Log::Logger::LogMessage(::Skateboard::Log::LogSeverity::Trace,	 SKTBD_LOG_COMPONENT ,flossy::format(__VA_ARGS__));}

// Client log macros					//
#define SKTBD_APP_CRITICAL(...)			{::Skateboard::Log::Logger::LogMessage(::Skateboard::Log::LogSeverity::Critical, "APP" ,flossy::format(__VA_ARGS__));}
#define SKTBD_APP_ERROR(...)			{::Skateboard::Log::Logger::LogMessage(::Skateboard::Log::LogSeverity::Error,	 "APP" ,flossy::format(__VA_ARGS__));}
#define SKTBD_APP_WARN(...)				{::Skateboard::Log::Logger::LogMessage(::Skateboard::Log::LogSeverity::Warn,	 "APP" ,flossy::format(__VA_ARGS__));}
#define SKTBD_APP_INFO(...)				{::Skateboard::Log::Logger::LogMessage(::Skateboard::Log::LogSeverity::Info,	 "APP" ,flossy::format(__VA_ARGS__));}
#define SKTBD_APP_DEBUG(...)			{::Skateboard::Log::Logger::LogMessage(::Skateboard::Log::LogSeverity::Debug,	 "APP" ,flossy::format(__VA_ARGS__));}
#define SKTBD_APP_TRACE(...)			{::Skateboard::Log::Logger::LogMessage(::Skateboard::Log::LogSeverity::Trace,	 "APP" ,flossy::format(__VA_ARGS__));}

#endif

