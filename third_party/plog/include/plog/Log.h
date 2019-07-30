//////////////////////////////////////////////////////////////////////////
//  Plog - portable and simple log for C++
//  Documentation and sources: https://github.com/SergiusTheBest/plog
//  License: MPL 2.0, http://mozilla.org/MPL/2.0/

#pragma once
#include <plog/Logger.h>
#include <plog/Init.h>

//////////////////////////////////////////////////////////////////////////
// Helper macros that get context info

#if _MSC_VER >= 1600 && !defined(__INTELLISENSE__) // >= Visual Studio 2010 and skip IntelliSense
#   define PLOG_GET_THIS()      __if_exists(this) { this } __if_not_exists(this) { 0 }
#else
#   define PLOG_GET_THIS()      nullptr
#endif

#ifdef _MSC_VER
#   define PLOG_GET_FUNC()      __FUNCTION__
#elif defined(__BORLANDC__)
#   define PLOG_GET_FUNC()      __FUNC__
#else
#   define PLOG_GET_FUNC()      __PRETTY_FUNCTION__
#endif

#if PLOG_CAPTURE_FILE
#   define PLOG_GET_FILE()      __FILE__
#else
#   define PLOG_GET_FILE()      ""
#endif

//////////////////////////////////////////////////////////////////////////
// Log severity level checker

#define IF_PLOG_(instance, severity)     !(plog::get<instance>() && plog::get<instance>()->checkSeverity(severity)) ? (void)0 :
#define IF_PLOG(severity)                IF_PLOG_(PLOG_DEFAULT_INSTANCE, severity)

//////////////////////////////////////////////////////////////////////////
// Main logging macros

#define PLOG_(instance, severity)        IF_PLOG_(instance, severity) (*plog::get<instance>()) += plog::Record(severity, PLOG_GET_FUNC(), __LINE__, PLOG_GET_FILE(), PLOG_GET_THIS())
#define PLOG(severity)                   PLOG_(PLOG_DEFAULT_INSTANCE, severity)

#define PLOG_VERBOSE                     PLOG(plog::verbose)
#define PLOG_DEBUG                       PLOG(plog::debug)
#define PLOG_INFO                        PLOG(plog::info)
#define PLOG_WARNING                     PLOG(plog::warning)
#define PLOG_ERROR                       PLOG(plog::error)
#define PLOG_FATAL                       PLOG(plog::fatal)

#define PLOG_VERBOSE_(instance)          PLOG_(instance, plog::verbose)
#define PLOG_DEBUG_(instance)            PLOG_(instance, plog::debug)
#define PLOG_INFO_(instance)             PLOG_(instance, plog::info)
#define PLOG_WARNING_(instance)          PLOG_(instance, plog::warning)
#define PLOG_ERROR_(instance)            PLOG_(instance, plog::error)
#define PLOG_FATAL_(instance)            PLOG_(instance, plog::fatal)

#define PLOGV                            PLOG_VERBOSE
#define PLOGD                            PLOG_DEBUG
#define PLOGI                            PLOG_INFO
#define PLOGW                            PLOG_WARNING
#define PLOGE                            PLOG_ERROR
#define PLOGF                            PLOG_FATAL

#define PLOGV_(instance)                 PLOG_VERBOSE_(instance)
#define PLOGD_(instance)                 PLOG_DEBUG_(instance)
#define PLOGI_(instance)                 PLOG_INFO_(instance)
#define PLOGW_(instance)                 PLOG_WARNING_(instance)
#define PLOGE_(instance)                 PLOG_ERROR_(instance)
#define PLOGF_(instance)                 PLOG_FATAL_(instance)

//////////////////////////////////////////////////////////////////////////
// Conditional logging macros

#define PLOG_IF_(instance, severity, condition)  !(condition) ? void(0) : PLOG_(instance, severity)
#define PLOG_IF(severity, condition)             PLOG_IF_(PLOG_DEFAULT_INSTANCE, severity, condition)

#define PLOG_VERBOSE_IF(condition)               PLOG_IF(plog::verbose, condition)
#define PLOG_DEBUG_IF(condition)                 PLOG_IF(plog::debug, condition)
#define PLOG_INFO_IF(condition)                  PLOG_IF(plog::info, condition)
#define PLOG_WARNING_IF(condition)               PLOG_IF(plog::warning, condition)
#define PLOG_ERROR_IF(condition)                 PLOG_IF(plog::error, condition)
#define PLOG_FATAL_IF(condition)                 PLOG_IF(plog::fatal, condition)

#define PLOG_VERBOSE_IF_(instance, condition)    PLOG_IF_(instance, plog::verbose, condition)
#define PLOG_DEBUG_IF_(instance, condition)      PLOG_IF_(instance, plog::debug, condition)
#define PLOG_INFO_IF_(instance, condition)       PLOG_IF_(instance, plog::info, condition)
#define PLOG_WARNING_IF_(instance, condition)    PLOG_IF_(instance, plog::warning, condition)
#define PLOG_ERROR_IF_(instance, condition)      PLOG_IF_(instance, plog::error, condition)
#define PLOG_FATAL_IF_(instance, condition)      PLOG_IF_(instance, plog::fatal, condition)

#define PLOGV_IF(condition)                      PLOG_VERBOSE_IF(condition)
#define PLOGD_IF(condition)                      PLOG_DEBUG_IF(condition)
#define PLOGI_IF(condition)                      PLOG_INFO_IF(condition)
#define PLOGW_IF(condition)                      PLOG_WARNING_IF(condition)
#define PLOGE_IF(condition)                      PLOG_ERROR_IF(condition)
#define PLOGF_IF(condition)                      PLOG_FATAL_IF(condition)

#define PLOGV_IF_(instance, condition)           PLOG_VERBOSE_IF_(instance, condition)
#define PLOGD_IF_(instance, condition)           PLOG_DEBUG_IF_(instance, condition)
#define PLOGI_IF_(instance, condition)           PLOG_INFO_IF_(instance, condition)
#define PLOGW_IF_(instance, condition)           PLOG_WARNING_IF_(instance, condition)
#define PLOGE_IF_(instance, condition)           PLOG_ERROR_IF_(instance, condition)
#define PLOGF_IF_(instance, condition)           PLOG_FATAL_IF_(instance, condition)
