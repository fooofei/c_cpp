
#ifndef PROCESS_H
#define PROCESS_H



class Process {

public:
    explicit Process();
    ~Process();

    pid_t GetPid() const;
    int Run(const std::vector<std::string>& exeArgs);
    // NOTE: returned exit code not reliable
    int Wait();
    bool IsAlive();
    void SendSignal(int sig);

private:
    pid_t pid_;
    int exitStatus_;
};

#endif
