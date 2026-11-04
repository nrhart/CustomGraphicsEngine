#pragma once
#include <string>
#include <Windows.h>

namespace eae6320
{
	namespace Audio
	{
		class cAudio
		{
		public:
			// Initialize / Shutdown
			static void Initialize();
			static void Shutdown();

			// Background music controls
			static void PlayMusic(const char* path, bool loop = true, float volume = 1.0f);
			static void PauseMusic();
			static void ResumeMusic();

			// Sound effects (can play on top of music)
			static void PlaySoundEffect(const char* path);

		private:
			static bool s_isMusicPlaying;
			static bool s_isMusicPaused;
			static bool s_shouldLoop;
			static std::string s_currentMusicPath;
		};
	}
}
