#pragma once

#include <llvm/IR/Value.h>
#include <string>
#include <map>

struct Enviroment {
	std::map<std::string, llvm::Value*> local_variable;
	std::map<std::string, llvm::Value*> global_variable;
};