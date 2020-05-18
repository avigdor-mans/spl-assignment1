//
// Created by aviv on 24/11/2019.
//

//
// Created by aviv on 19/11/2019.
//
#include "../include/Session.h"
#include "../include/json.hpp"
#include "../include/Action.h"
#include "../include/User.h"
#include "../include/Watchable.h"
#include <fstream>

using namespace std;
using json=nlohmann::json ;

//----------------------Session----------------//

Session::Session(const std::string &configFilePath) : content(),actionsLog(vector<BaseAction*>()),userMap(), activeUser(){
    setContent(configFilePath);
        userMap.insert(pair<string, User *>("default",  new LengthRecommenderUser("default")));
        activeUser = userMap["default"];
}
Session::Session(const Session & other) : content(),actionsLog(vector<BaseAction*>()), userMap(),activeUser() {
    for(int i=0; (unsigned )i<other.content.size();i++)
    {
        Watchable* watchable = other.content[i]->copy();
        content.push_back(watchable);
    }
    for(auto &tmp:other.actionsLog){
        BaseAction* baseAction = tmp->copy();
        actionsLog.push_back(baseAction);
    }
    for(auto &tmp:other.userMap){
        User * newUser= tmp.second->duplicate(tmp.first,*this);
        userMap.insert(pair<string,User*>(newUser->getName(),newUser));
        if (tmp.first==other.activeUser->getName())
            activeUser=newUser;
    }

}
Session&Session::operator=(const Session& other){
    if (this == &other) {
        return *this;
    }
    clear();

    for(int i=0; (unsigned )i<other.content.size();i++)
    {
        Watchable* watchable = other.content[i]->copy();
        content.push_back(watchable);
    }
    for(auto &tmp:other.actionsLog){
        BaseAction* baseAction = tmp->copy();
        actionsLog.push_back(baseAction);
    }
    for(auto &tmp:other.userMap){
        User * newUser= tmp.second->duplicate(tmp.first,*this);
        userMap.insert(pair<string,User*>(newUser->getName(),newUser));
        if (tmp.first==other.activeUser->getName())
            activeUser=newUser;
    }

    return *this;
}
Session::~Session() {
    clear();

}
Session::Session(Session &&other) : content(), actionsLog(vector<BaseAction*>()), userMap(),activeUser() {
    for(int i=0; (unsigned)i<other.content.size();i++)
    {
        Watchable* watchable = other.content[i]->copy();
        content.push_back(watchable);
    }
    for(auto &tmp:other.actionsLog){
        BaseAction* baseAction = tmp->copy();
        actionsLog.push_back(baseAction);
    }
    for(auto &tmp:other.userMap){
        User * newUser= tmp.second->duplicate(tmp.first,*this);
        userMap.insert(pair<string,User*>(newUser->getName(),newUser));
        if (tmp.first==other.activeUser->getName())
            activeUser=newUser;
    }
    other.clear();
}

Session& Session::operator=(Session &&other)
{
    if (this != &other)
    {
        clear();
        for(int i=0; (unsigned)i<other.content.size();i++)
        {
            Watchable* watchable = other.content[i]->copy();
            content.push_back(watchable);
        }
        for(auto &tmp:other.actionsLog){
            BaseAction* baseAction = tmp->copy();
            actionsLog.push_back(baseAction);
        }
        for(auto &tmp:other.userMap){
            User * newUser= tmp.second->duplicate(tmp.first,*this);
            userMap.insert(pair<string,User*>(newUser->getName(),newUser));
            if (tmp.first==other.activeUser->getName())
                activeUser=newUser;
        }
    }

    return *this;
}


void Session::clear() {
    for(int i=0; (unsigned)i<content.size();i++)
    {
        delete content[i];
        content[i] = nullptr;
    }

    for(int i=0; (unsigned)i<actionsLog.size();i++)
    {
        delete actionsLog[i];
        actionsLog[i] = nullptr;
    }
    for(auto &pair : userMap)
    {
        delete pair.second;
        pair.second = nullptr;
    }
    content.clear();
    userMap.clear();
    actionsLog.clear();
    activeUser = nullptr;
}


unordered_map<string,User*> & Session::getUsers(){
    return userMap;
}
vector<BaseAction*> & Session::getActionsLog() {
    return actionsLog;
}
vector<Watchable*>* Session::getContent() {
    return &content;
}
User* Session::getActiveUser() const {
    return activeUser;
}
void Session::setActiveUser(string _name) {
    activeUser=userMap[_name];
}

void Session::start() {

    cout<<"SPLFLIX is now on!"<<endl;

    BaseAction *baseAction;

    string action;
    cin>>action;

    baseAction = actionConverter(action.substr(0,action.find(' ')));

    while (action != "exit")
    {
        if (baseAction != nullptr) {
            baseAction->act(*this);

        }
        else {
            cout <<"no such action"<<endl;
        }
        cin >> action;
        baseAction = actionConverter(action);
    }
    baseAction->act(*this);
    baseAction = nullptr;

}
BaseAction* Session::actionConverter(std::string action) {
    if(action=="createuser") {
        return new CreateUser();
    }
    if(action=="changeuser") {
        return new ChangeActiveUser();
    }
    if(action== "deleteuser")
    {
        return  new DeleteUser();
    }
    if(action=="dupuser") {
        return  new DuplicateUser();
    }
    if(action=="content") {
        return  new PrintContentList();
    }
    if(action=="watchhist") {
        return new PrintWatchHistory();
    }
    if(action=="watch") {
        return new Watch();
    }
    if(action=="log") {
        return  new PrintActionsLog();
    }
    if(action=="exit") {
        return  new Exit();
    }
    return nullptr;
}
void Session::setContent(const std::string &configFilePath) {
    std::ifstream file(configFilePath);
    nlohmann::json x = nlohmann::json::parse(file);
    nlohmann::json Movies = x["movies"];
    nlohmann::json TvSeries = x["tv_series"];
    int id = 1;

    for (auto &object : Movies.items()) {
        nlohmann::json seret = object.value();
        int length = seret["length"];

        vector<string> tags;
        for (auto &t : seret["tags"]) {
            tags.push_back(t);
        }
        Movie *movie = new Movie(id, seret["name"], length, tags);
        content.push_back(movie);
        id = id + 1;
        movie = nullptr;
    }

    for (auto &object : TvSeries.items()) {
        nlohmann::json sidra = object.value();

        int length = sidra["episode_length"];

        vector<string> tags;
        for (auto &t : sidra["tags"]) {
            tags.push_back(t);
        }
        int seasonCounter = 1;
        Episode* lastOfSidra;
        for (auto &s : sidra["seasons"]) {
            for (int i = 1; i <= s; ++i) {
                Episode *episode = new Episode(id, sidra["name"], length, seasonCounter, i, tags);
                episode->setNextId(id+1);
                content.push_back(episode);
                id++;
                lastOfSidra = episode;
                episode = nullptr;
            }
            seasonCounter++;

        }
        lastOfSidra->setNextId(-1);
        lastOfSidra = nullptr;
    }

}
void Session::addAction(BaseAction * action) {
    actionsLog.push_back(action);
}
