#pragma once

#include "Settings.h"
#include "GameBody.h"

class Checkpoint : public GameBody
{
public:
    Checkpoint(Scene &scene);

    void Start() override;
    void Render() override;
    void OnCollisionEnter(GameCollision &collision) override;
    int getId() const;

private:
    RE_Animator m_animator;
    bool m_isActive;
    bool empty;
    int m_id;
    
    friend class LevelParser;
    friend class LevelScene;
};

inline int Checkpoint::getId() const
{
    return m_id;
}


