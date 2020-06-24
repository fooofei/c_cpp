

#ifndef SIG_QUIT_H
#define SIG_QUIT_H

void RegisterQuit();
bool IsQuit();
int GetQuitFd();
void CleanQuit();

#endif
