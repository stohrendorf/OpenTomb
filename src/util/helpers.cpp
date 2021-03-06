#include "helpers.h"

#include "ui/util.h"

#include <boost/log/trivial.hpp>

namespace util
{
std::string unescape(const std::string& escaped)
{
  std::string result;
  bool addUmlautDots = false;
  for(const char c : escaped)
  {
    if(c == ui::UmlautDots)
    {
      addUmlautDots = true;
      continue;
    }

    if(addUmlautDots)
    {
      switch(c)
      {
      case 'a': result += u8"ä"; break;
      case 'o': result += u8"ö"; break;
      case 'u': result += u8"ü"; break;
      case 'A': result += u8"Ä"; break;
      case 'O': result += u8"Ö"; break;
      case 'U': result += u8"Ü"; break;
      default:
        BOOST_LOG_TRIVIAL(warning) << "Cannot add umlaut dots to character " << c;
        result += c;
        break;
      }
      addUmlautDots = false;
      continue;
    }

    if(c == ui::SzLig)
    {
      result += u8"ß";
      continue;
    }

    result += c;
  }

  return result;
}

std::filesystem::path ensureFileExists(const std::filesystem::path& path)
{
  if(!std::filesystem::is_regular_file(path))
  {
    BOOST_LOG_TRIVIAL(fatal) << "Could not find required file " << path;
    BOOST_THROW_EXCEPTION(std::runtime_error("required file not found"));
  }
  return path;
}
} // namespace util
