/*
  ==============================================================================
  
  This file is part of the dRowAudio JUCE module
  Copyright 2004-12 by dRowAudio.
  
  ------------------------------------------------------------------------------
 
  dRowAudio can be redistributed and/or modified under the terms of the GNU General
  Public License (Version 2), as published by the Free Software Foundation.
  A copy of the license is included in the module distribution, or can be found
  online at www.gnu.org/licenses.
  
  dRowAudio is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
  
  ==============================================================================
*/

#ifndef __DROWAUDIO_PITCH_H__
#define __DROWAUDIO_PITCH_H__

#include "dRowAudio_AudioUtility.h"

//==============================================================================
/**
    This class contains some useful methods for storing and converting different
    representations of a pitch.
 */
class Pitch
{
public:
    //==============================================================================
    /** Create a default pitch object with a frequency of 0 Hertz.
     */
    Pitch()
        : frequency (0.0)
    {
    }
    
    /** Create a pitch object with a given frequency in Hertz.
     */
    Pitch (double frequencyHz)
        : frequency (frequencyHz)
    {
    }

    /** Creates a copy of another pitch object.
     */
    Pitch (const Pitch& other)
    {
        frequency = other.frequency;
    }

    /** Assign the frequency of anther pitch object to this one.
     */
    Pitch& operator= (const Pitch& other) noexcept
    {
        frequency = other.frequency;
        return *this;
    }

    //==============================================================================
    /** Returns a unicode sharp symbol.
     */
    static const juce_wchar getSharpSymbol()    {   return *CharPointer_UTF8 ("\xe2\x99\xaf");  }
    
    /** Returns a unicode flat symbol.
     */
    static const juce_wchar getFlatSymbol()     {   return *CharPointer_UTF8 ("\xe2\x99\xad");  }
    
    //==============================================================================
    /** Creates a Pitch object from a given frequency in Hertz e.g 440.
     */
    template <typename Type>
    static Pitch fromFrequency (const Type frequencyHz) noexcept
    {
        Pitch pitch;
        pitch.frequency = (double) frequencyHz;
        
        return pitch;
    }

    /** Creates a Pitch object from a given midi note number e.g. 69.
     */
    template <typename Type>
    static Pitch fromMidiNote (const Type midiNote) noexcept
    {
        Pitch pitch;
        pitch.frequency = midiToFrequency ((double) midiNote);
        
        return pitch;
    }
    
    /** Creates a Pitch object from a given note name e.g. A#3.
        This should be the pitch class followed by the octave. The pitch class can
        contain sharps and flats in the form of either #, b or the unicode character
        equivalents.
     
        If the String can not be parsed properly this will return a Pitch with a
        frequency of 0.
     
        @see getSharpSymbol, getFlatSymbol
     */
    static Pitch fromNoteName (const String& noteName) noexcept
    {
        const String octaveName (noteName.retainCharacters ("0123456789"));
        const String pitchClassName (noteName.toLowerCase().retainCharacters (getValidPitchClassLetters()));
        
        const int octave = octaveName.getIntValue();
        const int pitchClass = getPitchClass (pitchClassName);
        const int midiNote = (octave * 12) + pitchClass;
        
        if (pitchClass > 0)
            return fromMidiNote (midiNote);
        else
            return Pitch (0.0);
    }

    //==============================================================================
    /** Returns the frequncy of the pitch in Hertz.
     */
    inline double getFrequencyHz() const noexcept
    {
        return frequency;
    }
    
    /** Returns the midi note of the pitch e.g. 440 = 69.
     */
    inline double getMidiNote() const noexcept
    {
        return frequencyToMidi (frequency);
    }

    /** Returns the note name of the pitch e.g. 440 = A4.
     */
    inline String getMidiNoteName() const noexcept
    {
        const int midiNote = (int) getMidiNote();
        const int pitchClass = midiNote % 12;
        const int octave = midiNote / 12 - 1;
        
        String noteName;
        noteName << getNoteName (pitchClass) << octave;

        return noteName;
    }

private:
    //==============================================================================
    double frequency;
    
    //==============================================================================
    /*  Converts a pitch class number in the range of 0-12 to a letter.
     */
    static String getNoteName (int pitchClass) noexcept
    {
        switch (pitchClass)
        {
            case 0:     return "C";
            case 1:     return "C#";
            case 2:     return "D";
            case 3:     return "D#";
            case 4:     return "E";
            case 5:     return "F";
            case 6:     return "F#";
            case 7:     return "G";
            case 8:     return "G#";
            case 9:     return "A";
            case 10:    return "A#";
            case 11:    return "B";
            default:    return String::empty;
        }
    }
        
    /*  Returns the pitch class number for a given string.
        If the string is not in the required format i.e. A#4 etc. this
        will return -1.
     */
    static int getPitchClass (const String& pitchClassName) noexcept
    {
        int pitchClass = -1;
        const int numChars = pitchClassName.length();
        
        if (numChars > 0)
        {
            const juce_wchar base = pitchClassName.toLowerCase()[0];

            switch (base)
            {
                case 'c':   pitchClass = 0;     break;
                case 'd':   pitchClass = 2;     break;
                case 'e':   pitchClass = 4;     break;
                case 'f':   pitchClass = 5;     break;
                case 'g':   pitchClass = 7;     break;
                case 'a':   pitchClass = 9;     break;
                case 'b':   pitchClass = 11;    break;
                default:    pitchClass = -1;    break;
            }
        }
        
        if (numChars > 1)
        {
            const juce_wchar sharpOrFlat = pitchClassName[1];
            
            switch (sharpOrFlat)
            {
                case '#':   ++pitchClass;       break;
                case 'b':   --pitchClass;       break;
                default:                        break;
            }

            if (sharpOrFlat == getSharpSymbol())
                ++pitchClass;
            else if (sharpOrFlat == getFlatSymbol())
                --pitchClass;
            
            pitchClass %= 12;
        }
        
        return pitchClass;
    }
    
    /*  Returns the letters valid for a pitch class.
     */
    static const String getValidPitchClassLetters()
    {
        String chars ("abcdefg#b");
        chars << getSharpSymbol() << getFlatSymbol();
        return chars;
    }
    
    //==============================================================================
    JUCE_LEAK_DETECTOR (Pitch);
};


#endif  // __DROWAUDIO_PITCH_H__
