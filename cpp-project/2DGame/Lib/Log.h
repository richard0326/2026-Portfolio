#pragma once

#define LOG_NO_CONSOLE 1

void InitLog(int logLevel);

void ReleaseLog();

void AddLog(const wchar_t* format, ...);