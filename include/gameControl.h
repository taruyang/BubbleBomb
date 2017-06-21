#ifndef GAMECONTROL_H_INCLUDED
#define GAMECONTROL_H_INCLUDED

#define GLEW_NO_GLU
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace gl{

/**
 * @brief Class for Game control
 *        This class contains internal status and presents helper methods for game.
 */
class GameControl
{
    const int   PLAYER_ENERGY = 5;
    const int   ENERMY_ENERGY = 30;
    int         _enermyEnergy;
    int         _playerEnergy;
    int         _stage;
    double      _damagedTime;
    double      _stageClearTime;
    double      _shotTime;
    bool        _isFirst;

public:
    GameControl(){
        _playerEnergy = PLAYER_ENERGY;
        _enermyEnergy = ENERMY_ENERGY;
        _stage = 1;
        _damagedTime = 0.f;
        _stageClearTime = 0.f;
        _shotTime = 0.f;
        _isFirst = true;
    };

    ~GameControl(){};

    void AttackEnermy(){
        _enermyEnergy--;
        if(_enermyEnergy <= 0)
        {
            GotoNextStage();
            _stageClearTime = glfwGetTime();
        }
    }

    void Damaged(){
        _playerEnergy--;
        _damagedTime = glfwGetTime();
    }

    void Shoot(){
        _shotTime = glfwGetTime();
    }

    bool IsShotStatus() {
        if(_shotTime > 0.f && (glfwGetTime() - _shotTime) < 0.15f)
            return true;
        else
            return false;
    }

    bool IsDamagedStatus() {
        if(_damagedTime > 0.f && (glfwGetTime() - _damagedTime) < 1.5f)
            return true;
        else
            return false;
    }

    bool IsStageClearedStatus() {
        if(_isFirst) {
            _stageClearTime = glfwGetTime();
            _isFirst = false;
        }

        if(_stageClearTime > 0.f && (glfwGetTime() - _stageClearTime) < 0.5f)
            return true;
        else
            return false;
    }

    int GetEnermyEnergy() { return _enermyEnergy; }
    int GetPlayerEnergy() { return _playerEnergy; }
    int GetCurStage() {return _stage; }

    bool IsGameEnded(){
        if(_playerEnergy <= 0) return true;
        else return false;
    }

    void Restart() {
        _playerEnergy = PLAYER_ENERGY;
        _enermyEnergy = ENERMY_ENERGY;
        _stage = 1;
    }

    void GotoNextStage() {
        _stage++;
        _playerEnergy = PLAYER_ENERGY;
        _enermyEnergy = ENERMY_ENERGY * _stage;
    }

};

}

#endif // GAMECONTROL_H_INCLUDED
