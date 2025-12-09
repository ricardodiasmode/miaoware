//
// Created by Hector on 08/12/2025.
//

#include "TerminalHelper.h"

bool MatchesCmd(const std::string& input, const std::string& full)
{
  if (input == full) return true;
  if (input.size() == 1 && full.size() > 1 && input[0] == full[0])
    return true;

  return false;
}