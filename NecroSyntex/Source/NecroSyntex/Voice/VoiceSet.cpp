#include "VoiceSet.h"

UVoiceSet::UVoiceSet()
{
	for (uint8 i = 0; i < static_cast<uint8>(EVoiceCue::Count); ++i)
	{
		const EVoiceCue Cue = static_cast<EVoiceCue>(i);
		VoiceTable.FindOrAdd(Cue);
	}
}

void UVoiceSet::GetVoice(EVoiceCue Cue,
    USoundBase*& OutSound,
    USoundAttenuation*& OutAttenuation,
    USoundClass*& OutSoundClass) const
{
    if (const FVoiceLines* Entry = VoiceTable.Find(Cue))
    {
        OutSound = Entry->GetRandomClip();
        OutAttenuation = SharedAttenuation;
        OutSoundClass = SharedSoundClass;
    }
    else
    {
        OutSound = nullptr;
        OutAttenuation = SharedAttenuation;
        OutSoundClass = SharedSoundClass;
    }
}
