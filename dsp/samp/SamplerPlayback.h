#pragma once

#include <assert.h>

#include <cmath>

#include "CompiledRegion.h"
#include "RandomRange.h"

class WaveLoader;

/**
 * When a patch is asked to "play", it serves up one of these.
 * So this is the "output" of play.
 */
class VoicePlayInfo {
public:
    VoicePlayInfo() = default;
    VoicePlayInfo(CompiledRegionPtr region, int midiPitch, int sampleIndex);
    bool valid = false;
    int sampleIndex = 0;
    bool needsTranspose = false;
    float transposeAmt = 1;
    float gain = 1;  // assume full volume
    float ampeg_release = .001f;

    bool canPlay() const {
        return valid && (sampleIndex > 0);
    }
};
using VoicePlayInfoPtr = std::shared_ptr<VoicePlayInfo>;

/**
 * This is info send down to play.
 * So it is the "input" to play.
 */
class VoicePlayParameter {
public:
    int midiPitch = 0;
    int midiVelocity = 0;
};

class ISamplerPlayback {
public:
    virtual ~ISamplerPlayback() = default;

    /**
     * @param info is where the playback info is returned.
     * @param params are not play parameters.
     * @param loader is all the wave files. may be null for some tests.
     * @param sampleRate is the current sample rate. Ignored if loader is nullptr
     */
    virtual void play(VoicePlayInfo& info, const VoicePlayParameter& params, WaveLoader* loader, float sampleRate) = 0;
    virtual void _dump(int depth) const = 0;

protected:
    static void indent(int depth) {
        for (int i = 0; i < depth; ++i) {
            printf("  ");
        }
    }
};

using ISamplerPlaybackPtr = std::shared_ptr<ISamplerPlayback>;

/**
 * Data extracted from patch required to play one note 
 */

class CachedSamplerPlaybackInfo {
public:
    CachedSamplerPlaybackInfo() = delete;
    CachedSamplerPlaybackInfo(CompiledRegionPtr reg, int midiPitch, int sampleIndex) : sampleIndex(sampleIndex) {
        const int semiOffset = midiPitch - reg->keycenter;
        if (semiOffset == 0) {
            needsTranspose = false;
            transposeAmt = 1;
        } else {
            const float pitchMul = float(std::pow(2, semiOffset / 12.0));
            needsTranspose = true;
            transposeAmt = pitchMul;
        }
        amp_veltrack = reg->amp_veltrack;
        ampeg_release = reg->ampeg_release;
    }

    // properties that get served up unchanged
    bool needsTranspose = false;
    float transposeAmt = 1;
    const int sampleIndex;
    float ampeg_release = .001f;

    // properties that participate in calculations
    float amp_veltrack = 100;
};

using CachedSamplerPlaybackInfoPtr = std::shared_ptr<CachedSamplerPlaybackInfo>;

// TODO: move tis into a class??
inline void cachedInfoToPlayInfo(VoicePlayInfo& playInfo, const VoicePlayParameter& params, const CachedSamplerPlaybackInfo& cachedInfo) {
    assert(params.midiVelocity > 0 && params.midiVelocity <= 127);
    playInfo.sampleIndex = cachedInfo.sampleIndex;
    playInfo.needsTranspose = cachedInfo.needsTranspose;
    playInfo.transposeAmt = cachedInfo.transposeAmt;
    playInfo.ampeg_release = cachedInfo.ampeg_release;
    playInfo.valid = true;

    // compute gain
    {
        // first do the veltrack adjustment to the raw velocity
        const float v = float(params.midiVelocity);
        const float t = cachedInfo.amp_veltrack;
        const float x = (v * t / 100.f) + (100.f - t) * (127.f / 100.f);

        // then taper it
        auto temp = float(x) / 127.f;
        temp *= temp;
        playInfo.gain = temp;

        // printf("doing vel comp veloc=%d, track=%f x=%f, gain=%f\n", params.midiVelocity, t, x, playInfo.gain);
    }
}

class SimpleVoicePlayer : public ISamplerPlayback {
public:
    SimpleVoicePlayer() = delete;
    SimpleVoicePlayer(CompiledRegionPtr reg, int midiPitch, int sampleIndex) : data(reg, midiPitch, sampleIndex),
                                                                               lineNumber(reg->lineNumber) {
        assert(sampleIndex > 0);
    }
    void play(VoicePlayInfo& info, const VoicePlayParameter& params, WaveLoader* loader, float sampleRate) override;
    void _dump(int depth) const override {
        indent(depth);
        printf("simple voice player si=%d\n", data.sampleIndex);
    }

private:
    CachedSamplerPlaybackInfo data;
    const int lineNumber;  // in the source file
};

/**
 * PLayer that does nothing. Hopefully will not be used (often?)
 * in the real world, but need it now to cover corner cases without
 * crashing.
 */
class NullVoicePlayer : public ISamplerPlayback {
public:
    void play(VoicePlayInfo& info, const VoicePlayParameter&, WaveLoader* loader, float sampleRate) override {
        info.valid = false;
    }
    void _dump(int depth) const override {
        indent(depth);
        printf("NullVoicePlayer %p\n", this);
    }
};

#if 0 // don't use these any more
class RandomVoicePlayer : public ISamplerPlayback {
public:
    RandomVoicePlayer() : rand(0) {}
    void play(VoicePlayInfo& info, const VoicePlayParameter&, WaveLoader* loader, float sampleRate) override;
    void _dump(int depth) const override;
    void addEntry(CompiledRegionPtr region, int sampleIndex, int midiPitch);
    void finalize();

private:
    std::vector<CachedSamplerPlaybackInfoPtr> entries;
    RandomRange<float> rand;

    class TempHolder {
    public:
        CachedSamplerPlaybackInfoPtr info;
        float hirand = 0;
    };
    std::vector<TempHolder> tempEntries;
    bool finalized = false;
};

using RandomVoicePlayerPtr = std::shared_ptr<RandomVoicePlayer>;

class RoundRobinVoicePlayer : public ISamplerPlayback {
public:
    // let's add seq_position so we can play in the right order
    class RRPlayInfo : public CachedSamplerPlaybackInfo {
    public:
        RRPlayInfo(const CachedSamplerPlaybackInfo&);
        int seq_position = 0;
    };
    using RRPlayInfoPtr = std::shared_ptr<RRPlayInfo>;
    void play(VoicePlayInfo& info, const VoicePlayParameter&, WaveLoader* loader, float sampleRate) override;
    void _dump(int depth) const override;
    void addEntry(CompiledRegionPtr region, int sampleIndex, int midiPitch);
    void finalize();

private:
    std::vector<RRPlayInfoPtr> entries;
    int currentEntry = 0;
    int numEntries = 0;
};

using RoundRobinVoicePlayerPtr = std::shared_ptr<RoundRobinVoicePlayer>;
#endif