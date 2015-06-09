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

function detectExistingInstallation()
{
   if(tableColumnExists('_stories', '*') == false)
   {
      return 0;
   }
   if(tableColumnExists('_stories', 'locked') == false)
   {
      return 2;
   }
   return 3;
}

function installTables($is_upgrade = false)
{
   global $dbconn, $msg, $prefix, $system_config, $quit_on_fail;
   if($is_upgrade === false)
   {
      $msg .= "<li class='install-result'>adding topics table";
      $result = sql_query("CREATE TABLE ".$prefix."_topics (
        topic_id int(11) NOT NULL auto_increment,
        topic_name varchar(255) NOT NULL,
        active tinyint NOT NULL default 1,
        excluded tinyint NOT NULL default 0,
        PRIMARY KEY  (topic_id),
        KEY topic_id (topic_id)
      )", $dbconn, 0);

      if(!$result) $msg .= "<span class='install-error'> - ".mysql_error()." ".mysql_errno()."</span>";
      if(!$result && $quit_on_fail) return false;

      $msg .= "<li class='install-result'>adding regions table";
      $result = sql_query("CREATE TABLE ".$prefix."_regions (
        region_id int(11) NOT NULL auto_increment,
        region_name varchar(255) NOT NULL,
        active tinyint NOT NULL default 1,
        excluded tinyint NOT NULL default 0,
        PRIMARY KEY  (region_id),
        KEY region_id (region_id)
      )", $dbconn, 0);

      if(!$result) $msg .= "<span class='install-error'> - ".mysql_error()." ".mysql_errno()."</span>";
      if(!$result && $quit_on_fail) return false;

      $msg .= "<li class='install-result'>adding types table";
      $result = sql_query("CREATE TABLE ".$prefix."_types (
        type_id int(11) NOT NULL auto_increment,
        type_name varchar(255) NOT NULL,
        active tinyint NOT NULL default 1,
        excluded tinyint NOT NULL default 0,
        PRIMARY KEY  (type_id),
        KEY type_id (type_id)
      )", $dbconn, 0);

      if(!$result) $msg .= "<span class='install-error'> - ".mysql_error()." ".mysql_errno()."</span>";
      if(!$result && $quit_on_fail) return false;

      $msg .= "<li class='install-result'>adding languages table";
      $result = sql_query("CREATE TABLE ".$prefix."_languages (
        language_id int(11) NOT NULL auto_increment,
        language_name varchar(255) NOT NULL,
        language_code varchar(255) NOT NULL,
        active tinyint NOT NULL default 1,
        excluded tinyint NOT NULL default 0,
        PRIMARY KEY  (language_id),
        KEY type_id (language_id)
      )", $dbconn, 0);

      if(!$result) $msg .= "<span class='install-error'> - ".mysql_error()." ".mysql_errno()."</span>";
      if(!$result && $quit_on_fail) return false;

      $msg .= "<li class='install-result'>adding stories table";
      $result = sql_query("CREATE TABLE ".$prefix."_stories (
        story_id int(11) NOT NULL auto_increment,
        topic_id int(11) NOT NULL,
        region_id int(11) NOT NULL,
        type_id int(11) NOT NULL,
        language_id int(11) NOT NULL,
        time_posted timestamp NOT NULL,
        related_link varchar(255) NOT NULL default '',
        author_name varchar(255) NOT NULL,
        author_organisation varchar(255) NOT NULL default '',
        author_email varchar(255) NOT NULL default '',
        author_phone varchar(255) NOT NULL default '',
        author_address varchar(255) NOT NULL default '',
        story_title varchar(255) NOT NULL,
        story_subtitle varchar(255) default '',
        story_summary text NOT NULL,
        story_content mediumtext NOT NULL default '',
        hidden tinyint NOT NULL default 1,
        locked tinyint NOT NULL default 0,
        event_time timestamp,
        sticky_time timestamp,
        PRIMARY KEY  (story_id),
        KEY story_id (story_id),
        UNIQUE KEY time_posted (time_posted),
        INDEX event_time (event_time),
        INDEX topic_id (topic_id),
        INDEX region_id (region_id),
        INDEX type_id (type_id),
        INDEX author_name (author_name(16)),
        INDEX sticky_time (sticky_time)
      )", $dbconn, 0);

      if(!$result) $msg .= "<span class='install-error'> - ".mysql_error()." ".mysql_errno()."</span>";
      if(!$result && $quit_on_fail) return false;

      $msg .= "<li class='install-result'>adding comments table";
      $result = sql_query("CREATE TABLE ".$prefix."_comments (
        comment_id int(11) NOT NULL auto_increment,
        story_id int(11),
        time_posted timestamp NOT NULL,
        related_link varchar(255) NOT NULL default '',
        author_name varchar(255) NOT NULL,
        author_organisation varchar(255) NOT NULL default '',
        author_email varchar(255) NOT NULL default '',
        author_phone varchar(255) NOT NULL default '',
        author_address varchar(255) NOT NULL default '',
        comment_title varchar(255) NOT NULL,
        comment_content text NOT NULL default '',
        hidden tinyint NOT NULL default 1,
        PRIMARY KEY  (comment_id),
        KEY comment_id (comment_id),
        INDEX story_id (story_id),
        INDEX time_posted(time_posted)
      )", $dbconn, 0);

      if(!$result) $msg .= "<span class='install-error'> - ".mysql_error()." ".mysql_errno()."</span>";
      if(!$result && $quit_on_fail) return false;

      $msg .= "<li class='install-result'>adding attachments table";
      $result = sql_query("CREATE TABLE ".$prefix."_attachments (
        attachment_id int(11) NOT NULL auto_increment,
        story_id int(11),
        comment_id int(11),
        time_posted timestamp NOT NULL,
        attachment_file varchar(255) NOT NULL,
        description varchar(255),
        image int(11) default NULL,
        video int(11) default NULL,
        audio int(11) default NULL,
        hidden tinyint NOT NULL default 1,
        featurized tinyint NOT NULL default 1,
        PRIMARY KEY  (attachment_id),
        KEY attachment_id (attachment_id),
        INDEX story_id (story_id),
        INDEX comment_id (comment_id),
        INDEX time_posted (time_posted)
       )", $dbconn, 0);

      if(!$result) $msg .= "<span class='install-error'> - ".mysql_error()." ".mysql_errno()."</span>";
      if(!$result && $quit_on_fail) return false;

      // New table added in Oscailt 3.3
      $msg .= "<li class='install-result'>adding votes table";
      $result = sql_query("CREATE TABLE ".$prefix."_votes (
        item_id int(11) NOT NULL,
        user_id  int(8) NOT NULL,
        vote_value int(10) NOT NULL,
        item_type varchar(50) NOT NULL,
        last_write datetime NULL,
        PRIMARY KEY  (item_id),
        KEY item_id (item_id),
        INDEX user_id (user_id)
       )", $dbconn, 0);

      if(!$result) $msg .= "<span class='install-error'> - ".mysql_error()." ".mysql_errno()."</span>";
      if(!$result && $quit_on_fail) return false;

      // New table added in Oscailt 3.3
      $msg .= "<li class='install-result'>adding voted table";
      $result = sql_query("CREATE TABLE ".$prefix."_voted (
        vote_id  int(10) NOT NULL,
        editor_id int(10) NOT NULL,
        voter_ip varchar(255) NULL,
        last_write datetime NULL,
        PRIMARY KEY  (vote_id),
        KEY vote_id (vote_id),
        INDEX editor_id (editor_id)
      )", $dbconn, 0);

      if(!$result) $msg .= "<span class='install-error'> - ".mysql_error()." ".mysql_errno()."</span>";
      if(!$result && $quit_on_fail) return false;


      // New table added in Oscailt 3.3
      $msg .= "<li class='install-result'>adding editorial actions table";
      $result = sql_query("CREATE TABLE ".$prefix."_editorial_actions (
        action_id int(11) NOT NULL auto_increment,
        action_time datetime NOT NULL,
        editor_name varchar(255) NOT NULL,
        action varchar(255) NOT NULL,
        content_type varchar(255) NOT NULL,
    	editor_reason text NOT NULL,
        content_id varchar(20) NOT NULL,
        secondary_id int(11) NULL,
        PRIMARY KEY (action_id),
        INDEX action_id (action_id),
	INDEX action_time (action_time),
	INDEX content_id (content_id)
      )", $dbconn, 0);

      if(!$result) $msg .= "<span class='install-error'> - ".mysql_error()." ".mysql_errno()."</span>";
      if(!$result && $quit_on_fail) return false;


      // New table added in Oscailt 3.3
      $msg .= "<li class='install-result'>adding translations table";
      $result = sql_query("CREATE TABLE ".$prefix."_content_translations (
        translation_id int(11) NOT NULL auto_increment,
        content_type varchar(255) NOT NULL,
        original_content_id int(11) NOT NULL,
        translated_content_id int(11) NOT NULL,
        PRIMARY KEY (translation_id),
        INDEX trns (content_type, original_content_id)
      )", $dbconn, 0);
      if(!$result) $msg .= "<span class='install-error'> - ".mysql_error()." ".mysql_errno()."</span>";
      if(!$result && $quit_on_fail) return false;

      // New table added in Oscailt 3.3
      $msg .= "<li class='install-result'>adding import feeds status table";
      $result = sql_query("CREATE TABLE ".$prefix."_importfeeds_status (
        site_id     int(11) NOT NULL,
        object_id   int(11) NOT NULL,
        retries     int(4) default 0,
        feed_type   char(20) NULL,
        feed_status char(20) NULL,
        last_retry  datetime NULL,
        feed_url    varchar(255) NULL,
        last_error  varchar(255) NULL,
        PRIMARY KEY (site_id, object_id),
        INDEX main_key (site_id, object_id)
      )", $dbconn, 0);
      if(!$result) $msg .= "<span class='install-error'> - ".mysql_error()." ".mysql_errno()."</span>";
      if(!$result && $quit_on_fail) return false;


      $msg .= "<li class='install-result'>adding editors table";
      $result = sql_query("CREATE TABLE ".$prefix."_editors (
        editor_id int(11) NOT NULL auto_increment,
        editor_name varchar(255) NOT NULL,
        editor_password varchar(32) NOT NULL,
        editor_email varchar(255) NOT NULL default '',
        editor_lastlogin datetime NULL default NULL,
        editor_details varchar(255) NULL,
        PRIMARY KEY  (editor_id),
        KEY editor_id (editor_id),
        UNIQUE editor_name (editor_name)
      )", $dbconn, 0);

      if(!$result) $msg .= "<span class='install-error'> - ".mysql_error()." ".mysql_errno()."</span>";
      if(!$result && $quit_on_fail) return false;

      $msg .= "<li class='install-result'>adding configuration table";
         $result = sql_query("CREATE TABLE ".$prefix."_configuration (
           name varchar(255) NOT NULL,
           value varchar(255) default NULL,
           PRIMARY KEY  (name),
           KEY name (name)
      )", $dbconn, 0);

      if(!$result) $msg .= "<span class='install-error'> - ".mysql_error()." ".mysql_errno()."</span>";
      if(!$result && $quit_on_fail) return false;

   }
   else
   {
      if(version_compare($is_upgrade, '3.0.alpha') == -1)
      {
         $msg .= "<li class='install-result'>Modifying stories table to add 'Locked' attribute.";
         $result = sql_query("ALTER TABLE ".$prefix."_stories ADD locked TINYINT DEFAULT 0 NOT NULL AFTER hidden", $dbconn, 0);
         if(!$result) $msg .= "<span class='install-error'> - ".mysql_error()." ".mysql_errno()."</span>";
         if(!$result && $quit_on_fail) return false;
      }
      $msg .= "<li class='install-result'>Modifying attachment table to add 'Featurized' attribute.";
      $result = sql_query("ALTER TABLE ".$prefix."_attachments ADD featurized TINYINT DEFAULT 0 NOT NULL AFTER hidden", $dbconn, 0);
      if(!$result) $msg .= "<span class='install-error'> - ".mysql_error()." ".mysql_errno()."</span>";
      if(!$result && $quit_on_fail) return false;
      //all existing images are set to featurised
      $result = sql_query("UPDATE ".$prefix."_attachments set featurized=1", $dbconn, 0);
      if(!$result) $msg .= "<span class='install-error'> - ".mysql_error()." ".mysql_errno()."</span>";
      if(!$result && $quit_on_fail) return false;

      $added_story_indexes = array(
         "topic_id" => "topic_id",
         "region_id" => "region_id",
         "type_id" => "type_id",
         "author_name" => "author_name(16)");
      foreach($added_story_indexes as $k => $v)
      {
         $msg .= "<li class='install-result'>Modifying stories table to add $k index.";
         $sql = "ALTER TABLE $prefix"."_stories ADD INDEX $k ($v)";
         $result = sql_query($sql, $dbconn, 0);
         if(!$result) $msg .= "<span class='install-error'> - ".mysql_error()." ".mysql_errno()."</span>";
         if(!$result && $quit_on_fail) return false;
      }
      $msg .= "<li class='install-result'>Modifying comments table to add author_name index.";
      $sql = "ALTER TABLE $prefix"."_comments ADD INDEX author_name (author_name(16))";
      $result = sql_query($sql, $dbconn, 0);
      if(!$result) $msg .= "<span class='install-error'> - ".mysql_error()." ".mysql_errno()."</span>";
      if(!$result && $quit_on_fail) return false;

      $msg .= "<li class='install-result'>Modifying story table to set contents to medium text'";
      $sql = "ALTER TABLE $prefix"."_stories CHANGE story_content story_content MEDIUMTEXT NOT NULL";
      $result = sql_query($sql, $dbconn, 0);
      if(!$result) $msg .= "<span class='install-error'> - ".mysql_error()." ".mysql_errno()."</span>";
      if(!$result && $quit_on_fail) return false;
   }
   if($is_upgrade == false or version_compare($is_upgrade, '3.0.alpha') == -1)
   {
      $msg .= "<li class='install-result'>adding editors' roles table";
      $result = sql_query("CREATE TABLE ".$prefix."_editor_roles (
        editor_role_id int(11) NOT NULL auto_increment,
        editor_id int(11) NOT NULL,
        role_id varchar(255) NOT NULL,
        role_site_id varchar(255) NOT NULL,
        PRIMARY KEY  (editor_role_id),
        KEY editor_role_id (editor_role_id),
        INDEX role_id (role_id)
      )", $dbconn, 0);

      if(!$result) $msg .= "<span class='install-error'> - ".mysql_error()." ".mysql_errno()."</span>";
      if(!$result && $quit_on_fail) return false;

      $msg .= "<li class='install-result'>adding role table";
      $result = sql_query("CREATE TABLE ".$prefix."_roles (
        role_id int(11) NOT NULL auto_increment,
        role_name varchar(255) NOT NULL,
        role_type varchar(255) NOT NULL,
        role_description varchar(255) NOT NULL default '',
        PRIMARY KEY  (role_id),
        KEY role_id (role_id),
        UNIQUE role_name (role_name)
      )", $dbconn, 0);

      if(!$result) $msg .= "<span class='install-error'> - ".mysql_error()." ".mysql_errno()."</span>";
      if(!$result && $quit_on_fail) return false;

      $msg .= "<li class='install-result'>adding role permissions table";
      $result = sql_query("CREATE TABLE ".$prefix."_role_permissions (
        permission_id int(11) NOT NULL auto_increment,
        role_id int(11) NOT NULL,
        page varchar(255) NOT NULL,
        readonly tinyint default NULL,
        PRIMARY KEY  (permission_id),
        KEY permission_id (permission_id),
        INDEX role_id (role_id)
      )", $dbconn, 0);

      if(!$result) $msg .= "<span class='install-error'> - ".mysql_error()." ".mysql_errno()."</span>";
      if(!$result && $quit_on_fail) return false;

      $msg .= "<li class='install-result'>adding master data object table";
      $result = sql_query("CREATE TABLE ".$prefix."_master_data_objects (
        object_id int(11) NOT NULL auto_increment,
        site_id int(11) NOT NULL,
        object_type varchar(255) NOT NULL,
        storage varchar(255) NOT NULL,
        primary_language_code varchar(255) NOT NULL,
        master_content text NOT NULL default '',
        PRIMARY KEY  (object_id),
        KEY object_id (object_id),
        INDEX site_id (site_id),
        INDEX object_type (object_type(20))
      )", $dbconn, 0);

      if(!$result) $msg .= "<span class='install-error'> - ".mysql_error()." ".mysql_errno()."</span>";
      if(!$result && $quit_on_fail) return false;

      $msg .= "<li class='install-result'>adding data objects table";
      $result = sql_query("CREATE TABLE ".$prefix."_data_objects (
        data_id int(11) NOT NULL auto_increment,
        object_id int(11) NOT NULL,
        language_code varchar(255) NOT NULL,
        content text NOT NULL default '',
        last_update timestamp,
        revision int(11) NOT NULL default 0,
        active_version tinyint NOT NULL default 0,
        PRIMARY KEY  (data_id),
        KEY data_id (data_id),
        INDEX language_code (language_code),
        INDEX object_id (object_id)
      )", $dbconn, 0);

      if(!$result) $msg .= "<span class='install-error'> - ".mysql_error()." ".mysql_errno()."</span>";
      if(!$result && $quit_on_fail) return false;

      $msg .= "<li class='install-result'>adding edit lock table";
      $result = sql_query("CREATE TABLE ".$prefix."_edit_locks (
         lock_id int(11) NOT NULL auto_increment,
         target_id int(11) NOT NULL,
         target_type varchar(255) NOT NULL,
         lock_owner int(11) NOT NULL,
         lock_time_granted timestamp,
         lock_expiry_time timestamp,
         PRIMARY KEY  (lock_id),
         KEY lock_id (lock_id),
         INDEX t (target_id,target_type(20))
       )", $dbconn, 0);

      if(!$result) $msg .= "<span class='install-error'> - ".mysql_error()." ".mysql_errno()."</span>";
      if(!$result && $quit_on_fail) return false;

      $msg .= "<li class='install-result'>adding banned IP table";
      $result = sql_query("CREATE TABLE ".$prefix."_banned_ips (
        ip varchar(255) NOT NULL,
        time_limit timestamp,
        reason text NOT NULL,
        begin_ban datetime NULL,
        PRIMARY KEY  (ip),
        KEY ip (ip),
        INDEX time_limit (time_limit)
      )", $dbconn, 0);

      if(!$result) $msg .= "<span class='install-error'> - ".mysql_error()." ".mysql_errno()."</span>";
      if(!$result && $quit_on_fail) return false;

      $msg .= "<li class='install-result'>adding banned author table";
      $result = sql_query("CREATE TABLE ".$prefix."_banned_authors (
        author_name varchar(100) NOT NULL,
        time_limit timestamp,
        reason text NOT NULL,
        begin_ban datetime NULL,
        PRIMARY KEY  (author_name),
        KEY author_name (author_name),
        INDEX time_limit (time_limit)
      )", $dbconn, 0);

      if(!$result) $msg .= "<span class='install-error'> - ".mysql_error()." ".mysql_errno()."</span>";
      if(!$result && $quit_on_fail) return false;
   }
   return true;
}


function isEmptyTable($t_name)
{
   global $dbconn, $msg, $prefix, $system_config;
   $res = sql_query("Select count('*') from ".$prefix.$t_name. " WHERE 1", $dbconn, 0);
   if($res === false) return false;
   if(sql_num_rows( $res ) > 0)
   {
      list($c) = sql_fetch_row($res, $dbconn);
      if($c == 0) return true;
      return false;
   }
   else return false;
}

function tableColumnExists($t_name, $col_name)
{
   global $dbconn, $msg, $prefix, $system_config;
   $res = sql_query("Select count($col_name) from ".$prefix.$t_name. " WHERE 1", $dbconn, 0);
   if($res === false) return false;
   return true;
}

function installStarterRoles($is_upgrade=false)
{
   global $dbconn, $msg, $prefix, $system_config, $quit_on_fail, $permissionsList;
   $install_admin = true;
   if(!$is_upgrade && !(isEmptyTable('_editors') && isEmptyTable('_roles') && isEmptyTable('_role_permissions') && isEmptyTable('_role_permissions')))
   {
      $msg .= "<li class='install-error'>Roles, editors and permission tables are not empty!. Installation of starter data can only be carried out on Empty tables";
      if($quit_on_fail) return false;
      else return true;;
   }
   elseif($is_upgrade && !isEmptyTable('_roles'))
   {
      $msg .= "<li class='install-error'>Role table is not empty!. Upgrade of database can only be carried out when there is no role data present.";
      if($quit_on_fail) return false;
      else return true;
   }

   $msg .= "<li class='install-result'>Installing initial admin editor (admin/admin)";
   $result = sql_query("INSERT INTO ".$prefix."_editors values (NULL, 'admin', MD5('admin'), 'admin@admin.com', NULL, NULL)", $dbconn, 0);
   if(!$result) $msg .= "<span class='install-error'> - ".mysql_error()." ".mysql_errno()."</span>";
   if(!$result) return !$quit_on_fail;
   else $admin_id = mysql_insert_id($dbconn);

   $msg .= "<li class='install-result'>Installing initial administrative role (Total Control Role)";
   $result = sql_query("INSERT INTO ".$prefix."_roles values (1, 'Total Administrator', 'admin', 'Total Administrative Control')", $dbconn, 0);
   if(!$result) $msg .= "<span class='install-error'> - ".mysql_error()." ".mysql_errno()."</span>";
   if(!$result) return !$quit_on_fail;

   $result = sql_query("INSERT INTO ".$prefix."_role_permissions values (NULL, 1, 'editeditors', NULL)", $dbconn, 0);
   if(!$result) $msg .= "<span class='install-error'> - ".mysql_error()." ".mysql_errno()."</span>";
   if(!$result && $quit_on_fail) return false;

   $result = sql_query("INSERT INTO ".$prefix."_role_permissions values (NULL, 1, 'editroles', NULL)", $dbconn, 0);
   if(!$result) $msg .= "<span class='install-error'> - ".mysql_error()." ".mysql_errno()."</span>";
   if(!$result && $quit_on_fail) return false;

   $result = sql_query("INSERT INTO ".$prefix."_role_permissions values (NULL, 1, 'editconfiguration', NULL)", $dbconn, 0);
   if(!$result) $msg .= "<span class='install-error'> - ".mysql_error()." ".mysql_errno()."</span>";
   if(!$result && $quit_on_fail) return false;

   $result = sql_query("INSERT INTO ".$prefix."_role_permissions values (NULL, 1, 'importdataobjects', NULL)", $dbconn, 0);
   if(!$result) $msg .= "<span class='install-error'> - ".mysql_error()." ".mysql_errno()."</span>";
   if(!$result && $quit_on_fail) return false;

   $result = sql_query("INSERT INTO ".$prefix."_role_permissions values (NULL, 1, 'edittopics', NULL)", $dbconn, 0);
   if(!$result) $msg .= "<span class='install-error'> - ".mysql_error()." ".mysql_errno()."</span>";
   if(!$result && $quit_on_fail) return false;

   $result = sql_query("INSERT INTO ".$prefix."_role_permissions values (NULL, 1, 'editredirects', NULL)", $dbconn, 0);
   if(!$result) $msg .= "<span class='install-error'> - ".mysql_error()." ".mysql_errno()."</span>";
   if(!$result && $quit_on_fail) return false;

   $result = sql_query("INSERT INTO ".$prefix."_role_permissions values (NULL, 1, 'editself', NULL)", $dbconn, 0);
   if(!$result) $msg .= "<span class='install-error'> - ".mysql_error()." ".mysql_errno()."</span>";
   if(!$result && $quit_on_fail) return false;

   $result = sql_query("INSERT INTO ".$prefix."_role_permissions values (NULL, 1, 'viewstatus', NULL)", $dbconn, 0);
   if(!$result) $msg .= "<span class='install-error'> - ".mysql_error()." ".mysql_errno()."</span>";
   if(!$result && $quit_on_fail) return false;

   $result = sql_query("INSERT INTO ".$prefix."_role_permissions values (NULL, 1, 'editlanguages', NULL)", $dbconn, 0);
   if(!$result) $msg .= "<span class='install-error'> - ".mysql_error()." ".mysql_errno()."</span>";
   if(!$result && $quit_on_fail) return false;

   $result = sql_query("INSERT INTO ".$prefix."_role_permissions values (NULL, 1, 'edittypes', NULL)", $dbconn, 0);
   if(!$result) $msg .= "<span class='install-error'> - ".mysql_error()." ".mysql_errno()."</span>";
   if(!$result && $quit_on_fail) return false;

   $result = sql_query("INSERT INTO ".$prefix."_role_permissions values (NULL, 1, 'editregions', NULL)", $dbconn, 0);
   if(!$result) $msg .= "<span class='install-error'> - ".mysql_error()." ".mysql_errno()."</span>";
   if(!$result && $quit_on_fail) return false;

   $result = sql_query("INSERT INTO ".$prefix."_role_permissions values (NULL, 1, 'editmonitor', NULL)", $dbconn, 0);
   if(!$result) $msg .= "<span class='install-error'> - ".mysql_error()." ".mysql_errno()."</span>";
   if(!$result && $quit_on_fail) return false;

   $result = sql_query("INSERT INTO ".$prefix."_role_permissions values (NULL, 1, 'editbannedips', NULL)", $dbconn, 0);
   if(!$result) $msg .= "<span class='install-error'> - ".mysql_error()." ".mysql_errno()."</span>";
   if(!$result && $quit_on_fail) return false;

   $result = sql_query("INSERT INTO ".$prefix."_role_permissions values (NULL, 1, 'bulkdelete', NULL)", $dbconn, 0);
   if(!$result) $msg .= "<span class='install-error'> - ".mysql_error()." ".mysql_errno()."</span>";
   if(!$result && $quit_on_fail) return false;

   $result = sql_query("INSERT INTO ".$prefix."_role_permissions values (NULL, 1, 'createsiteobjects', NULL)", $dbconn, 0);
   if(!$result) $msg .= "<span class='install-error'> - ".mysql_error()." ".mysql_errno()."</span>";
   if(!$result && $quit_on_fail) return false;

   $result = sql_query("INSERT INTO ".$prefix."_role_permissions values (NULL, 1, 'editsiteobjects', NULL)", $dbconn, 0);
   if(!$result) $msg .= "<span class='install-error'> - ".mysql_error()." ".mysql_errno()."</span>";
   if(!$result && $quit_on_fail) return false;

   $result = sql_query("INSERT INTO ".$prefix."_role_permissions values (NULL, 1, 'deletesiteobjects', NULL)", $dbconn, 0);
   if(!$result) $msg .= "<span class='install-error'> - ".mysql_error()." ".mysql_errno()."</span>";
   if(!$result && $quit_on_fail) return false;

   $result = sql_query("INSERT INTO ".$prefix."_role_permissions values (NULL, 1, 'importdataobjects', NULL)", $dbconn, 0);
   if(!$result) $msg .= "<span class='install-error'> - ".mysql_error()." ".mysql_errno()."</span>";
   if(!$result && $quit_on_fail) return false;

   $result = sql_query("INSERT INTO ".$prefix."_role_permissions values (NULL, 1, 'editdataobjects', NULL)", $dbconn, 0);
   if(!$result) $msg .= "<span class='install-error'> - ".mysql_error()." ".mysql_errno()."</span>";
   if(!$result && $quit_on_fail) return false;

   $result = sql_query("INSERT INTO ".$prefix."_role_permissions values (NULL, 1, 'exportdataobjects', NULL)", $dbconn, 0);
   if(!$result) $msg .= "<span class='install-error'> - ".mysql_error()." ".mysql_errno()."</span>";
   if(!$result && $quit_on_fail) return false;

   $result = sql_query("INSERT INTO ".$prefix."_role_permissions values (NULL, 1, 'usehtml', NULL)", $dbconn, 0);
   if(!$result) $msg .= "<span class='install-error'> - ".mysql_error()." ".mysql_errno()."</span>";
   if(!$result && $quit_on_fail) return false;

   $result = sql_query("INSERT INTO ".$prefix."_role_permissions values (NULL, 1, 'usefreehtml', NULL)", $dbconn, 0);
   if(!$result) $msg .= "<span class='install-error'> - ".mysql_error()." ".mysql_errno()."</span>";
   if(!$result && $quit_on_fail) return false;

   $result = sql_query("INSERT INTO ".$prefix."_role_permissions values (NULL, 1, 'usecode', NULL)", $dbconn, 0);
   if(!$result) $msg .= "<span class='install-error'> - ".mysql_error()." ".mysql_errno()."</span>";
   if(!$result && $quit_on_fail) return false;

   $result = sql_query("INSERT INTO ".$prefix."_role_permissions values (NULL, 1, 'viewprotectedcategories', NULL)", $dbconn, 0);
   if(!$result) $msg .= "<span class='install-error'> - ".mysql_error()." ".mysql_errno()."</span>";
   if(!$result && $quit_on_fail) return false;

   $result = sql_query("INSERT INTO ".$prefix."_role_permissions values (NULL, 1, 'viewhiddencontent', NULL)", $dbconn, 0);
   if(!$result) $msg .= "<span class='install-error'> - ".mysql_error()." ".mysql_errno()."</span>";
   if(!$result && $quit_on_fail) return false;

   $msg .= "<li class='install-result'>Granting administrative role to admin user";
   $result = sql_query("INSERT INTO ".$prefix."_editor_roles values (NULL,".$admin_id.", '1', 'admin')", $dbconn, 0);
   if(!$result) $msg .= "<span class='install-error'> - ".mysql_error()." ".mysql_errno()."</span>";
   if(!$result && $quit_on_fail) return false;

   return true;
}

function installStarterTypes()
{
   global $dbconn, $msg, $prefix, $system_config, $quit_on_fail;
   if(!isEmptyTable('_types'))
   {
      $msg .= "<li class='install-error'>Type table is not empty!.  Installation of starter data can only be carried out on Empty tables";
      if($quit_on_fail) return false;
      else return true;
   }
   //creating basic types

   $msg .= "<li class='install-result'>Installing initial article types";
   $result = sql_query("INSERT INTO ".$prefix. "_types values (NULL, 'Feature', '1','0')", $dbconn, 0);
   if(!$result) $msg .= "<span class='install-error'> - ".mysql_error()." ".mysql_errno()."</span>";
   if(!$result && $quit_on_fail) return false;

   $result = sql_query("INSERT INTO ".$prefix."_types values (NULL, 'News Report', '1','0')", $dbconn, 0);
   if(!$result) $msg .= "<span class='install-error'> - ".mysql_error()." ".mysql_errno()."</span>";
   if(!$result && $quit_on_fail) return false;

   $result = sql_query("INSERT INTO ".$prefix."_types values (NULL, 'Opinion/Analysis', '1','0')", $dbconn, 0);
   if(!$result) $msg .= "<span class='install-error'> - ".mysql_error()." ".mysql_errno()."</span>";
   if(!$result && $quit_on_fail) return false;

   $result = sql_query("INSERT INTO ".$prefix."_types values (NULL, 'Press Release', '1','0')", $dbconn, 0);
   if(!$result) $msg .= "<span class='install-error'> - ".mysql_error()." ".mysql_errno()."</span>";
   if(!$result && $quit_on_fail) return false;

   $result = sql_query("INSERT INTO ".$prefix."_types values (NULL, 'Event Notice', '1','0')", $dbconn, 0);
   if(!$result) $msg .= "<span class='install-error'> - ".mysql_error()." ".mysql_errno()."</span>";
   if(!$result && $quit_on_fail) return false;

   $result = sql_query("INSERT INTO ".$prefix."_types values (NULL, 'Other Press', '1','0')", $dbconn, 0);
   if(!$result) $msg .= "<span class='install-error'> - ".mysql_error()." ".mysql_errno()."</span>";
   if(!$result && $quit_on_fail) return false;
   return true;
}

function installStarterLanguages()
{
   global $dbconn, $msg, $prefix, $system_config, $quit_on_fail;
   if(!isEmptyTable('_languages'))
   {
      $msg .= "<li class='install-error'>Language table is not empty!.  Installation of starter data can only be carried out on Empty tables";
      if($quit_on_fail) return false;
      else return true;
   }
   $msg .= "<li class='install-result'>Installing initial Language (english)";
   //language
   $result = sql_query("INSERT INTO ".$prefix."_languages values (NULL, 'English', 'en', '1','0')", $dbconn, 0);
   if(!$result) $msg .= "<span class='install-error'> - ".mysql_error()." ".mysql_errno()."</span>";
   if(!$result && $quit_on_fail) return false;
   return true;
}

function installStarterTopics()
{
   global $dbconn, $msg, $prefix, $system_config, $quit_on_fail;
   if(!isEmptyTable('_topics'))
   {
      $msg .= "<li class='install-error'>Topic table is not empty!.  Installation of starter data can only be carried out on Empty tables";
      if($quit_on_fail) return false;
      else return true;
   }

   $msg .= "<li class='install-result'>Installing initial Topic (miscellaneous)";
   //creating sample topic
   $result = sql_query("INSERT INTO ".$prefix."_topics values (NULL, 'Miscellaneous', '1','0')", $dbconn, 0);
   if(!$result) $msg .= "<span class='install-error'> - ".mysql_error()." ".mysql_errno()."</span>";
   if(!$result && $quit_on_fail) return false;
   return true;
}

function installStarterRegions()
{
   global $dbconn, $msg, $prefix, $system_config, $quit_on_fail;
   if(!isEmptyTable('_regions'))
   {
      $msg .= "<li class='install-error'>Region table is not empty!.  Installation of starter data can only be carried out on Empty tables";
      if($quit_on_fail) return false;
      else return true;
   }
   $msg .= "<li class='install-result'>Installing initial Regions (national and international)";
   //dummy regions
   $result = sql_query("INSERT INTO ".$prefix."_regions values (NULL, 'International', '1','0')", $dbconn, 0);
   if(!$result) $msg .= "<span class='install-error'> - ".mysql_error()." ".mysql_errno()."</span>";
   if(!$result && $quit_on_fail) return false;
   $result = sql_query("INSERT INTO ".$prefix."_regions values (NULL, 'National', '1','0')", $dbconn, 0);
   if(!$result) $msg .= "<span class='install-error'> - ".mysql_error()." ".mysql_errno()."</span>";
   if(!$result && $quit_on_fail) return false;
   return true;
}

function installStarterConfiguration($is_upgrade=false)
{
   //now configuration
   global $dbconn, $msg, $prefix, $system_config, $quit_on_fail;
   if(!$is_upgrade && !isEmptyTable('_configuration'))
   {
      $msg .= "<li class='install-error'>configuration table is not empty!.  Installation of starter data can only be carried out on Empty tables";
      if($quit_on_fail) return false;
      else return true;
   }
   $system_config = new SystemConfig();
   $system_config->user_error_reporting = 2;
   $system_config->sql_query_caching_enabled = false;//for install
   if(!$is_upgrade)
   {
      $system_config->timezone_offset = 25200;
      $system_config->image_attachment_max_size = 1024000;//1M
      $system_config->image_attachment_max_width = 1000;
      $system_config->video_attachment_max_size = 10240000;//10M
      $system_config->audio_attachment_max_size = 10240000;//10M
      $system_config->miscellaneous_attachment_max_size = 10240000;//10M
      $system_config->allow_story_image_attachments = 1;
      $system_config->allow_story_video_attachments = 1;
      $system_config->allow_story_video_attachments = 1;
      $system_config->allow_story_audio_attachments = 1;
      $system_config->allow_story_miscellaneous_attachments = 1;
      $system_config->allow_comment_image_attachments = 1;
      $system_config->allow_comment_audio_attachments = 1;
      $system_config->allow_comment_video_attachments = 1;
      $system_config->allow_comment_miscellaneous_attachments = 1;
      $system_config->story_max_attachments = 10;
      $system_config->comment_max_attachments = 10;
      $system_config->site_short_name = 'Indymedia Corporation :)';
      $system_config->site_long_name = 'Independent Media Centre Inc.';
      $system_config->site_url = 'www.example.com';
      $system_config->site_description = 'Indymedia is a collective of independent media organizations and hundreds of journalists offering grassroots, non-corporate coverage. Indymedia is a democratic media outlet for the creation of radical, accurate, and passionate tellings of truth.';
      $system_config->rss_query_cache_expiry = 3600;
      $system_config->mono_lingual_filter = false;
      $system_config->newswire_thumbnails_enabled = 1;
   }
   else
   {
      $system_config->load();
   }
   //these are new values not in previous versions
   $system_config->site_keywords = 'Indymedia,radical news, non-corporate media';
   $system_config->editor_max_attachments = 20;
   $system_config->editor_max_contents = 103424;//101k
   $system_config->editor_max_summary = 65536;//64k
   $system_config->editor_max_comment = 65536;//64k
   $system_config->user_max_contents = 103424;
   $system_config->user_max_comment = 65536;
   $system_config->user_max_summary = 65536;
   $system_config->prepopulate_editor_details = 1;
   $system_config->auto_cache_objects = false;
   $system_config->use_live_objects = false;
   $system_config->allow_live_objects = false;
   $system_config->default_strftime_format = "%A %B %d, %Y %H:%M";
   $system_config->edit_locking_time = 1200;
   $system_config->articleview_preview_story = 1;
   $system_config->banned_ips = "";
   $system_config->banned_referers = "stormfront.org";
   $system_config->redirect_banned_url = "http://www.fretnotguitarrepair.com/Bunnie%20Lover.htm";
   $system_config->use_friendly_urls = true;
   $system_config->save();

}


function installStarterData($is_upgrade = false)
{
   global $msg;
   if(!$is_upgrade)
   {
      if(installStarterLanguages() === false)
      {
         $msg .= " <b>failed to install Initial Languages, quitting</b>";
         return false;
      }
      if(installStarterTypes() === false)
      {
         $msg .= " <b>failed to install Initial Types, quitting</b>";
         return false;
      }
      if(installStarterTopics() === false)
      {
         $msg .= " <b>failed to install Initial Topics, quitting</b>";
         return false;
      }
      if(installStarterRegions() === false)
      {
         $msg .= " <b>failed to install Initial Regions, quitting</b>";
         return false;
      }
   }
   if(installStarterRoles($is_upgrade) === false)
   {
      $msg .= " <b>failed to install Initial Roles, quitting</b>";
      return false;
   }
   if(installStarterConfiguration($is_upgrade) === false)
   {
      $msg .= " <b>failed to install Initial Configuration, quitting</b>";
      return false;
   }
   $msg .= "<li class='install-result'>Successfully installed starter data</li>";
   return true;
}



?>
