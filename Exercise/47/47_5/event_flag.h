#ifndef EVENT_FLAG_H
#define EVENT_FLAG_H

int setEventFlag(int semId, int semNum);

int clearEventFlag(int semId, int semNum);

int waitForEventFlag(int semId, int semNum);

int getFlagState(int semId, int semNum);

#endif