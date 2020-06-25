#include "sigquit.h"

#include <signal.h>
#include <sys/eventfd.h>
#include <unistd.h>
#include <string.h>


static int g_quitFd = -1;
static bool g_isQuit = false;

static void signalHandler(int signum)
{
    // errno = 4 Interrupted system call, EINTR= 4
    if (signum == SIGINT || signum == SIGTERM || signum == SIGALRM) {
        g_isQuit = true;
        uint64_t one = 1;
        int rc;
        rc = write(g_quitFd, &one, sizeof one);
        (void)rc;
    }
}

void RegisterQuit()
{
    g_quitFd = eventfd(0, EFD_NONBLOCK);
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    signal(SIGPIPE, SIG_IGN);
    signal(SIGCHLD, SIG_IGN);
}

bool IsQuit()
{
    return g_isQuit;
}

void CleanQuit()
{
    if (g_quitFd > 0) {
        close(g_quitFd);
        g_quitFd = -1;
    }
}

int GetQuitFd()
{
    return g_quitFd;
}
