/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2020 UniPro <ugene@unipro.ru>
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

#include <U2Core/BioStruct3DObject.h>
#include <U2Core/DatatypeSerializeUtils.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Formats/PDBFormat.h>

#include "../gobjects/BioStruct3DObjectUnitTests.h"

#include "DatatypeSerializeUtilsUnitTest.h"

namespace U2 {

void CompareUtils::checkEqual(const DNAChromatogram &chr1, const DNAChromatogram &chr2, U2OpStatus &os) {
    CHECK_EXT(chr1.traceLength == chr2.traceLength, os.setError("traceLength"), );
    CHECK_EXT(chr1.seqLength == chr2.seqLength, os.setError("seqLength"), );
    CHECK_EXT(chr1.baseCalls == chr2.baseCalls, os.setError("baseCalls"), );
    CHECK_EXT(chr1.A == chr2.A, os.setError("A"), );
    CHECK_EXT(chr1.C == chr2.C, os.setError("C"), );
    CHECK_EXT(chr1.G == chr2.G, os.setError("G"), );
    CHECK_EXT(chr1.T == chr2.T, os.setError("T"), );
    CHECK_EXT(chr1.prob_A == chr2.prob_A, os.setError("prob_A"), );
    CHECK_EXT(chr1.prob_C == chr2.prob_C, os.setError("prob_C"), );
    CHECK_EXT(chr1.prob_G == chr2.prob_G, os.setError("prob_G"), );
    CHECK_EXT(chr1.prob_T == chr2.prob_T, os.setError("prob_T"), );
    CHECK_EXT(chr1.hasQV == chr2.hasQV, os.setError("hasQV"), );
}

IMPLEMENT_TEST(DatatypeSerializeUtilsUnitTest, DNAChromatogramSerializer_true) {
    DNAChromatogram src;
    src.traceLength = 1;
    src.seqLength = -2;
    src.baseCalls << 3; src.baseCalls << -4; src.baseCalls << 5;
    src.A << -6; src.A << 7; src.A << -8;
    src.C << 9; src.C << -10; src.C << 11;
    src.G << -12; src.G << 13; src.G << -14;
    src.T << 15; src.T << -16; src.T << 17;
    src.prob_A << 'a'; src.prob_A << -'b'; src.prob_A << 'c';
    src.prob_C << -'d'; src.prob_C << 'e'; src.prob_C << -'f';
    src.prob_G << 'g'; src.prob_G << -'h'; src.prob_G << 'i';
    src.prob_T << -'j'; src.prob_T << 'k'; src.prob_T << -'l';
    src.hasQV = true;
    QByteArray binary = DNAChromatogramSerializer::serialize(src);

    U2OpStatusImpl os;
    DNAChromatogram dst = DNAChromatogramSerializer::deserialize(binary, os);
    CHECK_NO_ERROR(os);
    CompareUtils::checkEqual(src, dst, os);
}

IMPLEMENT_TEST(DatatypeSerializeUtilsUnitTest, DNAChromatogramSerializer_false) {
    DNAChromatogram src;
    src.hasQV = false;
    QByteArray binary = DNAChromatogramSerializer::serialize(src);

    U2OpStatusImpl os;
    DNAChromatogram dst = DNAChromatogramSerializer::deserialize(binary, os);
    CHECK_NO_ERROR(os);
    CHECK_TRUE(src.hasQV == dst.hasQV, "");
}

IMPLEMENT_TEST(DatatypeSerializeUtilsUnitTest, NewickPhyTreeSerializer) {
    QByteArray treeData("(((Platanista_minor:0,Platanista_indi:0,Platanista_gangetica:0):0,((Delphinus_delphis:0,Delphinus_capensis:0,Delphinus_tropicalis:0):0,(Globicephala_melas:0,Globicephala_macrorhynchus:0,Globicephala_sp.:0,Globicephala_sp._IIC2000:0):0,Orcinus_orca:0,(Stenella_attenuata:0,Stenella_longirostris_orientalis:0,Stenella_coeruleoalba:0,Stenella_clymene:0,Stenella_frontalis:0):0,(Tursiops_truncatus:0,Tursiops_aduncus:0):0,(Cephalorhynchus_eutropia:0,Cephalorhynchus_hectori:0,Cephalorhynchus_commersonii:0,Cephalorhynchus_heavisidii:0):0,(Lagenorhynchus_albirostris:0,Lagenorhynchus_obscurus:0,Lagenorhynchus_acutus:0,Lagenorhynchus_obliquidens:0,Lagenorhynchus_australis:0,Lagenorhynchus_cruciger:0):0,(Lissodelphis_peronii:0,Lissodelphis_borealis:0,Lissodelphis_sp.:0):0,Steno_bredanensis:0,Orcaella_brevirostris:0,Delphinidae_gen._sp.:0,Pseudorca_crassidens:0,Grampus_griseus:0,Feresa_attenuata:0,Lagenodelphis_hosei:0,Peponocephala_electra:0,Sotalia_fluviatilis:0,Sousa_chinensis:0):0,((Phocoena_phocoena_vomerina:0,Phocoena_spinipinnis:0,Phocoena_sinus:0,Phocoena_phocoena_x_Phocoenoides_dalli:0):0,Phocoenoides_dalli_dalli:0,Australophocaena_dioptrica:0,Neophocaena_phocaenoides_asiaeorientalis:0):0,(Delphinapterus_leucas:0,Monodon_monoceros:0):0,((Kogia_simus:0,Kogia_breviceps:0):0,Physeter_catodon:0):0,((Mesoplodon_carlhubbsi:0,Mesoplodon_europaeus:0,Mesoplodon_peruvianus:0,Mesoplodon_densirostris:0,Mesoplodon_bidens:0,Mesoplodon_hectori:0,Mesoplodon_mirus:0,Mesoplodon_stejnegeri:0,Mesoplodon_bowdoini:0,Mesoplodon_grayi:0,Mesoplodon_layardii:0,Mesoplodon_perrini:0,Mesoplodon_traversii:0):0,Ziphius_cavirostris:0,(Berardius_bairdii:0,Berardius_arnuxii:0):0,(Hyperoodon_ampullatus:0,Hyperoodon_planifrons:0):0,Tasmacetus_shepherdi:0,Ziphiidae_sp.:0,Indopacetus_pacificus:0):0,Lipotes_vexillifer:0,Pontoporia_blainvillei:0,(Inia_geoffrensis_boliviensis:0,Inia_geoffrensis_geoffrensis:0,Inia_geoffrensis_humboldtiana:0):0):0,(Eschrichtius_robustus:0,((Balaenoptera_acutorostrata:0,Balaenoptera_borealis:0,Balaenoptera_edeni:0,Balaenoptera_physalus:0,Balaenoptera_musculus:0,Balaenoptera_bonaerensis:0,Balaenoptera_musculus_x_Balaenoptera_physalus:0):0,Megaptera_novaeangliae:0,Balaenopteridae_gen._sp.:0):0,((Balaena_mysticetus:0,Balaena_glacialis:0):0,(Eubalaena_glacialis:0,Eubalaena_australis:0):0):0,Caperea_marginata:0):0);\n");

    U2OpStatusImpl os;
    PhyTree tree = NewickPhyTreeSerializer::deserialize(treeData, os);
    CHECK_NO_ERROR(os);

    QByteArray treeData2 = NewickPhyTreeSerializer::serialize(tree);
    CHECK_TRUE(treeData == treeData2, "data");
}

IMPLEMENT_TEST(DatatypeSerializeUtilsUnitTest, NewickPhyTreeSerializer_failed) {
    QByteArray treeData("qweqweqweqweqweqewqweqwe()()()(9093129 3912000)0999(");

    U2OpStatusImpl os;
    PhyTree tree = NewickPhyTreeSerializer::deserialize(treeData, os);
    CHECK_TRUE(os.hasError(), "no error");
}

IMPLEMENT_TEST(DatatypeSerializeUtilsUnitTest, BioStruct3DSerializer) {
    U2OpStatusImpl os;
    BioStruct3D bioStruct1 = BioStruct3DObjectTestData::readBioStruct("Ncbi.pdb", os);
    CHECK_NO_ERROR(os);

    QByteArray data1 = BioStruct3DSerializer::serialize(bioStruct1);
    BioStruct3D bioStruct2 = BioStruct3DSerializer::deserialize(data1, os);
    CHECK_NO_ERROR(os);

    QByteArray data2 = BioStruct3DSerializer::serialize(bioStruct2);
    CHECK_TRUE(data1 == data2, "data");
}

IMPLEMENT_TEST(DatatypeSerializeUtilsUnitTest, BioStruct3DSerializer_failed) {
    BioStruct3D bioStruct;
    QByteArray data = BioStruct3DSerializer::serialize(bioStruct);
    QByteArray broken = data.left(data.size() / 2);

    U2OpStatusImpl os;
    BioStruct3DSerializer::deserialize(broken, os);
    CHECK_TRUE(os.hasError(), "no error");
}

IMPLEMENT_TEST(DatatypeSerializeUtilsUnitTest, WMatrixSerializer) {
    QVarLengthArray<float> data;
    data << 0.000000000000000f << 2.854232788085938f << 7.135581970214844f << 0.000000000000000f << 1.427116394042969f << 0.000000000000000 << 0.000000000000000f << 5.708465576171875f << 0.000000000000000f << 0.000000000000000f <<
    0.000000000000000f << 3.932225704193115f << 0.000000000000000f << 0.000000000000000f << 1.966112852096558f << 3.932225704193115f << 0.000000000000000f << 0.000000000000000f << 3.932225704193115f << 0.000000000000000f <<
    0.000000000000000f << 0.000000000000000f << 0.000000000000000f << 11.512926101684570 << 0.000000000000000f << 0.000000000000000f << 0.000000000000000f << 0.000000000000000f << 0.000000000000000f << 0.000000000000000f <<
    3.269632339477539f << 0.653926491737366f << 0.000000000000000f << 0.000000000000000f << 1.961779475212097f << 1.961779475212097f << 3.269632339477539f << 0.653926491737366f << 1.961779475212097f << 3.269632339477539f;

    PWMatrix m1(data, PWM_MONONUCLEOTIDE);
    QMap<QString, QString> props;
    props["aaa"] = "bbb";
    m1.setInfo(UniprobeInfo(props));

    QByteArray binary = WMatrixSerializer::serialize(m1);
    U2OpStatusImpl os;
    PWMatrix m2 = WMatrixSerializer::deserialize(binary, os);
    CHECK_NO_ERROR(os);

    CHECK_TRUE(1 == m2.getProperties().size(), "props");
    CHECK_TRUE("bbb" == m2.getProperty("aaa"), "aaa");
    CHECK_TRUE(PWM_MONONUCLEOTIDE == m2.getType(), "type");
    CHECK_TRUE(11 == (int)m2.getValue(2, 3), "value");
}

IMPLEMENT_TEST(DatatypeSerializeUtilsUnitTest, WMatrixSerializer_failed) {
    U2OpStatusImpl os;
    WMatrixSerializer::deserialize("qqqqqqqqqqqqqqqqq", os);
    CHECK_TRUE(os.hasError(), "no error");
}

IMPLEMENT_TEST(DatatypeSerializeUtilsUnitTest, FMatrixSerializer) {
    QVarLengthArray<int> data;
    data << 0 << 2 << 0 << 0 << 0 << 0 << 0 << 0 << 0 <<
    0 << 0 << 0 << 0 << 1 << 0 << 0 << 1 << 0 <<
    0 << 0 << 5 << 0 << 0 << 0 << 0 << 0 << 0 <<
    0 << 0 << 0 << 0 << 0 << 0 << 0 << 3 << 0 <<
    0 << 2 << 0 << 0 << 0 << 0 << 0 << 0 << 0 <<
    0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 <<
    0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 <<
    0 << 0 << 0 << 0 << 1 << 2 << 0 << 0 << 2 <<
    0 << 0 << 0 << 1 << 0 << 0 << 0 << 0 << 0 <<
    0 << 0 << 0 << 1 << 0 << 0 << 0 << 0 << 0 <<
    0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 <<
    0 << 0 << 0 << 3 << 0 << 0 << 0 << 0 << 0 <<
    2 << 1 << 0 << 0 << 0 << 0 << 4 << 0 << 0 <<
    2 << 0 << 0 << 0 << 1 << 0 << 0 << 1 << 0 <<
    0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 <<
    1 << 0 << 0 << 0 << 2 << 3 << 1 << 0 << 3;

    PFMatrix m1(data, PFM_DINUCLEOTIDE);
    QMap<QString, QString> props;
    props["aaa"] = "bbb";
    props["ccc"] = "ddd";
    m1.setInfo(JasparInfo(props));

    QByteArray binary = FMatrixSerializer::serialize(m1);
    U2OpStatusImpl os;
    PFMatrix m2 = FMatrixSerializer::deserialize(binary, os);
    CHECK_NO_ERROR(os);

    CHECK_TRUE(2 == m2.getProperties().size(), "props");
    CHECK_TRUE("bbb" == m2.getProperty("aaa"), "aaa");
    CHECK_TRUE("ddd" == m2.getProperty("ccc"), "ccc");
    CHECK_TRUE(PFM_DINUCLEOTIDE == m2.getType(), "type");
    CHECK_TRUE(2 == m2.getValue(7, 5), "value 1");
    CHECK_TRUE(3 == m2.getValue(15, 8), "value 2");
}

IMPLEMENT_TEST(DatatypeSerializeUtilsUnitTest, FMatrixSerializer_failed) {
    U2OpStatusImpl os;
    FMatrixSerializer::deserialize("qqqqqqqqqqqqqqqqq", os);
    CHECK_TRUE(os.hasError(), "no error");
}

} // U2
