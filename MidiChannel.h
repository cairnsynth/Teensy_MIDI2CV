#include "elapsedMillis.h"
#ifndef MIDICHANNEL_H
#define MIDICHANNEL_H
class MidiChannel {
  public:
    MidiChannel() {};

    void set_channel_number(int channelNumber)
    {
      _channelNumber = channelNumber;
    }
    int get_channel_number()
    {
      return _channelNumber;
    }
    void set_note(int note)
    {
      _note = note;
      calculate_output_note(_note);
      gateTimer = 0;
    }
    int get_note()
    {
      return _note;
    }
    int get_output_note()
    {
      return _outputNote;
    }
    void set_velocity(int velocity)
    {
      _velocity = velocity;
    }
    int get_velocity()
    {
      return _velocity;
    }
    void set_gate(bool gate)
    {
      _gate = gate;
      digitalWrite(_gatePin, _gate);
    }
    bool get_gate()
    {
      return _gate;
    }
    void set_pitch_bend(int pitchBend)
    {
      _pitchBend = pitchBend;
    }
    int get_pitch_bend()
    {
      return _pitchBend;
    }
    void set_offset(int offset)
    {
      _offset = offset;
    }
    int get_offset()
    {
      return _offset;
    }
    void set_octave(int octave)
    {
      _octave = octave;
    }
    int get_octave()
    {
      return _octave;
    }
    void set_gate_pin(int gatePin)
    {
      _gatePin = gatePin;
    }
    int get_gate_pin()
    {
      return _gatePin;
    }

    bool gateMode = 0;  // 0 = Gate, 1 = Trigger
    long unsigned int gateMillis = 25;
    elapsedMillis gateTimer;

  private:
    int _channelNumber = 0;
    int _note = 0;
    int _outputNote = 0;
    int _velocity = 0;
    bool _gate = 0;
    int _pitchBend = 0;
    int _offset = 0;
    int _octave = 0;

    int _gatePin = 32;

    void calculate_output_note(int note)
    {
      while(note < 60) { note +=12; }
      while(note > 96) { note -=12; }
      _outputNote = note + _offset - (_octave * 12);
    }
};

#endif  // MIDICHANNEL_H