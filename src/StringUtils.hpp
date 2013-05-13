#ifndef STRINGUTILS_HPP
#define STRINGUTILS_HPP

#include <sstream>

class StringUtils
{
   public:
   
      template<typename T>
      static bool from_string( const std::string & Str, T & Dest )
      {
          // créer un flux à partir de la chaîne donnée
          std::istringstream iss( Str );
          // tenter la conversion vers Dest
          return iss >> Dest != 0;
      }
      
      template<typename T>
      static bool to_string(const T& Value, std::string& str)
      {
          std::ostringstream oss;
          bool result = oss << Value != 0;
          
          str = oss.str();
          return result;
      }
};

#endif
