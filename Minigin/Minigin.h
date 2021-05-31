#pragma once
struct SDL_Window;

namespace Horizon
{
	class Minigin
	{
	public:
		void Initialize();
		void Cleanup();
		void Run();
	private:
		SDL_Window* m_pWindow{};
	};
}