#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <signal.h>
#include <execinfo.h>
#include <syslog.h>
#include <memory>
#include "controller.h"

using namespace std;

const char kPidFile[] = "/var/run/peakwork_daemon.pid";
enum eDaemonStatus {
    NeedToWork,
    NeedToTerminate
};
const unsigned int kFdLimit = 2048;
std::unique_ptr<daemonspace::Controller> dmn;

int PeakworkDaemonProc();
void CreatePidFile(const char* fileName);
int WorkProc();
static void SignalError(int sig, siginfo_t *si, void *ptr);
int SetFdLimit(int maxFd);

int main(int , char **)
{
    setlogmask(LOG_UPTO(LOG_NOTICE));
    openlog("PeakworkDaemon", LOG_CONS | LOG_NDELAY | LOG_PERROR | LOG_PID, LOG_USER);
#ifdef NDEBUG

    int status;
    int pid;
    pid = fork();

    if (pid == -1) {
        cerr << "Error: Start Daemon failed " << strerror(errno) << endl;
        return -1;
    } else if (!pid) {
        umask(0);
        setsid();
        if(chdir("/") != 0) {
            cerr << "Error: Can't change directory to / " << strerror(errno) << endl;
        }
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);

        status = PeakworkDaemonProc();

        return status;
    } else {
        return 0;
    }
#else
    return PeakworkDaemonProc();
    closelog();
#endif
}

int PeakworkDaemonProc()
{
#ifdef NDEBUG
    int pid;
    int status;
    int need_start = 1;
    sigset_t sigset;
    siginfo_t siginfo;

    sigemptyset(&sigset);
    sigaddset(&sigset, SIGQUIT);
    sigaddset(&sigset, SIGINT);
    sigaddset(&sigset, SIGTERM);
    sigaddset(&sigset, SIGCHLD);
    sigaddset(&sigset, SIGABRT);
    sigprocmask(SIG_BLOCK, &sigset, NULL);

    CreatePidFile(kPidFile);

    for (;;) {
        if (need_start) {
            pid = fork();
        }

        need_start = 1;
        if (pid == -1) {
            syslog(LOG_ERR, "[PeakworkDaemon] Fork failed (%s)\n", strerror(errno));
        } else if (!pid) {
            status = WorkProc();
            closelog();
            exit(status);
        } else {
            sigwaitinfo(&sigset, &siginfo);

            if (siginfo.si_signo == SIGCHLD)
            {
                wait(&status);
                status = WEXITSTATUS(status);
                if (status == NeedToTerminate) {
                    syslog(LOG_INFO, "[PeakworkDaemon] Child stopped\n");
                    break;
                } else if (status == NeedToWork) {
                    syslog(LOG_INFO, "[PeakworkDaemon] Child restart\n");
                }
            } else {
                syslog(LOG_INFO, "[PeakworkDaemon] Signal %s\n", strsignal(siginfo.si_signo));
                kill(pid, SIGTERM);
                status = 0;
                break;
            }
        }
    }

    syslog(LOG_INFO, "[PeakworkDaemon] Stop\n");
    closelog();
    unlink(kPidFile);

    return status;
#else
    return WorkProc();
#endif
}

void CreatePidFile(const char* fileName)
{
    ofstream f(fileName);
    if (f.is_open()) {
        f << getpid();
        f.close();
    } else {
        syslog(LOG_ERR, "[PeakworkDaemon] Can't open pid file");
    }
}

int WorkProc()
{
    struct sigaction sigact;
    sigset_t sigset;
    int signo;


    sigact.sa_flags = SA_SIGINFO;
    sigact.sa_sigaction = SignalError;
    sigemptyset(&sigact.sa_mask);

    sigaction(SIGFPE, &sigact, nullptr);
    sigaction(SIGSEGV, &sigact, nullptr);
    sigaction(SIGBUS, &sigact, nullptr);
    sigemptyset(&sigset);

    sigprocmask(SIG_BLOCK, &sigset, nullptr);

    SetFdLimit(kFdLimit);

    syslog(LOG_INFO, "[PeakworkDaemon] Started\n");

    dmn = std::unique_ptr<daemonspace::Controller>(new daemonspace::Controller());
    try {
        dmn->Start();
        for (;;) {
            sigwait(&sigset, &signo);
            break;
        }
        dmn->Stop();
    } catch(...) {
        syslog(LOG_ERR, "[PeakworkDaemon] Create work thread failed\n");
    }

    syslog(LOG_INFO, "[PeakworkDaemon] Stopped\n");

    return eDaemonStatus::NeedToTerminate;
}

void SignalError(int sig, siginfo_t *si, void *ptr)
{
    void *errorAddr;
    void *trace[16];
    int x;
    int traceSize;
    char **messages;
    std::ostringstream msg;
    msg << "[PeakworkDaemon] Signal: " << strsignal(sig) << ", Addr: " << si->si_addr << std::endl;
    syslog(LOG_ERR, "%s", msg.str().c_str());


    #if __WORDSIZE == 64
        errorAddr = (void*)((ucontext_t*)ptr)->uc_mcontext.gregs[REG_RIP];
    #else
        errorAddr = (void*)((ucontext_t*)ptr)->uc_mcontext.gregs[REG_EIP];
    #endif

    traceSize = backtrace(trace, 16);
    trace[1] = errorAddr;

    messages = backtrace_symbols(trace, traceSize);
    if (messages) {
        syslog(LOG_DEBUG, "== Backtrace ==\n");
        for (x = 1; x < traceSize; x++) {
            syslog(LOG_DEBUG, "%s\n", messages[x]);
        }
        syslog(LOG_DEBUG, "== End Backtrace ==\n");
        free(messages);
    }

    syslog(LOG_INFO, "[PeakworkDaemon] Stopped\n");

    dmn->Stop();
    dmn.release();
    exit(NeedToWork);
}

int SetFdLimit(int maxFd)
{
    struct rlimit lim;
    int status;

    lim.rlim_cur = maxFd;
    lim.rlim_max = maxFd;

    status = setrlimit(RLIMIT_NOFILE, &lim);

    return status;
}
