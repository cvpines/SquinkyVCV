
#include "SqLog.h"
#include "asserts.h"
#include "samplerTests.h"
#include "FilePath.h"

static void testFilePath0() {
    FilePath f("abc");
    assertEQ(f.toString(), "abc");

    assertNE(FilePath::nativeSeparator(), FilePath::foreignSeparator());
}

static void testFilePathFixup() {
    std::string input("\\\\////\\\\\\");
    FilePath f(input);
    const std::string s = f.toString();
    assertNE(s, input);

    bool b = s.find(FilePath::foreignSeparator()) != std::string::npos;
    assert(!b);
}

static void testFilePathFixup2() {
    const char* input ="\\\\////\\\\\\";
    FilePath f(input);
    const std::string s = f.toString();
    assertNE(s, input);

    bool b = s.find(FilePath::foreignSeparator()) != std::string::npos;
    assert(!b);
}

static void testFilePathConcat1() {
    FilePath a("a");
    FilePath b("b");
    a.concat(b);

    std::string s = a.toString();
    assertEQ(s.size(), 3);
    assertEQ(s.at(0), 'a');
    assertEQ(s.at(1), FilePath::nativeSeparator());
    assertEQ(s.at(2), 'b');
}

static void testFilePathConcat2() {
    FilePath a("a/");
    FilePath b("b");
    a.concat(b);

    std::string s = a.toString();
    assertEQ(s.size(), 3);
    assertEQ(s.at(0), 'a');
    assertEQ(s.at(1), FilePath::nativeSeparator());
    assertEQ(s.at(2), 'b');
}

static void testFilePathConcat3() {
    FilePath a("a\\");
    FilePath b("/b");
    a.concat(b);

    std::string s = a.toString();
    assertEQ(s.size(), 3);
    assertEQ(s.at(0), 'a');
    assertEQ(s.at(1), FilePath::nativeSeparator());
    assertEQ(s.at(2), 'b');
}

static void testFilePathConcat4() {
    FilePath a("a");
    FilePath b("./b");
    a.concat(b);

    std::string s = a.toString();
    assertEQ(s.size(), 3);
    assertEQ(s.at(0), 'a');
    assertEQ(s.at(1), FilePath::nativeSeparator());
    assertEQ(s.at(2), 'b');
}

static void testFilePathConcat5() {
    FilePath a("a");
    FilePath b(".");
    a.concat(b);

    std::string s = a.toString();
    assertEQ(s.size(), 1);
    assertEQ(s.at(0), 'a');
}

static void testFilePathConcat6() {
    FilePath a("");
    FilePath b("abc");
    a.concat(b);
    std::string s = a.toString();
    assertEQ(s, "abc");
}

 static void testFilePathGetPathPart() {
    FilePath a("abc/def\\ghi//a.txt");
    FilePath path = a.getPathPart();
    FilePath expected("abc\\def\\ghi\\");           // trailing separators don't really make a difference
    assertEQ(path.toString(), expected.toString());
 }

 static void testFilePathGetPathPart2() {
    FilePath a("a.txt");
    FilePath path = a.getPathPart();
    FilePath expected("");
    assertEQ(expected.toString(), path.toString());
 }

static void testFilePathGetFilenamePart() {
    FilePath a("abc/def\\ghi//a.txt");
    std::string fileName = a.getFilenamePart();
    assertEQ(fileName, "a.txt");
}

static void testFilePathGetFilenamePart2() {
    FilePath a("a.txt");
    std::string fileName = a.getFilenamePart();
    assertEQ(fileName, "a.txt");
}

static void testFilePathGetFilenamePart3() {
    FilePath a("abc/");
    std::string fileName = a.getFilenamePart();
    assertEQ(fileName, "");
}

static void testFilePathDoubleDot() {
    FilePath fp1("a");
    FilePath fp2("../b");
    fp1.concat(fp2);
    FilePath expected("a/../b");
    assertEQ(fp1.toString(), expected.toString());
}

void testx4() {
    testFilePath0();
    testFilePathFixup();
    testFilePathFixup2();
    testFilePathConcat1();
    testFilePathConcat2();
    testFilePathConcat3();
    testFilePathConcat4();
    testFilePathConcat5();
    testFilePathConcat6();
    testFilePathGetPathPart();
    testFilePathGetPathPart2();
    testFilePathGetFilenamePart();
    testFilePathGetFilenamePart2();
    testFilePathGetFilenamePart3();
    testFilePathDoubleDot();


}