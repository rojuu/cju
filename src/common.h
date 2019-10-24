#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <regex>
#include <cctype>
#include <unordered_map>

#include <cassert>

#include "json.hpp"

#define LEXER_USE_ASSERT
#define LEXER_SIZE_TYPE size_t
#include "lexer.h"
