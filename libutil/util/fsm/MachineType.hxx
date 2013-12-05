#ifndef UTIL_MACHINETYPE_HXX
#define UTIL_MACHINETYPE_HXX

#include <util/util.h>
#include <iostream>
#include <string>
using namespace std;

namespace Util
{
   class UTIL_API MachineType
   {
   public:
      MachineType(const string& name);
      ~MachineType();

      const string& getName() const { return mName; }
      void setName(const string& name) { mName = name; }

      bool operator==(const MachineType& rhs) const;
      bool operator!=(const MachineType& rhs) const;

   private:
      int mNumber;
      string mName;
   };
   ostream& operator<<(ostream& str, const MachineType& event);

} // namespace Util

#endif // #ifndef UTIL_MACHINETYPE_HXX.
