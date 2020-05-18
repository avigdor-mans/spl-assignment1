//
// Created by aviv on 24/11/2019.
//

#ifndef ASS1_USER_H
#define ASS1_USER_H

#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>
class Watchable;
class Session;

class User{
public:
    User(const std::string& name);
    User(const User &user);
    User&operator=(const User&);
    User(User &&other);
    User&operator =(User &&other);
    virtual ~User();
    void clear();
    virtual Watchable* getRecommendation(Session& s) = 0;
    std::string getName() const;
    std::vector<Watchable*> get_history() const;
    void addWatched(Watchable*);
    virtual User* duplicate(std::string,Session&) = 0;
protected:
    std::vector<Watchable*> history;
private:
    std::string name;

};


class LengthRecommenderUser : public User {
public:
    LengthRecommenderUser(const std::string& name);
    LengthRecommenderUser(const LengthRecommenderUser &lUser);
    virtual Watchable* getRecommendation(Session& s);
    virtual LengthRecommenderUser* duplicate(std::string,Session&);
private:
};

class RerunRecommenderUser : public User {
public:
    RerunRecommenderUser(const std::string& name);
    RerunRecommenderUser(const RerunRecommenderUser &rUser);
    virtual Watchable* getRecommendation(Session& s);
    virtual RerunRecommenderUser* duplicate(std::string,Session&);
private:
    int indexToRecommended;
};

class GenreRecommenderUser : public User {
public:
    GenreRecommenderUser(const std::string& name);
    GenreRecommenderUser(const GenreRecommenderUser &gUser);
    virtual Watchable* getRecommendation(Session& s);
    virtual GenreRecommenderUser* duplicate(std::string,Session&);
private:
};

#endif //ASS1_USER_H