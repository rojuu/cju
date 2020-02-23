#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <regex>
#include <cctype>
#include <unordered_map>
#include <variant>
#include <cstdint>
#include <memory>
#include <string>

#include <typeinfo>

#include <cassert>

#include "json.hpp"

#define LEXER_USE_ASSERT
#define LEXER_SIZE_TYPE size_t
#include "lexer.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/STLExtras.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Scalar/GVN.h>
#pragma GCC diagnostic pop
