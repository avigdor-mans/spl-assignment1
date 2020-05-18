//
// Created by aviv on 19/11/2019.
//
#include "../include/User.h"
#include "../include/Watchable.h"
#include "../include/Session.h"
#include <cmath>
using namespace std;

//----------------------User----------------//

User::User(const std::string& _name) : history(), name(_name) {

}
User::~User() {
    clear();
}
vector<Watchable*> User::get_history() const
{
    return history;
}
string User::getName() const {
    return name;
}
void User::addWatched(Watchable * watchable) {
    history.push_back(watchable);
}

void User::clear() {

        for(int i=0; (unsigned)i<history.size();i++)
        {
            history[i] = nullptr;
        }
}

User::User(const User &user) :  history(), name(user.name) {
    for(int i=0; (unsigned)i<user.history.size();i++)
    {
        history[i] = user.history[i]->copy();
    }
}

User &User::operator=(const User& other) {
    if (this == &other) {
        return *this;
    }
    clear();
    name = other.name;
    for(int i=0; (unsigned)i<other.history.size();i++)
    {
        history[i] = other.history[i]->copy();
    }
    return *this;
}

User::User(User &&other) : history(), name(other.name) {
    for(int i=0; (unsigned)i<other.history.size();i++)
    {
        history[i] = other.history[i]->copy();
    }
    other.clear();
}

User &User::operator=(User &&other) {
    if(this != &other)
    {
        clear();
        for(int i=0; (unsigned)i<other.history.size();i++)
        {
            history[i] = other.history[i]->copy();
        }
    }

    return *this;
}

//-------------------LengthRecommenderUser---------------------//

LengthRecommenderUser::LengthRecommenderUser(const std::string& _name) : User(_name){

}
LengthRecommenderUser::LengthRecommenderUser(const LengthRecommenderUser & lUser) : User(lUser.getName())
{
    history = lUser.history;
}


Watchable* LengthRecommenderUser::getRecommendation(Session& s)
{

    double sum =0;
    double average;
    for(int i=0; (unsigned)i< history.size(); i++)
    {
        sum = sum + history[i]->get_Length();
    }
    average = sum / history.size();
    long idOfBest=-1;
    double closest=10000000000;
    for (int j = 0; (unsigned)j < s.getContent()->size() && closest!=0; j++) {
        bool found = false;
        for (int i = 0; (unsigned)i < history.size() && !found; i++) {
            found = (*s.getContent())[j]->getId() == history[i]->getId();
        }
        if (!found)
        {
            double x = abs((*s.getContent())[j]->get_Length() - average);
            if(x < closest) {
                closest = x;
                idOfBest = j;
            }
        }
    }
    if (idOfBest == -1)
        return nullptr;
    return (*s.getContent())[idOfBest];
}


LengthRecommenderUser* LengthRecommenderUser::duplicate(std::string newUserName,Session& sess){

    LengthRecommenderUser* newUser = new LengthRecommenderUser(newUserName);
    for (int i = 0; (unsigned)i < history.size(); ++i) {
        newUser->addWatched((*sess.getContent())[history[i]->getId() - 1]);
    }
    return newUser;

}

//-------------------RerunRecommenderUser---------------------//

RerunRecommenderUser::RerunRecommenderUser(const std::string& _name) : User(_name),indexToRecommended(-1){

}

Watchable* RerunRecommenderUser::getRecommendation(Session &s) {

    if (history.size() ==0)
        return nullptr;
    indexToRecommended = (indexToRecommended+1)%history.size();
    return history[indexToRecommended];

}
RerunRecommenderUser* RerunRecommenderUser::duplicate(std::string newUserName,Session &sess){

    RerunRecommenderUser* newUser = new RerunRecommenderUser(newUserName);
    for (int i = 0; (unsigned)i < history.size(); ++i) {
        newUser->addWatched((*sess.getContent())[history[i]->getId() - 1]);
    }
    newUser->indexToRecommended=indexToRecommended;
    return newUser;
}

//-------------------GenreRecommenderUser---------------------//

GenreRecommenderUser::GenreRecommenderUser(const std::string& _name) : User(_name){

}
Watchable* GenreRecommenderUser::getRecommendation(Session &s) {

    vector<pair<string,int>> tags;
    for (int i = 0; (unsigned)i < history.size(); i++) {
        for (int j = 0; (unsigned)j < history[i]->getTags().size(); j++) {
            bool found=false;
            for(auto& pair : tags)
            {
                if(pair.first == history[i]->getTags()[j]){
                    found=true;
                    pair.second=pair.second+1;
                }
            }

            if (!found)
                tags.push_back(pair<string,int >(history[i]->getTags()[j],1));
        }
    }

    for (int i = 0; (unsigned)i < tags.size(); i++) {
        for(int j=0;(unsigned)j< tags.size()-1;j++)
        {
            if(tags[j].second<tags[j+1].second) {
                pair<std::string,int> temp = tags[j];
                tags[j] = tags[j + 1];
                tags[j + 1] = temp;
            }
        }
    }

    for (int i = 0; (unsigned)i < tags.size(); i++) {
        for(int j=0;(unsigned)j<tags.size() -1; j++)
        {
            if (tags[j].second == tags[j+1].second &&  tags[j].first.compare(tags[j+1].first)>0){
                pair<std::string,int> temp = tags[j];
                tags[j] = tags[j + 1];
                tags[j + 1] = temp;
            }
        }
    }
    for (int i = 0; (unsigned)i < tags.size(); i++) {
        for (int j = 0; (unsigned)j < s.getContent()->size(); j++) {
            for (int k = 0;(unsigned) k <(*s.getContent())[j]->getTags().size() ;k++) {
                if (tags[i].first==(*s.getContent())[j]->getTags()[k]){
                    bool wasWatch=false;
                    for (int l = 0; !wasWatch && (unsigned)l < history.size(); ++l) {
                        if((*s.getContent())[j]->getId()==history[l]->getId()){
                            wasWatch=true;
                        }

                    }
                    if (!wasWatch){
                        return (*s.getContent())[j];
                    }
                }

            }
        }
    }
    return nullptr;
}

GenreRecommenderUser* GenreRecommenderUser::duplicate(std::string newUserName,Session &sess){

    GenreRecommenderUser* newUser = new GenreRecommenderUser(newUserName);
    for (int i = 0; (unsigned)i < history.size(); ++i) {
        newUser->addWatched((*sess.getContent())[history[i]->getId() - 1]);
    }
    return newUser;
}