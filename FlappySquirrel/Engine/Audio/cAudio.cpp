#include "cAudio.h"
#include <mmsystem.h>
#include <fstream>
#include <vector>
#include <filesystem>
#include <string>
#pragma comment(lib, "winmm.lib")

namespace eae6320
{
	namespace Audio
	{
		bool cAudio::s_isMusicPlaying = false;
		bool cAudio::s_isMusicPaused = false;
		bool cAudio::s_shouldLoop = false;
		std::string cAudio::s_currentMusicPath = "";

		void cAudio::Initialize()
		{
			// Nothing special for now
		}

		void cAudio::Shutdown()
		{
			// Stop and close background music
			mciSendString(L"close bgm", nullptr, 0, nullptr);
			s_isMusicPlaying = false;
			s_isMusicPaused = false;
			s_currentMusicPath.clear();
		}

		void cAudio::PlayMusic(const char* path, bool loop, float volume)
		{
			s_currentMusicPath = path;
			s_isMusicPlaying = true;
			s_isMusicPaused = false;
			s_shouldLoop = loop;

			std::string strPath(path);

			// Helper lambda for playing a wide path via MCI
			auto PlayWithMCI = [&](const std::wstring& wPath)
				{
					mciSendString(L"close bgm", nullptr, 0, nullptr);

					std::wstring cmd = L"open \"" + wPath + L"\" type mpegvideo alias bgm";
					mciSendString(cmd.c_str(), nullptr, 0, nullptr);

					if (loop)
						mciSendString(L"play bgm repeat", nullptr, 0, nullptr);
					else
						mciSendString(L"play bgm", nullptr, 0, nullptr);
				};

			// Check for .audio files
			if (strPath.size() >= 6 && strPath.compare(strPath.size() - 6, 6, ".audio") == 0)
			{
				// Read header and extract WAV data
				std::ifstream file(path, std::ios::binary);
				if (!file.is_open())
				{
					OutputDebugStringA(("Failed to open audio: " + strPath + "\n").c_str());
					return;
				}

				struct sAudioHeader
				{
					uint32_t magic;
					uint32_t dataSize;
					uint8_t loop;
					float volume;
				} header{};

				file.read(reinterpret_cast<char*>(&header), sizeof(header));

				if (header.magic != 'AUD0')
				{
					OutputDebugStringA(("Invalid .audio header: " + strPath + "\n").c_str());
					return;
				}

				std::vector<char> wavData(header.dataSize);
				file.read(wavData.data(), header.dataSize);
				file.close();

				// Write temporary WAV
				std::filesystem::path tempPath = std::filesystem::temp_directory_path() / "temp_music.wav";
				std::ofstream out(tempPath, std::ios::binary);
				out.write(wavData.data(), header.dataSize);
				out.close();

				// Convert to wide and play with MCI
				std::wstring wTempPath(tempPath.wstring());
				PlayWithMCI(wTempPath);
				return;
			}

			// ===============================
			// Otherwise: play normal WAV/MP3
			// ===============================
			std::wstring wPath(std::begin(s_currentMusicPath), std::end(s_currentMusicPath));
			PlayWithMCI(wPath);
		}

		void cAudio::PauseMusic()
		{
			if (s_isMusicPlaying && !s_isMusicPaused)
			{
				mciSendString(L"pause bgm", nullptr, 0, nullptr);
				s_isMusicPaused = true;
			}
		}

		void cAudio::ResumeMusic()
		{
			if (s_isMusicPaused)
			{
				mciSendString(L"resume bgm", nullptr, 0, nullptr);
				s_isMusicPaused = false;
			}
		}

		// ============================
		// NEW: Helper for .audio files
		// ============================
		struct sAudioHeader
		{
			uint32_t magic;
			uint32_t dataSize;
			uint8_t loop;
			float volume;
		};

		static bool PlayAudioFile(const char* path)
		{
			std::ifstream file(path, std::ios::binary);
			if (!file.is_open())
			{
				OutputDebugStringA(("Failed to open audio: " + std::string(path) + "\n").c_str());
				return false;
			}

			sAudioHeader header{};
			file.read(reinterpret_cast<char*>(&header), sizeof(header));

			// Check magic
			if (header.magic != 'AUD0')
			{
				OutputDebugStringA(("Invalid audio header: " + std::string(path) + "\n").c_str());
				return false;
			}

			std::vector<char> wavData(header.dataSize);
			file.read(wavData.data(), header.dataSize);
			file.close();

			// Write to temp .wav file
			std::filesystem::path tempPath = std::filesystem::temp_directory_path() / "temp_audio.wav";
			std::ofstream out(tempPath, std::ios::binary);
			out.write(wavData.data(), header.dataSize);
			out.close();

			PlaySoundA(tempPath.string().c_str(), nullptr, SND_FILENAME | SND_ASYNC);
			return true;
		}

		void cAudio::PlaySoundEffect(const char* path)
		{
			std::string strPath(path);
			if (strPath.size() >= 6 && strPath.compare(strPath.size() - 6, 6, ".audio") == 0)
			{
				if (!PlayAudioFile(path))
				{
					OutputDebugStringA(("Failed to play .audio file: " + strPath + "\n").c_str());
				}
				return;
			}

			// Fallback: normal WAV or MP3 file
			PlaySoundA(path, nullptr, SND_FILENAME | SND_ASYNC);
		}
	}
}
