#ifndef ANALYZER_ISPY_NET_PRODUCER_H
# define ANALYZER_ISPY_NET_PRODUCER_H

# include "ISpy/Services/interface/IgNet.h"

sig_atomic_t s_stop = 0;
static void
interrupt(int /* sig */)
{
  s_stop = 1;
}

class ISpyNetProducer : public IgNet
{
public:
  bool
  shouldStop(void)
    {
      return s_stop != 0;
    } 

  ISpyNetProducer (bool verbose, int port)
    : IgNet("iguana-source")
    {
      logme() << "INFO: serving data at port " << port << '\n';
      debug(verbose);
      startLocalServer(port);
    }
};

#endif // ANALYZER_ISPY_NET_PRODUCER_H
