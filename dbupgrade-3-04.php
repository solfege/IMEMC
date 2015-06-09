<?php
/************************************************************************/
/* Oscailt                                                              */
/* Indepenent Media Centre Content Management System                    */
/* ==================================================================== */
/* Copyright (c)2003-2005 by Independent Media Centre Ireland           */
/* http://www.indymedia.ie                                              */
/* Development List: oscailt@lists.indymedia.org                        */
/* See contributions.txt for the list of contributors                   */
/*                                                                      */
/* This program is free software. You can redistribute it and/or modify */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation. http://www.gnu.org/copyleft/gpl.html   */
/*                                                                      */
/* This program is distributed in the hope that it will be useful,      */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of       */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        */
/* GNU General Public License for more details.                         */
/************************************************************************/

	$PHP_SELF = "dbupgrade-3-04.php";
        require_once("objects/systemconfig.inc");
	require_once("objects/utilities.inc");
	require_once("objects/querycache.inc");
	require_once("objects/sqllayer.inc");
	require_once("objects/memorymgmt.inc");
	$system_config = new SystemConfig();
	$system_config->memory_mgmt_installed = false;
        $system_config->memory_mgmt_activated = false;


	// **** IMPORTANT ****
	// To run this script, place it in the html/ directory and then when you are finished with it,
	// either delete it or move it somewhere else not accessible by the public.
	// Note: You may need to temporarily put an entry for this script in your .htaccess file. Be sure
	// to remove it again once complete.
	// **** ********* ****

function upgradeDb() 
{
	global $prefix, $dbconn, $PHP_SELF;
	?>
	<h2>Upgrading db from Oscailt 3.3 to 3.4 ... </h2><BR>
	<P>This upgrade will create two new tables <strong>banned_host_browser </strong> and 
        <strong>feed_rules</strong> and modify the existing table <b>editors</b>.
	<p>
	Ensure you have sufficient database permissions under the user account that you are accessing the database 
	with for this upgrade. If not then you may need to apply the CREATE table commands separately.</P>
	<P>
	A futher update is applied by inserting a row in the <strong>configuration</strong> table for each of the
	new configuration parameters. See <strong>UpgradeGuide_3-4.html</strong> for details.</P>
	<p> For upgrades to the database for Oscailt 3.1, 3.2 and 3.3, see the corresponding upgrade instructions.</p>
	<P>
	<?
	if (!isset($_REQUEST['go_btn'])) {
	   ?>
	   <FORM name="upgrade_3_4" action="<?=$PHP_SELF?>">
	   <input type='submit' name="go_btn" value="Upgrade Now >>>">
	   </FORM>
	   <?
	   return;
        }

	echo "Adding new table ".$prefix. "_banned_host_browser ... <BR>";
	$result = sql_query("CREATE TABLE ".$prefix."_banned_host_browser ( sub_hostname varchar(120) NOT NULL, browser_type varchar(160) NOT NULL, ban_type tinyint NOT NULL default 1, time_limit timestamp, reason text NOT NULL, begin_ban datetime NULL, PRIMARY KEY  (sub_hostname,browser_type), KEY sub_hostname (sub_hostname), INDEX time_limit (time_limit))", $dbconn, 0);


	if (!checkForError($result)) {
	    echo "Database upgrade failed. Check your database permissions or that the table exists already.<BR>";
	    disconnectFromDatabase();
            return;
	}

	echo "Adding new table ".$prefix. "_feed_rules ... <BR>";
        $result = sql_query("CREATE TABLE ".$prefix."_feed_rules ( site_id int(11) NOT NULL, object_id int(11) NOT NULL, rule_text varchar(255), PRIMARY KEY (site_id,object_id), INDEX rule_key (site_id,object_id) )", $dbconn, 0);

	if (!checkForError($result)) {
	    echo "Database upgrade failed. Check your database permissions or that the table exists already.<BR>";
	    disconnectFromDatabase();
            return;
	}


	echo "Adding new column editor_options to editors table ... <BR>";
	checkForError(sql_query("ALTER TABLE ".$prefix."_editors ADD COLUMN editor_options int(2) default 0", $dbconn));

	if (!checkForError($result)) {
	    echo "Database upgrade failed. Check your database permissions or that the table exists already.<BR>";
	    disconnectFromDatabase();
            return;
	}

	
	echo "Database updates complete. <BR>";
	disconnectFromDatabase();
	echo "Oscailt 3.4 Database Upgrade Complete!";
}

upgradeDb();

?>
