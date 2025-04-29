// Fill out your copyright notice in the Description page of Project Settings.


#include "DeBuffCameraShake.h"

UDeBuffCameraShake::UDeBuffCameraShake()
{
    // ��鸲 ���� ����
    OscillationDuration = 5.0f;  // ���� �ð�
    OscillationBlendInTime = 0.05f; // ���� �ε巴��
    OscillationBlendOutTime = 0.05f; // �� �ε巴��

    // ī�޶� ��鸲 (Pitch ����)
    RotOscillation.Pitch.Amplitude = 6.0f; // ��鸲 ũ��
    RotOscillation.Pitch.Frequency = 8.5f; // ��鸲 �ӵ�

    // ī�޶� ��鸲 (Yaw ����)
    RotOscillation.Yaw.Amplitude = 5.0f;
    RotOscillation.Yaw.Frequency = 6.0f;
}