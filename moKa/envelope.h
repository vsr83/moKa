#ifndef ENVELOPE_H
#define ENVELOPE_H


class Envelope {
public:
    Envelope();
    Envelope(double _attackTime,
             double _decayTime,
             double _releaseTime,
             double _peakAmplitude,
             double _sustainAmplitude);

    enum {
        STATE_OFF,
        STATE_ATTACK,
        STATE_DECAY,
        STATE_RELEASE
    };
    enum {
        MODE_EXP,
        MODE_LINEAR
    };

    void trigger(double t);
    void release(double t);
    bool isFinished();
    unsigned int getState();

    double eval(double t);
    void setMode(unsigned int _modeAttack,
                 unsigned int _modeDecay,
                 unsigned int _modeRelease);

    double attackTime;
    double decayTime;
    double releaseTime;
    double peakAmplitude;
    double sustainAmplitude;
private:
    unsigned int state;
    unsigned int modeAttack;
    unsigned int modeDecay;
    unsigned int modeRelease;

    double amplitudeAtRelease;
    double timeAtTrigger;
    double timeAtRelease;
};

#endif
