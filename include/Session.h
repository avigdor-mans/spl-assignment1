//
// Created by aviv on 24/11/2019.
//

#ifndef ASS1_SESSION_H
#define ASS1_SESSION_H

#include <vector>
#include <unordered_map>
#include <string>
#include "Action.h"

class User;
class Watchable;

using  namespace std;
class Session{
public:
    Session(const std::string &configFilePath);
    Session(const Session&);
    Session&operator=(const Session&);
    ~Session();
    Session(Session &&other);
    Session& operator=(Session &&other);
    void clear();
    User* getActiveUser () const;
    void start();
    BaseAction* actionConverter(std::string);
    unordered_map<string,User*> & getUsers();
    vector<BaseAction*> & getActionsLog();
    vector<Watchable*>* getContent() ;
    void setActiveUser(string _name);
    void setContent(const std::string &configFilePath);
    void addAction(BaseAction*);
    void addUser(User *);
private:

    std::vector<Watchable*> content;
    std::vector<BaseAction*> actionsLog;
    std::unordered_map<string,User*> userMap;
    User* activeUser;
};

#endif //ASS1_SESSION_H