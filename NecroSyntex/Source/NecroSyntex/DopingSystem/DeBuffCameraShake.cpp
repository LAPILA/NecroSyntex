// Fill out your copyright notice in the Description page of Project Settings.


#include "DeBuffCameraShake.h"

UDeBuffCameraShake::UDeBuffCameraShake()
{
    // ��鸲 ���� ����
    OscillationDuration = 5.0f;  // ���� �ð�
    OscillationBlendInTime = 0.05f; // ���� �ε巴��
    OscillationBlendOutTime = 0.05f; // �� �ε巴��

    // ī�޶� ��鸲 (Pitch ����)
    RotOscillation.Pitch.Amplitude = 8.0f; // ��鸲 ũ��
    RotOscillation.Pitch.Frequency = 10.0f; // ��鸲 �ӵ�

    // ī�޶� ��鸲 (Yaw ����)
    RotOscillation.Yaw.Amplitude = 8.0f;
    RotOscillation.Yaw.Frequency = 7.0f;
}