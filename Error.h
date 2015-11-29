#pragma once
#include <string>
#include <cstdio>

static const int FATAL_ERROR_CODE = 1;

void fatalError(const std::string error)
{
	printf(error.c_str());
	exit(FATAL_ERROR_CODE);
}