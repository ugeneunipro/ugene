/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
 * http://ugene.net
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#include "DatatypeSerializeUtils.h"

#include <QBitArray>
#include <QStack>
#include <QtEndian>

#include <U2Core/AppContext.h>
#include <U2Core/IOAdapterTextStream.h>
#include <U2Core/StringAdapter.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

const QString DNAChromatogramSerializer::ID = "chroma_1.14";
const QString NewickPhyTreeSerializer::ID = "newick_1.14";
const QString BioStruct3DSerializer::ID = "3d_1.14";
const QString WMatrixSerializer::ID = "wm_1.14";
const QString FMatrixSerializer::ID = "fm_1.14";

#define CHECK_SIZE(size, result) \
    if (offset + (size) > length) { \
        os.setError("The data are too short"); \
        return result; \
    }

namespace {
template<class T>
inline QByteArray packNum(const T& num) {
    T leNum = qToLittleEndian<T>(num);
    return QByteArray((char*)&leNum, sizeof(T));
}
template<>
inline QByteArray packNum(const double& num) {
    QByteArray numStr = QByteArray::number(num);
    return packNum<int>(numStr.size()) + numStr;
}
template<>
inline QByteArray packNum(const float& num) {
    QByteArray numStr = QByteArray::number(num);
    return packNum<int>(numStr.size()) + numStr;
}
template<class T>
inline T unpackNum(const uchar* data, int length, int& offset, U2OpStatus& os) {
    CHECK_SIZE(int(sizeof(T)), T());
    T result = qFromLittleEndian<T>(data + offset);
    offset += sizeof(T);
    return result;
}
inline QByteArray unpackReal(const uchar* data, int length, int& offset, U2OpStatus& os) {
    int size = unpackNum<int>(data, length, offset, os);
    CHECK_OP(os, "");
    CHECK_SIZE(size, "");
    QByteArray result((const char*)data + offset, size);
    offset += size;
    return result;
}
template<>
inline double unpackNum(const uchar* data, int length, int& offset, U2OpStatus& os) {
    QByteArray numStr = unpackReal(data, length, offset, os);
    CHECK_OP(os, 0.0);
    return numStr.toDouble();
}
template<>
inline float unpackNum(const uchar* data, int length, int& offset, U2OpStatus& os) {
    QByteArray numStr = unpackReal(data, length, offset, os);
    CHECK_OP(os, 0.0f);
    return numStr.toFloat();
}

template<class T>
inline QByteArray packNumVector(const QVector<T>& vector) {
    QByteArray result;
    result += packNum<int>(vector.size());
    foreach (const T& num, vector) {
        result += packNum<T>(num);
    }
    return result;
}
template<class T>
inline QVector<T> unpackNumVector(const uchar* data, int length, int& offset, U2OpStatus& os) {
    QVector<T> result;
    int size = unpackNum<int>(data, length, offset, os);
    CHECK_OP(os, result);
    for (int i = 0; i < size; i++) {
        T num = unpackNum<T>(data, length, offset, os);
        CHECK_OP(os, result);
        result << num;
    }
    return result;
}

inline QByteArray packCharVector(const QVector<char>& vector) {
    QByteArray result;
    result += packNum<int>(vector.size());
    foreach (const char& c, vector) {
        result += c;
    }
    return result;
}
inline QVector<char> unpackCharVector(const uchar* data, int length, int& offset, U2OpStatus& os) {
    QVector<char> result;
    int size = unpackNum<int>(data, length, offset, os);
    CHECK_OP(os, result);
    for (int i = 0; i < size; i++) {
        CHECK_SIZE(1, result);
        result << (char)data[offset];
        offset++;
    }
    return result;
}

inline QByteArray packBool(bool value) {
    char c = (value) ? 1 : 0;
    return QByteArray(1, c);
}

inline bool unpackBool(const uchar* data, int length, int& offset, U2OpStatus& os) {
    CHECK_SIZE(1, false);
    uchar c = data[offset];
    offset++;
    return c != 0;
}
}  // namespace

/************************************************************************/
/* DNAChromatogramSerializer */
/************************************************************************/
QByteArray DNAChromatogramSerializer::serialize(const Chromatogram& chroma) {
    QByteArray result;
    result += packNum<int>(chroma->traceLength);
    result += packNum<int>(chroma->seqLength);
    result += packNumVector<ushort>(chroma->baseCalls);
    result += packNumVector<ushort>(chroma->A);
    result += packNumVector<ushort>(chroma->C);
    result += packNumVector<ushort>(chroma->G);
    result += packNumVector<ushort>(chroma->T);
    result += packCharVector(chroma->prob_A);
    result += packCharVector(chroma->prob_C);
    result += packCharVector(chroma->prob_G);
    result += packCharVector(chroma->prob_T);
    result += packBool(chroma->hasQV);
    return result;
}

Chromatogram DNAChromatogramSerializer::deserialize(const QByteArray& binary, U2OpStatus& os) {
    Chromatogram result;
    auto data = (const uchar*)(binary.data());
    int offset = 0;
    int length = binary.length();

    result->traceLength = unpackNum<int>(data, length, offset, os);
    CHECK_OP(os, result);
    result->seqLength = unpackNum<int>(data, length, offset, os);
    CHECK_OP(os, result);
    result->baseCalls = unpackNumVector<ushort>(data, length, offset, os);
    CHECK_OP(os, result);
    result->A = unpackNumVector<ushort>(data, length, offset, os);
    CHECK_OP(os, result);
    result->C = unpackNumVector<ushort>(data, length, offset, os);
    CHECK_OP(os, result);
    result->G = unpackNumVector<ushort>(data, length, offset, os);
    CHECK_OP(os, result);
    result->T = unpackNumVector<ushort>(data, length, offset, os);
    CHECK_OP(os, result);
    result->prob_A = unpackCharVector(data, length, offset, os);
    CHECK_OP(os, result);
    result->prob_C = unpackCharVector(data, length, offset, os);
    CHECK_OP(os, result);
    result->prob_G = unpackCharVector(data, length, offset, os);
    CHECK_OP(os, result);
    result->prob_T = unpackCharVector(data, length, offset, os);
    CHECK_OP(os, result);
    result->hasQV = unpackBool(data, length, offset, os);
    return result;
}

/************************************************************************/
/* NewickPhyTreeSerializer */
/************************************************************************/
enum ReadState {
    RS_NAME,
    RS_WEIGHT,
    RS_QUOTED_NAME,
    RS_AFTER_CLOSING_BRACE
};

static void packTreeNode(QString& resultText, const PhyNode* node, U2OpStatus& os) {
    const QList<PhyBranch*>& childBranches = node->getChildBranches();
    if (!childBranches.isEmpty()) {
        resultText.append("(");
        for (int i = 0; i < childBranches.size(); i++) {
            PhyBranch* childBranch = childBranches[i];
            if (i > 0) {
                resultText.append(",");
            }
            packTreeNode(resultText, childBranch->childNode, os);
            CHECK_OP(os, );
            if (childBranch->nodeValue >= 0) {
                // TODO: this must be node name, not 'value'!
                resultText.append(QString::number(childBranch->nodeValue));
            }
            resultText.append(":");
            resultText.append(QString::number(childBranch->distance));
        }
        resultText.append(")");
    } else if (node->name.contains(QRegExp("\\s|[(]|[)]|[:]|[;]|[,]"))) {
        resultText.append(QString("\'%1\'").arg(node->name));
    } else {
        resultText.append(node->name);
    }
}

#define BUFF_SIZE 1024
/* TODO:
 Unquoted labels may not contain blanks, parentheses, square brackets, single_quotes, colons, semicolons, or commas.
 Single quote characters in a quoted label are represented by two single quotes.
 Blanks or tabs may appear anywhere except within unquoted labels or branch_lengths.
 Newlines may appear anywhere except within labels or branch_lengths.
 Comments are enclosed in square brackets and may appear anywhere newlines are permitted.
 */
QList<PhyTree> NewickPhyTreeSerializer::parseTrees(IOAdapterReader& reader, U2OpStatus& si) {
    QList<PhyTree> result;
    QString block;
    int blockLen;
    bool done = true;

    QBitArray ops(256);
    ops['('] = true;
    ops[')'] = true;
    ops[':'] = true;
    ops[','] = true;
    ops[';'] = true;
    ops['\''] = true;

    ReadState state = RS_NAME;
    QString lastStr;
    auto rootNode = new PhyNode();

    QStack<PhyNode*> nodeStack;
    QStack<PhyBranch*> branchStack;
    nodeStack.push(rootNode);
    while ((blockLen = reader.read(si, block, BUFF_SIZE)) > 0) {
        for (int i = 0; i < blockLen; i++) {
            QChar ch = block[i];
            auto latin1CharCode = (uchar)ch.toLatin1();
            if (TextUtils::WHITES[latin1CharCode]) {
                if (state == RS_QUOTED_NAME) {
                    lastStr.append(ch);
                }
                continue;
            }
            done = false;
            if (!ops[latin1CharCode]) {  // Not ops -> cache.
                lastStr.append(ch);
                continue;
            }
            // use cached value

            if (state == RS_AFTER_CLOSING_BRACE) {
                state = ch == ':' && lastStr.isEmpty() ? RS_WEIGHT : RS_NAME;
            }

            if (ch == '\'') {
                if (state == RS_NAME) {
                    state = RS_QUOTED_NAME;
                } else if (state == RS_QUOTED_NAME) {
                    QChar nextCh = block[i + 1];
                    if (nextCh == '\'') {
                        lastStr.append(ch);
                        i++;
                    } else {
                        state = RS_NAME;
                    }
                }
                continue;
            } else if (state == RS_QUOTED_NAME) {
                lastStr.append(ch);
                continue;
            }

            if (!lastStr.isEmpty()) {
                if (state == RS_NAME) {
                    nodeStack.top()->name = lastStr;
                } else {
                    CHECK_EXT_BREAK(state == RS_WEIGHT, si.setError(DatatypeSerializers::tr("Incorrect tree parsing state")));
                    if (!branchStack.isEmpty()) {  // Ignore root node weight if present.
                        if (nodeStack.size() < 2) {
                            si.setError(DatatypeSerializers::tr("Unexpected weight: %1").arg(lastStr));
                        }
                        bool ok = false;
                        branchStack.top()->distance = lastStr.toDouble(&ok);
                        CHECK_EXT_BREAK(ok, si.setError(DatatypeSerializers::tr("Error parsing weight: %1").arg(lastStr)));
                    }
                }
            }

            // Advance in state.
            if (ch == '(') {  // A new child.
                CHECK_EXT_BREAK(!nodeStack.isEmpty(), si.setError(DatatypeSerializers::tr("Tree node stack is empty")));
                auto pn = new PhyNode();
                PhyBranch* bd = PhyTreeUtils::addBranch(nodeStack.top(), pn, 0);
                nodeStack.push(pn);
                branchStack.push(bd);
                state = RS_NAME;
            } else if (ch == ':') {  // Weight start.
                if (state == RS_WEIGHT && !lastStr.isEmpty()) {
                    if (!branchStack.isEmpty()) {  // Ignore root node weight if present.
                        bool ok = false;
                        branchStack.top()->nodeValue = lastStr.toDouble(&ok);
                        if (!ok) {
                            si.setError(DatatypeSerializers::tr("Error parsing nodeValue: %1").arg(lastStr));
                            break;
                        }
                    }
                }
                state = RS_WEIGHT;
            } else if (ch == ',') {  // New sibling.
                CHECK_EXT_BREAK(!nodeStack.isEmpty(), si.setError(DatatypeSerializers::tr("Missing nodes in node stack")));
                CHECK_EXT_BREAK(!branchStack.isEmpty(), si.setError(DatatypeSerializers::tr("Missing branches in branch stack")));
                if (nodeStack.isEmpty() || branchStack.isEmpty()) {
                    si.setError(DatatypeSerializers::tr("Unexpected new sibling %1").arg(lastStr));
                    break;
                }
                nodeStack.pop();
                branchStack.pop();
                auto node = new PhyNode();
                PhyBranch* branch = PhyTreeUtils::addBranch(nodeStack.top(), node, 0);
                nodeStack.push(node);
                branchStack.push(branch);
                state = RS_NAME;
            } else if (ch == ')') {  // End of the branch, go up.
                nodeStack.pop();
                if (nodeStack.isEmpty()) {
                    si.setError(DatatypeSerializers::tr("Unexpected closing bracket :%1").arg(lastStr));
                    break;
                }
                CHECK_EXT_BREAK(!branchStack.isEmpty(), si.setError(DatatypeSerializers::tr("Branch node stack is empty")));
                branchStack.pop();
                state = RS_AFTER_CLOSING_BRACE;
            } else if (ch == ';') {
                if (!branchStack.isEmpty() || nodeStack.size() != 1) {
                    si.setError(DatatypeSerializers::tr("Unexpected end of file"));
                    break;
                }
                PhyTree tree(new PhyTreeData());
                tree->setRootNode(nodeStack.pop());
                result << tree;
                rootNode = new PhyNode();
                nodeStack.push(rootNode);
                done = true;
            }
            lastStr.clear();
        }
        if (si.isCoR()) {
            delete rootNode;
            rootNode = nullptr;
            break;
        }
        si.setProgress(reader.getProgress());
    }
    if (!si.isCoR()) {
        if (!branchStack.isEmpty() || nodeStack.size() != 1) {
            delete rootNode;
            si.setError(DatatypeSerializers::tr("Unexpected end of file"));
            return result;
        }
        if (!done) {
            PhyNode* node = nodeStack.pop();
            PhyTree tree(new PhyTreeData());
            tree->setRootNode(node);
            result << tree;
        } else {
            delete rootNode;
            if (result.empty()) {
                si.setError(DatatypeSerializers::tr("Empty file"));
            }
        }
    }
    return result;
}

QString NewickPhyTreeSerializer::serialize(const PhyTree& tree, U2OpStatus& os) {
    QString result;
    packTreeNode(result, tree->getRootNode(), os);
    CHECK_OP(os, "")
    result.append(";\n");
    return result;
}

PhyTree NewickPhyTreeSerializer::deserialize(const QString& text, U2OpStatus& os) {
    auto ioFactory = qobject_cast<StringAdapterFactory*>(AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(BaseIOAdapters::STRING));
    SAFE_POINT(ioFactory, "Failed to get StringAdapterFactory", {})
    StringAdapter io(text.toUtf8(), ioFactory);
    IOAdapterReader reader(&io);
    QList<PhyTree> trees = parseTrees(reader, os);
    CHECK_OP(os, {});
    CHECK_EXT(trees.size() == 1, os.setError(DatatypeSerializers::tr("Unexpected count of trees objects in input: %1").arg(trees.size())), {});
    return trees.first();
}

/************************************************************************/
/* BioStruct3DSerializer */
/************************************************************************/
namespace {
class PackContext {
public:
    QHash<const AtomData*, SharedAtom> atoms;
    QHash<const AtomData*, int> atomPositions;
    QHash<int, const AtomData*> atomByPosition;
};

template<class T>
T unpack(const uchar* data, int length, int& offset, U2OpStatus& os);
template<class T>
T unpack(const uchar* data, int length, int& offset, U2OpStatus& os, PackContext& ctx);

template<>
inline char unpack(const uchar* data, int length, int& offset, U2OpStatus& os) {
    CHECK_SIZE(1, 0);
    char result = (char)data[offset];
    offset++;
    return result;
}

inline QByteArray pack(const QByteArray& data) {
    return packNum<int>(data.size()) + data;
}
template<>
inline QByteArray unpack(const uchar* data, int length, int& offset, U2OpStatus& os) {
    int size = unpackNum<int>(data, length, offset, os);
    CHECK_SIZE(size, "");
    QByteArray result((const char*)data + offset, size);
    offset += size;
    return result;
}

inline QByteArray pack(const QString& data) {
    return pack(data.toUtf8());
}
template<>
inline QString unpack(const uchar* data, int length, int& offset, U2OpStatus& os) {
    return QString::fromUtf8(unpack<QByteArray>(data, length, offset, os));
}

inline QByteArray pack(const ResidueIndex& data) {
    QByteArray result;
    result += packNum<int>(data.toInt());
    result += packNum<int>(data.getOrder());
    result += data.getInsCode();
    return result;
}
template<>
inline ResidueIndex unpack(const uchar* data, int length, int& offset, U2OpStatus& os) {
    int resId = unpackNum<int>(data, length, offset, os);
    CHECK_OP(os, ResidueIndex());
    int order = unpackNum<int>(data, length, offset, os);
    CHECK_OP(os, ResidueIndex());
    char insCode = unpack<char>(data, length, offset, os);
    CHECK_OP(os, ResidueIndex());

    ResidueIndex result(resId, insCode);
    result.setOrder(order);
    return result;
}
template<>
inline QByteArray packNum(const ResidueIndex& num) {
    return pack(num);
}
template<>
inline ResidueIndex unpackNum(const uchar* data, int length, int& offset, U2OpStatus& os) {
    return unpack<ResidueIndex>(data, length, offset, os);
}

inline QByteArray pack(const ResidueData& data) {
    QByteArray result;
    result += char(data.type);
    result += pack(data.name);
    result += data.acronym;
    result += packNum<int>(data.chainIndex);
    return result;
}
inline QByteArray pack(const SharedResidue& data, PackContext& /*ctx*/) {
    return pack(*data.data());
}
template<>
inline SharedResidue unpack(const uchar* data, int length, int& offset, U2OpStatus& os, PackContext& /*ctx*/) {
    SharedResidue result(new ResidueData());
    result->type = ResidueData::Type(unpack<char>(data, length, offset, os));
    CHECK_OP(os, result);

    result->name = unpack<QByteArray>(data, length, offset, os);
    CHECK_OP(os, result);

    result->acronym = unpack<char>(data, length, offset, os);
    CHECK_OP(os, result);

    result->chainIndex = unpackNum<int>(data, length, offset, os);
    return result;
}

inline QByteArray pack(const Vector3D& data) {
    QByteArray result;
    result += packNum<double>(data.x);
    result += packNum<double>(data.y);
    result += packNum<double>(data.z);
    return result;
}
template<>
inline Vector3D unpack(const uchar* data, int length, int& offset, U2OpStatus& os) {
    Vector3D result;
    result.x = unpackNum<double>(data, length, offset, os);
    CHECK_OP(os, result);
    result.y = unpackNum<double>(data, length, offset, os);
    CHECK_OP(os, result);
    result.z = unpackNum<double>(data, length, offset, os);
    return result;
}

inline QByteArray pack(const Matrix44& data) {
    QByteArray result;
    for (int i = 0; i < 16; i++) {
        result += packNum<float>(data[i]);
    }
    return result;
}
template<>
inline Matrix44 unpack(const uchar* data, int length, int& offset, U2OpStatus& os) {
    Matrix44 result;
    for (int i = 0; i < 16; i++) {
        result[i] = unpackNum<float>(data, length, offset, os);
        CHECK_OP(os, result);
    }
    return result;
}

inline QByteArray pack(const AtomData& data) {
    QByteArray result;
    result += packNum<int>(data.atomicNumber);
    result += packNum<int>(data.chainIndex);
    result += pack(data.residueIndex);
    result += pack(data.name);
    result += pack(data.coord3d);
    result += packNum<float>(data.occupancy);
    result += packNum<float>(data.temperature);
    return result;
}
inline QByteArray pack(const SharedAtom& data, PackContext& ctx) {
    int num = ctx.atomPositions.value(data.constData(), -1);
    if (-1 != num) {
        return packNum<int>(num);
    }

    QByteArray result;
    result += packNum<int>(ctx.atoms.size());
    result += pack(*data.data());

    int position = ctx.atomPositions.size();
    ctx.atoms.insert(data.constData(), data);
    ctx.atomPositions.insert(data.constData(), position);
    ctx.atomByPosition.insert(position, data.constData());

    return result;
}
template<>
inline AtomData unpack(const uchar* data, int length, int& offset, U2OpStatus& os) {
    AtomData result;
    result.atomicNumber = unpackNum<int>(data, length, offset, os);
    CHECK_OP(os, result);
    result.chainIndex = unpackNum<int>(data, length, offset, os);
    CHECK_OP(os, result);
    result.residueIndex = unpack<ResidueIndex>(data, length, offset, os);
    CHECK_OP(os, result);
    result.name = unpack<QByteArray>(data, length, offset, os);
    CHECK_OP(os, result);
    result.coord3d = unpack<Vector3D>(data, length, offset, os);
    CHECK_OP(os, result);
    result.occupancy = unpackNum<float>(data, length, offset, os);
    CHECK_OP(os, result);
    result.temperature = unpackNum<float>(data, length, offset, os);
    return result;
}
template<>
inline SharedAtom unpack(const uchar* data, int length, int& offset, U2OpStatus& os, PackContext& ctx) {
    int num = unpackNum<int>(data, length, offset, os);
    CHECK_OP(os, SharedAtom());
    if (num < ctx.atoms.size()) {
        return ctx.atoms.value(ctx.atomByPosition.value(num, nullptr));
    }
    SAFE_POINT_EXT(num == ctx.atoms.size(), os.setError("Unexpected atom number"), SharedAtom());
    AtomData atom = unpack<AtomData>(data, length, offset, os);
    CHECK_OP(os, SharedAtom());
    SharedAtom result(new AtomData(atom));

    int position = ctx.atomPositions.size();
    ctx.atoms.insert(result.constData(), result);
    ctx.atomPositions.insert(result.constData(), position);
    ctx.atomByPosition.insert(position, result.constData());

    return result;
}

inline QByteArray pack(const Bond& data, PackContext& ctx) {
    QByteArray result;
    result += pack(data.getAtom1(), ctx);
    result += pack(data.getAtom2(), ctx);
    return result;
}
template<>
inline Bond unpack(const uchar* data, int length, int& offset, U2OpStatus& os, PackContext& ctx) {
    SharedAtom atom1 = unpack<SharedAtom>(data, length, offset, os, ctx);
    CHECK_OP(os, Bond(SharedAtom(), SharedAtom()));
    SharedAtom atom2 = unpack<SharedAtom>(data, length, offset, os, ctx);
    return {atom1, atom2};
}

inline QByteArray pack(const SecondaryStructure& data) {
    QByteArray result;
    result += char(data.type);
    result += data.chainIdentifier;
    result += packNum<int>(data.chainIndex);
    result += packNum<int>(data.startSequenceNumber);
    result += packNum<int>(data.endSequenceNumber);
    return result;
}
inline QByteArray pack(const SharedSecondaryStructure& data, PackContext& /*ctx*/) {
    return pack(*data.data());
}
template<>
inline SharedSecondaryStructure unpack(const uchar* data, int length, int& offset, U2OpStatus& os, PackContext& /*ctx*/) {
    SharedSecondaryStructure result(new SecondaryStructure());
    result->type = SecondaryStructure::Type(unpack<char>(data, length, offset, os));
    CHECK_OP(os, result);

    result->chainIdentifier = unpack<char>(data, length, offset, os);
    CHECK_OP(os, result);

    result->chainIndex = unpackNum<int>(data, length, offset, os);
    result->startSequenceNumber = unpackNum<int>(data, length, offset, os);
    result->endSequenceNumber = unpackNum<int>(data, length, offset, os);
    return result;
}

template<class T>
inline QByteArray packList(const QList<T>& data, PackContext& ctx) {
    QByteArray result;
    result += packNum<int>(data.size());
    foreach (const T& d, data) {
        result += pack(d, ctx);
    }
    return result;
}
template<class T>
inline QList<T> unpackList(const uchar* data, int length, int& offset, U2OpStatus& os, PackContext& ctx) {
    QList<T> result;
    int size = unpackNum<int>(data, length, offset, os);
    CHECK_OP(os, result);
    for (int i = 0; i < size; i++) {
        result << unpack<T>(data, length, offset, os, ctx);
        CHECK_OP(os, result);
    }
    return result;
}

inline QByteArray pack(const Molecule3DModel& data, PackContext& ctx) {
    QByteArray result;
    result += packList<SharedAtom>(data.atoms, ctx);
    result += packList<Bond>(data.bonds, ctx);
    return result;
}
template<>
inline Molecule3DModel unpack(const uchar* data, int length, int& offset, U2OpStatus& os, PackContext& ctx) {
    Molecule3DModel result;
    result.atoms = unpackList<SharedAtom>(data, length, offset, os, ctx);
    CHECK_OP(os, result);
    result.bonds = unpackList<Bond>(data, length, offset, os, ctx);
    CHECK_OP(os, result);
    return result;
}

template<class KeyT, class ValueT>
inline QByteArray packMap(const QMap<KeyT, ValueT>& data, PackContext& ctx) {
    QByteArray result;
    result += packNum<int>(data.size());
    foreach (const KeyT& idx, data.keys()) {
        result += packNum<KeyT>(idx);
        result += pack(data[idx], ctx);
    }
    return result;
}
template<class KeyT, class ValueT>
inline QMap<KeyT, ValueT> unpackMap(const uchar* data, int length, int& offset, U2OpStatus& os, PackContext& ctx) {
    QMap<KeyT, ValueT> result;
    int size = unpackNum<int>(data, length, offset, os);
    CHECK_OP(os, result);
    for (int i = 0; i < size; i++) {
        KeyT key = unpackNum<KeyT>(data, length, offset, os);
        CHECK_OP(os, result);
        ValueT value = unpack<ValueT>(data, length, offset, os, ctx);
        CHECK_OP(os, result);
        result[key] = value;
    }
    return result;
}

inline QByteArray pack(const MoleculeData& data, PackContext& ctx) {
    QByteArray result;
    result += packMap<ResidueIndex, SharedResidue>(data.residueMap, ctx);
    result += packMap<int, Molecule3DModel>(data.models, ctx);
    result += pack(data.name);
    result += data.chainId;
    result += packBool(data.engineered);
    return result;
}
inline QByteArray pack(const SharedMolecule& data, PackContext& ctx) {
    return pack(*data.data(), ctx);
}
template<>
inline SharedMolecule unpack(const uchar* data, int length, int& offset, U2OpStatus& os, PackContext& ctx) {
    SharedMolecule result(new MoleculeData());
    result->residueMap = unpackMap<ResidueIndex, SharedResidue>(data, length, offset, os, ctx);
    CHECK_OP(os, result);
    result->models = unpackMap<int, Molecule3DModel>(data, length, offset, os, ctx);
    CHECK_OP(os, result);
    result->name = unpack<QString>(data, length, offset, os);
    CHECK_OP(os, result);
    result->chainId = unpack<char>(data, length, offset, os);
    CHECK_OP(os, result);
    result->engineered = unpackBool(data, length, offset, os);
    return result;
}

inline QByteArray pack(const AtomCoordSet& data, PackContext& ctx) {
    QByteArray result;
    result += packNum<int>(data.size());
    foreach (int idx, data.keys()) {
        result += packNum<int>(idx);
        result += pack(data[idx], ctx);
    }
    return result;
}
template<>
inline AtomCoordSet unpack(const uchar* data, int length, int& offset, U2OpStatus& os, PackContext& ctx) {
    AtomCoordSet result;
    int size = unpackNum<int>(data, length, offset, os);
    CHECK_OP(os, result);
    for (int i = 0; i < size; i++) {
        int key = unpackNum<int>(data, length, offset, os);
        CHECK_OP(os, result);
        SharedAtom value = unpack<SharedAtom>(data, length, offset, os, ctx);
        CHECK_OP(os, result);
        result[key] = value;
    }
    return result;
}
}  // namespace

QByteArray BioStruct3DSerializer::serialize(const BioStruct3D& bioStruct) {
    PackContext ctx;
    QByteArray result;
    result += packMap<int, SharedMolecule>(bioStruct.moleculeMap, ctx);
    result += packMap<int, AtomCoordSet>(bioStruct.modelMap, ctx);
    result += packList<SharedSecondaryStructure>(bioStruct.secondaryStructures, ctx);
    result += packList<Bond>(bioStruct.interMolecularBonds, ctx);
    result += pack(bioStruct.descr);
    result += pack(bioStruct.pdbId);
    result += packNum<double>(bioStruct.getRadius());
    result += pack(bioStruct.getCenter());
    result += pack(bioStruct.getTransform());
    return result;
}

BioStruct3D BioStruct3DSerializer::deserialize(const QByteArray& binary, U2OpStatus& os) {
    auto data = (const uchar*)(binary.data());
    int offset = 0;
    int length = binary.length();

    PackContext ctx;
    BioStruct3D result;
    result.moleculeMap = unpackMap<int, SharedMolecule>(data, length, offset, os, ctx);
    CHECK_OP(os, result);
    result.modelMap = unpackMap<int, AtomCoordSet>(data, length, offset, os, ctx);
    CHECK_OP(os, result);
    result.secondaryStructures = unpackList<SharedSecondaryStructure>(data, length, offset, os, ctx);
    CHECK_OP(os, result);
    result.interMolecularBonds = unpackList<Bond>(data, length, offset, os, ctx);
    CHECK_OP(os, result);
    result.descr = unpack<QString>(data, length, offset, os);
    CHECK_OP(os, result);
    result.pdbId = unpack<QByteArray>(data, length, offset, os);
    CHECK_OP(os, result);
    result.setRadius(unpackNum<double>(data, length, offset, os));
    CHECK_OP(os, result);
    result.setCenter(unpack<Vector3D>(data, length, offset, os));
    CHECK_OP(os, result);
    result.setTransform(unpack<Matrix44>(data, length, offset, os));
    return result;
}

/************************************************************************/
/* WMatrixSerializer */
/************************************************************************/
namespace {
template<class T>
inline QByteArray packArray(const QVarLengthArray<T>& data) {
    QByteArray result;
    result += packNum<int>(data.size());
    foreach (const T& d, data) {
        result += packNum<T>(d);
    }
    return result;
}
template<class T>
inline QVarLengthArray<T> unpackArray(const uchar* data, int length, int& offset, U2OpStatus& os) {
    QVarLengthArray<T> result;
    int size = unpackNum<int>(data, length, offset, os);
    CHECK_OP(os, result);
    for (int i = 0; i < size; i++) {
        result << unpackNum<T>(data, length, offset, os);
        CHECK_OP(os, result);
    }
    return result;
}
inline QByteArray packMap(const QMap<QString, QString>& data) {
    QByteArray result;
    result += packNum<int>(data.size());
    foreach (const QString& key, data.keys()) {
        result += pack(key);
        result += pack(data[key]);
    }
    return result;
}
inline QMap<QString, QString> unpackMap(const uchar* data, int length, int& offset, U2OpStatus& os) {
    QMap<QString, QString> result;
    int size = unpackNum<int>(data, length, offset, os);
    CHECK_OP(os, result);
    for (int i = 0; i < size; i++) {
        QString key = unpack<QString>(data, length, offset, os);
        CHECK_OP(os, result);
        QString value = unpack<QString>(data, length, offset, os);
        CHECK_OP(os, result);
        result[key] = value;
    }
    return result;
}
}  // namespace

QByteArray WMatrixSerializer::serialize(const PWMatrix& matrix) {
    QByteArray result;
    result += packArray<float>(matrix.data);
    result += char(matrix.type);
    result += packMap(matrix.info.getProperties());
    return result;
}

PWMatrix WMatrixSerializer::deserialize(const QByteArray& binary, U2OpStatus& os) {
    auto data = (const uchar*)(binary.data());
    int offset = 0;
    int length = binary.length();

    QVarLengthArray<float> matrix = unpackArray<float>(data, length, offset, os);
    CHECK_OP(os, PWMatrix());
    auto type = PWMatrixType(unpack<char>(data, length, offset, os));
    CHECK_OP(os, PWMatrix());
    QMap<QString, QString> props = unpackMap(data, length, offset, os);
    CHECK_OP(os, PWMatrix());

    PWMatrix result(matrix, type);
    result.info = UniprobeInfo(props);

    return result;
}

/************************************************************************/
/* FMatrixSerializer */
/************************************************************************/
QByteArray FMatrixSerializer::serialize(const PFMatrix& matrix) {
    QByteArray result;
    result += packArray<int>(matrix.data);
    result += char(matrix.type);
    result += packMap(matrix.info.getProperties());
    return result;
}

PFMatrix FMatrixSerializer::deserialize(const QByteArray& binary, U2OpStatus& os) {
    auto data = (const uchar*)(binary.data());
    int offset = 0;
    int length = binary.length();

    QVarLengthArray<int> matrix = unpackArray<int>(data, length, offset, os);
    CHECK_OP(os, PFMatrix());
    auto type = PFMatrixType(unpack<char>(data, length, offset, os));
    CHECK_OP(os, PFMatrix());
    QMap<QString, QString> props = unpackMap(data, length, offset, os);
    CHECK_OP(os, PFMatrix());

    PFMatrix result(matrix, type);
    result.info = JasparInfo(props);

    return result;
}

}  // namespace U2
