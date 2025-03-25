#pragma once

#include <string>
#include <vector>

#include "ISkeleton.h"

void _LineToBoneInfo(const char szLineBuffer[], std::vector<std::string>& childBoneNames);