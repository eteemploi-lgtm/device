#pragma once
#include <QString>
#include <QList>

// ─────────────────────────────────────────────
//  Structures de données partagées
// ─────────────────────────────────────────────

enum class UserProfile { Operator, Administrator };

struct ConnectionConfig {
    QString chassis;      // ex. "PXI0"
    int     slot;         // ex. 1
    QString interface_;   // ex. "Thunderbolt 3"
    UserProfile profile;
    // QString visaResource; // ex. "PXI0::CHASSIS1::SLOT1::INSTR"
    //   → Décommenté quand NI-VISA est disponible
};

struct HydrophoneConfig {
    int     id;
    int     channel;
    double  gain;           // dB
    int     sampleRate;     // Hz
    double  dynamic;        // V (ex. 1.0, 2.0, 5.0, 10.0)
    double  distance;       // m hydro-transducteur
    double  depth;          // m immersion
    QString model;
    QString sensitivityFile;
};

struct AmplifierConfig {
    int     id;
    QString wavFile;
    QString filterFile;

    // Porteuse MLI
    QString carrierType;    // "Triangle" ou "Dent de scie"
    double  carrierFreq;    // Hz
    double  amplitude;      // 0..2
    double  offset;         // -1..+1

    // Température
    int     tempProbeCount;
    QString tempType;       // K, J, T, E, N, R, S, B
    double  tempAcqFreq;
    QString tempLabel;

    // Tension
    int     voltChannelCount;
    QString voltDivision;   // "1/10", "1/100", "1/1000"
    double  voltAcqFreq;
    QString voltLabel;

    // Courant
    int     currChannelCount;
    QString currDivision;
    double  currAcqFreq;
    QString currLabel;
};

struct SvConfig {
    int     listenChannel;
    int     hydrophoneId;
    double  gain;
    int     sampleRate;
    double  pitchMin, pitchMax;   // gîte °
    double  rollMin,  rollMax;    // site °
    double  angularResolution;    // °
    double  measureTime;          // ms
    double  freqMin, freqMax;     // Hz
    double  freqStep;             // Hz
    double  hydroDepth;           // m
    double  transducerDepth;      // m
    double  distance;             // m
};

struct SystemConfig {
    ConnectionConfig         connection;
    QList<HydrophoneConfig>  hydrophones;
    QList<AmplifierConfig>   amplifiers;
    SvConfig                 sv;
};
