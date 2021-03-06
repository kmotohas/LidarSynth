/*
 Copyright (C) 2016 Apple Inc. All Rights Reserved.
 See LICENSE.txt for this sample’s licensing information
 
 Abstract:
 Instrument AU
 */

/*
 This is a test implementation of a sin wave synth using AUInstrumentBase classes
 
 It illustrates a basic usage of these classes
 
 It artificially limits the number of notes at one time to 12, so the note-stealing
 algorithm is used - you should know how this works!
 
 Most of the work you need to do is defining a Note class (see TestNote). AUInstrument manages the
 creation and destruction of notes, the various stages of a note's lifetime.
 
 Alot of printfs have been left in (but are if'def out)
 These can be useful as you figure out how this all fits together. This is true in the AUInstrumentBase
 classes as well; simply define DEBUG_PRINT to 1 and this turns all this on
 
 The project also defines CA_AUTO_MIDI_MAP (OTHER_C_FLAGS). This adds all the code that is needed
 to map MIDI messages to specific parameter changes. This can be seen in AU Lab's MIDI Editor window
 CA_AUTO_MIDI_MAP is implemented in AUMIDIBase.cpp/.h
 */

#include "SinSynth.h"
#include <thread>
#include <zmq.hpp>
#include <iostream>
#include <sweep/sweep.hpp>
#include <boost/circular_buffer.hpp>

const int buf_size = 128;
boost::circular_buffer<std::int32_t> c_buf(buf_size);
float moving_average = 0;
float beta = 0.9;

static const UInt32 kMaxActiveNotes = 8;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const double twopi = 2.0 * 3.14159265358979;

inline double pow5(double x) { double x2 = x*x; return x2*x2*x; }

#pragma mark SinSynth Methods

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//COMPONENT_ENTRY(SinSynth)
AUDIOCOMPONENT_ENTRY(AUMusicDeviceFactory, SinSynth)

static const AudioUnitParameterID kGlobalVolumeParam = 0;
static const CFStringRef kGlobalVolumeName = CFSTR("global volume");
static const AudioUnitParameterID kGlobalAmpAttackParam = 1;
static const CFStringRef kGlobalAmpAttackName = CFSTR("VCA attack");
static const AudioUnitParameterID kGlobalAmpReleaseParam = 2;
static const CFStringRef kGlobalAmpReleaseName = CFSTR("VCA release");

struct thread_aborted{};  // temporary exception class
std::atomic<bool> exit_flag(false);

void check_exit() {
    if (exit_flag)
        throw thread_aborted{};
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	SinSynth::SinSynth
//
// This synth has No inputs, One output
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SinSynth::SinSynth(AudioUnit inComponentInstance)
: AUMonotimbralInstrumentBase(inComponentInstance, 0, 1)
{
    CreateElements();
    
    Globals()->UseIndexedParameters(3); // we're only defining one param
    Globals()->SetParameter (kGlobalVolumeParam, 1.0);
    Globals()->SetParameter (kGlobalAmpAttackParam, 0.0);
    Globals()->SetParameter (kGlobalAmpReleaseParam, 0.0);
    
    // subscriber Lidar values
    std::thread subscriber([](){
        sweep::sweep device{"/dev/cu.usbserial-DM00KVQW"};
        device.start_scanning();
        while (true) try {
            const sweep::scan scan = device.get_scan();
            for (const sweep::sample& sample : scan.samples) {
                std::cout << sample.distance << std::endl;
                c_buf.push_back(sample.distance);
                // add 1. to avoid zero division error
                moving_average = beta * moving_average + (1 - beta) * sample.distance + 1.;
            }
            check_exit();
        } catch (thread_aborted& e){
            //
        }
    });
    subscriber.detach();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	SinSynth::~SinSynth
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SinSynth::~SinSynth()
{
    exit_flag = true;
}


void SinSynth::Cleanup()
{
#if DEBUG_PRINT
    printf("SinSynth::Cleanup\n");
#endif
}

OSStatus SinSynth::Initialize()
{	
#if DEBUG_PRINT
    printf("->SinSynth::Initialize\n");
#endif
    AUMonotimbralInstrumentBase::Initialize();
    
    SetNotes(kNumNotes, kMaxActiveNotes, mTestNotes, sizeof(TestNote));
#if DEBUG_PRINT
    printf("<-SinSynth::Initialize\n");
#endif
    
    return noErr;
}

AUElement* SinSynth::CreateElement(AudioUnitScope scope,
                                   AudioUnitElement element)
{
    switch (scope)
    {
        case kAudioUnitScope_Group :
            return new SynthGroupElement(this, element, new MidiControls);
        case kAudioUnitScope_Part :
            return new SynthPartElement(this, element);
        default :
            return AUBase::CreateElement(scope, element);
    }
}

OSStatus SinSynth::GetParameterInfo(AudioUnitScope inScope,
                                    AudioUnitParameterID inParameterID,
                                    AudioUnitParameterInfo &outParameterInfo)
{
    if (inScope != kAudioUnitScope_Global) {
        return kAudioUnitErr_InvalidScope;
    } else {
        switch (inParameterID) {
            case kGlobalVolumeParam:
                AUBase::FillInParameterName (outParameterInfo, kGlobalVolumeName, false);
                outParameterInfo.flags = SetAudioUnitParameterDisplayType (0, kAudioUnitParameterFlag_DisplaySquareRoot);
                outParameterInfo.flags += kAudioUnitParameterFlag_IsWritable;
                outParameterInfo.flags += kAudioUnitParameterFlag_IsReadable;
        
                outParameterInfo.unit = kAudioUnitParameterUnit_LinearGain;
                outParameterInfo.minValue = 0;
                outParameterInfo.maxValue = 1.0;
                outParameterInfo.defaultValue = 1.0;
                break;
                
            case kGlobalAmpAttackParam:
                AUBase::FillInParameterName (outParameterInfo, kGlobalAmpAttackName, false);
                outParameterInfo.flags = SetAudioUnitParameterDisplayType (0, kAudioUnitParameterFlag_DisplaySquareRoot);
                outParameterInfo.flags += kAudioUnitParameterFlag_IsWritable;
                outParameterInfo.flags += kAudioUnitParameterFlag_IsReadable;
                
                outParameterInfo.unit = kAudioUnitParameterUnit_Seconds;
                outParameterInfo.minValue = 0.001;
                outParameterInfo.maxValue = 5.0;
                outParameterInfo.defaultValue = 0.001;
                break;
            
            case kGlobalAmpReleaseParam:
                AUBase::FillInParameterName (outParameterInfo, kGlobalAmpReleaseName, false);
                outParameterInfo.flags = SetAudioUnitParameterDisplayType (0, kAudioUnitParameterFlag_DisplaySquareRoot);
                outParameterInfo.flags += kAudioUnitParameterFlag_IsWritable;
                outParameterInfo.flags += kAudioUnitParameterFlag_IsReadable;
                
                outParameterInfo.unit = kAudioUnitParameterUnit_Seconds;
                outParameterInfo.minValue = 0.001;
                outParameterInfo.maxValue = 5.0;
                outParameterInfo.defaultValue = 0.001;
                break;
                
            default:
                return kAudioUnitErr_InvalidParameter;
        }
    }
    return noErr;
}


#pragma mark TestNote Methods

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void TestNote::Release(UInt32 inFrame)
{
    SynthNote::Release(inFrame);
#if DEBUG_PRINT
    printf("TestNote::Release %p %d\n", this, GetState());
#endif
}

void TestNote::FastRelease(UInt32 inFrame) // voice is being stolen.
{
    SynthNote::Release(inFrame);
#if DEBUG_PRINT
    printf("TestNote::Release %p %d\n", this, GetState());
#endif
}

void TestNote::Kill(UInt32 inFrame) // voice is being stolen.
{
    SynthNote::Kill(inFrame);
#if DEBUG_PRINT
    printf("TestNote::Kill %p %d\n", this, GetState());
#endif
}

OSStatus TestNote::Render(UInt64 inAbsoluteSampleFrame, UInt32 inNumFrames, AudioBufferList** inBufferList, UInt32 inOutBusCount)
{
    float *left, *right;
    /* ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
     Changes to this parameter (kGlobalVolumeParam) are not being de-zippered;
     Left as an exercise for the reader
     ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ */
    float globalVol = GetGlobalParameter(kGlobalVolumeParam);
    float globalAmpAttack = GetGlobalParameter(kGlobalAmpAttackParam);
    float globalAmpRelease = GetGlobalParameter(kGlobalAmpReleaseParam);
    
    // TestNote only writes into the first bus regardless of what is handed to us.
    const int bus0 = 0;
    int numChans = inBufferList[bus0]->mNumberBuffers;
    if (numChans > 2) return -1;
    
    left = (float*)inBufferList[bus0]->mBuffers[0].mData;
    right = numChans == 2 ? (float*)inBufferList[bus0]->mBuffers[1].mData : 0;
    
    double sampleRate = SampleRate();
    double freq = Frequency() * (twopi/sampleRate);
    
    
#if DEBUG_PRINT_RENDER
    printf("TestNote::Render %p %d %g %g\n", this, GetState(), phase, amp);
#endif
    switch (GetState())
    {
        case kNoteState_Attacked :
        case kNoteState_Sostenutoed :
        case kNoteState_ReleasedButSostenutoed :
        case kNoteState_ReleasedButSustained :
        {
            for (UInt32 frame=0; frame<inNumFrames; ++frame)
            {
                // if (amp < maxamp) amp += up_slope;
                if (amp < maxamp) amp += maxamp / (sampleRate * globalAmpAttack);
                if (amp > maxamp) amp = maxamp;
                // float out = pow5(sin(phase)) * amp * globalVol;  // original
                int idx = int(phase/twopi*buf_size);
                float val;
                try {
                    val = (c_buf[idx] < 1000) ? c_buf[idx] : 1000;
                } catch (...) {
                    val = 0.;
                }
                float out = (val - moving_average) / moving_average * amp * globalVol;
                phase += freq;
                if (phase > twopi) phase -= twopi;
                left[frame] += out;
                if (right) right[frame] += out;
            }
        }
            break;
            
        case kNoteState_Released :
        {
            UInt32 endFrame = 0xFFFFFFFF;
            for (UInt32 frame=0; frame<inNumFrames; ++frame)
            {
                // if (amp > 0.0) amp += dn_slope;
                if (amp > 0.0) amp -= maxamp / (sampleRate * globalAmpRelease);
                else if (endFrame == 0xFFFFFFFF) endFrame = frame;
                // float out = pow5(sin(phase)) * amp * globalVol;  // original
                int idx = int(phase/twopi*buf_size);
                float val;
                try {
                    val = (c_buf[idx] < 1000) ? c_buf[idx] : 1000;
                } catch (...) {
                    val = 0.;
                }
                float out = (val - moving_average) / moving_average * amp * globalVol;
                // float out = (c_buf[idx] - moving_average) / moving_average * amp * globalVol;
                phase += freq;
                left[frame] += out;
                if (right) right[frame] += out;
            }
            if (endFrame != 0xFFFFFFFF) {
#if DEBUG_PRINT
                printf("TestNote::NoteEnded  %p %d %g %g\n", this, GetState(), phase, amp);
#endif
                NoteEnded(endFrame);
            }
        }
            break;
            
        case kNoteState_FastReleased :
        {
            UInt32 endFrame = 0xFFFFFFFF;
            for (UInt32 frame=0; frame<inNumFrames; ++frame)
            {
                if (amp > 0.0) amp += fast_dn_slope;
                else if (endFrame == 0xFFFFFFFF) endFrame = frame;
                // float out = pow5(sin(phase)) * amp * globalVol;  // original
                int idx = int(phase/twopi*buf_size);
                float val;
                try {
                    val = (c_buf[idx] < 1000) ? c_buf[idx] : 1000;
                } catch (...) {
                    val = 0.;
                }
                float out = (val - moving_average) / moving_average * amp * globalVol;
                //float out = (c_buf[idx] - moving_average) / moving_average * amp * globalVol;
                phase += freq;
                left[frame] += out;
                if (right) right[frame] += out;
            }
            if (endFrame != 0xFFFFFFFF) {
#if DEBUG_PRINT
                printf("TestNote::NoteEnded  %p %d %g %g\n", this, GetState(), phase, amp);
#endif
                NoteEnded(endFrame);
            }
        }
            break;
        default :
            break;
    }
    return noErr;
}

