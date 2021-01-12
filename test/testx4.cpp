
#include "asserts.h"
#include "samplerTests.h"
#include "SqLog.h"

static void testRandomPlayerEmpty() {
    VoicePlayInfo info;
    VoicePlayParameter params;
    RandomVoicePlayerPtr player = std::make_shared<RandomVoicePlayer>();
    player->finalize();
    player->play(info, params);
    assert(!info.valid);
}

/*
  VoicePlayInfo(CompiledRegionPtr region, int midiPitch, int sampleIndex);
    bool valid = false;
    int sampleIndex = 0;
    bool needsTranspose = false;
    float transposeAmt = 1;
    float gain = 1;             // assume full volume
    float ampeg_release = .001f;

*/

static void testRandomPlayerOneEntry() {
    CompiledRegionPtr cr = st::makeRegion(R"foo(<region>sample=a lorand=0 hirand=1)foo");

    VoicePlayInfo info;
    VoicePlayParameter params;
    params.midiVelocity = 20;
    params.midiPitch = 60;
    RandomVoicePlayerPtr player = std::make_shared<RandomVoicePlayer>();
    player->addEntry(cr, 1, 60);
    player->finalize();
    player->play(info, params);
    assert(info.valid);
    assertEQ(info.sampleIndex, 1);
}

static void testRandomPlayerTwoEntrySub(const char* reg1, const char* reg2, bool flipped) {
 
    CompiledRegionPtr cr1 = st::makeRegion(reg1);
    CompiledRegionPtr cr2 = st::makeRegion(reg2);

    VoicePlayInfo info;
    VoicePlayParameter params;
    params.midiPitch = 60;
    params.midiVelocity = 100;
    RandomVoicePlayerPtr player = std::make_shared<RandomVoicePlayer>();

    player->addEntry(cr1, 100, 60);
    player->addEntry(cr2, 101, 60);
    player->finalize();
    int ct100 = 0;
    int ct101 = 0;

    for (int i = 0; i < 100; ++i) {
        player->play(info, params);
        assert(info.valid);
        switch (info.sampleIndex) {
            case 100:
                ct100++;
                break;
            case 101:
                ct101++;
                break;
            default:
                assert(false);
        }
    }
    if (!flipped) {
        assertClose(ct100, 20, 10);
        assertClose(ct101, 80, 10);
    }
    else {
        assertClose(ct101, 20, 10);
        assertClose(ct100, 80, 10);
    }
}

static void testRandomPlayerTwoEntry() {
    SQINFO("testRandomPlayerTwoEntry");
    testRandomPlayerTwoEntrySub(R"foo(<region>sample=a lorand=0 hirand=.2)foo",
                                R"foo(<region>sample=a lorand=.2 hirand=1)foo",
        false);
}

static void testRandomPlayerTwoEntryB() {
    SQINFO("testRandomPlayerTwoEntrySubB");
    testRandomPlayerTwoEntrySub(
        R"foo(<region>sample=a lorand=.2 hirand=1)foo",
        R"foo(<region>sample=a lorand=0 hirand=.2)foo",
        true);
}
void testx4() {
    testRandomPlayerEmpty();
    testRandomPlayerOneEntry();
    testRandomPlayerTwoEntry();
    testRandomPlayerTwoEntryB();
}