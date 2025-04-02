// Fill out your copyright notice in the Description page of Project Settings.


#include "DeBuffCameraShake.h"

UDeBuffCameraShake::UDeBuffCameraShake()
{
    // 흔들림 강도 설정
    OscillationDuration = 5.0f;  // 지속 시간
    OscillationBlendInTime = 0.05f; // 시작 부드럽게
    OscillationBlendOutTime = 0.05f; // 끝 부드럽게

    // 카메라 흔들림 (Pitch 방향)
    RotOscillation.Pitch.Amplitude = 8.0f; // 흔들림 크기
    RotOscillation.Pitch.Frequency = 10.0f; // 흔들림 속도

    // 카메라 흔들림 (Yaw 방향)
    RotOscillation.Yaw.Amplitude = 8.0f;
    RotOscillation.Yaw.Frequency = 7.0f;
}