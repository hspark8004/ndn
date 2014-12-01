#include <event2/event.h>
#include <event2/util.h>
#include <event2/thread.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <iostream>
#include <string>
#include "face/TcpSenderFace.hpp"
#include "face/TcpReceiverFace.hpp"
#include "Common.hpp"

using namespace std;

struct event_base* eventBase;

int main(void)
{
  evthread_use_pthreads();
  event_enable_debug_mode();
  eventBase = event_base_new();

  TcpSenderFace sender("/ndn", 10000);
  TcpReceiverFace receiver("/ndn", 20000);
#ifdef __DEBUG_MODE
  int channel1[2];
  int channel2[2];

  socketpair(AF_UNIX, SOCK_STREAM, 0, channel1);
  socketpair(AF_UNIX, SOCK_STREAM, 0, channel2);

  sender.setSendSocket(channel1[0]);
  receiver.setRecvSocket(channel1[1]);
  receiver.setSendSocket(channel2[1]);
  sender.setRecvSocket(channel2[0]);
#endif /* __DEBUG_MODE */

#if 0
  // TODO after signal processing
  sigset_t sigset;
  int signum;

  sigfillset(&sigset);
  sigprocmask(SIG_BLOCK, &sigset, NULL);

  // FIXME set "SIGINT" action
  for(;;) {
    sigwait(&sigset, &signum);

    if(signum == SIGINT) {
      break;
    }
  }
#endif

  event_base_dispatch(eventBase);
  cout << "게이트웨이를 종료합니다." << endl;
}
