// Copyright 2019 The OpenSDS Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may
// not use this file except in compliance with the License. You may obtain
// a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations
// under the License.

package org.opensds.ecosystem.adapter.model.meta;

import java.util.HashMap;
import java.util.Map;

public enum DataType {

    TotalIOCount(1),

    ReadIORatio(2),

    WriteIORatio(3),

    CurrentBandWidth(4),

    MaxBandWidth(5),

    CurrentIOPS(6),

    MaxIOPS(7),

    MaxLatency(8),

    AverageLatency(9),

    CacheHit(10),

    LocalWriteCacheRatio(11),

    MirrorWriteCacheRatio(12),

    ReadIOTraffic(13),

    WriteIOTraffic(14),

    MaxPowerConsume(15),

    AvgPowerConsume(16),

    MinPowerConsume(17),

    Usage(18),

    QueueSize(19),

    ResponseTime(20),

    BandWidth(21),

    Throughput(22),

    ReadBandWidth(23),

    AverageReadIO(24),

    ReadThroughput(25),

    WriteBandWidth(26),

    AverageWriteIO(27),

    WirteThroughput(28),

    ServiceTime(29),

    ReadIOStatistics512B(30),

    ReadIOStatistics1KB(31),

    ReadIOStatistics2KB(32),

    ReadIOStatistics4KB(33),

    ReadIOStatistics8KB(34),

    ReadIOStatistics16KB(35),

    ReadIOStatistics32KB(36),

    ReadIOStatistics64KB(37),

    ReadIOStatistics128KB(38),

    ReadIOStatistics256KB(39),

    ReadIOStatistics512KB(40),

    WriteIOStatistics512B(41),

    WriteIOStatistics1KB(42),

    WriteIOStatistics2KB(43),

    WriteIOStatistics4KB(44),

    WriteIOStatistics8KB(45),

    WriteIOStatistics16KB(46),

    WriteIOStatistics32KB(47),

    WriteIOStatistics64KB(48),

    WriteIOStatistics128KB(49),

    WriteIOStatistics256KB(50),

    WriteIOStatistics512KB(51),

    ReadWriteIOStatistics512B(52),

    ReadWriteIOStatistics1KB(53),

    ReadWriteIOStatistics2KB(54),

    ReadWriteIOStatistics4KB(55),

    ReadWriteIOStatistics8KB(56),

    ReadWriteIOStatistics16KB(57),

    ReadWriteIOStatistics32KB(58),

    ReadWriteIOStatistics64KB(59),

    CycleWriteIOCnt(60),

    CycleReadIOCnt(61),

    ReadWriteIOStatistics512KB(62),

    WriteIOInPeriod(63),

    ReadIOInPeriod(64),

    RandomSequenceRatio(65),

    Usage2(66),

    ReadBandWidthUsage(67),

    CPUUsage(68),

    MemoryUsage(69),

    SwapUsage(70),

    NetReadTraffic(71),

    NetWriteTraffic(72),

    NetInPackage(73),

    NetOutPackage(74),

    WorkTemperture(75),

    PowerConsume(76),

    Voltage(77),

    AverageIOResponseTime(78),

    MaxIOResponseTime(79),

    DirtyPageRatio(80),

    RefreshCacheDivideWriteRequest(81),

    RefreshCacheBandWidth(82),

    RefreshCacheBecauseHightWater(83),

    RefreshCacheBecauseScheduleTimer(84),

    CacheLowWater(85),

    MaxBandWidthWrite(86),

    FetchFlux(87),

    FetchRatio(88),

    ReadCacheHit(89),

    ReadCacheNotHit(90),

    ReadCacheHitWhenWrite(91),

    ReadCacheHitWhenRead(92),

    ReadCacheRatio(93),

    ReadCacheReHitRatio(94),

    WriteCacheHitRatio(95),

    WriteCacheReHitRatio(96),

    WriteCacheHit(97),

    WriteCacheNotHit(98),

    AsyncTime(99),

    AsyncData(100),

    ReadeRequestCntFromSnapshot(101),

    ReadeRequestCntFromSourceLUN(102),

    WriteRequestCntFromSourceLUN(103),

    ReadRequestCntSnapshotPool(104),

    WriteRequestCntSnapshotPool(105),

    WriteRequestCntMorethanChunk(106),

    ChunkCntUsedBySnapshotCopy(107),

    LocalCacheUsage(108),

    MirrorCacheUsage(109),

    LocalReadCacheRatio(110),

    TimeForHostToSendData(111),

    TimeForSavingWriteData(112),

    TimeForReleasingWriteRequestResources(113),

    TimeForExecutingAWriteRequest(114),

    TimeForReleasingReadRequestResources(115),

    CacheReadIOLatency(116),

    CacheWriteIOLatency(117),

    CacheMirrorIOLatency(118),

    TimeForAllocatingMemoryToAWriteRequest(119),

    CacheWriteUsage(120),

    NumberOfCacheDataFlushIO(121),

    NumberOfCacheDataFlushPages(122),

    ReadBandwidthKB(123),

    WriteBandwidthKB(124),

    AverageReadIOControllerA(125),

    AverageWriteIOControllerA(126),

    MaxIOLatencyControllerA(127),

    AverageIOLatencyControllerA(128),

    UsageRatioControllerA(129),

    QueueLengthControllerA(130),

    ServiceTimeControllerA(131),



    NumberOfCombinedReadIOsPerSecondControllerA(132),



    NumberOfCombinedWriteIOsPerSecondControllerA(133),

    ReadWaitingTimeControllerA(134),

    WriteWaitingTimeControllerA(135),

    AverageIOSizeControllerA(136),

    AverageReadIOControllerB(137),

    AverageWriteIOControllerB(138),

    MaxIOLatencyControllerB(139),

    AverageIOLatencyControllerB(140),

    UsageRatioControllerB(141),

    QueueLengthControllerB(142),

    ServiceTimeControllerB(143),



    NumberOfCombinedReadIOsPerSecondControllerB(144),



    NumberOfCombinedWriteIOsPerSecondControllerB(145),

    ReadWaitingTimeControllerB(146),

    WriteWaitingTimeControllerB(147),

    AverageIOSizeControllerB(148),

    ReadIOGranularityDistribution4KB(149),

    ReadIOGranularityDistribution8KB(150),

    ReadIOGranularityDistribution16KB(151),

    ReadIOGranularityDistribution32KB(152),

    ReadIOGranularityDistribution64KB(153),

    ReadIOGranularityDistribution128KB(154),

    ReadIOGranularityDistribution256KB(155),

    ReadIOGranularityDistribution512KB(156),

    ReadIOGranularityDistribution1MB(157),

    ReadIOGranularityDistribution2MB(158),

    ReadIOGranularityDistributionMoreThan2MB(159),

    WriteIOGranularityDistribution4KB(160),

    WriteIOGranularityDistribution8KB(161),

    WriteIOGranularityDistribution16KB(162),

    WriteIOGranularityDistribution32KB(163),

    WriteIOGranularityDistribution64KB(164),

    WriteIOGranularityDistribution128KB(165),

    WriteIOGranularityDistribution256KB(166),

    WriteIOGranularityDistribution512KB(167),

    WriteIOGranularityDistribution1MB(168),

    WriteIOGranularityDistribution2MB(169),

    WriteIOGranularityDistributionMoreThan2MB(170),

    ReadAndWriteIOGranularityDistribution4KB(171),

    ReadAndWriteIOGranularityDistribution8KB(172),

    ReadAndWriteIOGranularityDistribution16KB(173),

    ReadAndWriteIOGranularityDistribution32KB(174),

    ReadAndWriteIOGranularityDistribution64KB(175),

    ReadAndWriteIOGranularityDistribution128KB(176),

    ReadAndWriteIOGranularityDistribution256KB(177),

    ReadAndWriteIOGranularityDistribution512KB(178),

    ReadAndWriteIOGranularityDistribution1MB(179),

    ReadAndWriteIOGranularityDistribution2MB(180),

    ReadAndWriteIOGranularityDistributionMoreThan2MB(181),

    OPS(182),

    NumberofOnlineUser(183),

    FileSpaceHardQuota(184),

    UsedCapacity(185),

    TotalCapacity(186),

    DedupeRate(187),

    DiskBusyRatio(192),

    PackagesRate(193),

    RvgBandWidth(194),

    ErrorPackage(190),

    DroppedPackage(189),

    OverrunPackage(191),

    XFerPackage(188),

    MaxReadIOLatency(195),

    MaxWriteIOLatency(196),

    AverageReadIOLatency(197),

    AverageWriteIOLatency(198),

    ReadIOLatencyDistribution10ms(199),

    ReadIOLatencyDistribution20ms(200),

    ReadIOLatencyDistribution50ms(201),

    ReadIOLatencyDistribution100ms(202),

    ReadIOLatencyDistribution200ms(203),

    ReadIOLatencyDistributionMoreThan200ms(204),

    WriteIOLatencyDistribution10ms(205),

    WriteIOLatencyDistribution20ms(206),

    WriteIOLatencyDistribution50ms(207),

    WriteIOLatencyDistribution100ms(208),

    WriteIOLatencyDistribution200ms(209),

    WriteIOLatencyDistributionMoreThan200ms(210),

    ReadandWriteIOLatencyDistribution10ms(211),

    ReadandWriteIOLatencyDistribution20ms(212),

    ReadandWriteIOLatencyDistribution50ms(213),

    ReadandWriteIOLatencyDistribution100ms(214),

    ReadandWriteIOLatencyDistribution200ms(215),

    ReadandWriteIOLatencyDistributionMoreThan200ms(216),

    MaxCPUUsage(217),

    MaxMemoryUsage(218),

    NFSBandwidth(219),

    NFSInBandwidth(220),

    NFSOutBandwidth(221),

    CIFSBandwidth(222),

    CIFSInBandwidth(223),

    CIFSOutBandwidth(224),

    ConnectedClientCount(225),

    ConnectedNFSClientCount(226),

    ConnectedCIFSClientCount(227),

    AverageIOSize(228),

    NetworkPacketRate(229),

    NetworkInboundPacketRate(230),

    NetworkOutboundPacketRate(231),

    ReadOPS(232),

    WriteOPS(233),

    NetTraffic(236),

    ReadL2CacheHit(237),

    ReadL2CacheNotHit(238),

    ReadL2CacheHitRatio(239),

    AverageQueueSize(240),

    CompleteSCSICommandsPerSecond(241),

    VerifyCommandsPerSecond(242),

    DiskTotalCapacity(243),

    DiskAllocedCapacity(244),

    DiskAllocedUsage(245),

    DiskFreeUsage(246),

    WPCount(247),

    WPLimit(248),

    WPUtilization(249),

    CacheUsed(250),

    HostHit(251),

    HostReadHit(252),

    HostWriteHit(253),

    HostMiss(254),

    HostReadMiss(255),

    HostWriteMiss(256),

    BEReq(257),

    BEReadReq(258),

    BEWriteReq(259),

    BEMBstransferred(260),

    BEMBsRead(261),

    BEMBsWritten(262),

    ReadMissResponseTime(263),

    WriteMissResponseTime(264),

    ReadPercent(265),

    WritePercent(266),

    ReadHitPercent(267),

    WriteHitPercent(268),

    ReadMissPercent(269),

    WriteMissPercent(270),

    BEReadRequestTime(271),

    BEDiskReadResponseTime(272),

    BEReadTaskTime(273),

    BEReadResponseTime(274),

    BEWriteResponseTime(275),

    BEAvgResponseTime(276),

    BEReadsPercent(277),

    BEWritesPercent(278),

    Request(279),

    ReadRequest(280),

    WriteRequest(281),

    ReadHitRequest(282),

    WriteHitRequest(283),

    MissRequest(284),

    ReadMissRequest(285),

    WriteMissRequest(286),

    ReadRequestPercent(287),

    WriteRequestPercent(288),

    HitRequestPercent(289),

    ReadRequestHitPercent(290),

    IOs(291),

    Requests(292),

    ReadRequests(293),

    WriteRequests(294),

    Flow(295),

    PortFlow(296),

    PortIOs(297),

    ReadFlow(298),

    WrittenFlow(299),

    PortAvgReqSize(300),

    MaxWPThreshold(301),

    RandomHits(302),

    HitPercent(303),

    Missercent(304),

    RandomReadsPercent(305),

    RandomWritesPercent(306),

    MaxIOPSsPerSec(307),

    FailedIOs(308),

    FailedIOsPerSec(309),

    FailedIORatio(310),

    SNAS_DISK_THROUGHPUT(311),

    SNAS_DISK_READ_DATA_THROUGHPUT(312),

    SNAS_DISK_WRITE_DATA_THROUGHPUT(313),

    Cache_Water(333),

    Read_MBs(334),

    Write_MBs(335),

    t351(351),

    t352(352),

    t353(353),

    t354(354),

    t355(355),

    t356(356),

    t357(357),

    t358(358),

    t359(359),

    t360(360),

    t361(361),

    t362(362),

    t363(363),

    t364(364),

    t365(365),

    t366(366),

    t367(367),

    t368(368),

    AverageIOResponseTimeUs(370),

    MaxIOResponseTimeUs(371),

    MaxReadIOResponseTimeUs(382),

    MaxWriteIOResponseTimeUs(383),

    AverageReadIOResponseTimeUs(384),

    AverageWriteIOResponseTimeUs(385),

    NFS_AVERAGE_LATENCY(410),

    NFS_MAX_LATENCY(411),

    NFS_WRITE_AVERAGE_LATENCY(412),

    NFS_WRITE_MAX_LATENCY(413),

    NFS_READ_AVERAGE_LATENCY(414),

    NFS_READ_MAX_LATENCY(415),

    CIFS_AVERAGE_LATENCY(416),

    CIFS_MAX_LATENCY(417),

    CIFS_WRITE_AVERAGE_LATENCY(418),

    CIFS_WRITE_MAX_LATENCY(419),

    CIFS_READ_AVERAGE_LATENCY(420),

    CIFS_READ_MAX_LATENCY(421),

    AVERAGE_LATENCY(428),

    MAX_LATENCY(429),

    WRITE_AVERAGE_LATENCY(430),

    WRITE_MAX_LATENCY(431),

    READ_AVERAGE_LATENCY(432),

    READ_MAX_LATENCY(433),

    COUNT_OF_READ_LARGE_LATENCY_CONTINUOUS_OCCURRED_FOR_CIFS(435),

    COUNT_OF_READ_LARGE_LATENCY_CONTINUOUS_OCCURRED_FOR_NFS(436),

    COUNT_OF_READ_LARGE_LATENCY_CONTINUOUS_OCCURRED(437),

    NFS_MIN_LATENCY(438),

    BPS(439),

    NFS_SIZE_0K_512B_READ(440),

    NFS_SIZE_512B_1K_READ(441),

    NFS_SIZE_1K_2K_READ(442),

    NFS_SIZE_2K_4K_READ(443),

    NFS_SIZE_4K_8K_READ(444),

    NFS_SIZE_8K_16K_READ(445),

    NFS_SIZE_16K_32K_READ(446),

    NFS_SIZE_32K_64K_READ(447),

    NFS_SIZE_64K_128K_READ(448),

    NFS_SIZE_128K_256K_READ(449),

    NFS_SIZE_OVER_256K_READ(450),

    NFS_SIZE_0K_512B_WRITE(451),

    NFS_SIZE_512B_1K_WRITE(452),

    NFS_SIZE_1K_2K_WRITE(453),

    NFS_SIZE_2K_4K_WRITE(454),

    NFS_SIZE_4K_8K_WRITE(455),

    NFS_SIZE_8K_16K_WRITE(456),

    NFS_SIZE_16K_32K_WRITE(457),

    NFS_SIZE_32K_64K_WRITE(458),

    NFS_SIZE_64K_128K_WRITE(459),

    NFS_SIZE_128K_256K_WRITE(460),

    NFS_SIZE_OVER_256K_WRITE(461),

    SPACE_USAGE(462),

    SPACE_SIZE(463),

    AVERAGE_READ_OPS_RESPONSE_TIME(464),

    AVERAGE_WRITE_OPS_RESPONSE_TIME(465),

    ReadIOStatistics0BTo512B(466),

    ReadIOStatistics512BTo1KB(467),

    ReadIOStatisticsMoreThan256KB(468),

    WriteIOStatistics0BTo512B(469),

    WriteIOStatistics512BTo1KB(470),

    WriteIOStatisticsMoreThan256KB(471),

    MIN_LATENCY(472),

    FILE_BANDWIDTH(475),

    FILE_OPS(476),

    AVG_LATENCY_OPS_US(508),

    MAX_LATENCY_OPS_US(509),

    MIN_LATENCY_OPS_US(510),

    LOGICAL_BANDWITH(473),

    TOTAL_MOVED_DATA_SIZE(513),

    MOVED_TO_SSD_DATA_SIZE(514),

    MOVED_TO_SAS_DATA_SIZE(515),

    MOVED_TO_NLSAS_DATA_SIZE(516),

    SAS_MOVED_TO_SSD_DATA_SIZE(517),

    NLSAS_MOVED_TO_SSD_DATA_SIZE(518),

    SSD_MOVED_TO_SAS_DATA_SIZE(519),

    NLSAS_MOVED_TO_SAS_DATA_SIZE(520),

    SSD_MOVED_TO_NLSAS_DATA_SIZE(521),

    SAS_MOVED_TO_NLSAS_DATA_SIZE(522),

    FILE_BANDWIDTH_KBs(511),

    THROUGHPUT_KBs(512),

    SERVICE_TIME_US(523),

    AVG_READ_OPS_RESPONSE_US(524),

    AVG_WRITE_OPS_RESPONSE_US(525),

    READ_IO_LATENCY_DISTRIBUTION_0MS_TO_5MS(530),

    READ_IO_LATENCY_DISTRIBUTION_5MS_TO_10MS(396),

    READ_IO_LATENCY_DISTRIBUTION_15MS_TO_20MS(200),

    READ_IO_LATENCY_DISTRIBUTION_20MS_TO_50MS(201),

    READ_IO_LATENCY_DISTRIBUTION_50MS_TO_100MS(202),

    READ_IO_LATENCY_DISTRIBUTION_100MS_TO_200MS(203),

    READ_IO_LATENCY_DISTRIBUTION_200MSMORE(204),

    WRITE_IO_LATENCY_DISTRIBUTION_0MS_TO_5MS(531),

    WRITE_IO_LATENCY_DISTRIBUTION_5MS_TO_10MS(402),

    WRITE_IO_LATENCY_DISTRIBUTION_15MS_TO_20MS(206),

    WRITE_IO_LATENCY_DISTRIBUTION_20MS_TO_50MS(207),

    WRITE_IO_LATENCY_DISTRIBUTION_50MS_TO_100MS(208),

    WRITE_IO_LATENCY_DISTRIBUTION_100MS_TO_200MS(209),

    WRITE_IO_LATENCY_DISTRIBUTION_200MSMORE(210),

    READ_IO_FAILED(532),

    WRITE_IO_FAILED(533),

    CAPCITY_USEAGE(90004),

    FILE_SYSTEM_BANDWIDTH(534),

    FILE_SYSTEM_READ_BANDWIDTH(535),

    FILE_SYSTEM_WRITE_BANDWIDTH(536),

    S3_BANDWIDTH(537),

    S3_READ_BANDWIDTH(538),

    S3_WRITE_BANDWIDTH(539),

    S3_DELETE_NUMBER(540),

    S3_DELETE_FAILED_NUMBER(541),

    S3_DELETE_FAILED_NUMBER_CLIENTS_CAUSE(542),

    S3_GET_NUMBER(543),

    S3_GET_FAILED_NUMBER(544),

    S3_GET_FAILED_NUMBER_CLIENTS_CAUSE(545),

    S3_PUT_NUMBER(546),

    S3_PUT_FAILED_NUMBER(547),

    S3_PUT_FAILED_NUMBER_CLIENTS_CAUSE(548),

    S3_CONNECT_FAILED_RATE(549),

    S3_HEAD_NUMBER(1061),

    S3_HEAD_FAILED_NUMBER(1062),

    S3_HEAD_FAILED_NUMBER_CLIENTS_CAUSE(1063),

    S3_POST_NUMBER(1064),

    S3_POST_FAILED_NUMBER(1065),

    S3_POST_FAILED_NUMBER_CLIENTS_CAUSE(1066),

    SWIFT_BANDWIDTH(550),

    SWIFT_READ_BANDWIDTH(551),

    SWIFT_WRITE_BANDWIDTH(552),

    SWIFT_DELETE_NUMBER(553),

    SWIFT_DELETE_FAILED_NUMBER(554),

    SWIFT_DELETE_FAILED_NUMBER_CLIENTS_CAUSE(555),

    SWIFT_GET_NUMBER(556),

    SWIFT_GET_FAILED_NUMBER(557),

    SWIFT_GET_FAILED_NUMBER_CLIENTS_CAUSE(558),

    SWIFT_PUT_NUMBER(559),

    SWIFT_PUT_FAILED_NUMBER(560),

    SWIFT_PUT_FAILED_NUMBER_CLIENTS_CAUSE(561),

    SWIFT_CONNECT_FAILED_RATE(562),

    SWIFT_HEAD_NUMBER(1067),

    SWIFT_HEAD_FAILED_NUMBER(1068),

    SWIFT_HEAD_FAILED_NUMBER_CLIENTS_CAUSE(1069),

    SWIFT_POST_NUMBER(1070),

    SWIFT_POST_FAILED_NUMBER(1071),

    SWIFT_POST_FAILED_NUMBER_CLIENTS_CAUSE(1072),

    SYNCHRONIZATION_DURATION(1054),

    CACHE_PAGE_UTILIZATION(1055),

    CACHE_CHUNK_UTILIZATION(1056),

    CACHE_PAGEUNIT_UTILIZATION(1057),

    THE_CACHE_PAGE_PRESERVATION(1059),

    THE_CACHE_CHUNK_PRESERVATION(1060),

    SCSI_IOPS(1079),

    ISCSI_IOPS(1073),

    NFS_OPERSTION_COUNT_PER_SECOND(627),

    CIFS_OPERSTION_COUNT_PER_SECOND(1074),

    TOTAL_DISK_IOPS(1076),

    READ_DISK_IOPS(1077),

    WRITE_DISK_IOPS(1078),

    DISK_MAX_USAGE(1075),

    Total_Capacity(10000),

    Used_Capacity(10001),

    Free_Capacity(10002),

    Capacity_Usage_Rate(10003),

    Capacity_Free_Rate(10004);

    private static Map<Integer, DataType> collect = new HashMap<Integer, DataType>(DataType.values().length);

    private int value;

    DataType(int value) {
        this.value = value;
    }

    public int getValue() {
        return this.value;
    }

    public static DataType valueOf(int intValue) {
        return collect.get(intValue);
    }

    static {
        for (DataType value : DataType.values()) {
            collect.put(value.getValue(), value);
        }
    }
}
