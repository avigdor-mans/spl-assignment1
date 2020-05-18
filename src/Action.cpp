//
// Created by aviv on 22/11/2019.
//

#include "../include/Action.h"
#include "../include/User.h"
#include "../include/Session.h"
#include "../include/Watchable.h"
using namespace std;

//-------------------------BaceAction---------------//

BaseAction::BaseAction() : errorMsg(),status(PENDING) {
}
BaseAction::~BaseAction()=default;


ActionStatus BaseAction::getStatus() const{
    return status;
}
std::string BaseAction::getStringStatus()const {
    if (status == PENDING)
        return "PENDING";
    if (status == ERROR)
        return "ERROR: " + errorMsg;
    if (status == COMPLETED)
        return "COMPLETED";
    return  "no-status-error";
}
void BaseAction::complete() {
    status = COMPLETED;
}
void BaseAction::error(const std::string &errorMsg) {
    this->errorMsg = errorMsg;
    status = ERROR;
    cout<< "ERROR: " + errorMsg<<endl;
}
std::string BaseAction::getErrorMsg() const{
    return  errorMsg;
}
bool BaseAction::isUsernameExist(Session &sess,string username){
    bool exist= false;
    for(auto & user: sess.getUsers()){
        if(user.first==username)
            exist= true;
    }
    return exist;
}
void BaseAction::setStatus(BaseAction& other){
    status=other.status;
    if(status==ERROR)
        errorMsg=other.errorMsg;
}
//-------------------------CreateUser---------------//

void CreateUser::act(Session &sess) {
    sess.addAction(this);
    string username;
    string userType;
    cin>>username;
    cin>>userType;
    if (isUsernameExist(sess,username))
    {
        error("Cannot Create User, username already exist");
    }

    else{
        bool typeCheck = false;
        if(userType=="len"){
            sess.getUsers().insert(pair<string,User*>(username,new LengthRecommenderUser(username)));
            typeCheck = true;
        }
        if(userType=="rer"){
            sess.getUsers().insert(pair<string,User*>(username,new RerunRecommenderUser(username)));
            typeCheck = true;
        }
        if(userType=="gen"){
            sess.getUsers().insert(pair<string,User*>(username,new GenreRecommenderUser(username)));
            typeCheck = true;
        }
        if (!typeCheck)
            error("Invalid user Type");
        else
            complete();
    }
}

string CreateUser::toString() const {
    return "CreateUser " + getStringStatus();
}

CreateUser *CreateUser::copy() {
    CreateUser * newAction = new CreateUser();
    newAction->setStatus(*this);
    return newAction;
}

//-------------------------ChangeActiveUser---------------//

void ChangeActiveUser::act(Session &sess) {
    sess.addAction(this);
    string username;
    cin>>username;
    if(!isUsernameExist(sess,username))
    {
        error("cannot Change, username doesnt exist");
    }
    else
    {
        sess.setActiveUser(username);
        complete();
    }
}

string ChangeActiveUser::toString() const {
    return "ChangeUser " + getStringStatus();
}

ChangeActiveUser *ChangeActiveUser::copy() {
    ChangeActiveUser * newAction = new ChangeActiveUser();
    newAction->setStatus(*this);
    return newAction;
}

//-------------------------DeleteUser---------------//

void DeleteUser::act(Session &sess) {
    sess.addAction(this);
    string username;
    cin>>username;
    if(!isUsernameExist(sess,username))
    {
        error("cannot Delete, username doesnt exist");
    }
    else
    {
        delete sess.getUsers()[username];
        unordered_map<string,User*>::iterator iterator;
        iterator = sess.getUsers().find(username);
        sess.getUsers().erase(iterator);
        complete();
    }
}
string DeleteUser::toString() const {
    return "DeleteUser " + getStringStatus();
}

DeleteUser *DeleteUser::copy() {
    DeleteUser * newAction = new DeleteUser();
    newAction->setStatus(*this);
    return newAction;
}

//-------------------------DuplicateUser---------------//

void DuplicateUser::act(Session &sess) {
    sess.addAction(this);
    string originalUsername;
    string newUsername;
    cin>>originalUsername;
    cin>>newUsername;

    if (!isUsernameExist(sess,originalUsername))
    {
        error("original name doesnt exist");
    }
    else if(isUsernameExist(sess,newUsername) ){
        error("new name already exist");
    }
    else{
        User* newUser = sess.getUsers()[originalUsername]->duplicate(newUsername,sess);
        sess.getUsers().insert(pair<string,User*>(newUsername,newUser));
        complete();
    }

}
string DuplicateUser::toString() const {
    return "DuplicateUser " + getStringStatus();
}

DuplicateUser *DuplicateUser::copy() {
    DuplicateUser * newAction = new DuplicateUser();
    newAction->setStatus(*this);
    return newAction;
}

//-------------------------PrintContentList---------------//

void PrintContentList::act(Session &sess) {
    sess.addAction(this);
    for (int i = 0; (unsigned)i < (*sess.getContent()).size() ; i++) {
        std::string a = std::to_string(i + 1) + ". " + (*sess.getContent())[i]->toString() + " " + std::to_string((*sess.getContent())[i]->get_Length())
                        + " minutes "+ ((*sess.getContent())[i]->getStringTags());
        cout << a << endl;
    }
    complete();
}
string PrintContentList::toString() const {
    return "PrintContentList " + getStringStatus();
}

PrintContentList *PrintContentList::copy() {
    PrintContentList * newAction = new PrintContentList();
    newAction->setStatus(*this);
    return newAction;
}

//-------------------------PrintWatchHistory---------------//

void PrintWatchHistory::act(Session &sess) {
    sess.addAction(this);
    cout<<"Watch history for " + sess.getActiveUser()->getName()<<endl;
    for (int i = 0; (unsigned)i < sess.getActiveUser()->get_history().size() ; i++) {
        string a = std::to_string(i + 1) + ". " + (*(sess.getActiveUser())).get_history()[i]->toString();
        cout << a << endl;
    }
    complete();
}
string PrintWatchHistory::toString() const {
    return "PrintWatchHistory "+ getStringStatus();
}

PrintWatchHistory *PrintWatchHistory::copy() {
    PrintWatchHistory * newAction = new PrintWatchHistory();
    newAction->setStatus(*this);
    return newAction;
}

//-------------------------Watch---------------//
Watch::Watch() : isContinuedWatch(false),lastRecommended("") {

}

void Watch::act(Session &sess) {
    sess.addAction(this);
    Watchable *toWatch;

    if (!isContinuedWatch)
    {
        int id;
        cin>>id;
        if(id<1||(unsigned)id>(*sess.getContent()).size()+1) {
            error("dont have movie/tv series with that id");
            return;
        }
        else
        {
            toWatch=(*sess.getContent())[id-1];
            std::string watchName=toWatch->toString();
            cout<<"Watching "+watchName<<endl;
            sess.getActiveUser()->addWatched(toWatch);
            complete();
        }
    }
    else
    {
        cout<<"Watching "+lastRecommended<<endl;
        for(auto & a:*sess.getContent()){
            if (a->getName()==lastRecommended){
                sess.getActiveUser()->addWatched(a);
                toWatch = a;
            }
        }
        complete();
    }
    Watchable* nextWatch = toWatch->getNextWatchable(sess);
    if(nextWatch != nullptr) {
        string newRec = nextWatch->getName();
        cout << "We recommend watching " + newRec + ", continue watching? [y/n]" << endl;
        string answer;
        cin >> answer;
        if (answer == "y") {
            Watch *watchAction = new Watch();
            watchAction->isContinuedWatch = true;
            watchAction->lastRecommended = newRec;
            watchAction->act(sess);
        }
    }
    nextWatch = nullptr;
}
string Watch::toString() const {
    return "Watch " + getStringStatus();
}

Watch *Watch::copy() {
    Watch * newAction = new Watch();
    newAction->setStatus(*this);
    return newAction;
}

//-------------------------PrintActionsLog---------------//

void PrintActionsLog::act(Session &sess) {
    sess.addAction(this);
    for (int i = 0; (unsigned)i < sess.getActionsLog().size() -1; i++) {
        cout<<sess.getActionsLog()[i]->toString()<<endl;
    }
    complete();
    cout<<sess.getActionsLog()[sess.getActionsLog().size()-1]->toString()<<endl;
}
string PrintActionsLog::toString() const {
    return "PrintActionsLog " + getStringStatus();
}

PrintActionsLog *PrintActionsLog::copy() {
    PrintActionsLog * newAction = new PrintActionsLog();
    newAction->setStatus(*this);
    return newAction;
}

//-------------------------Exit---------------//

void Exit::act(Session &sess) {
    sess.addAction(this);
    complete();
}
string Exit::toString() const {
    return "Exit " + getStringStatus();
}

Exit *Exit::copy() {
    Exit* newAction = new Exit();
    newAction->setStatus(*this);
    return newAction;
}