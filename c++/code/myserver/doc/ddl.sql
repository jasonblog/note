# noinspection SqlNoDataSourceInspectionForFile
/*
SQLyog Community v12.02 (64 bit)
MySQL - 5.7.18 : Database - my_game
*********************************************************************
*/


/*!40101 SET NAMES utf8 */;

/*!40101 SET SQL_MODE = '' */;

/*!40014 SET @OLD_UNIQUE_CHECKS = @@UNIQUE_CHECKS, UNIQUE_CHECKS = 0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS = @@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS = 0 */;
/*!40101 SET @OLD_SQL_MODE = @@SQL_MODE, SQL_MODE = 'NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES = @@SQL_NOTES, SQL_NOTES = 0 */;
CREATE DATABASE /*!32312 IF NOT EXISTS */`my_game` /*!40100 DEFAULT CHARACTER SET latin1 */;

USE `my_game`;

/*Table structure for table `alliance` */

CREATE TABLE `alliance` (
  `id`          INT(11)     NOT NULL,
  `icon`        INT(11)NOT NULL,
  `name`        VARCHAR(32) NOT NULL,
  `short_name`  VARCHAR(16) NOT NULL,
  `declaration` VARCHAR(256)         DEFAULT NULL,
  `language`    VARCHAR(32) NOT NULL,
  `integral`    INT(11)     NOT NULL,
  `honour`      INT(11)     NOT NULL,
  `tech`        BLOB,
  `create_at`   INT(11),
  `rank_history` INT(11),
  `condition`   BLOB,
  `trader_index` INT(11) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE = InnoDB DEFAULT CHARSET = utf8;

/*Table structure for table `alliance_assistance` */

CREATE TABLE `alliance_assistance` (
  `id`             INT(11) NOT NULL,
  `alliance_id`    INT(11)    NOT NULL,
  `sender_id`      BIGINT(20) NOT NULL,
  `type`           TINYINT(4) NOT NULL,
  `level`          INT(11)    NOT NULL,
  `times`          INT(11)    NOT NULL,
  `sender_task_id` INT(11)    NOT NULL,
  `target_id`      INT(11)    NOT NULL,
  `helped_ids`     BLOB   COMMENT '已经帮助过的玩家ID',
  PRIMARY KEY (`id`),
  KEY `alliance_assistance_fk` (`alliance_id`),
  CONSTRAINT `alliance_assistance_fk` FOREIGN KEY (`alliance_id`) REFERENCES `alliance` (`id`)
    ON DELETE CASCADE
    ON UPDATE NO ACTION
) ENGINE = InnoDB DEFAULT CHARSET = utf8;

/*Table structure for table `alliance_member` */

CREATE TABLE `alliance_member` (
  `alliance_id`   INT(11)    NOT NULL,
  `player_id`     BIGINT(20) NOT NULL,
  `position`      TINYINT(4) NOT NULL,
  `donation`      INT(11)    NOT NULL,
  `join_time`     INT(11)    NOT NULL,
  UNIQUE KEY `alliance_unique_player` (`alliance_id`, `player_id`),
  CONSTRAINT `alliance_member_fk` FOREIGN KEY (`alliance_id`) REFERENCES `alliance` (`id`)
    ON DELETE CASCADE
    ON UPDATE NO ACTION
) ENGINE = InnoDB DEFAULT CHARSET = utf8;

/*Table structure for table `alliance_news` */

CREATE TABLE `alliance_news` (
  `id`          INT(11) NOT NULL,
  `alliance_id` INT(11)    NOT NULL,
  `type`        TINYINT(4) NOT NULL,
  `create_at`   INT(11),
  `detail`      BLOB         NOT NULL,
  PRIMARY KEY (`id`),
  KEY `alliance_news_fk` (`alliance_id`),
  CONSTRAINT `alliance_news_fk` FOREIGN KEY (`alliance_id`) REFERENCES `alliance` (`id`)
    ON DELETE CASCADE
    ON UPDATE NO ACTION
) ENGINE = InnoDB DEFAULT CHARSET = utf8;

/*Table structure for table `alliance_apply` */
CREATE TABLE `alliance_apply` (
  `alliance_id` INT(11) NOT NULL,
  `player_id` BIGINT(20) NOT NULL,
  `news_id` INT(11) NOT NULL,
  UNIQUE KEY `alliance_unique_player` (`alliance_id`,`player_id`),
  CONSTRAINT `alliance_apply_fk` FOREIGN KEY (`alliance_id`) REFERENCES `alliance` (`id`) ON DELETE CASCADE ON UPDATE NO ACTION
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

/*Table structure for table `player_data` */

CREATE TABLE `player_data` (
  `player_id`  BIGINT(20) NOT NULL COMMENT '玩家ID',
  `properties` BLOB       NOT NULL COMMENT '基础属性信息',
  `city`       BLOB       NOT NULL COMMENT '城镇信息',
  `timer`      BLOB COMMENT '定时任务信息',
  `bag`        BLOB COMMENT '背包信息',
  `counter`    BLOB COMMENT '数据控制信息',
  `task`       BLOB COMMENT '任务信息',
  `map`        BLOB COMMENT '地图相关信息',
  `wall`       BLOB COMMENT '玩家城墙信息',
  `alliance`   BLOB COMMENT '联盟私有信息',
  `friend`     BLOB COMMENT '好友信息',
  PRIMARY KEY (`player_id`)
) ENGINE = InnoDB DEFAULT CHARSET = utf8;

/*Table structure for table `player_profile` */

CREATE TABLE `player_profile` (
  `player_id`     BIGINT(20)  NOT NULL COMMENT '玩家ID',
  `name`          VARCHAR(32) NOT NULL COMMENT '名称',
  `image_type`    TINYINT(4)  NOT NULL COMMENT '玩家形象',
  `icon`          VARCHAR(256)NOT NULL COMMENT '头像',
  `level`         INT(11)     NOT NULL COMMENT '等级',
  `exp`           INT(11)     NOT NULL COMMENT '当前等级经验',
  `castle_level`  INT(10)     NOT NULL COMMENT '城堡等级',
  `map_id`        INT(11)     NOT NULL COMMENT '地图id',
  `alliance_id`   INT(11)     NOT NULL,
  `battle_value`  INT(11)     NOT NULL COMMENT '战力',
  `create_at`     INT(11)     NOT NULL COMMENT '创建时间',
  `last_login_at` INT(11)     NOT NULL COMMENT '上次登录时间',
  `protecting`    BLOB COMMENT '保护信息',
  `vip`           BLOB COMMENT 'VIP信息',
  `more`          BLOB COMMENT '扩展信息',
  PRIMARY KEY (`player_id`)
) ENGINE = InnoDB DEFAULT CHARSET = utf8;

/*Table structure for table `region_meta` */

CREATE TABLE `region_meta` (
  `region_id` INT(11) NOT NULL,
  `open_at`   INT(11) NOT NULL,
  `free_trader`   BLOB,
  PRIMARY KEY (`region_id`)
) ENGINE = InnoDB DEFAULT CHARSET = utf8;

/*Table structure for table `report_battle` */

CREATE TABLE `report_battle` (
  `report_id` BIGINT(20)  NOT NULL AUTO_INCREMENT COMMENT '战报ID',
  `type`      TINYINT(4)  NOT NULL COMMENT '类型',
  `create_at` INT(11)     NOT NULL COMMENT '创建时间',
  `data`      BLOB        NOT NULL COMMENT '数据',
  `ref_count` SMALLINT(6) NOT NULL COMMENT '引用数',
  PRIMARY KEY (`report_id`)
) ENGINE = InnoDB DEFAULT CHARSET = latin1;

/*Table structure for table `report_spy` */

CREATE TABLE `report_spy` (
  `report_id` BIGINT(20) NOT NULL AUTO_INCREMENT
  COMMENT '侦查报告ID',
  `type`      TINYINT(4) NOT NULL
  COMMENT '类型',
  `data`      BLOB       NOT NULL
  COMMENT '数据',
  `create_at` INT(11)    NOT NULL
  COMMENT '创建时间',
  PRIMARY KEY (`report_id`)
) ENGINE = InnoDB DEFAULT CHARSET = utf8;

/*Table structure for table `user` */

CREATE TABLE `user` (
  `player_id` BIGINT(20)   NOT NULL AUTO_INCREMENT,
  `platform`  VARCHAR(60)  NOT NULL,
  `puid`      VARCHAR(128) NOT NULL,
  PRIMARY KEY (`player_id`),
  UNIQUE KEY `platform_user` (`platform`, `puid`)
) ENGINE = InnoDB DEFAULT CHARSET = utf8;

/*Table structure for table `world_map` */

CREATE TABLE `world_map` (
  `id`     INT(11)    NOT NULL COMMENT '生成ID',
  `type`   TINYINT(5) NOT NULL COMMENT '类型',
  `center` BLOB       NOT NULL COMMENT '中心点位置',
  `detail` BLOB       NOT NULL COMMENT '详细数据',
  PRIMARY KEY (`id`)
) ENGINE = InnoDB DEFAULT CHARSET = utf8;

/*Table structure for table `world_troop` */

CREATE TABLE `world_troop` (
  `id`             BIGINT(20) NOT NULL COMMENT '出征ID',
  `type`           TINYINT(4) NOT NULL COMMENT '类型',
  `owner`          BLOB       NOT NULL COMMENT '所有者类型',
  `state`          TINYINT(4) NOT NULL COMMENT '状态',
  `target_id`      BIGINT(20) NOT NULL COMMENT '目标ID',
  `from`           BLOB       NOT NULL COMMENT '从哪里来',
  `to`             BLOB       NOT NULL COMMENT '到哪里去',
  `speed`          INT(11)    NOT NULL COMMENT '走多快',
  `start_at`       INT(11)    NOT NULL COMMENT '什么时候开始',
  `duration`       INT(11)    NOT NULL COMMENT '多长时间到达',
  `total_duration` INT(11)    NOT NULL COMMENT '总需时间',
  `speed_up_times` INT(11)    NOT NULL,
  `detail`         BLOB       COMMENT '出征详情',
  PRIMARY KEY (`id`)
) ENGINE = InnoDB DEFAULT CHARSET = utf8;

CREATE TABLE IF NOT EXISTS `chat_group` (
  `id` bigint(20) unsigned NOT NULL,
  `data` blob NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

/*!40101 SET SQL_MODE = @OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS = @OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS = @OLD_UNIQUE_CHECKS */;
/*!40111 SET SQL_NOTES = @OLD_SQL_NOTES */;
