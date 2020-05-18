//
// Created by aviv on 19/11/2019.
//
#include <sstream>
#include <iterator>
#include "../include/Watchable.h"
#include "../include/User.h"
#include "../include/Session.h"

//----------------------Watchable----------------//

Watchable::Watchable(long _id, int _length, const std::vector<std::string> &_tags) : id(_id), length(_length),
                                                                                     tags(_tags) {

}
Watchable::Watchable(const Watchable & other) : id(other.id), length(other.length),tags(other.tags) {

}




int Watchable::get_Length() const {
    return length;
}
std::vector<std::string>& Watchable::getTags() {
    return tags;
}
std::string Watchable::getStringTags()const{
    std::string stringTags = "[";
    for(auto &tag: tags){
        stringTags=stringTags+tag+", ";
    }
    stringTags=stringTags.substr(0,stringTags.size()-2)+"]";
    return stringTags;
}
long Watchable::getId()const{
    return id;
}

Watchable::~Watchable() = default;


//------------------movie------------------//


Movie::Movie(long _id, const std::string& _name, int _length, const std::vector<std::string>& _tags) : Watchable(_id, _length, _tags), name(_name)
{

}
std::string Movie::toString() const {
    std::string movie = name;
    return movie;
}
Watchable* Movie::getNextWatchable(Session & session) const {
    return session.getActiveUser()->getRecommendation(session);
}

std::string Movie::getName()const{
    std::string a = name;
    return a;
}
long Movie::getId() const{
    return Watchable::getId();
}
Movie* Movie::copy() {
    return new Movie(getId(),getName(),get_Length(),getTags());
}
//-------------------------------episode-------------------//
Episode::Episode(long id, const std::string &seriesName, int length, int season, int episode,
        const std::vector<std::string> &tags) : Watchable(id,length,tags),
        seriesName(seriesName),season(season),episode(episode), nextEpisodeId()
{

}
std::string Episode::toString() const {
    std::string episod = seriesName + " S" + std::to_string(season) + "E" + std::to_string(episode);
    return episod;
}
Watchable* Episode::getNextWatchable(Session & session) const {
    if (nextEpisodeId == -1)
        return session.getActiveUser()->getRecommendation(session);
    return (*session.getContent())[nextEpisodeId-1];

}
std::string Episode::getName()const{
    std::string a = seriesName +" S"+std::to_string(season)+"E"+std::to_string(episode);
    return a;
}
long Episode::getId() const{
    return Watchable::getId();
}
void Episode::setNextId(long nextId) {
    nextEpisodeId = nextId;
}
Episode* Episode::copy() {

    std::istringstream iss(getName());
    std::vector<std::string> results(std::istream_iterator<std::string>{iss},
                                     std::istream_iterator<std::string>());
    string seriesName;
    for (int i = 0; (unsigned )i < results.size() -1; ++i) {
        seriesName = seriesName + results[i] + " ";
    }
    seriesName= seriesName.substr(0,seriesName.size()-1);
    Episode* newEpisode = new Episode(getId(),seriesName,get_Length(),season,episode,getTags());
    newEpisode->setNextId(nextEpisodeId);
    return newEpisode;
}
