//
// Created by aviv on 24/11/2019.
//

#ifndef ASS1_WATCHABLE_H
#define ASS1_WATCHABLE_H

#include <string>
#include <vector>


class Session;

class Watchable{
public:
    Watchable(long id, int length, const std::vector<std::string>& tags);
    Watchable(const  Watchable&);
    virtual ~ Watchable();
    virtual int get_Length() const;
    virtual std::string toString() const = 0;
    virtual Watchable* getNextWatchable(Session&) const = 0;
    virtual std::vector<std::string>& getTags();
    std::string getStringTags() const;
    virtual std::string getName()const = 0;
    virtual long getId()const;
    virtual Watchable* copy() =0;
private:
    const long id;
    int length;
    std::vector<std::string> tags;
};

class Movie : public Watchable{
public:
    Movie(long id, const std::string& name, int length, const std::vector<std::string>& tags);
    virtual std::string toString() const;
    virtual Watchable* getNextWatchable(Session&) const;
    virtual std::string getName()const;
    virtual long getId() const;
    virtual Movie* copy();
private:
    std::string name;
};


class Episode: public Watchable{
public:
    Episode(long id, const std::string& seriesName,int length, int season, int episode ,const std::vector<std::string>& tags);
    virtual std::string toString() const;
    virtual Watchable* getNextWatchable(Session&) const;
    virtual std::string getName()const;
    virtual long getId() const;
    void setNextId(long);
    virtual Episode* copy();
private:
    std::string seriesName;
    int season;
    int episode;
    long nextEpisodeId;
};

#endif //ASS1_WATCHABLE_H