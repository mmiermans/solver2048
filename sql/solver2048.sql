-- phpMyAdmin SQL Dump
-- version 4.0.10deb1
-- http://www.phpmyadmin.net
--
-- Host: localhost
-- Generation Time: Jul 27, 2014 at 05:58 PM
-- Server version: 5.5.38-0ubuntu0.14.04.1
-- PHP Version: 5.5.9-1ubuntu4.3

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

--
-- Database: `solver2048`
--

DELIMITER $$
--
-- Procedures
--
CREATE DEFINER=`root`@`localhost` PROCEDURE `get_game_moves`(IN `p_game_id` INT, IN `p_move_count` INT)
    READS SQL DATA
BEGIN
	SELECT `id`, `has_ended`
	FROM games
	WHERE id=p_game_id;
	
	SELECT
		Cast(`board_before_move` as char),
		`score_before_move`,
		`move_direction`,
		`move_count`,
		`new_tile_value`,
		`new_tile_position`
	FROM moves
	WHERE game_id=p_game_id AND move_count>=p_move_count
	;
END$$

CREATE DEFINER=`root`@`localhost` PROCEDURE `get_newest_game_moves`()
    READS SQL DATA
    DETERMINISTIC
BEGIN
	SELECT `id`, `has_ended`
	FROM games
	WHERE id=(SELECT MAX(id) FROM games);
	
	SELECT
		Cast(`board_before_move` as char),
		`score_before_move`,
		`move_direction`,
		`move_count`,
		`new_tile_value`,
		`new_tile_position`
	FROM moves
	WHERE game_id=(SELECT MAX(id) FROM games);
END$$

DELIMITER ;

-- --------------------------------------------------------

--
-- Table structure for table `games`
--

CREATE TABLE IF NOT EXISTS `games` (
  `id` bigint(20) NOT NULL AUTO_INCREMENT,
  `start_date` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT 'The default value is a hack to make it now() as default for MySQL 5.5 (only one CURRENT_TIMESTAMP column allowed).',
  `end_date` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `score` int(11) NOT NULL DEFAULT '0' COMMENT 'Score as calculated per rules of 2048.',
  `max_tile` int(11) NOT NULL DEFAULT '0' COMMENT 'Value of highest tile.',
  `has_ended` tinyint(1) NOT NULL DEFAULT '0' COMMENT 'True if the game has reached a deadlocked position.',
  `board` bigint(20) unsigned zerofill NOT NULL DEFAULT '00000000000000000000' COMMENT 'Board position encoded in 64-bit integer.',
  `move_count` int(11) NOT NULL DEFAULT '0' COMMENT 'Number of moves so far including this move.',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COMMENT='Stores game state and statistics' AUTO_INCREMENT=1 ;

-- --------------------------------------------------------

--
-- Table structure for table `moves`
--

CREATE TABLE IF NOT EXISTS `moves` (
  `id` bigint(20) NOT NULL AUTO_INCREMENT,
  `game_id` bigint(20) NOT NULL,
  `board_before_move` bigint(20) unsigned zerofill NOT NULL DEFAULT '00000000000000000000' COMMENT 'Board before move has been executed.',
  `score_before_move` int(11) NOT NULL DEFAULT '0' COMMENT 'Score before move took place.',
  `move_direction` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT 'Move that player performed on board (before new tile was inserted). Up=1, Right=2, Down=4, Left=8.',
  `move_count` int(11) NOT NULL,
  `new_tile_value` int(11) NOT NULL DEFAULT '2',
  `new_tile_position` int(11) NOT NULL DEFAULT '0' COMMENT 'Position encoded as x+4y.',
  `time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`),
  UNIQUE KEY `game_move_index` (`game_id`,`move_count`),
  KEY `game_id` (`game_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COMMENT='Information regarding individual moves.' AUTO_INCREMENT=1 ;

--
-- Constraints for dumped tables
--

--
-- Constraints for table `moves`
--
ALTER TABLE `moves`
  ADD CONSTRAINT `moves_game_id` FOREIGN KEY (`game_id`) REFERENCES `games` (`id`) ON DELETE NO ACTION ON UPDATE NO ACTION;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
