#ifndef __UTILS_H
#define __UTILS_H

#include <string>
#include <vector>

void Tokenize(const std::string &str, std::vector<std::string> &tokens,
              const std::string &delimiters = " ");

#endif // __UTILS_H
