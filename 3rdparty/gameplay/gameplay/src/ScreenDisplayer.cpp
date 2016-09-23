#include "Base.h"
#include "ScreenDisplayer.h"

namespace gameplay
{

ScreenDisplayer* ScreenDisplayer::__scriptInstance = NULL;

ScreenDisplayer::ScreenDisplayer() : _time(0L), _startTime(0)
{
}

ScreenDisplayer::~ScreenDisplayer()
{
    std::chrono::microseconds elapsedTime = Game::getInstance()->getGameTime() - _startTime;
    if (elapsedTime < _time)
        Platform::sleep(_time - elapsedTime);
}

void ScreenDisplayer::start(const std::chrono::microseconds& time)
{
    if (__scriptInstance == NULL)
    {
        __scriptInstance = new ScreenDisplayer();
    }

    __scriptInstance->_time = time;
    Game::getInstance()->renderOnce();
    __scriptInstance->_startTime = Game::getInstance()->getGameTime();
}

void ScreenDisplayer::finish()
{
    SAFE_DELETE(__scriptInstance);
}



}