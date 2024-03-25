/* Include files */

#include "Sim_cgxe.h"
#include "m_85qzIhcwNmRLBIOOWogW5B.h"

unsigned int cgxe_Sim_method_dispatcher(SimStruct* S, int_T method, void* data)
{
  if (ssGetChecksum0(S) == 3650129363 &&
      ssGetChecksum1(S) == 3859116284 &&
      ssGetChecksum2(S) == 3625440425 &&
      ssGetChecksum3(S) == 3813788456) {
    method_dispatcher_85qzIhcwNmRLBIOOWogW5B(S, method, data);
    return 1;
  }

  return 0;
}
