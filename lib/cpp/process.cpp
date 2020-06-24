
#include "process.h"

#include <signal.h>
#include <sys/wait.h>
#include <string.h>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <vector>



Process::Process()
{
    exitStatus_ = 0;
    pid_ = -1;
}

int Process::Run(const std::vector<std::string> &exeArgs)
{
    Wait(); // clear last process
    if (exeArgs.size() < 1) {
        return -1;
    }
    pid_ = fork();
    if (pid_ < 0) {
        return -1;
    }
    if (pid_ > 0) {
        // parent
        return 0;
    }
    char *c_args[20 + 1];
    const std::string &program = exeArgs[0];
    c_args[0] = const_cast<char *>(program.c_str());
    size_t i = 1;
    for (; i < exeArgs.size() && i < 20; ++i) {
        c_args[i] = const_cast<char *>(exeArgs[i].c_str());
    }
    c_args[i] = (char *)NULL;
    execvp(program.c_str(), c_args);
    return 0;
}

Process::~Process()
{
    Wait();
}

pid_t Process::GetPid() const
{
    return pid_;
}

int parseStatus(int waitStatus)
{
    if (WIFEXITED(waitStatus)) {
        // if program exit code is 10, the waitStatus will be 2560,
        // WEXITSTATUS() will convert it to 10.
        return WEXITSTATUS(waitStatus);
    } else if (WIFSIGNALED(waitStatus)) {
        // not convert exit code 139 (core dump) to 11 (SIGSEV)
        // return WTERMSIG(waitStatus);
        return waitStatus;
    } else if (WIFSTOPPED(waitStatus)) {
        // return WSTOPSIG(waitStatus);
        return waitStatus;
    } else {
        return 0;
    }
}

bool Process::IsAlive()
{
    // Cannot detect process alive
    // return kill(pid_, 0) == 0;
    if (pid_ <= 0) {
        return false;
    }
    int status = 0;
    // status only return once, so we will save states in IsAlive() and Wait()
    // WUNTRACED
    pid_t r = waitpid(pid_, &status, WNOHANG);
    if (r == pid_) {
        exitStatus_ = parseStatus(status);
    }
    return r == 0;
}

int Process::Wait()
{
    int status = 0;
    if (pid_ <= 0) {
        return 0;
    }
    pid_t r = waitpid(pid_, &status, 0);
    if (r == pid_) {
        exitStatus_ = parseStatus(status);
    }
    pid_ = -1;
    status = exitStatus_;
    exitStatus_ = 0;
    return status;
}

void Process::SendSignal(int sig)
{
    if (pid_ <= 0) {
        return;
    }
    kill(pid_, sig);
}
