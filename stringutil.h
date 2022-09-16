#ifndef _STRING_UTIL_H_
#define _STRING_UTIL_H_

#include <QString>

namespace feiker{
    class StringUtil{
        public:
        static bool StringFuzzySearch(const QString& s, const QString& ch){
            size_t it = s.indexOf(ch);
            if(it != -1)
                return true;

            return false;
        }
    };
}

#endif //_STRING_UTIL_H_