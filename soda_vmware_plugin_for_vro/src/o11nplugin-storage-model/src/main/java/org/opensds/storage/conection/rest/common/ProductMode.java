// Copyright 2019 The OpenSDS Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may
// not use this file except in compliance with the License. You may obtain
// a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations
// under the License.

package org.opensds.storage.conection.rest.common;

public enum ProductMode {
    /**
     * V1500
     */
    V1500(0),
    /**
     * V1800
     */
    V1800(1),
    /**
     * S2100
     */
    S2100(2),
    /**
     * S2300
     */
    S2300(3),
    /**
     * S5100
     */
    S5100(4),
    /**
     * S5300
     */
    S5300(5),
    /**
     * S5500
     */
    S5500(6),
    /**
     * S
     */
    S(7),
    /**
     * VIS
     */
    VIS(8),
    /**
     * S2600
     */
    S2600(9),
    /**
     * S6800E
     */
    S6800E(10),
    /**
     * V1500N
     */
    V1500N(11),
    /**
     * S2300E
     */
    S2300E(12),
    /**
     * COMMON
     */
    COMMON(13),
    /**
     * N8000
     */
    N8000(15),
    /**
     * S6900
     */
    S6900(16),
    /**
     * S3900
     */
    S3900(17),
    /**
     * S5500T
     */
    S5500T(18),
    /**
     * ST
     */
    S5600T(19),
    /**
     * S6800T
     */
    S6800T(20),
    /**
     * S6900_M100
     */
    S6900_M100(21),
    /**
     * S3900_M200
     */
    S3900_M200(22),
    /**
     * S3900_M300
     */
    S3900_M300(23),
    /**
     * S5900_M100
     */
    S5900_M100(24),
    /**
     * S8100
     */
    S8100(25),
    /**
     * S5800T
     */
    S5800T(26),
    /**
     * V1600N
     */
    V1600N(27),
    /**
     * S5900_M200
     */
    S5900_M200(28),
    /**
     * N8001
     */
    N8001(29),
    /**
     * S2600T
     */
    S2600T(30),
    /**
     * S8000_I
     */
    S8000_I(31),
    /**
     * Dorado5100
     */
    Dorado5100(32),
    /**
     * T3200
     */
    T3200(33),
    /**
     * T3500
     */
    T3500(34),
    /**
     * N8000_R710
     */
    N8000_R710(35),
    /**
     * N8000_C2
     */
    N8000_C2(36),
    /**
     * N8000_C2_SYMC
     */
    N8000_C2_SYMC(40),
    /**
     * VTL
     */
    VTL(41),
    /**
     * S2900
     */
    S2900(42),
    /**
     * S2200T
     */
    S2200T(43),
    /**
     * Dorado2100_G2
     */
    Dorado2100_G2(44),
    /**
     * VIS_6000
     */
    VIS_6000(45),
    /**
     * HVS85T
     */
    HVS85T(46),
    /**
     * HVS88T
     */
    HVS88T(47),
    /**
     * N9000
     */
    N9000(48),
    /**
     * T3200_G2
     */
    T3200_G2(49),
    /**
     * T3500_G2
     */
    T3500_G2(50),
    /**
     * T3200_G3
     */
    T3200_G3(51),
    /**
     * T3500_G3
     */
    T3500_G3(52),
    /**
     * S5700
     */
    S5700(53),
    /**
     * CE6800
     */
    CE6800(54),
    /**
     * CE12800
     */
    CE12800(55),
    /**
     * HVS_18800F
     */
    HVS_18800F(56),
    /**
     * HVS_18500
     */
    HVS_18500(57),
    /**
     * HVS_18800
     */
    HVS_18800(58),
    /**
     * SSS
     */
    SSS(60),
    /**
     * 6800 V3
     */
    U_6800_V3(61),
    /**
     * 6900 V3
     */
    U_6900_V3(62),
    /**
     * 5600 V3
     */
    U_5600_V3(63),
    /**
     * 5800 V3
     */
    U_5800_V3(64),
    /**
     * T3200 MINI
     */
    T3200_MINI(65),
    /**
     * WUSHAN_SQL
     */
    WUSHAN_SQL(66),
    /**
     * WUSHAN_HD
     */
    WUSHAN_HD(67),
    /**
     * 5500 V3
     */
    U_5500_V3(68),
    /**
     * 2600 V3
     */
    U_2600_V3(69),
    /**
     * 5300 V3
     */
    U_5300_V3(70),
    /**
     * 2800 V3
     */
    U_2800_V3(71),
    /**
     * 18500_V3
     */
    U_18500_V3(72),
    /**
     * 18800_V3
     */
    U_18800_V3(73),
    /**
     * 2200_V3
     */
    U_2200_V3(74),
    /**
     * RH2288V3_25
     */
    RH2288V3_25(75),
    /**
     * 2600F V3
     */
    U_2600F_V3(84),
    /**
     * 5500F V3
     */
    U_5500F_V3(85),
    /**
     * 5600F V3
     */
    U_5600F_V3(86),
    /**
     * 5800F V3
     */
    U_5800F_V3(87),
    /**
     * 6800F V3
     */
    U_6800F_V3(88),
    /**
     * 18500F V3
     */
    U_18500F_V3(89),
    /**
     * 18800F V3
     */
    U_18800F_V3(90),
    /**
     * 2800 V5
     */
    U_2800_V5(92),
    /**
     * 5300 V5
     */
    U_5300_V5(93),
    /**
     * 5300F V5
     */
    U_5300F_V5(94),
    /**
     * 5500 V5
     */
    U_5500_V5(95),
    /**
     * 5500F V5
     */
    U_5500F_V5(96),
    /**
     * 5600 V5
     */
    U_5600_V5(97),
    /**
     * 5600F V5
     */
    U_5600F_V5(98),
    /**
     * 5800 V5
     */
    U_5800_V5(99),
    /**
     * 5800F V5
     */
    U_5800F_V5(100),
    /**
     * 6800 V5
     */
    U_6800_V5(101),
    /**
     * 6800F V5
     */
    U_6800F_V5(102),
    /**
     * 18500 V5
     */
    U_18500_V5(103),
    /**
     * 18500F V5
     */
    U_18500F_V5(104),
    /**
     * 18800 V5
     */
    U_18800_V5(105),
    /**
     * 18800F V5
     */
    U_18800F_V5(106),
    /**
     * 5500 V5 Elite
     */
    U_5500_V5_Elite(107),
    /**
     * 2100 V3
     */
    U_2100_V3(108),
    /**
     * 2200_V3_Enhanced
     */
    U_2200_V3_Enhanced(112),
    /**
     * 2600_V3_Enhanced
     */
    U_2600_V3_Enhanced(113),
    /**
     * 2200F_V3_Enhanced
     */
    U_2600F_V3_Enhanced(114),
    /**
     * 5300_V5_Enhanced
     */
    U_5300_V5_Enhanced(115),
    /**
     * N5000
     */
    N5000(401),
    /**
     * VIS8200
     */
    VIS8200(800),
    /**
     * VIS8400
     */
    VIS8400(801),
    /**
     * VIS8600
     */
    VIS8600(802),
    /**
     * VIS8800
     */
    VIS8800(803),
    /**
     * CLOUD_PROTECTOR
     */
    CLOUD_PROTECTOR(804),
    /**
     * Dorado5000 V3
     */
    Dorado5000_V3(805),
    /**
     * Dorado6000 V3
     */
    Dorado6000_V3(806),
    /**
     * Dorado18000 V3
     */
    Dorado18000_V3(807),

    /**
     * DoradoNAS
     */
    DoradoNAS(808),

    /**
     * DoradoNAS Enhanced
     */
    U_DoradoNAS_Enhanced(809),

    /**
     * Dorado3000 V3
     */
    Dorado3000_V3(810),

    /**
     * 5110 V5
     */
    U_5110_V5(116),

    /**
     * 5110F V5
     */
    U_5110F_V5(117),

    /**
     * 5210 V5
     */
    U_5210_V5(118),

    /**
     * 5210F V5
     */
    U_5210F_V5(119),
    U_5310_V5(120),
    U_5310F_V5(121),
    U_5510_V5(122),
    U_5510F_V5(123),
    U_5610_V5(124),
    U_5610F_V5(125),
    U_5810_V5(126),
    U_5810F_V5(127),
    U_6810_V5(128),
    U_6810F_V5(129),
    U_15810_V5(130),
    U_15810F_V5(131),
    U_18810_V5(132),
    U_188810F_V5(133),
    U_5210_V5_Enhanced(134),
    U_5210F_V5_Enhanced(135),
    Dorado5300_V6(825),
    Dorado5500_V6(826),
    Dorado5600_V6(827),
    Dorado5800_V6(828),
    Dorado6800_V6(829),
    Dorado18500_V6(830),
    Dorado18800_V6(831),

    PRODUCT_MODE_D5000V6(811),
    PRODUCT_MODE_D5000V6_N(812),
    PRODUCT_MODE_D5000V6_I(821),
    PRODUCT_MODE_D6000V6(813),
    PRODUCT_MODE_D6000V6_N(814),
    PRODUCT_MODE_D8000V6(815),
    PRODUCT_MODE_D8000V6_N(816),
    PRODUCT_MODE_D18000V6(817),
    PRODUCT_MODE_D18000V6_N(818),
    PRODUCT_MODE_D3000V6(819),
    PRODUCT_MODE_D6000V6_I(822),
    PRODUCT_MODE_D8000V6_I(823),
    PRODUCT_MODE_D18000V6_I(824);

    private int value;

    private ProductMode(int value) {
        this.value = value;
    }

    /**
     */
    public static ProductMode valueOf(int iValue) {
        for (ProductMode value : ProductMode.values()) {
            if (value.value == iValue) {
                return value;
            }
        }
        return null;
    }

    /**
     */
    public int getValue() {
        return this.value;
    }

    /**
     */

    public String toString() {
        return this.getClass().getSimpleName() + "." + this.name();
    }
}