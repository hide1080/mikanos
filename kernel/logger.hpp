/**
 * @file logger.hpp
 *
 * ログ関係.
 */
 
#pragma once

enum LogLevel {
  kError  = 3,
  kWarn   = 4,
  kInfo   = 6,
  kDebug  = 7,
};

/**
 * グローバルなログ優先度のしきい値を設定する.
 */
void SetLogLevel(enum LogLevel level);

/**
 * 指定された優先度でログを記録する.
 * 優先度がしきい値未満ならばログは記録されない.
 */
int Log(enum LogLevel level, const char* format, ...);
