#include <engine/M22Engine.h>

std::vector<Mix_Chunk*> M22Sound::SOUND_FX;
std::vector<Mix_Music*> M22Sound::MUSIC;
float* M22Sound::MUSIC_VOLUME = &M22Engine::OPTIONS.MUSIC_VOLUME;
float* M22Sound::SFX_VOLUME = &M22Engine::OPTIONS.SFX_VOLUME;
int M22Sound::currentTrack = 0;
std::vector<std::string> M22Sound::MUSIC_NAMES;
std::vector<std::string> M22Sound::SFX_NAMES;

short int M22Sound::PlaySting(short int _position)
{
	if(M22Sound::SOUND_FX[_position])
	{
		if(!Mix_Playing(M22Sound::MIXERS::SFX))
		{
			Mix_PlayChannel( M22Sound::MIXERS::SFX, M22Sound::SOUND_FX[_position], 0);
			return 0;
		}
		else
		{
			return -2;
		};
	}
	else
	{
		return -1;
	};
};

short int M22Sound::InitializeMusic()
{
	std::fstream input("sfx/music/index.txt");
	int length;
	if(input)
	{
		length=int(std::count(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>(), '\n'));
		length++; // Linecount is number of '\n' + 1
		input.seekg(0, std::ios::beg);
		for(int i = 0; i < length; i++)
		{
			std::string currentfile;
			input >> currentfile;
			Mix_Music *temp = Mix_LoadMUS(currentfile.c_str());
			if(!temp)
			{
				std::cout << "Failed to load file: " << currentfile << std::endl;
				return -1;
			};
			M22Sound::MUSIC_NAMES.push_back(currentfile);
			M22Sound::MUSIC.push_back(temp);
		};
	}
	else
	{
		std::cout << "Failed to load index file for music!" << std::endl;
		return -1;
	};
	input.close();
	return 0;
};

short int M22Sound::InitializeSFX()
{
	std::fstream input("sfx/stings/index.txt");
	int length;
	if(input)
	{
		length=int(std::count(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>(), '\n'));
		length++; // Linecount is number of '\n' + 1
		input.seekg(0, std::ios::beg);
		for(int i = 0; i < length; i++)
		{
			std::string currentfile;
			input >> currentfile;
			Mix_Chunk *temp = Mix_LoadWAV(currentfile.c_str());
			if(!temp)
			{
				std::cout << "Failed to load file: " << currentfile << std::endl;
				return -1;
			};
			M22Sound::SOUND_FX.push_back(temp);
			M22Sound::SFX_NAMES.push_back(currentfile);
			//Mix_VolumeChunk(M22Sound::SOUND_FX[i], int(MIX_MAX_VOLUME*M22Sound::SFX_VOLUME));
		};
	}
	else
	{
		std::cout << "Failed to load index file for music!" << std::endl;
		return -1;
	};
	input.close();
	return 0;
};

short int M22Sound::InitializeSound()
{
	if(M22Sound::InitializeMusic() != 0)
	{
		printf("Failed to initialize music!");
		return -1;		
	};
	if(M22Sound::InitializeSFX() != 0)
	{
		printf("Failed to initialize SFX!");
		return -2;		
	};
	return 0;
};

void M22Sound::UpdateSound()
{
	if( !Mix_PlayingMusic() )
	{
		M22Sound::ChangeMusicTrack(M22Sound::currentTrack);
	};
	return;
};

short int M22Sound::PlaySting(short int _position, bool _forceplayback)
{
	if(M22Sound::SOUND_FX[_position])
	{
		if(!Mix_Playing(M22Sound::MIXERS::SFX) || _forceplayback == true)
		{
			Mix_PlayChannel( M22Sound::MIXERS::SFX, M22Sound::SOUND_FX[_position], 0);
			return 0;
		}
		else
		{
			return -2;
		};
	}
	else
	{
		return -1;
	};
};

void M22Sound::StopLoopedStings(void)
{
	Mix_HaltChannel(M22Sound::MIXERS::LOOPED_SFX);
	return;
};

short int M22Sound::PlayLoopedSting(short int _position)
{
	if(M22Sound::SOUND_FX[_position])
	{
		Mix_PlayChannel( M22Sound::MIXERS::LOOPED_SFX, M22Sound::SOUND_FX.at(_position), -1);
		return 0;
	}
	else
	{
		return -1;
	};
};

short int M22Sound::PlayLoopedSting(std::string _name)
{
	for(size_t i = 0; i < M22Sound::SOUND_FX.size(); i++)
	{
		if(_name == M22Sound::SFX_NAMES[i])
		{
			if(!Mix_Playing(M22Sound::MIXERS::LOOPED_SFX))
			{
				Mix_PlayChannel( M22Sound::MIXERS::LOOPED_SFX, M22Sound::SOUND_FX[i], -1);
				return 0;
			};
			return -2;
		};
	};
	return -1;
};

/*
	0 = success
	-1 = sound doesn't exist
	-2 = cannot play, already playing
*/
short int M22Sound::PlaySting(std::string _name, bool _forceplayback)
{
	for(size_t i = 0; i < M22Sound::SOUND_FX.size(); i++)
	{
		if(_name == M22Sound::SFX_NAMES[i])
		{
			if(!Mix_Playing(M22Sound::MIXERS::SFX) || _forceplayback == true)
			{
				Mix_PlayChannel( M22Sound::MIXERS::SFX, M22Sound::SOUND_FX[i], 0);
				return 0;
			};
			return -2;
		};
	};
	printf("Failed to find and play sting: %s", _name.c_str());
	return -1;
};

short int M22Sound::ChangeMusicTrack(std::string _name)
{
	for(size_t i = 0; i < M22Sound::MUSIC.size(); i++)
	{
		if(_name == M22Sound::MUSIC_NAMES.at(i))
		{
			Mix_PlayMusic( M22Sound::MUSIC.at(i), -1 );
			M22Sound::currentTrack = i;
			return 0;
		};
	};
	printf("Failed to find music file: %s", _name.c_str());
	M22Sound::StopMusic();
	return -1;
};

short int M22Sound::ChangeMusicTrack(short int _position)
{
	if(M22Sound::MUSIC.at(_position))
	{
		Mix_PlayMusic( M22Sound::MUSIC.at(_position), -1 );
		M22Sound::currentTrack = _position;
		return 0;
	}
	else
	{
		return -1;
	};
};

void M22Sound::StopMusic()
{
	Mix_HaltMusic();
	M22Sound::currentTrack = 0;
	return;
};

void M22Sound::PauseMusic()
{
	Mix_PauseMusic();
	return;
};

void M22Sound::ResumeMusic()
{
	Mix_ResumeMusic();
	return;
};