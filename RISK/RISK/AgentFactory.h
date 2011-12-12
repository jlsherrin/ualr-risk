#ifndef AgentFactory_H
#define AgentFactory_H

//All new AGENTS must 
#include "RiskAgent.h"
class RiskAgent;


class Factory
{
public:
    virtual RiskAgent* create() = 0;
};


template <class agent_type>
class AgentFactory : public Factory
{
public:
   RiskAgent* create(){ return new agent_type(); }
};

#endif