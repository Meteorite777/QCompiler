/* Recursive Descent Support Functions */
#ifndef RDSUPPORT_H
#define RDSUPPORT_H

/* Global Variable */
extern int CurToken;

#define ENTER       Enter(__func__)
#define LEAVE_FAIL  LeaveFail()
#define LEAVE_SUCC  LeaveSucc()
void Enter(const char *Name);
bool LeaveFail();
bool LeaveSucc();

void ParseError(char *Message);
bool Match(int ExpectedToken);

#endif
