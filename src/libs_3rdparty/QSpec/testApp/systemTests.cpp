#include "systemTests.h"

#include <system/GTFile.h>

using namespace HI;

TEST_CLASS_DEFINITION(FilePermissionTest)
{
    QDir dir("TestFile.dir");
    bool mk = dir.mkdir(".");
    qDebug() << "created dir: " << mk;
    QString testDir = dir.path();

    // create a file and check we can write to it
    QString f1(testDir+"/" + "testfile1");
    GTFile::create(os, f1);
    qint64 f1_size = GTFile::readAll(os, f1).size();
    QFile ff1(f1);
    qDebug() << "Initial testfile1 perms " << ff1.permissions();
    bool res = ff1.open(QFile::ReadWrite);
    CHECK_SET_ERR(res, "cannot open testfile1 to write");
    res = (5 == ff1.write("aaaaa"));
    ff1.close();
    CHECK_SET_ERR(res, "cannot write to testfile1");
    f1_size = GTFile::getSize(os, f1);
    CHECK_SET_ERR(f1_size == 5, "wrong size of testfile1");

    // set the file RO and check we cannot modify it
    //GTFile::setReadOnly(os, f1); TODO: restore after merging with master
    qDebug() << "RO testfile1 perms " << ff1.permissions();
    res = ff1.open(QFile::ReadWrite);
    CHECK_SET_ERR(!res, "should not open testfile1 to write");
    res = (-1 == ff1.write("bb"));
    ff1.close();
    CHECK_SET_ERR(res, "should not write to testfile1");
    f1_size = GTFile::getSize(os, f1);
    CHECK_SET_ERR(f1_size == 5, "wrong size of testfile1");

    // set the file RW and check we can modify it
    GTFile::setReadWrite(os, f1);
    qDebug() << "RW testfile1 perms " << ff1.permissions();
    QFile ff2(f1);
    res = ff2.open(QFile::Append);
    CHECK_SET_ERR(res, "should open testfile1 to write");
    res = (3 == ff2.write("ccc"));
    ff2.close();
    CHECK_SET_ERR(res, "should write to testfile1");
    f1_size = GTFile::getSize(os, f1);
    CHECK_SET_ERR(f1_size == 8, "wrong size of testfile1");

    // set the file RO and check we can remove it
    // TODO this does not work on Windows due to RO attr set
//    GTFile::setReadOnly(os, f1);
//    res = ff1.remove();
//    CHECK_SET_ERR(res, "failed to delete RO testfile1");
}

TEST_CLASS_DEFINITION(DirPermissionTest)
{
    QDir dir("TestDir.dir");
    bool mk = dir.mkdir(".");
    qDebug() << "created dir: " << mk;
    QString testDir = dir.path();

    // check we can create a file in it
    QString f1(testDir+"/" + "testfile1");
    GTFile::create(os, f1);

    // set the dir RO and check we can read the file
    //GTFile::setReadOnly(os, testDir); TODO: restore after merging with master
    QFile ff1(f1);
    qDebug() << "Initial testfile1 perms " << ff1.permissions();
    qint64 f1_size = GTFile::readAll(os, f1).size();
    CHECK_SET_ERR(f1_size == 0, "cannot access testfile1");

    // but cannot create a new file
    QString f2(testDir+"/" + "testfile2");
    bool res = QFile(f2).open(QIODevice::WriteOnly);
    CHECK_SET_ERR(!res, "should not create new file");

    // set the dir RW and check we can create the new file again
    GTFile::setReadWrite(os, testDir);
    GTFile::create( os, f2 );

//    // set the dir RO and check we can delete it
//    GTFile::setReadOnly(os, testDir);

}
