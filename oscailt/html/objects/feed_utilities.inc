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

/************************************************************************/
/* This file has utility functions for reading and writing to the       */ 
/* imported feed status table.                                          */
/************************************************************************/
class ImportFeedStatus {

    var $found_row; 
    var $site_id;
    var $object_id;
    var $retries;
    var $feed_type;
    var $feed_status;
    var $last_retry;
    var $feed_url;
    var $last_status; 
    var $last_error; 
}

function checkFeedStatus($object_id, $site_id)
{
    global $dbconn, $prefix, $system_config;

    $result_rss = sql_query("SELECT site_id, object_id, retries, feed_type, feed_status, UNIX_TIMESTAMP(last_retry), feed_url, last_error FROM ".$prefix."_importfeeds_status WHERE object_id=".$object_id, $dbconn, 2);
    checkForError($result_rss);

    // There should only be row, just take the first anyhow.
    if(sql_num_rows( $result_rss ) > 0)
    {
        $total_rows = sql_num_rows( $result_rss );

        list($t_site, $t_obj, $t_retries, $t_feedtype, $t_feedstatus, $t_last_retry, $t_feed_url, $t_last_error) = sql_fetch_row($result_rss, $dbconn);
        $t_feed_obj = new ImportFeedStatus();
        $t_feed_obj->site_id = $t_site;
        $t_feed_obj->object_id = $t_obj;
        $t_feed_obj->retries = $t_retries;
        $t_feed_obj->feed_type = $t_feedtype;
        $t_feed_obj->feed_status = $t_feedstatus;
        $t_feed_obj->last_status = $t_feedstatus;
        $t_feed_obj->last_retry = $t_last_retry;
        $t_feed_obj->feed_url = $t_feed_url;
        $t_feed_obj->last_error = $t_last_error;
        $t_feed_obj->found_row = true;

        if ($total_rows > 1 && $system_config->user_error_reporting >= 2) 
            logMessage("Query to ".$prefix."_importfeeds_status table for object id ".$object_id." returned ".$total_rows." rows","sitelog.txt");

    } else {
        $t_feed_obj = new ImportFeedStatus();
        $t_feed_obj->site_id = $site_id;
        $t_feed_obj->object_id = $object_id;
        $t_feed_obj->retries = 0;
        $t_feed_obj->feed_type = "unknown";
        $t_feed_obj->feed_status = "unknown";
        $t_feed_obj->last_status = "unknown";
        $t_feed_obj->last_retry = 0;
        $t_feed_obj->found_row = false;
    }

    return $t_feed_obj;
}

function updateFeedStatus($FeedStatObj)
{
    global $dbconn, $prefix, $system_config;

    if ($FeedStatObj->retries > 10 ) return;

    $update_time = "FROM_UNIXTIME(". time() .")";
    $t_last_error = addslashes($FeedStatObj->last_error);
    if (strlen($t_last_error) > 255) $t_last_error = substr($t_last_error,0,254);
    
    // echo "len status ".strlen($FeedStatObj->feed_status);
    // echo " <BR> len type   ".strlen($FeedStatObj->feed_type );
    // echo " <BR> len type   ".strlen($t_last_error) ."[".$t_last_error."]";

    if ($FeedStatObj->found_row == true) {
        $feed_query = "UPDATE ".$prefix."_importfeeds_status SET retries=".$FeedStatObj->retries.", feed_type='".$FeedStatObj->feed_type."', feed_status='".$FeedStatObj->feed_status."', last_retry=".$update_time.",feed_url='".$FeedStatObj->feed_url."', last_error='".$t_last_error."' WHERE object_id=".$FeedStatObj->object_id;
    } else {
        $feed_query = "INSERT INTO ".$prefix."_importfeeds_status VALUES(".$FeedStatObj->site_id.", ".$FeedStatObj->object_id.",".$FeedStatObj->retries.",'".$FeedStatObj->feed_type."','".$FeedStatObj->feed_status."',".$update_time.",'".$FeedStatObj->feed_url."','".$t_last_error."')";
    }

    $result_rss = sql_query($feed_query, $dbconn, 2);
    checkForError($result_rss);

}
function getFixupRules($make_lines_array=false)
{
    global $dbconn, $prefix, $system_config;

    $result_rss = sql_query("SELECT site_id, object_id, rule_text FROM ".$prefix."_feed_rules", $dbconn, 1);
    checkForError($result_rss);

    // There should only be row, just take the first anyhow.
    $rules_set = array();
    if(sql_num_rows( $result_rss ) > 0)
    {
        $total_rows = sql_num_rows( $result_rss );
        for ($i_row = 0; $i_row < $total_rows; $i_row++)
	{
            list($t_site, $t_obj, $t_rule_text) = sql_fetch_row($result_rss, $dbconn);
            $t_key = $t_site ."-".$t_obj;
            $rules_set[$t_key] = $t_rule_text;
        }

        parseRules(&$rules_set, $make_lines_array);
    }

    return $rules_set;
}

function updateFixupRules($site_id, $object_id, $rule_text, $insert_row=true)
{
    global $dbconn, $prefix;

    if ($insert_row == true)
        $result_rss = sql_query("INSERT INTO ".$prefix."_feed_rules VALUES(".$site_id.",".$object_id.",'".$rule_text."')", $dbconn, 1);
    else
        $result_rss = sql_query("UPDATE ".$prefix."_feed_rules SET rule_text='".$rule_text."' WHERE site_id=".$site_id." AND object_id=".$object_id, $dbconn, 1);
    checkForError($result_rss);
}

function deleteFixupRules($site_id, $object_id)
{
    global $dbconn, $prefix;

    $result_rss = sql_query("DELETE FROM ".$prefix."_feed_rules WHERE site_id=".$site_id." AND object_id=".$object_id, $dbconn, 1);
    checkForError($result_rss);
}

function parseRules($rules_set, $make_lines_array)
{
    // Format is: rule1: rule_data; rule2: rule_data
    foreach ($rules_set as $t_key => $rule_str)
    {
        $rule_str_array = explode(";", $rule_str);
	if (count($rule_str_array) > 0 ) {
		$parsed_rules = array();
		foreach($rule_str_array as $rule_cmd_action)
		{
		    //echo "Parsing [".$rule_cmd_action."] <BR>";
                    // Format is: rule1: rule_data
                    $rule_parts = explode(":",$rule_cmd_action);
                    if ($rule_parts[0] == "remove") {
			if ($make_lines_array == true)
                            $parsed_rules[] = array("remove" => explode(" ",trim($rule_parts[1])));
		        else
                            $parsed_rules[] = array("remove" => trim($rule_parts[1]));
		    }
		    else if ($rule_parts[0] == "replace")
		    {
                        array_shift($rule_parts);
                        $parsed_rules[] = array("replace" => $rule_parts);
		    }
		    else
                        continue;

		}
                $rules_set[$t_key] = $parsed_rules;
	}

    }
}
