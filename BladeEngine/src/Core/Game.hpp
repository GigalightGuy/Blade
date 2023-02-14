#pragma once

extern int main();

namespace BladeEngine
{
    class Game
    {
    public:
        Game();
        virtual ~Game() = default;

    private:
        virtual void Update() = 0;

        void Run();

        bool m_ShouldExit = false;

        friend int ::main();
    };

    extern Game* CreateGameInstance();
    
}