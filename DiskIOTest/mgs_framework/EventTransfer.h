#pragma once

#include <map>
#include <set>

class EventTerminal;
class EventCenter
{
public:
    void PushEvent(int eventid,...);
    void PushEvent(int eventid,va_list arg_ptr);
public:
    EventCenter();
    ~EventCenter();

private:
    void Register(int eventid,EventTerminal* terminal);
    void Unregister(int eventid,EventTerminal* terminal);

    std::map<int,std::set<EventTerminal*> > m_mapEvent;
    friend class EventTerminal;
};

class EventTerminal
{
protected:
    EventTerminal(EventCenter* center):m_evtCenter(center){}
    void PushEvent(int eventid,...);

public:
    ~EventTerminal();

    bool RegEvent(int eventid);
    bool UnregEvent(int eventid);

    virtual void OnEvent(int eventid,va_list arg_ptr) {}
private:
    EventCenter* m_evtCenter;
    std::set<int> m_setEvtIDs;
    friend class EventCenter;
};


