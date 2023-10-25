#ifndef BUILTIN_H
#define BUILTIN_H

void early_Threadless_Hook(struct commands * cs);

bool hook(struct command * c);

void wait_On_Hooked();

#endif

