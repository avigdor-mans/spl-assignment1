//
// Created by aviv on 24/11/2019.
//

#ifndef ASS1_ACTION_H
#define ASS1_ACTION_H


#include <string>
#include <iostream>
class Session;

enum ActionStatus{
    PENDING, COMPLETED, ERROR
};


class BaseAction{
public:
    BaseAction();
    virtual ~BaseAction();
    ActionStatus getStatus() const;
    std::string getStringStatus() const ;
    virtual void act(Session& sess)=0;
    virtual std::string toString() const=0;
    bool isUsernameExist(Session& sess,std::string);
    virtual BaseAction* copy()=0;
    void setStatus(BaseAction&);
protected:
    void complete();
    void error(const std::string& errorMsg);
    std::string getErrorMsg() const;
private:
    std::string errorMsg;
    ActionStatus status;
};

class CreateUser  : public BaseAction {
public:
    virtual void act(Session& sess);
    virtual std::string toString() const;
    virtual CreateUser* copy();
};

class ChangeActiveUser : public BaseAction {
public:
    virtual void act(Session& sess);
    virtual std::string toString() const;
    virtual ChangeActiveUser* copy();

};

class DeleteUser : public BaseAction {
public:
    virtual void act(Session & sess);
    virtual std::string toString() const;
    virtual DeleteUser* copy();

};


class DuplicateUser : public BaseAction {
public:
    virtual void act(Session & sess);
    virtual std::string toString() const;
    virtual DuplicateUser* copy();
};

class PrintContentList : public BaseAction {
public:
    virtual void act (Session& sess);
    virtual std::string toString() const;
    virtual PrintContentList* copy();
};

class PrintWatchHistory : public BaseAction {
public:
    virtual void act (Session& sess);
    virtual std::string toString() const;
    virtual PrintWatchHistory* copy();

};


class Watch : public BaseAction {
public:
    Watch();
    virtual void act(Session& sess);
    virtual std::string toString() const;
    virtual Watch* copy();

private:
    bool isContinuedWatch;
    std::string lastRecommended;
};


class PrintActionsLog : public BaseAction {
public:
    virtual void act(Session& sess);
    virtual std::string toString() const;
    virtual PrintActionsLog* copy();

};

class Exit : public BaseAction {
public:
    virtual void act(Session& sess);
    virtual std::string toString() const;
    virtual Exit* copy();

};

#endif //ASS1_ACTION_H